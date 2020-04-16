

int argument(int argc, char **argv){
    if(argc == 1){
        printf("error, lose parameter!\r\n");
        printf("start       start program as debug mode\r\n");
        printf("daemon      start program as daemon mode\r\n");
        printf("status      view program running status\r\n");
        printf("stop        stop program in daemon mode\r\n");
        printf("restart     stop program and restart it in daemon mode\r\n\r\n");
        printf("current beeServer version: 20180129\r\n");
        printf("beeServer is a TCP socket server for cloud plat.\r\n\r\n");
        return -1;
    }
    
    if(argc != 2){
        printf("error, number of program argument is wrong!\r\n");
        return -1;
    }

    if(strcmp(argv[1],"start")!=0 && strcmp(argv[1],"daemon")!=0 && strcmp(argv[1],"status")!=0 \
    && strcmp(argv[1],"stop")!=0 && strcmp(argv[1],"restart")!=0){
        printf("error, this porgram argument is wrong!\r\n");
        return -1;
    }
    
    return 0;
}


