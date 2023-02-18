#include "dns.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF 1024

int resolv_udp(const struct request r, void *buffer, size_t *len)
{
    int sockfd;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    int result = getaddrinfo(r.addr, "53", &hints, &res);
    if (result != 0)
    {
        return -1;
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("Cannot create socket.");
        return -1;
    }
    unsigned int n;
    struct question q;
    q.qname = r.qname;
    q.qtype = r.type;
    q.qclass = 1;
    void *buf;
    buf = malloc(1024);
    if (buf == NULL)
    {
        perror("Cannot allocate memory");
        return -1;
    }
    int s = create_request(&q, buf, 1024);
    result = sendto(sockfd, buf, s, MSG_CONFIRM, res->ai_addr, res->ai_addrlen);
    free(buf);
    if (result == -1)
    {
        perror("Cannot send data to socket");
        return -1;
    }

    *len = recvfrom(sockfd, buffer, BUF, MSG_WAITALL, res->ai_addr, &n);
    close(sockfd);

    return 0;
}