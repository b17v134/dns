from ctypes import *
from enum import Enum

import pydns

# https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml#dns-parameters-4
DNS_TYPE_ERROR = 0
DNS_TYPE_A = 1
DNS_TYPE_NS = 2
DNS_TYPE_MD = 3
DNS_TYPE_MF = 4
DNS_TYPE_CNAME = 5
DNS_TYPE_SOA = 6
DNS_TYPE_MB = 7
DNS_TYPE_MG = 8
DNS_TYPE_MR = 9
DNS_TYPE_NULL = 10
DNS_TYPE_WKS = 11
DNS_TYPE_PTR = 12
DNS_TYPE_HINFO = 13
DNS_TYPE_MINFO = 14
DNS_TYPE_MX = 15
DNS_TYPE_TXT = 16
DNS_TYPE_RP = 17
DNS_TYPE_AFSDB = 18
DNS_TYPE_X25 = 19
DNS_TYPE_ISDN = 20
DNS_TYPE_RT = 21
DNS_TYPE_NSAP = 22
DNS_TYPE_NSAP_PTR = 23
DNS_TYPE_SIG = 24
DNS_TYPE_KEY = 25
DNS_TYPE_PX = 26
DNS_TYPE_GPOS = 27
DNS_TYPE_AAAA = 28
DNS_TYPE_LOC = 29
DNS_TYPE_NXT = 30
DNS_TYPE_EID = 31
DNS_TYPE_NIMLOC = 32
DNS_TYPE_SRV = 33
DNS_TYPE_ATMA = 34
DNS_TYPE_NAPTR = 35
DNS_TYPE_KX = 36
DNS_TYPE_CERT = 37
DNS_TYPE_A6 = 38
DNS_TYPE_DNAME = 39
DNS_TYPE_SINK = 40
DNS_TYPE_OPT = 41
DNS_TYPE_APL = 42
DNS_TYPE_DS = 43
DNS_TYPE_SSHFP = 44
DNS_TYPE_IPSECKEY = 45
DNS_TYPE_RRSIG = 46
DNS_TYPE_NSEC = 47
DNS_TYPE_DNSKEY = 48
DNS_TYPE_DHCID = 49
DNS_TYPE_NSEC3 = 50
DNS_TYPE_NSEC3PARAM = 51
DNS_TYPE_TLSA = 52
DNS_TYPE_SMIMEA = 53

DNS_TYPE_HIP = 55
DNS_TYPE_NINFO = 56
DNS_TYPE_RKEY = 57
DNS_TYPE_TALINK = 58
DNS_TYPE_CDS = 59
DNS_TYPE_CDNSKEY = 60
DNS_TYPE_OPENPGPKEY = 61
DNS_TYPE_CSYNC = 62
DNS_TYPE_ZONEMD = 63
DNS_TYPE_SVCB = 64
DNS_TYPE_HTTPS = 65

DNS_TYPE_SPF = 99
DNS_TYPE_UINFO = 100
DNS_TYPE_UID = 101
DNS_TYPE_GID = 102
DNS_TYPE_UNSPEC = 103
DNS_TYPE_NID = 104
DNS_TYPE_L32 = 105
DNS_TYPE_L64 = 106
DNS_TYPE_LP = 107
DNS_TYPE_EUI48 = 108
DNS_TYPE_EUI64 = 109

DNS_TYPE_TKEY = 249
DNS_TYPE_TSIG = 250
DNS_TYPE_IXFR = 251
DNS_TYPE_AXFR = 252
DNS_TYPE_MAILB = 253
DNS_TYPE_MAILA = 254

DNS_TYPE_URI = 256
DNS_TYPE_CAA = 257
DNS_TYPE_AVC = 258
DNS_TYPE_DOA = 259
DNS_TYPE_AMTREPLAY = 260

DNS_TYPE_TA = 32768
DNS_TYPE_DLV = 32769

class Header(dict):
    def __init__(
        self, 
        Id: int = 0,
        Qr: int = 0,
        Opcode: int = 0,
        Aa: int = 0,
        Tc: int = 0,
        Rd: int = 0,
        Ra: int = 0,
        Z: int = 0,
        Rcode: int = 0,
        Qdcount: int = 0,
        Ancount: int = 0,
        Nscount: int = 0,
        Arcount: int = 0,
    ):
        dict.__init__(
            self,
            Id = Id,
            Qr = Qr,
            Opcode = Opcode,
            Aa = Aa,
            Tc = Tc,
            Rd = Rd,
            Ra = Ra,
            Z = Z,
            Rcode = Rcode,
            Qdcount = Qdcount,
            Ancount = Ancount,
            Nscount = Nscount,
            Arcount = Arcount, 
        )

class Question(dict):
    def __init__(self, qname = "", qtype = "", qclass = ""):
        dict.__init__(self, Qname = qname, Qtype = qtype, Qclass = qclass)

class ResourceRecord(dict):
    def __init__(self, name ="", type=0, cls=0, ttl=0, rdlength=0, rdata=""):
        dict.__init__(self, Name=name, Type=type, Class=cls, Ttl=ttl, Rdlength=rdlength, Rdata=rdata)

