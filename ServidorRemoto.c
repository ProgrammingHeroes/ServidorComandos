
// ServidorRemoto.c -----------------------------------------------------------
// http://es.tldp.org/Tutoriales/PROG-SOCKETS/prog-sockets.html
// http://www.tutorialspoint.com/unix_sockets/socket_core_functions.htm


#include "Sockets.h"

#include <signal.h>

#define EXIT_COMMAND "exit"
#define CLEAR_COMMAND "clear"
#define PORT_COMMAND "port"

void comunicate(int);
void listeningProcess(int);
void sendFile(int, int, int);
void writeExec(char*, int);

int main(int argc, char* argv[])
{
	int socketDescriptor = createInetStreamSocket();
	listenAtPort(socketDescriptor, PORT, MAX_CLIENTS_QUEUE);

	printf("%sServer listening in port %d.\n%s", WHITE, PORT, RESET_COLOR);
	printf("%sWrite \"%s\" to clear the terminal.%s\n", WHITE, CLEAR_COMMAND, RESET_COLOR);
	printf("%sWrite \"%s\" to see the listening port.%s\n", WHITE, PORT_COMMAND, RESET_COLOR);
	printf("%sWrite \"%s\" to quit.%s\n", WHITE, EXIT_COMMAND, RESET_COLOR);

	char command[10];

	int pid = fork();

	switch (pid)
	{
		case -1:
			perror("Error creating child process.\n");
			break;
		case 0:
			listeningProcess(socketDescriptor);
			exit(0);
		default:
			do
			{
				scanf("%s", command);
				if (strcmp(command, CLEAR_COMMAND) == 0)
				{
					if (fork() == 0)
						execlp("clear", "clear", NULL);
				}
				else if (strcmp(command, PORT_COMMAND) == 0)
				{
					printf("%sServer listening in port %d.\n%s", WHITE, PORT, RESET_COLOR);
				}
				else if (strcmp(command, EXIT_COMMAND) != 0)
				{
					printf("%sInvalid command%s\n", RED, RESET_COLOR);
				}
			}
			while (strcmp(command, EXIT_COMMAND) != 0);
			break;
	}

	kill(pid, SIGKILL);
	wait();
	close(socketDescriptor);
	printf("%sSocket closed. Bye!%s\n", RED, RESET_COLOR);
}

void listeningProcess(int socketDescriptor)
{
	while (1)
	{
		int clientDescriptor = getClient(socketDescriptor);

		int pid_client = fork();
		switch (pid_client)
		{
			case -1:
				perror("Error creating fill process.\n");
				break;
			case  0:
				printf("%sClient connected. IP: %s%s\n", BLUE, nstoh(), RESET_COLOR);
				comunicate(clientDescriptor);
				exit(0);
			default:
				break;
		}
	}
}

void comunicate(int clientDescriptor)
{
	Message command;
	readMessage(clientDescriptor, &command, sizeof(command));
	printf("%sThe client wants the command: \"%s\"%s\n",
			 GREEN, command, RESET_COLOR);

	int pipeDescriptors[2];
	int pipeCode = pipe(pipeDescriptors);
	if (pipeCode < 0)
	{
		perror("Can't create pipes.\n");
		exit(-1);
	}

	pid_t pid = fork();
	switch (pid)
	{
		case -1:
			perror("Error creating child process.\n");
			exit(-1);
		case  0:
			writeExec(command, clientDescriptor);//pipeDescriptors);
			exit(-1);
		default:
			wait();
	}

	close(clientDescriptor);
	printf("%sClient connexion closed.%s\n", YELLOW, RESET_COLOR);
}


void writeExec(char* command, int socket)
{
	int i;
	int spaces = 1;
	for (i=0; *(command+i) != '\0'; i++)
	{
		if (*(command+i) == ' ')
		{
			spaces++;
		}
	}

	char* arguments[spaces+1];

	spaces = 0;
	char* lastToken = NULL;
	for (i=0; *(command+i) != '\0'; i++)
	{
		if (*(command+i) == ' ')
		{
			*(command+i) = '\0';
			arguments[spaces++] = lastToken;
			lastToken = NULL;
		}
		else if (lastToken == NULL)
		{
			lastToken = command+i;
		}
	}
	arguments[spaces++] = lastToken;
	arguments[spaces++] = NULL;

	close(1);
	close(2);
	dup(socket);
	dup(socket);

	execvp(arguments[0], arguments);
	perror("Error in execlp.\n");
	write(socket, "ERROR in the command execution\0", 32);
}

// ServidorRemoto.c -----------------------------------------------------------
