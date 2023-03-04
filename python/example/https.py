import dns

request = dns.Request()
request.Address = "dns.server"
request.Port = 443
request.Protocol = dns.Protocol.HTTPS
request.Qname = "github.com"
request.Type = dns.DNS_TYPE_A
request.Class = dns.Class.CH
request.Ca = "rootCA.crt"
request.Certificate = "certificate.pem"

response = dns.Resolv(request)
print(response) 