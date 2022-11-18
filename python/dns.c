#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <dns/dns.h>
#include <stdio.h>

static PyObject *method_resolv(PyObject *self, PyObject *args)
{
    char *addr;
    uint16_t port;
    int pr;
    char *qname;
    int16_t type;
    struct response *rsp;
    rsp = malloc(sizeof(struct response));
    if (!PyArg_ParseTuple(args, "sHish", &addr, &port, &pr, &qname, &type))
    {
        return NULL;
    }

    struct request r =
        {
            addr,
            port,
            pr,
            qname,
            type};
    printf("addr = '%s'\nport = %d\npr = %d\nqname = '%s'\ntype = %d\n", addr, port, pr, qname, type);
    int result = resolv(r, rsp);
    print_response(*rsp);
    return PyLong_FromVoidPtr(rsp);
}

static PyMethodDef dnsMethods[] = {
    {"resolv", method_resolv, METH_VARARGS, "resolv function"},
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef dnsmodule = {
    PyModuleDef_HEAD_INIT,
    "pydns",
    "pydns",
    -1,
    dnsMethods,
};

PyMODINIT_FUNC PyInit_pydns(void)
{
    return PyModule_Create(&dnsmodule);
}