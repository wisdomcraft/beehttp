
extern char *pidfile;


int status(void){
    if(access(pidfile, 0) == -1){
        printf("there is not exists pid file, the process maybe is not running\r\n");
        printf("You can run linux command to check by yourself\r\n");
        printf("    '# /usr/bin/ps aux | grep beehttp' or '# lsof -i:8081'\r\n\r\n");
        return 0;
    }
    
    int pid = pidFromPidfile();
    
    if(kill(pid, 0) == 0){
        printf("the process is running in debug or daemon mode, pid: %d\r\n\r\n", pid);
    }else{
        printf("the process is not running\r\n\r\n");
        printf("You can run linux command to check by yourself\r\n");
        printf("    '# /usr/bin/ps aux | grep beehttp' or '# lsof -i:8081'\r\n\r\n");
    }
    
    return 0;
}



