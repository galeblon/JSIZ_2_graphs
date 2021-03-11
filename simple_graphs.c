#include "Python.h"
#include "stddef.h"
#include "structmember.h"

#define MAX_VERTICES 64

// Helper functions section
uint64_t bitCount(uint64_t val)
{
	uint64_t res = 0;
	while(val != 0) {
		val &= val-1;
		res++;
	}
	return res;
}
// Helper functions section end

typedef struct
{
    PyObject_HEAD
    uint64_t vertices;
    uint64_t edges_matrix[MAX_VERTICES];
} AdjacencyMatrixObject;

// Graph methods
static PyObject* AdjacencyMatrix_number_of_vertices(AdjacencyMatrixObject* self, PyObject *Py_UNUSED(ignored))
{
    return PyLong_FromLong(bitCount(self->vertices));
}

static PyObject* AdjacencyMatrix_vertices(AdjacencyMatrixObject* self, PyObject *Py_UNUSED(ignored))
{
    PyObject* temp;
    uint64_t v_temp = self->vertices;
    PyObject* v_set = PySet_New(NULL);
    uint64_t vi = 0;
    while(v_temp) {
        if(v_temp&0x01)
        {
            temp = PyLong_FromLong(vi);
            PySet_Add(v_set, temp);
            Py_DECREF(temp);
        }
        v_temp >>= 1;
        vi++;
    }

    return v_set;
}

static PyObject* AdjacencyMatrix_vertex_degree(AdjacencyMatrixObject* self, PyObject* o)
{
    if(PyLong_Check(o)) {
        return PyLong_FromLong(bitCount(self->edges_matrix[PyLong_AsLong(o)%MAX_VERTICES]));
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
}

static PyObject* AdjacencyMatrix_vertex_neighbors(AdjacencyMatrixObject* self, PyObject* o)
{
    PyObject* temp;
    if(PyLong_Check(o)) {
        uint64_t e_temp = self->edges_matrix[PyLong_AsLong(o)%64];
        PyObject* n_set = PySet_New(NULL);
        uint64_t ni = 0;
        while(e_temp) {
            if(e_temp&0x01)
            {
                temp = PyLong_FromLong(ni);
                PySet_Add(n_set, temp);
                Py_DECREF(temp);
            }
            e_temp >>= 1;
            ni++;
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
        uint64_t ni = 0;
        while(self->edges_matrix[v]) {
            if(self->edges_matrix[v] & 0x01) {
                self->edges_matrix[ni] &= ~ ((uint64_t)1 << v);
            }
            self->edges_matrix[v] >>= 1;
            ni++;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
    return Py_BuildValue("");
};


static PyObject* AdjacencyMatrix_number_of_edges(AdjacencyMatrixObject* self, PyObject *Py_UNUSED(ignored))
{
    uint64_t edges = 0;
    uint64_t vi = 0;
    uint64_t vertices_temp = self->vertices;
    while(vertices_temp) {
        if(vertices_temp & 0x01) {
            edges += bitCount(self->edges_matrix[vi]);
        }
        vertices_temp >>= 1;
        vi++;
    }
    return PyLong_FromLong(edges/2);
}

static PyObject* AdjacencyMatrix_edges(AdjacencyMatrixObject* self, PyObject *Py_UNUSED(ignored))
{
    PyObject* temp;
    PyObject* e_set = PySet_New(NULL);
    uint64_t vertices_temp = self->vertices;
    uint64_t vi = 0;
    while(vertices_temp) {
        if(vertices_temp & 0x01) {
            uint64_t vj = vi + 1;
            uint64_t edges = self->edges_matrix[vi];
            while(edges) {
                if(edges & 0x01) {
                    temp = PyTuple_Pack(2, PyLong_FromLong(vi), PyLong_FromLong(vj));
                    PySet_Add(e_set, temp);
                    Py_DECREF(temp);
                }
                edges >>= 1;
                vj++;
            }
        }
        vertices_temp >>= 1;
        vi++;
    }
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

    uint64_t is_edge = self->edges_matrix[v1%MAX_VERTICES] & ((uint64_t)1 << (v2%MAX_VERTICES));

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

static PyObject* AdjacencyMatrix_vertices_of_degree(AdjacencyMatrixObject* self, PyObject* o)
{
    PyObject* temp;
    if(PyLong_Check(o)) {
        uint64_t degree = self->edges_matrix[PyLong_AsLong(o)];
        PyObject* v_set = PySet_New(NULL);
        uint64_t vi = 0;
        uint64_t vertices_temp = self->vertices;
        while(vertices_temp) {
            if(bitCount(self->edges_matrix[vi]) == degree) {
                temp = PyLong_FromLong(vi);
                PySet_Add(v_set, temp);
                Py_DECREF(temp);
            }
            vertices_temp >>=1;
            vi++;
        }
        return v_set;
    } else {
        PyErr_SetString(PyExc_TypeError, "Expected integer number as argument");
        return NULL;
    }
}

static PyObject* AdjacencyMatrix_richcompare(PyObject *self, PyObject* other, int op)
{
    AdjacencyMatrixObject *s = (AdjacencyMatrixObject *)self;
    AdjacencyMatrixObject *o = (AdjacencyMatrixObject *)other;
    uint64_t vertices_temp, vi;
    switch(op) {
        case Py_LT:
        break;
        case Py_LE:
        break;
        case Py_EQ:
        if(s->vertices != o->vertices)
            return Py_False;
        vertices_temp = s->vertices;
        vi = 0;
        while(vertices_temp) {
            if(s->edges_matrix[vi] != o->edges_matrix[vi])
                return Py_False;
            vertices_temp >>=1;
            vi++;
        }
        return Py_True;
        break;
        case Py_NE:
        if(s->vertices != o->vertices)
            return Py_True;
        vertices_temp = s->vertices | o->vertices;
        vi = 0;
        while(vertices_temp) {
            if(s->edges_matrix[vi] != o->edges_matrix[vi])
                return Py_True;
            vertices_temp >>=1;
            vi++;
        }
        return Py_False;
        case Py_GT:
        break;
        case Py_GE:
        break;
    }
    return Py_NotImplemented;
}
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
    // Additional operation
    {"vertices_of_degree", (PyCFunction) AdjacencyMatrix_vertices_of_degree, METH_O, "Returns vertices of given degree."},
    {NULL} // Sentinel
};

static PyObject* AdjacencyMatrix_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    AdjacencyMatrixObject *self;
    self = (AdjacencyMatrixObject *) type->tp_alloc(type, 0);
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

static void AdjacencyMatrix_dealloc(AdjacencyMatrixObject* self)
{
    Py_TYPE(self)->tp_free((PyObject *) self);
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
    .tp_members = AdjacencyMatrix_members,
    .tp_methods = AdjacencyMatrix_methods,
    .tp_richcompare = AdjacencyMatrix_richcompare,
    .tp_dealloc = (destructor) AdjacencyMatrix_dealloc
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