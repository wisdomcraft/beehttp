#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include "cJSON.h"
#include "cJSON.c"
#include <bson.h>
#include <bcon.h>
#include <mongoc.h>
#include "log.c"
#include "argument.c"
#include "runningcheck.c"
#include "start.c"
#include "status.c"
#include "stop.c"
#include "urldecode.c"
#include "mongodb.c"
#include "receive.c"
#include "server.c"


/*
* stdio.h
* printf(),fprintf(),fopen(),fwrite(),fseek(),rewind(),fread(),fclose(),remove()
*
* string.h
* strcmp(),strstr(),strerror(),strlen(),strcat(),strcpy()
*
* unistd.h
* getpid(),fork(),access()
*
* signal.h
* signal(),kill()
*
* stdlib.h
* exit(),malloc(),realloc(),free(),atoi(),system()
*
* sys/socket.h
* socket(),bind(),accept()
*
* netinet/in.h
* struct sockaddr_in, htons(), INADDR_ANY
*
* errno.h
* errno
*
* wait.h
* waitpid()
* 
* cJSON.h, cJSON.c
* json
*
* bson.h, bcon.h, mongoc.h
* to connect MongoDB serer
*/


//----------------------------------------------------------------


/*
*global variable
*/
unsigned int debug;
char *pidfile = "../tmp/master.pid";


//----------------------------------------------------------------


/*
* all functions
*/

//log.c
char *mylogName(char *type);
int mylog(char *mytype, char *description);

//argument.c
int argument(int argc, char **argv);

//runningcheck.c
int pidFromPidfile(void);
int runningCheck(void);

//start.c
int start();
void exitStartMode(int s);

//status.c
int status();

//stop.c
int stop();

//urldecode.c
int isDigit(unsigned char ch);
unsigned char character_to_hex(unsigned char highByte, unsigned char lowByte);
char *urldecode(char *str);

//mongodb.c
char *collectionName(void);
int cacheToMongodb(char *sn, char *update_json, char *insert_json);

//receive.c
int receiveHandle(char *receive, char *client_address);

//server.c
int methodCheck(char *receive);
int server(void);

//main.c
int main(int argc, char **argv);


//----------------------------------------------------------------


/*
* main function
*/
int main(int argc, char **argv){
    if(argument(argc, argv) == -1) return -1;
    
    if(strcmp(argv[1], "start")==0){
        if(runningCheck() == -1){
            printf("the process is running in debug or daemon mode, pid: %d\r\n", pidFromPidfile());
            printf("You can also run linux command to check by yourself\r\n");
            printf("    '# /usr/bin/ps aux | grep beehttp' or '# lsfo -i:8081'\r\n\r\n");
            exit(0);
        }
        debug = 1;
        if(start() == -1)   return 0;
        signal(SIGINT, exitStartMode);
        if(server() == -1)  return 0;
    }
    
    if(strcmp(argv[1], "daemon")==0){
        if(runningCheck() == -1){
            printf("the process is running in debug or daemon mode, pid: %d\r\n", pidFromPidfile());
            printf("You can also run linux command to check by yourself\r\n");
            printf("    '# /usr/bin/ps aux | grep beehttp' or '# lsfo -i:8081'\r\n\r\n");
            exit(0);
        }
        
        int fpid = fork();
        if(fpid == 0){
            if(start() == -1)   return 0;
            if(server() == -1)  return 0;
        }else if(fpid > 0){
            printf("successfully, the process start running, pid: %d\r\n\r\n", fpid);
            exit(0);
        }else{
            printf("error, fork process failed in main.c #126\r\n");
            exit(0);
        }
    }
    
    if(strcmp(argv[1], "status")==0){
        status();
    }
    
    if(strcmp(argv[1], "stop")==0){
        stop();
    }
    
    if(strcmp(argv[1], "restart")==0){
        if(stop() == -1){
            printf("error, restart program failed!\r\n\r\n");
            exit(0);
        }

        system("./beehttp daemon");
    }
    
    return 0;
}


