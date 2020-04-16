
extern char *pidfile;


int pidFromPidfile(void){
    FILE *fstream;
    fstream = fopen(pidfile, "r");
    if(fstream == NULL){
        if(debug == 1) printf("open pid file '%s' failed\r\n", pidfile);
        return -1;
    }
    
    int fileLength = 0;
    fseek(fstream, 0, SEEK_END);
    fileLength = ftell(fstream);
    rewind(fstream);

    char fileContent[fileLength];
    fread(fileContent, 1, fileLength, fstream);
    fileContent[fileLength] = '\0';
    
    fclose(fstream);
    
    int pid = atoi(fileContent);
    
    return pid;
}


//running -1, not running 0
int runningCheck(void){
    if(access(pidfile, 0) == -1)    return 0;
    int pid = pidFromPidfile();
    
    if(kill(pid, 0) == 0)           return -1;
    
    remove(pidfile);
    
    return 0;
}


