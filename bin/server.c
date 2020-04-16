
extern unsigned int debug;


//check http method, 0:get or error, 1:post
int methodCheck(char *receive){
    if(strlen(receive)>4 && strstr(receive, "POST")==NULL)  return 0;
    
    if(strstr(receive, "POST") == NULL)                     return 0;
    if(strcmp(receive, strstr(receive, "POST")) == 0)       return 1;
    
    return 0;
}


int server(void){
    int sServer;
    
    if((sServer = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        if(debug == 1) printf("socket error!\r\n");
        mylog("error", "socket error!\r\n");
        return -1;
    }
    
    int port = 8081;
    struct sockaddr_in sin;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    if(bind(sServer, (struct sockaddr*)&sin, sizeof(struct sockaddr)) == -1){
        if(debug == 1) printf("bind error!\r\n");
        mylog("error", "bind error!\r\n");
        close(sServer);
        return -1;
    }
    
    if(listen(sServer, 5) == -1){
        if(debug == 1) printf("listen error!\r\n");
        mylog("error", "listen error!\r\n");
        close(sServer);
        return -1;
    }
    
    int client = -1;
    struct sockaddr_in remoteAddr;
    socklen_t nAddrlen = sizeof(remoteAddr);
    while(1){
        if(client<1) client = accept(sServer, (struct sockaddr*)&remoteAddr, &nAddrlen);
        
        if(client < 1){
            if(debug == 1) printf("accept error!\r\n");
            mylog("error", "accept error!\r\n");
            if(errno != 0){
                printf("program exit, server.c #99, error: %d, %s\r\n", errno, strerror(errno));
                break;
            }else{
                continue;
            }
        }
        
        int fpid = fork();
        if(fpid == 0){
            if(debug == 1) printf("client: %d, childpid: %d\r\n", client, getpid());
            
            int fork_client     = -1;
            fork_client         = client;
            client              = -1;
            while(1){
                char *client_address = inet_ntoa(remoteAddr.sin_addr);
                
                char receive_each[1024];
                int  length;
                char *receive       = (char*)malloc(1);
                strcpy(receive, "");
                
                int receive_error   = 0;
                int loop_times      = 1;
                
                while((length = recv(fork_client,receive_each,1024,0)) > 0){
                    receive_each[length] = '\0';
                    if(debug == 1) printf("\r\nreceive_each:\r\n%s\r\n", receive_each);
                    
                    receive = realloc(receive, strlen(receive_each)+strlen(receive)+1);
                    strcat(receive, receive_each);
                    
                    memset(receive_each, 0, sizeof(receive_each));
                    
                    if(methodCheck(receive) == 0){
                        receive_error = 1;
                        break;
                    }
                    
                    if(strstr(receive, "Content-Length:") != NULL) break;
                    if(strstr(receive, "Content-Length:")==NULL && loop_times>=2){
                        receive_error = 2;
                        break;
                    }
                    loop_times++;
                }
                if(debug == 1) printf("\r\nreceive:\r\n%s\r\n", receive);
                
                if(receive_error == 1){
                    char *response = "HTTP/1.1 200 OK\r\nContent-Length: 59\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n{\"status\":\"error\",\"description\":\"only support post method\"}";
                    send(fork_client, response, strlen(response), 0);
                    if(debug == 1) printf("\r\n----------------------------------------\r\n");
                }else if(receive_error == 2){
                    char *response = "HTTP/1.1 200 OK\r\nContent-Length: 137\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n{\"status\":\"error\",\"description\":\"request information from client is not complete, it is better to send it only by one tcp data package!\"}";
                    send(fork_client, response, strlen(response), 0);
                    if(debug == 1) printf("\r\n----------------------------------------\r\n");
                }else{
                    char *response = "HTTP/1.1 200 OK\r\nContent-Length: 60\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n{\"status\":\"success\",\"description\":\"server received message\"}";
                    send(fork_client, response, strlen(response), 0);
                    receiveHandle(receive, client_address);
                }
                
                close(fork_client);
                fork_client = -1;
                exit(0);
            }
        }else if(fpid > 0){
            close(client);
            client          = -1;
            waitpid(-1, NULL, WNOHANG);
        }else{
            if(debug == 1) printf("fork error, process exit, server.c #137, %d, %s\r\n", errno, strerror(errno));
            close(client);
            client = -1;
            break;
        }

    }
    
    close(sServer);
    
    return 0;
}




