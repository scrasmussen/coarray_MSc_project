CC=g++
MPICC=mpich
LIBFILE=/lib/libcaf_mpi.so
CPPLIBFILE=coarray_cpp.cpp
WORKDIR=/home/jerome/coarray_MSc_project
SRCFILE=/home/jerome/coarray_MSc_project/C++_Test_Cases/hello_multiverse.cpp

default: all

all: libcoarray_cpp.so preprocess /usr/include/libcaf.h
	${CC} -L${WORKDIR} -Wall -Wextra -pedantic -o RunMe ${SRCFILE} -lcoarray_cpp

preprocess:
	rm -f ${SRCFILE}.tmp
	sed -e 's/<<</.get_from(/g; s/>>>/)/g' ${SRCFILE} > ${SRCFILE}.tmp
	diff ${SRCFILE} ${SRCFILE}.tmp || true
	mv ${SRCFILE} ${SRCFILE}.bkp
	mv ${SRCFILE}.tmp ${SRCFILE}

clean:
	rm -rf *.o RunMe

libcoarray_cpp.so: coarray_cpp.o
	${CC} -shared -o libcoarray_cpp.so coarray_cpp.o

coarray_cpp.o: ${CPPLIBFILES}
	${CC} -E -L/lib -c -Wall -Werror -fpic ${CPPLIBFILE} -lcaf_mpi