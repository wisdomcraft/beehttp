
extern char *pidfile;
extern unsigned int debug;


int start(void){
    int pid = getpid();
    FILE *fstream;
    fstream = fopen(pidfile,"w");
    if(fstream == NULL){
        if(debug == 1) printf("open pid file '%s' failed, program cannot write pid into this file\r\n", pidfile);
        return -1;
    }
    fprintf(fstream,"%d",pid);
    fclose(fstream);
    
    return 0;
}


void exitStartMode(int s){
    if(s == SIGINT){
        printf("  exit seccessfully!\r\n");
        remove(pidfile);
        exit(0);
    }
}


