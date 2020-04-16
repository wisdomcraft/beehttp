
extern unsigned int debug;


char *mylogName(char *type){
    time_t now;
    struct tm *p;
    time(&now);
    p = localtime(&now);
    
    char year[8];
    sprintf(year, "%d", (1900+p->tm_year));
    char month[2];
    if((p->tm_mon) < 9){
        sprintf(month, "0%d", (1+p->tm_mon));
    }else{
        sprintf(month, "%d", (1+p->tm_mon));
    }
    char day[2];
    if((p->tm_mday) < 10){
        sprintf(day, "0%d", (p->tm_mday));
    }else{
        sprintf(day, "%d", (p->tm_mday));
    }
    
    strcat(year, month);
    strcat(year, day);
    
    char *filename = (char*)malloc(9);
    strcpy(filename, "../log/");
    if(realloc(filename, (strlen(type)+strlen(year)+strlen(filename)+strlen(".log"))) == NULL){
        if(debug == 1) printf("error, realloc() failed in log.c #32");
        exit(0);
    }
    strcat(filename, type);
    strcat(filename, "_");
    strcat(filename, year);
    strcat(filename, ".log");
    
    return filename;
}


int mylog(char *mytype, char *description){
    char *log_name = mylogName(mytype);
    
    FILE *fstream;
    fstream = fopen(log_name,"a+");
    if(fstream == NULL){
        if(debug == 1) printf("error, create or open file '%s' failed in log.c #50\r\n", log_name);
        return -1;
    }
    int write_result = fwrite(description, 1, strlen(description), fstream);
    fwrite("\r\n",1,strlen("\r\n"),fstream);
    fclose(fstream);
    
    if(write_result < 1){
        if(debug == 1){
            printf("error, write data into '%s' failed in log.c #58\r\n", log_name);
            printf("error, log.c #58, the needed writed content: %s\r\n", description);
        }
        return -1;
    }
    
    return 0;
}



