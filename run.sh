#!/bin/bash
mpirun -mca btl_tcp_if_include eth0 --hostfile hosts2 bin/main-linux Histogram/Tests/10000/10000.a Histogram/Tests/10000/10000.b
