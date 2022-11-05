#include <Python.h>

#include <dns/dns.h>

static PyObject *method_resolv(PyObject *self, PyObject *args)
{
    const struct request r;
    struct response rsp;

    if (!PyArg_ParseTuple(args, "00", &r, &rsp))
    {
        return NULL;
    }

    return PyLong_FromLong(resolv(r, &rsp));
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