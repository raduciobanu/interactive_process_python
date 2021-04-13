import subprocess
import sys
import signal
import time
import os.path

from contextlib import contextmanager
from subprocess import Popen, PIPE, STDOUT
from os import path
from time import sleep

@contextmanager
def timeout(time):
  signal.signal(signal.SIGALRM, raise_timeout)
  signal.alarm(time)

  try:
    yield
  except TimeoutError:
    pass
  finally:
    signal.signal(signal.SIGALRM, signal.SIG_IGN)

def raise_timeout(signum, frame):
  raise TimeoutError

def make_target(target):
  subprocess.run(["make " + target], shell=True)
  return path.exists(target)

def make_clean():
  subprocess.run(["make clean"], shell=True)

def exit_if_condition(condition, message):
  if condition:
    print(message)
    make_clean()
    quit()

class Process:
  def __init__(self, command):
    self.command = command
    self.started = False

  def start(self):
    try:
      self.proc = Popen(self.command, universal_newlines=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)
      self.started = True
    except FileNotFoundError as e:
      print(e)
      quit()

  def finish(self):
    if self.started:
      self.proc.terminate()
      self.proc.wait(timeout=0.2)

  def send_input(self, proc_in):
    if self.started:
      self.proc.stdin.write(proc_in + "\n")
      self.proc.stdin.flush()

  def get_output(self):
    if self.started:
      return self.proc.stdout.readline()
    else:
      return ""

  def get_output_timeout(self, tout):
    if self.started:
      with timeout(tout):
        try:
          return self.proc.stdout.readline()
        except TimeoutError as e:
          return "timeout"
    else:
      return ""

  def is_alive(self):
    if self.started:
      return self.proc.poll() is None
    else:
      return False

def tcp_test():
  # clean up
  make_clean()

  # run make
  exit_if_condition(not make_target("client"), "Client could not be built")
  exit_if_condition(not make_target("server"), "Server could not be built")

  # create the process objects
  server = Process(["./server"])
  client = Process(["./client"])

  # start the server and check that it is up
  server.start()
  sleep(1)
  exit_if_condition(not server.is_alive(), "Server is not up")

  # check that the server prints the initial message
  outs = server.get_output_timeout(3)
  exit_if_condition(outs == "timeout", "Server did not print in time")

  # start the client and check that it is up
  client.start()
  sleep(1)
  exit_if_condition(not client.is_alive(), "Client is not up")

  # check that the client prints the initial message
  outc = client.get_output_timeout(3)
  exit_if_condition(outc == "timeout", "Client did not print in time")

  # send a newline in the client
  client.send_input("")

  # check that the server receives the connection from the client
  outs = server.get_output_timeout(3)
  exit_if_condition(outs == "timeout", "Server did not receive the connection")
  print(outs)

  # check that the client prints the second message
  outc = client.get_output_timeout(3)
  exit_if_condition(outc == "timeout", "Client did not print in time")

  # send a newline in the client
  client.send_input("")

  # check that the server receives the data from the client
  outs = server.get_output_timeout(3)
  exit_if_condition(outs == "timeout", "Server did not receive the data")
  print(outs)

  # check that the client and the server have exited
  sleep(2)
  exit_if_condition(client.is_alive(), "Client is still up")
  exit_if_condition(server.is_alive(), "Server is still up")

  # stop the processes
  server.finish()
  client.finish()

  # clean up
  make_clean()

tcp_test()
