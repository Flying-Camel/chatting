#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
#define NAME_SIZE 10

int serv_sock, clnt_sock;
struct sockaddr_in serv_adr, clnt_adr;
struct timeval timeout;
fd_set reads, cpy_reads;

socklen_t adr_sz;
int fd_max, str_len, quit, fd_num, i, j;
char buf[BUF_SIZE];
char name[10] = {0,};
char user[9][NAME_SIZE] = {0,};
char tmp[5] = {0,};

char *token;
char gap[] = "\n ", buf_tmp[BUF_SIZE];
int flag=0;

void error_handling(char *buf);
void login();
void logout();
void mess();
void list();
int isWhisper();

int main(int argc, char *argv[])
{
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max=serv_sock;

	while(1)
	{
		cpy_reads=reads;
		timeout.tv_sec;
		timeout.tv_usec=5000;

		if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
			break;

		if(fd_num==0)
			continue;

		for(i=0; i<fd_max+1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				if(i==serv_sock){    //로그인
					login();
				}
				else    //메세지를 읽음
				{
					quit = read(i,name,NAME_SIZE);
					str_len = read(i, buf, BUF_SIZE);

					if(!strncmp(buf,"@list",5)){  //리스트
						list();
					}
					else if(!strncmp(buf,"@quit",5) || quit==0){    //로그아웃
						logout();
					}		
					else{      //메세지 보내기
						mess();
					}
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}
void login(){
	adr_sz=sizeof(clnt_adr);
	clnt_sock=
		accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
	FD_SET(clnt_sock, &reads);

	if(fd_max<clnt_sock)
		fd_max=clnt_sock;

	read(clnt_sock, buf, NAME_SIZE);
	strcpy(user[clnt_sock],buf);				

	strcpy(tmp,"log\0");
	for(j=4;j<fd_max+1;j++){
		write(j,tmp,5);
		write(j,user[clnt_sock],NAME_SIZE);
	}

	printf("%s 님이 접속하셨습니다.\n", buf);
}


void logout(){

	strcpy(tmp,"quit\0");

	for(j=4;j<fd_max+1;j++){
		write(j,tmp,5);
		write(j,name, NAME_SIZE);
	}

	printf("%s님이 접속을 종료하셨습니다.\n",name);
	memset(user[i],0,NAME_SIZE);

	shutdown(i,SHUT_WR);
	read(i,tmp,5);
	printf("%s\n",tmp);

	FD_CLR(i, &reads);
	close(i);


}
int isWhisper(){
	if(!strncmp(buf,"@",1)){
		strcpy(buf_tmp,buf);
		token = strtok(buf_tmp,gap);
		token+=1;

		for(j=4;j<fd_max+1;j++){
			if(!strcmp(user[j],token)){
				write(j,tmp,5);
				write(j,name, NAME_SIZE);
				write(j, buf, str_len);    

				strcpy(tmp,"drc\0");
				write(i,tmp,5);

				return 1;
			}
		}
	}
	return 0;
}
void mess(){
	strcpy(tmp,"all\0");

	if(!isWhisper()){
		for(j=4;j<fd_max+1;j++){
			write(j,tmp,5);
			write(j,name, NAME_SIZE);
			write(j, buf, str_len);   
		}
	}

}
void list(){
	strcpy(tmp,"lst\0");
	write(i,tmp,5);

	for(j=4;j<9;j++)
		write(i,user[j],NAME_SIZE);


}


void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}
