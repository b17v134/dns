#include "dns.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define BUF 1024

struct header create_header()
{
    srand(time(NULL));

    struct header hdr = {};
    memset(&hdr, 0, sizeof(struct header));
    hdr.id = rand() % 0b1111111111111111;
    hdr.qr = 1;
    hdr.rd = 1;
    hdr.qdcount = 1;
    hdr.ancount = 0;
    hdr.nscount = 0;
    hdr.arcount = 0;

    return hdr;
}

uint8_t create_request(struct question *question, void *buf, uint16_t buf_size)
{
    struct header r = create_header();
    int header_length = write_header(buf, r);
    int length = write_question(buf + header_length, question);
    return header_length + length;
}

int resolv(const struct request r, char *buffer)
{
    int sockfd;
    struct sockaddr_in servaddr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(r.port);
    servaddr.sin_addr.s_addr = inet_addr(r.addr);

    int n, len;
    struct question q;
    q.qname = r.qname;
    q.qtype = r.type;
    q.qclass = 1;
    void *buf;
    buf = malloc(1024);
    if (buf == NULL)
    {
        perror("Cannot allocate memory");
        exit(1);
    }
    int s = create_request(&q, buf, 1024);
    size_t result = sendto(sockfd, buf, s, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    free(buf);

    n = recvfrom(sockfd, (char *)buffer, BUF, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';
    close(sockfd);
    return n;
}

struct response get_response(void *buffer, int len)
{
    struct response result = {0};

    read_header(buffer, &(result.hdr));

    int pos = 12;
    result.questions = malloc(sizeof(struct question) * result.hdr.qdcount);
    for (int i = 0; i < result.hdr.qdcount; i++)
    {
        pos = read_question(buffer, pos, &result.questions[i]);
    }
    result.answers = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.ancount);
    for (int i = 0; i < result.hdr.ancount; i++)
    {
        pos = read_resource_record(buffer, pos, &(result.answers[i]));
    }

    result.authority_records = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.nscount);
    for (int i = 0; i < result.hdr.nscount; i++)
    {
        pos = read_resource_record(buffer, pos, &(result.authority_records[i]));
    }

    result.additional_records = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.arcount);
    for (int i = 0; i < result.hdr.arcount; i++)
    {
        pos = read_resource_record(buffer, pos, &(result.additional_records[i]));
    }

    return result;
}
