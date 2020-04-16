
extern char *pidfile;


//success stop 0, error stop -1
int stop(void){
    if(access(pidfile, 0) == -1){
        printf("there is not exists pid file, the process maybe is not running\r\n");
        printf("You can run linux command to check and kill process by yourself\r\n");
        printf("    '# /usr/bin/ps aux | grep beehttp' or '# lsfo -i:8081'\r\n");
        printf("    '# kill [pid]'\r\n\r\n");
        return -1;
    }
    
    int pid = pidFromPidfile();
    
    if(kill(pid, 0) == 0){
        if(kill(pid, SIGKILL) == 0){
            remove(pidfile);
            printf("the process is stopped\r\n\r\n");
            return 0;
        }else{
            printf("the process stop failed, pid: %d\r\n", pid);
            printf("You can run linux command to check and kill process by yourself\r\n");
            printf("    '# /usr/bin/ps aux | grep beehttp' or '# lsfo -i:8081'\r\n");
            printf("    '# kill [pid]'\r\n\r\n");
        }
    }else{
        remove(pidfile);
        printf("the process is not running\r\n");
        printf("You can run linux command to check and kill process by yourself\r\n");
        printf("    '# /usr/bin/ps aux | grep beehttp' or '# lsfo -i:8081'\r\n");
        printf("    '# kill [pid]'\r\n\r\n");
    }
    
    return -1;
}