class Protocol(Enum):
    TCP = 0
    UDP = 1
    TLS = 2
    HTTPS = 3

class Class(Enum):
    IN = 1
    CH = 3
    HS = 4

class Request:
    Address: str
    Port: int
    Protocol: Protocol
    Qname: str
    Type: int
    Class: Class
    Ca: str
    Certificate: str

class Response(dict):
    def __init__(
        self, 
        hdr = "", 
        questions = [], 
        answers = [], 
        authorityRecords = [],  
        additionalRecords = [],
    ):
        dict.__init__(
            self, 
            Hdr = hdr, 
            Questions = questions, 
            Answers = answers, 
            AuthorityRecords = authorityRecords, 
            AdditionalRecords = additionalRecords,
        )

class CRequest(Structure):
    _fields_ = [
        ('addr', c_char_p),
        ('port', c_uint16),
        ('pr', c_int),
        ('qname', c_char_p),
        ('type', c_int16),
        ('class', c_uint16),
        ('ca', c_char_p),
        ('certificate', c_char_p)
    ]

class CHeader(Structure):
    _fields_ = [
        ('id', c_uint16),
        ('qr', c_uint8),
        ('opcode', c_uint8),
        ('aa', c_uint8),
        ('tc', c_uint8),
        ('rd', c_uint8),
        ('ra', c_uint8),
        ('z', c_uint8),
        ('rcode', c_uint8),
        ('qdcount', c_uint16),
        ('ancount', c_uint16),
        ('nscount', c_uint16),
        ('arcount', c_uint16),
    ]

class CResponse(Structure):
    _fields_ = [
        ('header', CHeader),
        ('question', c_void_p),
        ('answers', c_void_p),
        ('authority_records', c_void_p),
        ('additional_records', c_void_p),
    ]

class CQuestion(Structure):
    _fields_ = [ 
        ('qname', c_char_p),
        ('qtype', c_uint16),
        ('qclass', c_uint16),
    ]

class CResourceRecord(Structure):
    _fields_ = [
        ('name', c_char_p),
        ('type', c_uint16),
        ('class', c_uint16),
        ('ttl', c_uint32),
        ('rdlength', c_uint16),
        ('rdata', c_char_p),
    ]

def Resolv(request: Request)->Response:
    result = pydns.resolv(
        request.Address, 
        request.Port, 
        request.Protocol.value,  
        request.Qname, 
        request.Type, 
        request.Class.value,
        getattr(request, "Ca", ""), 
        getattr(request, "Certificate", ""),
    )

    response:Response = Response()
    g:CResponse = cast(result, POINTER(CResponse))
    cresponse: CResponse = g[0]

    header = Header(
        cresponse.header.id,
        cresponse.header.qr,
        cresponse.header.opcode,
        cresponse.header.aa,
        cresponse.header.tc,
        cresponse.header.rd,
        cresponse.header.ra,
        cresponse.header.z,
        cresponse.header.rcode,
        cresponse.header.qdcount,
        cresponse.header.ancount,
        cresponse.header.nscount,
        cresponse.header.arcount,
    )

    questions = []
    for i in range(0, header["Qdcount"]):
        question:CQuestion = cast(cresponse.question, POINTER(CQuestion))
        question:Question = Question(
            qname = question[i].qname.decode('ascii'),
            qtype = question[i].qtype,
            qclass = question[i].qclass,
        )
        questions.append(question)


    answers = []
    for i in range(0, header["Ancount"]):
        cResourceRecord:CResourceRecord = cast(cresponse.answers, POINTER(CResourceRecord))
        resourceRecord:ResourceRecord = ResourceRecord(
            cResourceRecord[i].name.decode('ascii'),
            cResourceRecord[i].type,
            getattr(cResourceRecord[i],"class"),
            cResourceRecord[i].ttl,
            cResourceRecord[i].rdlength,
            cResourceRecord[i].rdata.decode('ascii'),
        )
        answers.append(resourceRecord)

    authorityRecords = []
    for i in range(0, header["Nscount"]):
        cResourceRecord:CResourceRecord = cast(cresponse.authority_records, POINTER(CResourceRecord))
        resourceRecord:ResourceRecord = ResourceRecord(
            cResourceRecord[i].name.decode('ascii'),
            cResourceRecord[i].type,
            getattr(cResourceRecord[i],"class"),
            cResourceRecord[i].ttl,
            cResourceRecord[i].rdlength,
            cResourceRecord[i].rdata.decode('ascii'),
        )
        authorityRecords.append(resourceRecord)

    additionalRecords = []
    for i in range(0, header["Arcount"]):
        cResourceRecord:CResourceRecord = cast(cresponse.additional_records, POINTER(CResourceRecord))
        resourceRecord:ResourceRecord = ResourceRecord(
            cResourceRecord[i].name.decode('ascii'),
            cResourceRecord[i].type,
            getattr(cResourceRecord[i],"class"),
            cResourceRecord[i].ttl,
            cResourceRecord[i].rdlength,
            cResourceRecord[i].rdata.decode('ascii'),
        )
        additionalRecords.append(resourceRecord)

    response:Response = Response(header, questions, answers, authorityRecords, additionalRecords)
 
    pydns.free_response(result)
    return response
