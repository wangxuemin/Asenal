#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <event.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <arpa/inet.h>
#include "xdebug.h"
typedef struct sockaddr SA;
#define MAXLINE 1024

uint64_t NowMicros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int     n;
    int cnt = 2000;
    int len = 500;
    char **strs = (char **)malloc(sizeof(char *) * cnt);
    for (int i = 0; i < cnt; i++) {
        strs[i] = (char *)malloc(sizeof(char) * len + 1);
    }
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < len; j++) {
            strs[i][j] = (random() % 25) + 'a';
        }
        strs[i][len] = '\0';
    }
    uint64_t st, ed;
    st = NowMicros();
    int totn = 0;
    int nwrite;
    
    int ctime = 10;
    while (ctime--) {
        for (int i = 0; i < cnt; i++) {
            nwrite = sendto(sockfd, strs[i], strlen(strs[i]), 0, pservaddr, servlen);
            totn += nwrite;
        }
        log_info("totn %d", totn);
        sleep(1);
    }
    ed = NowMicros();
    printf("cost time %lld\n", ed - st);
    printf("totn %d", totn);
}

int main(int argc, char **argv)
{

    int     sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2) {
        return -1;
        // err_quit("usage: udpcli <IPaddress>");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9877);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int default_buf;
    socklen_t optlen = sizeof(default_buf);

    int res = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &default_buf, &optlen);
    log_info("default_buf %d", default_buf);

    // int buflen = 3 * 1024 * 1024;
    // if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &buflen, sizeof(buflen)) == -1) {
    //     log_err("set socket opt error");
    // }

    // if (setsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT, &buflen, sizeof(buflen)) == -1) {
    //     log_err("set socket opt error");
    // }

    dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));

    return 0;
}
