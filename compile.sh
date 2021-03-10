#!/bin/bash

python3.9 setup.py build
cp build/lib.linux-x86_64-3.8/simple_graphs.cpython-38-x86_64-linux-gnu.so simple_graphs.so
