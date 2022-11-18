import dns

request = dns.Request()
request.Address = "10.0.2.3"
request.Port = 53
request.Protocol = 1
request.Qname = "github.com"
request.Type = 1

response = dns.Resolv(request)
print(response) 