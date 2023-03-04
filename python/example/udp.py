import dns

request = dns.Request()
request.Address = "10.0.2.3"
request.Port = 53
request.Protocol = dns.Protocol.UDP
request.Qname = "github.com"
request.Type = dns.DNS_TYPE_A
request.Class = dns.Class.CH

response = dns.Resolv(request)
print(response) 