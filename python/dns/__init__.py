from ctypes import *

import pydns

class Request(Structure):
    _fields_ = [
        ('address', c_char_p),
        ('port', c_uint16),
        ('protocol', c_int),
        ('qname', c_char_p),
        ('type', c_int16),
    ]

class Header(Structure):
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