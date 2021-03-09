#include "Python.h"
#include "stddef.h"
#include "structmember.h"

#define MAX_VERTICES 64

typedef struct
{
    PyObject_HEAD
    uint64_t vertices;
    uint64_t edges_matrix[MAX_VERTICES];
} AdjacencyMatrixObject;

// Graph methods
static PyObject* AdjacencyMatrix_method_test2(AdjacencyMatrixObject* self, PyObject* o)
{
    return PyLong_FromLong(620);
};

static PyObject* AdjacencyMatrix_number_of_vertices(AdjacencyMatrixObject* self, PyObject *Py_UNUSED(ignored))
{
    uint64_t v_temp = self->vertices;
    uint count = 0;
    for(size_t vi=0; vi<MAX_VERTICES; vi++) {
        count += (v_temp)&0x01;
        v_temp >>= 1;
    }

    return PyLong_FromLong(count);
}

static PyObject* AdjacencyMatrix_vertices(AdjacencyMatrixObject* self, PyObject *Py_UNUSED(ignored))
{
    uint64_t v_temp = self->vertices;
    PyObject* v_set = PySet_New(NULL);
    for(size_t vi=0; vi<MAX_VERTICES; vi++) {
        if(v_temp&0x01)
        {
            PySet_Add(v_set, PyLong_FromLong(vi));
        }
        v_temp >>= 1;
    }

    return v_set;
}

