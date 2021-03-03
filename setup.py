from distutils.core import setup, Extension

setup(
    name="simple_graphs",
    version="1.0",
    ext_modules=[Extension("simple_graphs", ["simple_graphs.c"])])
