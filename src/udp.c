#include "dns.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF 1024

int resolv_udp(const struct request r, struct response* rsp)
{
    char* buffer = malloc(sizeof(char) * 4096);
    if (buffer == NULL) {
        perror("Cannot allocate memory");
        return -1;
    }
    int sockfd;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    int result = getaddrinfo(r.addr, "53", &hints, &res);
    if (result != 0) {
        free(buffer);
        return -1;
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror(strerror(errno));
        free(buffer);
        return -1;
    }
    int n;
    unsigned int len;
    struct question q;
    q.qname = r.qname;
    q.qtype = r.type;
    q.qclass = r.class;

    void* buf;
    buf = malloc(1024);
    if (buf == NULL) {
        perror("Cannot allocate memory");
        free(buffer);
        close(sockfd);
        return -1;
    }
    int s = create_request(&q, buf, 1024);
    result = sendto(sockfd, buf, s, MSG_CONFIRM, res->ai_addr, res->ai_addrlen);
    if (result == -1) {
        perror(strerror(errno));
        free(buf);
        free(buffer);
        close(sockfd);
        return -1;
    }

    free(buf);

    n = recvfrom(sockfd, (char*)buffer, BUF, MSG_WAITALL, res->ai_addr, &len);
    close(sockfd);

    *rsp = get_response(buffer, n);
    free(buffer);
    return 0;
}
