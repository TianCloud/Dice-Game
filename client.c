#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>


//Client side
int main(int argc, char* argv[]) {
	//variable declaration
	int message;
	int cTotal,sTotal;
	int server, pNum, pid, n, status;
	socklen_t len;
	struct sockaddr_in sAddress;
	int cDice, sDice;
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
	
	//check if user entered arguments with correct format
	if (argc != 3) {
		fprintf(stderr, "Call model:%s <IP> <Port>\n", argv[0]);
		exit(0);
	}
	//check if socket was created
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Cannot create a socket\n");
		exit(1);
	}

	//config the server socket
	sAddress.sin_family = AF_INET;
	sscanf(argv[2], "%d", &pNum);
	sAddress.sin_port = htons((uint16_t)pNum);
	if (inet_pton(AF_INET, argv[1], &sAddress.sin_addr) < 0) {
		fprintf(stderr, " inet_pton() has failed\n");
		exit(2);
	}
	//connect to the server
	if (connect(server, (struct sockaddr*) &sAddress, sizeof(sAddress)) < 0) {
		fprintf(stderr, "Connection refused\n");
		exit(3);
	}

	printf("Connection Success!\n");
	printf("Game Start, glhf!\n");
	printf("\n");
	while (1) {

			printf("Client turn, press [enter] to roll dice\n");
			fflush(stdin);
			getchar();
			cDice = rand()%6 + 1;
			printf("\n");
			cTotal = cTotal + cDice;
			printf("Client rolled: %d\n", cDice);
			printf("Client total: %d\n", cTotal);
			printf("\n");
			printf("------------------------------\n");
			message = cDice;
			//write to server with dice value
			if(n = write(server, &message, sizeof(int)) < 0){
				printf("Send failed\n");
				return 1;
			}
			//check if client won
			if(cTotal >= 100){
				printf("You won, Congradulations!\n");
				close(server);
				kill(getppid(),SIGTERM);
				exit(0);
			}
			//read dice value from server
			if (n = read(server, &message, sizeof(int))) {
				
				sDice = message;
				sTotal = sTotal + sDice;
				printf("Server rolled: %d\n", sDice);
				printf("Server total: %d\n", sTotal);
				//check if server won
				if(sTotal >= 100){
					printf("Game Over, you lose\n");
					close(server);
					kill(getppid(),SIGTERM);
					exit(0);
				}
			}
	}
}


