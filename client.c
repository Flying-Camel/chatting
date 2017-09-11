#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUF_SIZE 1024
#define NAME_SIZE 10
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	int fd_num;

	char name[10] = {0,};

	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected! \n");

	printf("이름을 입력하세요 : ");
	scanf("%s",name);
	getchar();
	write(sock,name,NAME_SIZE);

	FD_ZERO(&reads);
	FD_SET(sock,&reads);
	FD_SET(0,&reads);

	while(1) 
	{
		cpy_reads = reads;
		timeout.tv_sec=0;
		timeout.tv_usec=5000;

		if((fd_num = select(sock+1,&cpy_reads, 0, 0, &timeout)) == -1 )
			break;

		if(FD_ISSET(sock,&cpy_reads)){

			char tmp[5] = {0,};
			read(sock,tmp,5);

			if(!strcmp(tmp,"all")){
				str_len=read(sock, message, NAME_SIZE);
				message[str_len]=0;
				printf("<%s> : ", message);

				str_len=read(sock, message, BUF_SIZE);
				message[str_len]=0;
				printf("%s", message);
			}
			else if(!strcmp(tmp,"lst")){
				printf("\n접속중인 회원\n");
				str_len=0;

				while(str_len<50){
					str_len += read(sock,message,NAME_SIZE);
					if(message[0] != '\0')
						printf("<%s> ",message);
				}
				printf("\n");
			}
			else if(!strcmp(tmp,"drc")){
				printf("귓속말이 전달되었습니다.\n");
			}
			else if(!strcmp(tmp,"log")){
				str_len=read(sock, message, NAME_SIZE);
				message[str_len]=0;
				printf("%s님이 접속하셨습니다.\n", message);

			}
			else if(!strcmp(tmp,"quit")){
				str_len=read(sock, message, NAME_SIZE);
				message[str_len]=0;
				printf("%s님이 접속을 종료 하셨습니다.\n", message);
				if(!strcmp(name,message))
				{
					write(sock,"BYE~",5);
					close(sock);
					break;
				}
			}

		}
		else if(FD_ISSET(0,&cpy_reads)){

			fgets(message, BUF_SIZE, stdin);

			write(sock, name,NAME_SIZE);
			write(sock, message, strlen(message));
		}
	}

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
