#include "dns.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

const char *version = "0.0.1";

static int help_flag;
static int verbose_flag;
static int version_flag;

static uint16_t type = DNS_TYPE_A;
static char *server;
static int port = 53;

static struct option long_options[] = {
    {"help", no_argument, &help_flag, 1},
    {"server", required_argument, 0, 0},
    {"type", required_argument, 0, 0},
    {"verbose", no_argument, &verbose_flag, 1},
    {"version", no_argument, &version_flag, 1},
};

void print_version();
void print_usage();
void set_server(char *srv);
void set_port(char *port_arg);

int main(int argc, char *argv[])
{
    int c;

    while (1)
    {
        int option_index = 0;
        c = getopt_long(argc, argv, "hvVs:t:p:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case 'h':
            print_usage();
            break;
        case 'p':
            set_port(optarg);
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

    if (help_flag)
    {
        print_usage();
    }

    if (verbose_flag)
        puts("verbose flag is set");

    if (version_flag)
    {
        print_version();
    }

    if (optind < argc)
    {
        while (optind < argc)
        {
            char *qname = argv[optind++];
            struct request r = {server, port, udp, qname, type};
            char *buffer = malloc(sizeof(char) * 1024);
            if (buffer == NULL)
            {
                perror("Cannot allocate memory");
                exit(1);
            }
            int len = resolv(r, buffer);
            struct response resp = get_response(buffer, len);
            free(buffer);
            print_response(resp);
            putchar('\n');
        }
    }

    if (!server)
    {
        free(server);
    }
    return 0;
}

void print_version()
{
    printf("resolv %s\n", version);
    exit(0);
}

void print_usage()
{
    puts("Usage: resolv [options...] <value>\n\
 -c, --class         Query class [default: IN]\n\
 -h, --help          Show help and exit\n\
 -s, --server <ip>   Server ip\n\
 -p, --port <number> Port number [default: 53]\n\
 -t, --type          Query type [default: A]\n\
 -v, --verbose       Verbose mode\n\
 -V, --version       Show version number and exit\n\
");
    exit(0);
}

void set_server(char *srv)
{
    server = malloc(sizeof(char) + (strlen(optarg) + 1));
    if (!server)
    {
        perror("Cannot allocate memory");
        exit(1);
    }
    strcpy(server, optarg);
}

void set_port(char *port_arg)
{
    int p = atoi(port_arg);
    if ((p <= 0) || (p > 65535))
    {
        perror("Incorrect port number.");
        exit(EXIT_FAILURE);
    }

    port = p;
}