#include "Python.h"
#include "stddef.h"
#include "structmember.h"

typedef struct
{
    PyObject_HEAD
    uint64_t vertices;
    uint64_t edges_matrix[64];
} AdjacencyMatrixGraphObject;

// Graph methods
static PyObject* AdjacencyMatrixGraph_method_test2(AdjacencyMatrixGraphObject* self, PyObject* o)
{
    return PyLong_FromLong(620);
};

static PyObject* AdjacencyMatrixGraph_method_test3(AdjacencyMatrixGraphObject* self, PyObject* o)
{
    return Py_BuildValue("");
};

static PyObject* AdjacencyMatrixGraph_number_of_vertices(AdjacencyMatrixGraphObject* self, PyObject *Py_UNUSED(ignored))
{
    uint64_t v_temp = self->vertices;
    uint count = 0;
    for(size_t vi=0; vi<64; vi++) {
        count += (v_temp)&0x01;
        v_temp >>= 1;
    }

    return PyLong_FromLong(count);
}

static PyObject* AdjacencyMatrixGraph_vertices(AdjacencyMatrixGraphObject* self, PyObject *Py_UNUSED(ignored))
{
    uint64_t v_temp = self->vertices;
    PyObject* v_set = PySet_New(NULL);
    for(size_t vi=0; vi<64; vi++) {
        if(v_temp&0x01)
        {
            PySet_Add(v_set, PyLong_FromLong(vi));
        }
        v_temp >>= 1;
    }

    return v_set;
}

static PyObject* AdjacencyMatrixGraph_vertex_degree(AdjacencyMatrixGraphObject* self, PyObject* o)
{
    if(PyLong_Check(o)) {
        uint64_t e_temp = self->edges_matrix[PyLong_AsLong(o)%64];
        uint degree = 0;
        for(size_t ei=0; ei<64; ei++) {
            degree += (e_temp)&0x01;
            e_temp >>= 1;
        }
        return PyLong_FromLong(degree);
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
}

static PyObject* AdjacencyMatrixGraph_vertex_neighbors(AdjacencyMatrixGraphObject* self, PyObject* o)
{
    if(PyLong_Check(o)) {
        uint64_t e_temp = self->edges_matrix[PyLong_AsLong(o)%64];
        PyObject* n_set = PySet_New(NULL);
        for(size_t ni=0; ni<64; ni++) {
            if(e_temp&0x01)
            {
                PySet_Add(n_set, PyLong_FromLong(ni));
            }
            e_temp >>= 1;
        }
        return n_set;
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
}

static PyObject* AdjacencyMatrixGraph_add_vertex(AdjacencyMatrixGraphObject* self, PyObject* o)
{
    if(PyLong_Check(o)) {
        self->vertices |= ((uint64_t) 1) << PyLong_AsLong(o);
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
    return Py_BuildValue("");
};
// Graph methods end

static PyMemberDef AdjacencyMatrixGraph_members[] ={
    {NULL} // Sentinel
};

static PyMethodDef AdjacencyMatrixGraph_methods[] = {
    // Base operations
    {"number_of_vertices", (PyCFunction) AdjacencyMatrixGraph_number_of_vertices, METH_NOARGS, "Returns the number of vertices of current graph."},
    {"vertices", (PyCFunction) AdjacencyMatrixGraph_vertices, METH_NOARGS, "Returns the vertices of current graph as a 64-bit number."},
    {"vertex_degree", (PyCFunction) AdjacencyMatrixGraph_vertex_degree, METH_O, "Returns the degree of given vertex."},
    {"vertex_neighbors", (PyCFunction) AdjacencyMatrixGraph_vertex_neighbors, METH_O, "Returns the neighbors of given vertex."},
    {"add_vertex", (PyCFunction) AdjacencyMatrixGraph_add_vertex, METH_O, "Adds new vertex to the graph."},
    //TODO WIP methods
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