#include "dns.h"

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT_FORMAT_PLAIN 0
#define OUTPUT_FORMAT_JSON 1

const char* version = "0.0.1";

static int help_flag;
static int output_format = OUTPUT_FORMAT_PLAIN;
static int verbose_flag;
static int version_flag;
static enum protocol pr = udp;
static int class = DNS_CLASS_IN;

static uint16_t type = DNS_TYPE_A;
static char* server;
static int port = 53;
static char* ca = NULL;
static char* certificate = NULL;

static struct option long_options[] = {
    { "help", no_argument, &help_flag, 1 },
    { "server", required_argument, 0, 0 },
    { "type", required_argument, 0, 0 },
    { "verbose", no_argument, &verbose_flag, 1 },
    { "version", no_argument, &version_flag, 1 },
};

void print_version(void);
void print_usage(void);
void set_output_format(char* format);
void set_server(char* srv);
void set_port(char* port_arg);
void set_class(char* arg);
void set_protocol(char* port_arg);

int main(int argc, char* argv[])
{
    while (1) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "c:hvVs:t:p:r:A:E:o:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1) {
            break;
        }

        switch (c) {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case 'A':
            ca = strdup(optarg);
            break;
        case 'c':
            set_class(optarg);
            break;
        case 'E':
            certificate = strdup(optarg);
            break;
        case 'h':
            print_usage();
            break;
        case 'o':
            set_output_format(optarg);
            break;
        case 'p':
            set_port(optarg);
            break;
        case 'r':
            set_protocol(optarg);
            break;
        case 'v':
            verbose_flag = 1;
            break;
        case 'V':
            print_version();
            break;
        case 's':
            set_server(optarg);
            break;
        case 't':
            type = dns_type_to_int(optarg);
            break;
        default:
            puts("Error");
            return 1;
        }
    }

    if (help_flag) {
        print_usage();
    }

    if (verbose_flag)
        puts("verbose flag is set");

    if (version_flag) {
        print_version();
    }

    if (optind < argc) {
        while (optind < argc) {
            char* qname = argv[optind++];
            struct request r = { server, port, pr, qname, type, class, ca, certificate };
            char* buffer = malloc(sizeof(char) * 1024);
            if (buffer == NULL) {
                perror("Cannot allocate memory");
                exit(1);
            }
            struct response resp;
            int len = resolv(r, &resp);
            if (len != 0) {
                perror("error");
            } else {
                if (output_format == OUTPUT_FORMAT_PLAIN) {
                    print_response(resp);
                } else {
                    print_json_response(resp);
                }
                free_response(resp);
            }
        }
    }

    if (!server) {
        free(server);
    }
    return 0;
}

void print_version(void)
{
    printf("resolv %s\n", version);
    exit(0);
}

void print_usage(void)
{
    puts("Usage: resolv [options...] <value>\n\
 -A <file>                  CA certificate\n\
 -E, --cert <certificate>   Certificate\n\
 -c, --class                Query class [default: IN]\n\
 -h, --help                 Show help and exit\n\
 -s, --server <ip>          Server ip\n\
 -o  <output_format>        Formatting output (json, plain) [default: plain]\n\
 -p, --port <number>        Port number [default: 53]\n\
 -r, --protocol <name>      Protocol (https, tcp, tls, udp) [default: udp]\n\
 -t, --type                 Query type [default: A]\n\
 -v, --verbose              Verbose mode\n\
 -V, --version              Show version number and exit\n\
");
    exit(0);
}

void set_output_format(char* format)
{
    if (strcmp(format, "plain") == 0) {
        output_format = OUTPUT_FORMAT_PLAIN;
        return;
    }

    if (strcmp(format, "json") == 0) {
        output_format = OUTPUT_FORMAT_JSON;
        return;
    }

    perror("Incorrect output format.");
    exit(EXIT_FAILURE);
}

void set_server(char* srv)
{
    server = malloc(sizeof(char) + (strlen(optarg) + 1));
    if (!server) {
        perror("Cannot allocate memory");
        exit(1);
    }
    strcpy(server, optarg);
}

void set_port(char* port_arg)
{
    int p = atoi(port_arg);
    if ((p <= 0) || (p > 65535)) {
        perror("Incorrect port number.");
        exit(EXIT_FAILURE);
    }

    port = p;
}

void set_class(char* arg)
{
    class = dns_class_to_int(arg);
}

void set_protocol(char* port_arg)
{
    if (strcmp(port_arg, "https") == 0) {
        pr = https;
        return;
    }

    if (strcmp(port_arg, "tcp") == 0) {
        pr = tcp;
        return;
    }

    if (strcmp(port_arg, "tls") == 0) {
        pr = tls;
        return;
    }

    if (strcmp(port_arg, "udp") == 0) {
        pr = udp;
        return;
    }

    perror("Incorrect protocol.");
    exit(EXIT_FAILURE);
}
