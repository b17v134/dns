#include <Python.h>

#include <dns/dns.h>

static PyObject *method_resolv(PyObject *self, PyObject *args)
{
    const struct request r;
    struct response *rsp;

    if (!PyArg_ParseTuple(args, "w#O", &r, rsp))
    {
        return NULL;
    }

    return PyLong_FromLong(resolv(r, rsp));
}

static PyMethodDef dnsMethods[] = {
    {"resolv", method_resolv, METH_VARARGS, "resolv function"},
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef dnsmodule = {
    PyModuleDef_HEAD_INIT,
    "resolv",
    "resolv function",
    -1,
    dnsMethods,
};

PyMODINIT_FUNC PyInit_dns(void)
{
    return PyModule_Create(&dnsmodule);
}