static PyObject* AdjacencyMatrix_vertex_degree(AdjacencyMatrixObject* self, PyObject* o)
{
    if(PyLong_Check(o)) {
        uint64_t e_temp = self->edges_matrix[PyLong_AsLong(o)%MAX_VERTICES];
        uint degree = 0;
        for(size_t ei=0; ei<MAX_VERTICES; ei++) {
            degree += (e_temp)&0x01;
            e_temp >>= 1;
        }
        return PyLong_FromLong(degree);
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
}

static PyObject* AdjacencyMatrix_vertex_neighbors(AdjacencyMatrixObject* self, PyObject* o)
{
    if(PyLong_Check(o)) {
        uint64_t e_temp = self->edges_matrix[PyLong_AsLong(o)%64];
        PyObject* n_set = PySet_New(NULL);
        for(size_t ni=0; ni<MAX_VERTICES; ni++) {
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

static PyObject* AdjacencyMatrix_add_vertex(AdjacencyMatrixObject* self, PyObject* o)
{
    if(PyLong_Check(o)) {
        self->vertices |= ((uint64_t) 1) << PyLong_AsLong(o);
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
    return Py_BuildValue("");
};


static PyObject* AdjacencyMatrix_delete_vertex(AdjacencyMatrixObject* self, PyObject* o)
{
    uint64_t v = PyLong_AsLong(o) % MAX_VERTICES;
    if(PyLong_Check(o)) {
        self->vertices &= ~(((uint64_t) 1) << v);
        self->edges_matrix[v] = 0;
        for(size_t vi=0; vi<MAX_VERTICES; vi++)
            self->edges_matrix[vi] &= ~((uint64_t)1 << v);
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
    return Py_BuildValue("");
};


static PyObject* AdjacencyMatrix_number_of_edges(AdjacencyMatrixObject* self, PyObject *Py_UNUSED(ignored))
{
    uint64_t edges = 0;
    for(size_t vi = 0; vi<MAX_VERTICES; vi++) {
        uint64_t e_temp = self->edges_matrix[vi];
        for(size_t ei=0; ei<MAX_VERTICES; ei++) {
            edges += (e_temp)&0x01;
            e_temp >>= 1;
        }
    }
    return PyLong_FromLong(edges/2);
}

static PyObject* AdjacencyMatrix_edges(AdjacencyMatrixObject* self, PyObject *Py_UNUSED(ignored))
{
    PyObject* e_set = PySet_New(NULL);
    for(uint64_t vi=0; vi<MAX_VERTICES; vi++)
        for(uint64_t vj=vi+1; vj<MAX_VERTICES; vj++)
            if(self->edges_matrix[vi] & ((uint64_t)1 << vj))
                PySet_Add(e_set, PyTuple_Pack(2, PyLong_FromLong(vi), PyLong_FromLong(vj)));
    return e_set;
}

static PyObject* AdjacencyMatrix_is_edge(AdjacencyMatrixObject* self, PyObject* o)
{
    uint64_t v1, v2;
    if(!PyArg_ParseTuple(o, "kk", &v1, &v2))
    {
        PyErr_SetString(PyExc_TypeError, "Couldn't unpack argument");
        return NULL;
    }

    uint64_t is_edge = 0;
    is_edge = self->edges_matrix[v1%MAX_VERTICES] & ((uint64_t)1 << (v2%MAX_VERTICES));

    return PyBool_FromLong(is_edge);
};

static PyObject* AdjacencyMatrix_add_edge(AdjacencyMatrixObject* self, PyObject* o)
{
    uint64_t v1, v2;
    if(!PyArg_ParseTuple(o, "kk", &v1, &v2))
    {
        PyErr_SetString(PyExc_TypeError, "Couldn't unpack argument");
        return NULL;
    }

    self->vertices |= ((uint64_t)1 << (v2%MAX_VERTICES));
    self->vertices |= ((uint64_t)1 << (v1%MAX_VERTICES));
    self->edges_matrix[v1%MAX_VERTICES] |= ((uint64_t)1 << (v2%MAX_VERTICES));
    self->edges_matrix[v2%MAX_VERTICES] |= ((uint64_t)1 << (v1%MAX_VERTICES));

    return Py_BuildValue("");
};

static PyObject* AdjacencyMatrix_delete_edge(AdjacencyMatrixObject* self, PyObject* o)
{
    uint64_t v1, v2;
    if(!PyArg_ParseTuple(o, "kk", &v1, &v2))
    {
        PyErr_SetString(PyExc_TypeError, "Couldn't unpack argument");
        return NULL;
    }

    self->edges_matrix[v1%MAX_VERTICES] &= ~((uint64_t)1 << (v2%MAX_VERTICES));
    self->edges_matrix[v2%MAX_VERTICES] &= ~((uint64_t)1 << (v1%MAX_VERTICES));

    return Py_BuildValue("");
};
// Graph methods end

static PyMemberDef AdjacencyMatrix_members[] ={
    {NULL} // Sentinel
};

static PyMethodDef AdjacencyMatrix_methods[] = {
    // Base operations
    {"number_of_vertices", (PyCFunction) AdjacencyMatrix_number_of_vertices, METH_NOARGS, "Returns the number of vertices of current graph."},
    {"vertices", (PyCFunction) AdjacencyMatrix_vertices, METH_NOARGS, "Returns the vertices of current graph as a 64-bit number."},
    {"vertex_degree", (PyCFunction) AdjacencyMatrix_vertex_degree, METH_O, "Returns the degree of given vertex."},
    {"vertex_neighbors", (PyCFunction) AdjacencyMatrix_vertex_neighbors, METH_O, "Returns the neighbors of given vertex."},
    {"add_vertex", (PyCFunction) AdjacencyMatrix_add_vertex, METH_O, "Adds new vertex to the graph."},
    {"delete_vertex", (PyCFunction) AdjacencyMatrix_delete_vertex, METH_O, "Removes existing vertex from the graph."},
    {"number_of_edges", (PyCFunction) AdjacencyMatrix_number_of_edges, METH_NOARGS, "Returns the number of edges in graph."},
    {"edges", (PyCFunction) AdjacencyMatrix_edges, METH_NOARGS, "Returns the edges of the graph."},
    {"is_edge", (PyCFunction) AdjacencyMatrix_is_edge, METH_VARARGS, "Returns whether the vertices form an edge in the graph."},
    {"add_edge", (PyCFunction) AdjacencyMatrix_add_edge, METH_VARARGS, "Adds new edge between specified vertices in the graph."},
    {"delete_edge", (PyCFunction) AdjacencyMatrix_delete_edge, METH_VARARGS, "Removed edge between specified vertices in the graph."},
    //TODO WIP methods
    // Additional operation
    {"vertices_of_degree", (PyCFunction) AdjacencyMatrix_method_test2, METH_O, "Returns vertices of given degree."},
    {NULL} // Sentinel
};

static PyObject* AdjacencyMatrix_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    AdjacencyMatrixObject *self;
    self = (AdjacencyMatrixObject *) type->tp_alloc(type, 0);
    if(self != NULL) {
        self->vertices = 0;
        for(size_t v = 0; v<MAX_VERTICES; v++)
            self->edges_matrix[v] = 0;
    }
    return (PyObject *) self;
}

static int AdjacencyMatrix_init(AdjacencyMatrixObject *self, PyObject *args, PyObject *kwds)
{
    char *str = "?";
    if(!PyArg_ParseTuple(args, "|s", &str))
    {
        PyErr_SetString(PyExc_TypeError, "Couldn't unpack argument");
        return -1;
    }
    if(strlen(str) == 0) {
        PyErr_SetString(PyExc_TypeError, "g6 sequence cannot be empty");
        return -1;
    }
    //Vertices
    char *g6_sequence = str;
    uint64_t num_of_vertices = g6_sequence[0] - 63;
    while(num_of_vertices > 0)
        self->vertices |=  (uint64_t)1 << --num_of_vertices;
    num_of_vertices = g6_sequence[0] - 63;
    //Edges
    size_t g6_index = 1;
    char n_vect = g6_sequence[g6_index] - 63;
    char offset = 5;
    char counter = 6;
    for(size_t vi = 0; vi < num_of_vertices; vi++) {
        for(size_t vj = 0; vj < vi; vj++) {
            if(n_vect & (1 << offset)) {
                self->edges_matrix[vi] |= (uint64_t)1 << vj;
                self->edges_matrix[vj] |= (uint64_t)1 << vi;
            }
            n_vect <<= 1;
            if(--counter == 0) {
                counter = 6;
                n_vect = g6_sequence[++g6_index] - 63;
            }
        }
    }
    return 0;
}

static PyTypeObject simple_graphs_AdjacencyMatrixType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "simple_graphs.AdjacencyMatrix",
    .tp_doc = "AdjacencyMatrix objects",
    .tp_basicsize = sizeof(AdjacencyMatrixObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = AdjacencyMatrix_new,
    .tp_init = (initproc)AdjacencyMatrix_init,
    //TODO .tp_dealloc = ?
    .tp_members = AdjacencyMatrix_members,
    .tp_methods = AdjacencyMatrix_methods,
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
    PyObject *m;

    if(PyType_Ready(&simple_graphs_AdjacencyMatrixType) < 0)
        return NULL;

    m = PyModule_Create(&simple_graphs_module);
    if(m == NULL)
        return NULL;

    Py_INCREF(&simple_graphs_AdjacencyMatrixType);
    if(PyModule_AddObject(m, "AdjacencyMatrix", (PyObject *)&simple_graphs_AdjacencyMatrixType) < 0) {
        Py_DECREF(&simple_graphs_AdjacencyMatrixType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}