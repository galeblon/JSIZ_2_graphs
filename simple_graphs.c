#include "Python.h"
#include "stddef.h"
#include "structmember.h"

typedef struct {
    PyObject_HEAD
    /* fields */
    uint64_t vertices;

} AdjacencyMatrixGraphObject;

// Graph methods
static PyObject* AdjacencyMatrixGraph_method_test(AdjacencyMatrixGraphObject* self, PyObject *Py_UNUSED(ignored)) {
    return PyLong_FromLong(300);
};

static PyObject* AdjacencyMatrixGraph_method_test2(AdjacencyMatrixGraphObject* self, PyObject* o) {
    return PyLong_FromLong(620);
};

static PyObject* AdjacencyMatrixGraph_method_test3(AdjacencyMatrixGraphObject* self, PyObject* o) {
    return Py_BuildValue("");
};

// Graph methods end

static PyMemberDef AdjacencyMatrixGraph_members[] = {
    {NULL} // Sentinel
};

static PyMethodDef AdjacencyMatrixGraph_methods[] = {
    // Base operations
    {"number_of_vertices", (PyCFunction) AdjacencyMatrixGraph_method_test, METH_NOARGS, "Returns the number of vertices of current graph."},
    {"vertices", (PyCFunction) AdjacencyMatrixGraph_method_test, METH_NOARGS, "Returns the vertices of current graph as a 64-bit number."},
    {"vertex_degree", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_O, "Returns the degree of given vertex."},
    {"vertex_neighbors", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_O, "Returns the neighbors of given vertex."},
    {"add_vertex", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_O, "Adds new vertex to the graph."},
    {"delete_vertex", (PyCFunction) AdjacencyMatrixGraph_method_test3, METH_O, "Removes existing vertex from the graph."},
    {"number_of_edges", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_NOARGS, "Returns the number of edges in graph."},
    {"edges", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_NOARGS, "Returns the edges of the graph."},
    {"is_edge", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_VARARGS, "Returns whether the vertices form an edge in the graph."},
    {"add_edge", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_VARARGS, "Adds new edge between specified vertices in the graph."},
    {"delete_edge", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_VARARGS, "Removed edge between specified vertices in the graph."},
    // Additional operation
    {"vertices_of_degree", (PyCFunction) AdjacencyMatrixGraph_method_test2, METH_O, "Returns vertices of given degree."},
    {NULL} // Sentinel
};

static PyTypeObject simple_graphs_AdjacencyMatrixGraphType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "simple_graphs.AdjacencyMatrixGraph",
    .tp_doc = "AdjacencyMatrixGraph objects",
    .tp_basicsize = sizeof(AdjacencyMatrixGraphObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    //TODO .tp_new = ?
    //TODO .tp_init = ?
    //TODO .tp_dealloc = ?
    .tp_members = AdjacencyMatrixGraph_members,
    .tp_methods = AdjacencyMatrixGraph_methods,
    //TODO .tp_richcompare = ? for EQ and NE
};


// Module definitions
static PyModuleDef simple_graphs_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "simple_graphs",
    .m_doc = "Module with simple graph implementation",
    .m_size = -1
};

static PyMethodDef simple_graphs_methods[] = {
    {NULL} // Sentinel
};
// Module definitions end


PyMODINIT_FUNC
PyInit_simple_graphs(void)
{
    PyObject* m;
    simple_graphs_AdjacencyMatrixGraphType.tp_new = PyType_GenericNew;

    if(PyType_Ready(&simple_graphs_AdjacencyMatrixGraphType) < 0)
        return NULL;

    m = PyModule_Create(&simple_graphs_module);
    if(m == NULL)
        return NULL;

    Py_INCREF(&simple_graphs_AdjacencyMatrixGraphType);
    if(PyModule_AddObject(m, "AdjacencyMatrixGraph", (PyObject*)&simple_graphs_AdjacencyMatrixGraphType) < 0) {
        Py_DECREF(&simple_graphs_AdjacencyMatrixGraphType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}