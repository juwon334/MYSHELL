#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

void ms(char **command1,char **command2,char *str);
int valu(char **command1);
void tocken(char **command1,char **command2,char* str,char* tok);
void dtocken(char **command1,char* str);
int ac(char **command1,char* pt);
int ac2(char **command1,char **command2,char* pt);
void runpipe(char **command1,char **command2,int* fd);
void re1(char **command1,char **command2,int fdr);
void re2(char **command1,char **command2,int fdr);
void def(char **command1);
int ptt(char* pt);

int main(){
	char *command1[4];
	char *command2[5];
	char str[1024];
	int fdr;
	int fd[2];
	char pt[1024];
	char *get_current_dir_name(void);
	sigset_t blockset;
	sigemptyset(&blockset);
	sigaddset(&blockset,SIGINT);
	sigaddset(&blockset,SIGQUIT);
	sigprocmask(SIG_BLOCK, &blockset, NULL);
	ptt(pt);

	while(1){
		ms(command1,command2,str);
		printf("%c[1;35m",27);
		printf("%s:",getlogin());
		printf("%c[1;34m",27);
		printf("[%s]$",get_current_dir_name());
		printf("%c[0m",27);
		fgets(str,sizeof(str),stdin);
		str[strlen(str) - 1] = '\0';
		fflush(stdin);

		if(feof(stdin))
			exit(0);

		if(strchr(str, '|') != NULL){
			tocken(command1,command2,str,"|");
			if(ac2(command1,command2,pt) == -1)
				continue;
			runpipe(command1,command2,fd);
		}

		else if(strchr(str,'>') != NULL){
			tocken(command1,command2,str,">");
			if(ac(command1,pt) == -1)
				continue;
			re1(command1,command2,fdr);
		}

		else if(strchr(str,'<') != NULL) {
			tocken(command1,command2,str,"<");
			if(ac(command1,pt) == -1)
				continue;
			re2(command1,command2,fdr);
		}

		else {
			dtocken(command1,str);
			if (command1[0] != NULL) {
				if(valu(command1) == 1)
					continue;
				if(ac(command1,pt) == -1){
                                        continue; 
				}
				def(command1);
                        }
			wait(NULL);
		}
	}

	return 0;
}

void ms(char **command1,char **command2,char *str){
	memset(str,0x00,sizeof(str));
	memset(command2,0,sizeof(command2));
	memset(command1,0,sizeof(command1));
}
int valu(char **command1){
	if(strcmp(command1[0], "exit") == 0) {
		exit(0);
	}
	else if (strcmp(command1[0], "cd") == 0) {
		chdir(command1[1]);
		return 1;
	}
}
void tocken(char **command1,char **command2,char* str,char* tok){
	int i =0;
	char* pch;
	char* text1 = NULL;
	char* text2 = NULL;
	char* command11[4];
	char* command22[5];
	text1 = strtok(str,tok);
	text2 = strtok(NULL,tok);
	strcat(text1,"\0");
	strcat(text2,"\0");
	i = 0;
	pch = strtok(text1," ");
	while(pch!= NULL && i<3){
		command11[i] = pch;
		pch = strtok(NULL," ");
		i++;
	}
	command11[i] = (char*)0;
	i =0;

	pch = strtok(text2," ");

	while(pch != NULL && i<3){
		command22[i] = pch;
		pch = strtok(NULL, " ");
		i++;
	}

	command22[i] = (char *)0;
	for(int j =0;j<4;j++){
		command1[j] = command11[j];
	}
	for(int j =0;j<5;j++){
		command2[j] = command22[j];
	}

}
void dtocken(char **command1,char* str){
	int i = 0;
	char* pch;
	str[strlen(str)] = '\0';
	pch = strtok(str," ");
	while(pch != NULL && i <3)
	{
		command1[i] = pch;
		pch = strtok(NULL, " ");
		i++;
	}
	command1[i] = (char*)0;
}
int ptt(char* pt){
        int fdd;
	int size = 0;
	if((fdd = open("./.myshell",O_RDONLY)) < 0){
                fprintf(stderr,"Error\n");
                fflush(stderr);
                return -1;
        }
        size = read(fdd,pt,1024);
        if(size == 0){
                return -1;
        }

}
int ac(char **command1,char *pt){
	char tmp[1024];
	char buf[1024];
	int count = 0;
	char* path[5];
	char* ptr;
	int i =0;
	memset(&tmp,0x00,sizeof(tmp));
	memset(path,0x00,sizeof(path));
	memset(&buf,0x00,sizeof(buf));

	strcpy(buf,pt);
	ptr = strtok(buf,"=");
	while(ptr!=NULL){
		path[i] = ptr;
		ptr = strtok(NULL,":,\n");
		i++;
	}
	path[i] = '\0';
	for(int k = 1;k<4;k++){
		strcpy(tmp,path[k]);
		strcat(tmp,command1[0]);
		if(access(tmp,F_OK)!=0){
			count++;
		}
	}
	if(count == 3){
		printf("Command Not Found\n");
		return -1;
	}
	else
		return 0;

}

int ac2(char **command1,char **command2,char* pt){
	if(ac(command1,pt) || ac(command2,pt) == -1){
		fprintf(stderr,"Command Not Found\n");
		fflush(stderr);
		return -1;

	}
	else
		return 0;
}

void runpipe(char **command1,char **command2,int* fd){
	if(pipe(fd) == -1){
		printf("Can't make pipe\n");
		exit(1);
	}

	switch(fork())
	{
		case -1: perror("fork error");
			 break;
		case 0:
			 if(close(1) == -1) perror("close1");
			 if(dup(fd[1]) != 0);
			 if(close(fd[0]) == -1 || close(fd[1]) == -1){
				 perror("close2");
			 }
			 execvp(command1[0], command1);
			 printf("Command not found\n");
			 exit(0);
	}

	switch(fork()){
		case -1 : perror("fork error"); break;
		case 0:
			  if(close(0) == -1 )
				  perror("close3");
			  if(dup(fd[0]) != 0);
			  if(close(fd[0]) == -1 || close(fd[1]) == -1)
				  perror("close4");
			  execvp(command2[0], command2);
			  printf("Command not found\n");
			  exit(0);
	}

	if(close(fd[0]) == -1 || close(fd[1]) == -1)
		perror("close5");
	while(wait(NULL) != -1 );
}
void re1(char **command1,char **command2,int fdr){
	switch(fork()){
		case -1: perror("fork error");
			 break;
		case 0:
			 fdr = open(command2[0],O_WRONLY | O_CREAT | O_TRUNC, 0644);
			 if(fdr == -1){
				 perror("Can't make new file");
				 exit(1);
			 }

			 if(dup2(fdr,1) == -1)
				 perror("fdr dup error");

			 close(fdr);

			 execvp(command1[0],command1);
			 printf("Command not found\n");
			 exit(0);
			 break;

		default:
			 wait(NULL);
	}
}
void re2(char **command1,char **command2,int fdr){
	switch(fork()){
		case -1: perror("fork error");
			 break;
		case 0:
			 fdr = open(command1[0],O_WRONLY | O_CREAT | O_TRUNC,0644);

			 if(fdr == -1) {
				 perror("Create error");
			 }
			 close(fdr);
			 execvp(command2[0],command2);
			 printf("Command not fount\n");
			 exit(0);
			 break;
		default:
			 wait(NULL);
	}

}
void def(char **command1){
	if (fork() == 0) {
		execvp(command1[0], command1);
		exit(0);
	}
}

