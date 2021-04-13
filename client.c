#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc,char**argv)
{
	setvbuf (stdout, NULL, _IONBF, BUFSIZ);

	// se creeaza socket-ul care va fi folosit la comunicatie
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Eroare la crearea socket-ului");
		return 1;
	}

	// se completeaza adresa si portul serverului la care ne conectam
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12346);
	inet_aton("127.0.0.1", &addr.sin_addr);

	printf("Enter pentru conectare\n");
	getchar();

	// se realizeaza o conexiune la server
	int c = connect(sock, (struct sockaddr*) &addr, sizeof(addr));
	if (c < 0) {
		perror("Eroare la conectare");
		close(sock);
		return 1;
	}

	printf("Enter pentru trimitere\n");
	getchar();

	// se trimit date catre server
	char buf[100] = "test";
	int buflen = strlen(buf) + 1;

	int s = send(sock, buf, buflen, 0);
	//int s = write(sock, buf, buflen);
	if (s < 0) {
		perror("Eroare la trimiterea pe socket");
		close(sock);
		return 1;
	}

	// se inchide socket-ul
	close(sock);

	return 0;
}
