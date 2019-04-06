/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
    I hereby claims all copyright interes in the program “blackstar.c”
    Written by Jon Cox at cloneozone@gmail.com.
 
    You may compile this code with the following:
    
    gcc -O0 blackstar.c -o blackstar -lpthread -s */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <pthread.h>

/* counter for threads */
int count = 0;

/* perform_work function args struct */
typedef struct {
    char *arg_1;
} arg_struct;

int usage(){
	printf("\nUsage: ./blackstar [flags] [arguments]\n"
            "-f  Path to file of hosts.\n"
            "-l  Host: ip or domain.\n"
            "-u  Username: should have sudo, but not enforced.\n"
            "-p  Port: if different that 22 for SSH.\n"
	    "-w  Password for user.\n"
	    "-d  Decrypt password file made with ccrypt.\n"
            "-e  Commands to be executed. Use quotes for multiple.\n"
	    "-o  Skip host checking\n"
	    "-h  Usage text.\n\n");
	return 1;
}

/* function to execute in every thread */
void *perform_work(void *argument){
   arg_struct *actual_args = argument;
   system(actual_args->arg_1);
   ++count;

   return NULL;
}

int file_open(char *name, char *comm){
    int lines_allocated = 100;
    int	max_line_len = 100;

    /* allocate lines of text */
    char **words = (char **)malloc(sizeof(char*)*lines_allocated);
    if (words==NULL) {
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }

    FILE *fp = fopen(name, "r");
    int i;
    for (i=0;1;i++) {
        int j;

        /* have we gone over our line allocation? */
        if (i >= lines_allocated) {
            int new_size;

            /* Double our allocation and re-allocate */
            new_size = lines_allocated*2;
            words = (char **)realloc(words,sizeof(char*)*new_size);
            if (words==NULL){
                fprintf(stderr,"Out of memory.\n");
                exit(3);
            }
            lines_allocated = new_size;
        }

        /* allocate space for the next line */
        words[i] = malloc(max_line_len);
        if (words[i]==NULL){
            fprintf(stderr,"Out of memory (3).\n");
            exit(4);
        }
        if (fgets(words[i],max_line_len-1,fp)==NULL)
            break;

        /* get rid of CR or LF at end of line */
        for (j=strlen(words[i])-1;j>=0 && (words[i][j]=='\n' || words[i][j]=='\r');j--)
            ;
        words[i][j+1]='\0';
    }
    fclose(fp);

    pthread_t threads[i];
    int result_code, index;
    int *ptr[i];

    /* create all threads one by one */
    arg_struct *args = malloc(sizeof(arg_struct)*i);
    for (index = 0; index < i; ++index) { 
        args[index].arg_1 = malloc((sizeof(words) + sizeof(comm))*i);    
        strcpy(args[index].arg_1, words[index]);
	strcat(args[index].arg_1, comm);
        result_code = pthread_create(&threads[index], NULL, perform_work, &args[index]);
        assert(0 == result_code);
    }

    /* wait for each thread to complete */
    for (index = 0; index < i; ++index) {
      // block until thread 'index' completes
      result_code = pthread_join(threads[index], (void**)&(ptr[index]));
      assert(0 == result_code);
    }

    /* free memory */
    for (;i>=0;i--)
        free(words[i]);
    free(words);

    return 0;
}

int main (int argc, char **argv) {
  int opt, fflag, lflag, uflag, wflag, pflag, eflag, oflag, dflag = 0;
  char *user = NULL;
  char *host = NULL;
  char *pass = NULL;  
  char *sshpass = NULL;
  char *port = NULL; 
  char *dcrypt = NULL;
  char *ccrypt = NULL; 
  char *input = NULL;
  char *exec = NULL;

while(optind < argc) {
  if(( opt = getopt(argc, argv, "f:l:u:p:w:e:d:ho")) != -1){
   switch(opt){
     case 'f':
       fflag = 1;
       input=(char *)malloc(strlen(optarg));
       strcpy(input, optarg);
       break;
     case 'l':
       lflag = 1;
       host=(char *)malloc(strlen(optarg));
       strcpy(host, optarg);
       break;
     case 'u':
       uflag = 1;
       user=(char *)malloc(strlen(optarg));
       strcpy(user, optarg);
       break;
     case 'p':
       pflag = 1;
       port=(char *)malloc(strlen(optarg));
       strcpy(port, optarg);
       break;
     case 'w':
       wflag = 1;
       pass=(char *)malloc(strlen(optarg));
       strcpy(pass, optarg);
       break;
     case 'd':
       dflag = 1;
       dcrypt=(char *)malloc(strlen(optarg));
       strcpy(dcrypt, optarg);
       break;
     case 'e':
       eflag = 1;
       exec=(char *)malloc(strlen(optarg));
       strcpy(exec, " ");
       strcat(exec, optarg);
       break;
     case 'o':
       oflag = 1;
       break;
     case 'h':
       usage();
       break;
     default:
      usage();
      break;
     }
   }
   else {
      optind++;
   }
}

   if(dflag == 1){
	ccrypt=(char *)malloc(strlen(dcrypt) + 1);   
	strcpy(ccrypt, "ccrypt -d ");
	strcat(ccrypt, dcrypt);
	system(ccrypt);
   }

   if (fflag == 1 && eflag == 1){ 
	file_open(input, exec);
   }
   else if(fflag == 0 && lflag == 1 && uflag == 1 && eflag == 1){
	sshpass=(char *)malloc((sizeof(host) + sizeof(user) + sizeof(exec) + sizeof(port) + sizeof(pass))*4);
	if(wflag == 1){
		strcpy(sshpass, "sshpass -p '");
		strcat(sshpass, pass);
		strcat(sshpass, "' ");
	}
	strcat(sshpass, "ssh ");
	if(oflag == 1){
		strcat(sshpass, "-o StrictHostKeyChecking=no ");
	}
	strcat(sshpass, "-T ");
	strcat(sshpass, user);
	strcat(sshpass, "@");
	strcat(sshpass, host);
	strcat(sshpass, " '");
	strcat(sshpass, exec);
	strcat(sshpass, "'");
	if(pflag == 1){
		strcat(sshpass, " -p ");
		strcat(sshpass, port);
	}
	//system(sshpass);
	printf("%s\n", sshpass);
   }
   else{
	usage();
   }	   

   if(dflag == 1){ 
	printf("*File was decrypted for usage. Now re-encrypt it.*\n");
	dcrypt[strlen(dcrypt)-4] = 0;
	strcpy(ccrypt, "ccrypt ");
        strcat(ccrypt, dcrypt);	
	system(ccrypt);	
   }

   if(!sshpass){free(sshpass);} 
   if(!host){free(host);} 
   if(!user){free(user);} 
   if(!exec){free(exec);} 
   if(!pass){free(pass);} 
   if(!port){free(port);} 
   if(!ccrypt){free(ccrypt);} 
   if(!dcrypt){free(dcrypt);}  

  return 0;
}
