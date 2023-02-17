import dns

request = dns.Request()
request.Address = "dns.server"
request.Port = 853
request.Protocol = dns.Protocol.TLS
request.Qname = "github.com"
request.Type = dns.DNS_TYPE_A
request.Ca = "rootCA.crt"
request.Certificate = "certificate.pem"

response = dns.Resolv(request)
print(response) 