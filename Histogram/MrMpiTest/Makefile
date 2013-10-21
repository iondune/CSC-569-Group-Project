
all: build

build:
	mpic++ HelloWorld.cpp -o HelloWorld ../../deps/src/lib/linux/libmrmpi_linux.a -I../../deps/src/inc/

run:
	mpirun -mca btl_tcp_if_include eth0 --hostfile hosts4 HelloWorld
