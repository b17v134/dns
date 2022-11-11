import dns

request = dns.Request()
request.Address = "10.0.2.3"
request.Port = 53
request.Protocol = 1
request.Qname = "github.com"
request.Type = dns.DNS_TYPE_A

response = dns.Response()
result = dns.Resolv(request, response)
print(result)