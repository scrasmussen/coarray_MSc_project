CC=gcc
MPICC=mpich
LIBFILE="/lib/libcaf_mpi.so"

default:
	clean all

all:
	preprocess

preprocess:
        rm -f ${file}.tmp
        sed -e 's/<<</.get_from(/g; s/>>>/)/g' ${file} > ${file}.tmp
        diff ${file} ${file}.tmp || true

clean:
	rm -rf *.o RunMe