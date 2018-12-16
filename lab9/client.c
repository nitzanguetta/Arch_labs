/*
** client.c -- a stream socket client demo
*/
#include "line_parser.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "2018"//"3490" // the port client will be connecting to 

#define MAXDATASIZE LS_RESP_SIZE // max number of bytes we can get at once 


client_state _client_state;
c_state _c_state;
struct addrinfo *p;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int IS_QUIT(char* input) {return !strcmp("quit\n",input);}
int IS_NL(char* input) {return !strcmp("\n",input);}
int IS_CONN(char* input) {return !strcmp("conn",input);}
int IS_BYE(char* input) {return !strcmp("bye",input);}
int IS_HELLO(char* input) {return !strcmp("hello",input);}
int IS_LS(char* input) {return !strcmp("ls",input);}
int IS_NOK(char* input) {return !strcmp("nok",input);}
int IS_OK(char* input) {return !strcmp("ok",input);}

int conn(char* srv_addr){
    if(_client_state.conn_state==IDLE){
        _client_state.conn_state=CONNECTING;

        int sockfd, numbytes; 
        struct addrinfo hints, *servinfo;
        int rv;
        char s[INET6_ADDRSTRLEN];
        char buf[MAXDATASIZE];

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo(srv_addr, PORT, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            _client_state.conn_state=IDLE;
            return 1;
        }

        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("client: connect");
                continue;
            }

            break;
        }

        if (p == NULL) {
            _client_state.conn_state=IDLE;
            fprintf(stderr, "client: failed to connect\n");
            return 2;
        }

        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                s, sizeof s);
        printf("client: connecting to %s\n", s);

        freeaddrinfo(servinfo); // all done with this structure
         if ((numbytes = sendto(sockfd, "hello\n", strlen("hello\n"), 0,p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            _client_state.conn_state=IDLE;
            exit(1);
        }


        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            _client_state.conn_state=IDLE;
            exit(1);
        }

        buf[numbytes] = '\0';

        cmd_line *cmd_parse;
        cmd_parse = parse_cmd_lines(buf);

        if(IS_HELLO(cmd_parse->arguments[0])){
            if(cmd_parse->arguments[1]){
                _client_state.server_addr=malloc(sizeof(srv_addr));
                _client_state.server_addr=strcpy(_client_state.server_addr,srv_addr);
                _client_state.conn_state=CONNECTED;
                _client_state.client_id=cmd_parse->arguments[1];
                _client_state.sock_fd=sockfd;
            }
        }

        free_cmd_lines(cmd_parse);
        return 0;
    }
    else return -2;
}
void reset_client_state(){
    _client_state.server_addr="nil";
    _client_state.conn_state=IDLE;
    _client_state.client_id=NULL;
    _client_state.sock_fd=-1;
}

int bye(){
    if(_client_state.conn_state==CONNECTED){
        int numbytes; 
        if ((numbytes = sendto(_client_state.sock_fd, "bye\n", strlen("bye\n"), 0,p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        
        reset_client_state();
        close(_client_state.sock_fd);
        return 0;
    }else
        return -2;
}

int ls(char* cmd){
    if(_client_state.conn_state==CONNECTED){
        int numbytes;
        char buf[MAXDATASIZE]; 
        if ((numbytes = sendto(_client_state.sock_fd, cmd, strlen(cmd), 0,p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        if ((numbytes = recv(_client_state.sock_fd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            _client_state.conn_state=IDLE;
            exit(1);
        }
        cmd_line *cmd_parse;
        cmd_parse = parse_cmd_lines(buf);
        if(IS_NOK(cmd_parse->arguments[0])){
            printf("Server Error: %s\n",buf+4);
            bye();
        }else if(IS_OK(cmd_parse->arguments[0])){
            printf("%s", buf+3);
        }
        free_cmd_lines(cmd_parse);
        return 0;
    }else
        return -2;
}

void exec(cmd_line *line, char * buf){
    if(IS_CONN(line->arguments[0]))     conn(line->arguments[1]);
    else if(IS_BYE(line->arguments[0])) bye();
    else if(IS_LS(line->arguments[0])) ls(buf);
}



int main(int argc, char *argv[])
{
    reset_client_state();

    char buf[MAXDATASIZE];
    cmd_line *cmd_parse;
    
    while(1){
        printf("server:%s>", _client_state.server_addr);
        fgets(buf, MAXDATASIZE, stdin);
        if(IS_QUIT(buf)) break;
        if(IS_NL(buf)) continue;
        cmd_parse = parse_cmd_lines(buf);
        exec(cmd_parse,buf);
        free_cmd_lines(cmd_parse);
    }
    return 0;
}
