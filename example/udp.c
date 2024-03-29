#include <dns/dns.h>
#include <stdio.h>

int main(void)
{
    struct request r = { "10.0.2.3", 53, udp, "github.com", DNS_TYPE_A, DNS_CLASS_IN };
    struct response resp;
    int result = resolv(r, &resp);
    if (result != 0) {
        perror("error");
    } else {
        print_response(resp);
        free_response(resp);
    }

    return 0;
}