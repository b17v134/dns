from ctypes import *
from typing import List

import pydns

from enum import Enum 

class Header:
    """Identifier."""
    Id: int

    """A one bit field that specifies whether this message is a query (0), or a response (1)."""
    Qr: int

    """A four bit field that specifies kind of query."""
    Opcode: int

    """Authoritative Answer"""
    Aa: int

    """Specifies that this message was truncated."""
    Tc: int

    """Recursion Desired."""
    Rd: int

    """Recursion Available."""
    Ra: int

    """Reserved for future use. Must be zero in all queries and responses."""
    Z: int

    """Response code."""
    Rcode: int

    """Number of entries in the question section."""
    Qdcount: int

    """Number of resource records in the answer section"""
    Ancount: int

    """Number of name server resource records in the authority records section."""
    Nscount: int

    """Number of resource records in the additional records section."""
    Arcount: int

class Question:
    Qname: str
    Qtype: int
    Qclass: int

class ResourceRecord:
    Name: str
    Type: int
    Class: int
    Ttl: int
    Rdlength: int
    Rdata: str

class Protocol(Enum):
    TCP: 0
    UDP: 1

class Request:
    Address: str
    Port: int
    Protocol: Protocol
    Qname: str
    Type: int

class Response:
    Hdr: Header
    Questions: List[Question]
    Answers: List[ResourceRecord]
    AuthorityRecords: List[ResourceRecord]
    AdditionalRecords: List[ResourceRecord]

class CRequest(Structure):
    _fields_ = [
        ('addr', c_char_p),
        ('port', c_uint16),
        ('pr', c_int),
        ('qname', c_char_p),
        ('type', c_int16),
    ]

class CHeader(Structure):
    _fields_ = [
        ('id', c_uint16),
        ('qr', c_uint8),
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

def Resolv(request: Request, response: Response)->int:
    cRequest = CRequest()
    cRequest.addr = request.Address.encode('utf-8')
    cRequest.port = request.Port
    cRequest.pr = request.Protocol
    cRequest.type = request.Type
    r = c_char_p()
    result = pydns.resolv(cRequest, r)

    return result
