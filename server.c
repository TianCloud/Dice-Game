#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

void serverClient(int sd) {
	//variable declaration
	int message;
	int n, pid, status;
	int cTotal,sTotal;
	int cDice, sDice;	
	time_t t;
	char enterToRoll;
	char prev = 0;	

	//initializing dice value and total score
	//for server and client
	cTotal = 0;
	sTotal = 0;
	cDice = 0;
	sDice = 0;

	//initializing random number generator
	srand(time(0));

	while (1) {

			//read dice value from client
			if (n = read(sd, &message, sizeof(int))) {
				
				cDice = message;
				cTotal = cTotal + cDice;
				printf("Client rolled: %d\n", cDice);
				printf("Client total: %d\n", cTotal);
				//check if client won
				if (cTotal >= 100) {
					printf("Client won, Server lost\n");
					close(sd);
					kill(getppid(), SIGTERM);
					exit(0);
				}
				printf("\n");
				printf("Your turn now, press [enter] to roll dice\n");
				fflush(stdin);
				getchar();
				sDice = (rand()+5)%6 + 1;
				printf("\n");
				sTotal = sTotal + sDice;
				printf("Server rolled: %d\n", sDice);
				printf("Server total: %d\n", sTotal);
				printf("\n");
				printf("------------------------------\n");
				message = sDice;
				//write to client with dice value
				if(n = write(sd, &message, sizeof(int)) < 0){
					printf("Send failed\n");
					exit(0);
				}
				//check if server won
				if(sTotal >= 100){
					printf("Server won, Congradulations!\n");
					close(sd);
					kill(getppid(),SIGTERM);
					exit(0);
				}
			}
	}
}

//server side
int main(int argc, char* argv[]) {

	int sd, client, pNum, status;
	socklen_t len;

	struct sockaddr_in sAddress;

	//check if user entered arguments with correct format
	if (argc != 2) {
		printf("Call model: %s <Port:#>\n", argv[0]);
		exit(0);
	}
	//check is socket was created
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Wrong creating new socket");
		exit(1);
	}

	//config the server socket
	sAddress.sin_family = AF_INET;
	sAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	sscanf(argv[1], "%d", &pNum);
	sAddress.sin_port = htons((uint16_t)pNum);
	bind(sd, (struct sockaddr*) &sAddress, sizeof(sAddress));

	// start listening to the assigned port
	listen(sd, 5);
	while (1) {
		// wait for clients from any IP address
		client = accept(sd, (struct sockaddr*) NULL, NULL);
		printf("A client is connected to the server\n");
		printf("Game Start, glhf!\n");
		// fork a process for each client
		if (!fork())
			serverClient(client);
		close(client);
		waitpid(0, &status, WNOHANG);
	}

}

