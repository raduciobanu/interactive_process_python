#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc,char**argv)
{
	setvbuf (stdout, NULL, _IONBF, BUFSIZ);

	// se creeaza socket-ul care va fi folosit la comunicatie
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Eroare la crearea socket-ului");
		return 1;
	}

	// se completeaza informatii despre adresa si portul pe care ascultam
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12346);
	addr.sin_addr.s_addr = INADDR_ANY;

	// se asociaza socketul cu portul
	int b = bind(sock, (struct sockaddr*) &addr, sizeof(addr));
	if (b < 0) {
		perror("Eroare la legarea adresei de socket");
		close(sock);
		return 1;
	}

	// se face socket-ul pasiv (pentru a primi cereri de conexiune de la clienti)
	int l = listen(sock, 1);
	if (l < 0) {
		perror("Eroare la setarea socket-ului pasiv");
		close(sock);
		return 1;
	}

	printf("Serverul a pornit cu succes, acum asteapta sa se conecteze un client\n");

	// se accepta conexiune de la un client
	struct sockaddr_in cli_addr;
	socklen_t socklen = sizeof(cli_addr);
	int sock_cli = accept(sock, (struct sockaddr*) &cli_addr, &socklen);
	if (sock_cli < 0) {
		perror("Eroare la acceptarea unei conexiuni");
		close(sock);
		return 1;
	}

	printf("S-a conectat clientul cu adresa %s si portul %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

	// se primesc date de la client
	char buf[100];
	int r = recv(sock_cli, buf, 100, 0);
	//int r = read(sock_cli, buf, 100);
	if (r < 0) {
		perror("Eroare la primirea pe socket");
		close(sock);
		return 1;
	}

	printf("S-a primit mesajul \"%s\"\n", buf);

	// se inchide socket-ul de client, si la final socket-ul pasiv
	close(sock_cli);
	close(sock);

	return 0;
}
