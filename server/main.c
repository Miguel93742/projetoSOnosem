/*#include "unix.h"*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include "lib/hash.h"
#include "fs.h"
#include "sync.h"
#include <sys/socket.h>

#define MAX_COMMANDS 10
#define MAX_INPUT_SIZE 100 

#define EXIT_ORDER "x\n"

#define MAXLINE 512/* Servidordo tiposocket stream. Reenviaas linhasrecebidaspara o cliente*/


//Variaveis com numero produtor/consumidor
int prod = 0, cons = 0;

FILE * output;

struct timeval t0, t1;

tecnicofs* fs;

pthread_mutex_t consMutex;
pthread_mutex_t prodMutex;

int flagProd = 0;


char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];


static void displayUsage (const char* appName){
    printf("Usage: %s\n", appName);
    exit(EXIT_FAILURE);
}


static void parseArgs (long argc, char* const argv[]){
    
    if (argc != 4){
        fprintf(stderr, "Invalid format:\n");
        displayUsage(argv[0]);
    }
    else if (atoi(argv[3]) <= 0){
        fprintf(stderr, "Invalid format:\n");
        displayUsage(argv[0]);
    }

    else if((output = fopen(argv[2], "w")) == NULL){
        fprintf(stderr, "Invalid format:\n");
        displayUsage(argv[0]);
    }
}


void serverCreate(){

    int sockfd, servlen;

    struct sockaddr_un cli_addr, serv_addr;
    /* Criasocket stream */
    if ((sockfd= socket(AF_UNIX,SOCK_STREAM,0)) < 0)
        err_dump("server: can't open stream socket");
    
    //Eliminaonome,paraocasodejáexistir.
    unlink(UNIXSTR_PATH);
    /* O nomeserve para queosclientespossamidentificaro servidor*/
    bzero((char *)&serv_addr, sizeof(serv_addr));
    
    serv_addr.sun_family= AF_UNIX;
    
    strcpy(serv_addr.sun_path, UNIXSTR_PATH);
    
    servlen= strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    
    if (bind(sockfd, (structsockaddr*) &serv_addr, servlen)< 0)
        err_dump("server, can't bind local address");
    

    listen(sockfd, 5);
}

int makeConnection(int *sockfd){

    int newsockfd, clilen, childpid;

    for(;;){

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr*)&cli_addr, &clilen);
    if(newsockfd<0)
        err_dump("server:accepterror");/*Lançaprocessofilhoparatratardocliente*/

    if((childpid=fork())<0)
        err_dump("server:forkerror");

    else if(childpid==0){/*Processofilho.FechasockfdjáquenãoéutilizadopeloprocessofilhoOsdadosrecebidosdoclientesãoreenviadosparaocliente*/
        close(sockfd);
        str_echo(newsockfd);
        exit(0);
    }/*Processopai.Fechanewsockfdquenãoutiliza*/
    close(newsockfd);
    }
}


int str_echo(intsockfd){
    
    int n;
    char line[MAXLINE];

    for (;;) {/* Lêumalinhado socket */
    n = readline(sockfd, line, MAXLINE);
    if (n == 0)
        return;
    else if (n < 0)
        err_dump("str_echo: readlineerror");/*Reenviaalinhaparaosocket.ncontacomo\0dastring,casocontrárioperdia-sesempreumcaracter!*/
    
    if (write(sockfd,line,n) != n)
        err_dump("str_echo:writeerror");
    }
}




void insertCommand(char* data) {
/*

Params:char* data (Contains commands from the input file)
Return:void
Description:Using semaphores this is the producer fucntion 
that inserts the parameters of the function in the vector 
containing the commands.

*/
    lockMutex(&prodMutex);

    strcpy(inputCommands[prod], data);
    prod = (prod + 1) % MAX_COMMANDS;

    unlockMutex(&prodMutex);

}


char* removeCommand() {
    char *res;

    res = inputCommands[cons];
    if (strcmp(res, EXIT_ORDER) !=  0)
        cons = (cons + 1) % MAX_COMMANDS;
    return res;  
}


void errorParse(){
    fprintf(stderr,"Error: command invalid\n");
    exit(EXIT_FAILURE);
}


/*

Params:NONE
Return:Void
Descripiton: In this function we implemented a semaphore in such a way
its possible to insert commands in the vector of commands and consume them 
at the same time. The semaphore is implemented in the insertCommand function

*/
void * processInput(){
    
    char line[MAX_INPUT_SIZE];
    // Just change stdin in fgets to input wich is the FILE
    while (fgets(line, sizeof(line)/sizeof(char), stdin)) { ////////////////////////////////////////////////////NOT STDIN but process from SOCKET

        char token;
        char name[MAX_INPUT_SIZE];
        char nameR[MAX_INPUT_SIZE];

        int numTokens = sscanf(line, "%c %s %s", &token, name, nameR);

        /* perform minimal validation */
        if (numTokens < 1) {
            continue;
        }
        switch (token) {
            case 'c':
            case 'l':
            case 'd':
            case 'r':
                if(numTokens > 3)
                    errorParse();
                //Because theres no limit as far as what fits in the vector no verification is required
                insertCommand(line);
                break;
            case '#':
                break;
            default: { /* error */
                errorParse();
            }
        }
    }

    insertCommand(EXIT_ORDER); //EXIT_ORDER is defined in the beginning of the code and its a command to signal that the commads are all 'produced'
    return NULL;
}



void *applyCommands(){

    while(1){

        lockMutex(&consMutex);

        const char* command = removeCommand();
        
        if (strcmp(command, EXIT_ORDER) == 0){
            unlockMutex(&consMutex);
            return NULL;
        }

        if (command == NULL)
            unlockMutex(&consMutex);
        
        char token;
        char name[MAX_INPUT_SIZE];
        char nameR[MAX_INPUT_SIZE];
        
        int numTokens = sscanf(command, "%c %s %s", &token, name, nameR);
        
        if (numTokens >  3) {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }
        int searchResult;
        int iNumber;

        switch (token) {
            case 'c':

                iNumber = obtainNewInumber(fs);
                unlockMutex(&consMutex);

                create(fs, name, iNumber, NOT_REN);

                break;
            case 'l':
                unlockMutex(&consMutex);
                
                searchResult = lookup(fs, name, NOT_REN);

                if(!searchResult)
                    printf("%s not found\n", name);
                else
                    printf("%s found with inumber %d\n", name, searchResult);

                break;
            case 'd':
                unlockMutex(&consMutex);

                delete(fs, name, NOT_REN); 
    
                break;
            case 'r':
                unlockMutex(&consMutex);

                renameFile(fs, name, nameR);

                break;

            default: { /* error */

                fprintf(stderr, "Error: command to apply\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return NULL;
}


void printTime(struct timeval ti, struct timeval tf){

    //Suming the seconds plus the milliseconds
    double timeSpent =  (tf.tv_sec + tf.tv_usec * pow(10, -6)) - (ti.tv_sec + ti.tv_usec * pow(10, -6));
    
    printf("TecnicoFS completed in %0.4f seconds.\n", timeSpent);
    
}


int main(int argc, char* argv[]) {
    
    parseArgs(argc, argv);

    fs = new_tecnicofs(atoi(argv[3]));
      
    print_tecnicofs_tree(output, fs);
    
    if (fclose(output) != 0)
        exit(EXIT_FAILURE);

    free_tecnicofs(fs);
    
    printTime(t0, t1);
    
    exit(EXIT_SUCCESS);
}
