#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <dns.h>
#include <stdio.h>

static PyObject* method_resolv(PyObject* self, PyObject* args)
{
    char* addr;
    uint16_t port;
    int pr;
    char* qname;
    int16_t type;
    int16_t class;
    char* ca;
    char* certificate;
    struct response* rsp;

    if (!PyArg_ParseTuple(args, "sHishhss", &addr, &port, &pr, &qname, &type, &class, &ca, &certificate)) {
        return NULL;
    }

    rsp = malloc(sizeof(struct response));

    struct request r = {
        addr,
        port,
        pr,
        qname,
        type,
        class,
        ca,
        certificate,
    };
    int result = resolv(r, rsp);

    if (result != 0) {
        return NULL;
    }
    return PyLong_FromVoidPtr(rsp);
}

static PyObject* method_free_response(PyObject* self, PyObject* args)
{
    void* tmp;

    if (!PyArg_ParseTuple(args, "L", &tmp)) {
        return NULL;
    }

    struct response* rsp = tmp;
    free_response(*rsp);
    return PyLong_FromVoidPtr(tmp);
}

static PyMethodDef dnsMethods[] = {
    { "resolv", method_resolv, METH_VARARGS, "resolv function" },
    { "free_response", method_free_response, METH_VARARGS, "free_response function" },
    { NULL, NULL, 0, NULL },
};

static struct PyModuleDef dnsmodule = {
    PyModuleDef_HEAD_INIT,
    "pydns",
    "pydns",
    -1,
    dnsMethods,
};

// cppcheck-suppress unusedFunction
PyMODINIT_FUNC PyInit_pydns(void)
{
    return PyModule_Create(&dnsmodule);
}