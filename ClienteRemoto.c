
// ClienteRemoto.c ------------------------------------------------------------

#include "Sockets.h"


int main(int argc, char* argv[])
{
	if (argc == 3)
	{
		int socketDescriptor = createInetStreamSocket();
		connectToInetServer(socketDescriptor, PORT, argv[1]);
		
		printf("%sConnected successful.%s\n", BLUE, RESET_COLOR);

		Message command;
		strcpy(command, argv[2]);
		writeMessage(socketDescriptor, command, sizeof(command));
		printf("I've requested for: \"%s\"\n\n", command);
		
		int end = 0;
		int size = 0;
		Message message;
		while (!end && (size = read(socketDescriptor, &message, sizeof(message))) > 0)
		{
			write(1, &message, size);

			if ((size < sizeof(message)-1) || (message[size-1] == '\0'))
			{
				end = 1;
			}
		}

		if (size == -1)
		{
			perror("Error de lectura.\n");
		}
		
		close(socketDescriptor);
		printf("%s\nServer connexion closed, socket closed.%s\n", RED, RESET_COLOR);
	}
	else
	{
		perror("Invalid sintax. Use: ClienteRemoto SERVER_IP COMMAND\n");
		exit(-1);
	}
}

// ClienteRemoto.c ------------------------------------------------------------
