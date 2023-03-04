#include <dns/dns.h>
#include <stdio.h>

int main(void)
{
    struct request r = { "dns.server", 853, tls, "github.com", DNS_TYPE_A, DNS_CLASS_IN, "rootCA.crt", "certificate.pem" };
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