CC=g++
MPICC=mpicxx
LIBFILE=/lib/libcaf_mpi.so
CPPLIBFILE=coarray_cpp.cpp
WORKDIR=/home/jerome/coarray_MSc_project
SRCFILE=/home/jerome/coarray_MSc_project/C++_Test_Cases/hello_multiverse_scalar.cpp

# run `mpic++ -show` and enter that result, minus the g++
MPIOPT=-I/opt/mpich/include -L/opt/mpich/lib -lmpicxx -Wl,-rpath -Wl,/opt/mpich/lib -Wl,--enable-new-dtags -lmpi
# this is the path to the directory where libcaf_mpi.so is installed
COPT=/lib

default: all

all: libcoarray_cpp.so preprocess
	${MPICC} -g -Wall -Wextra -pedantic -o RunMe ${SRCFILE} -L${WORKDIR} -lcoarray_cpp -lcaf_mpi ${MPIOPT}

preprocess:
	rm -f ${SRCFILE}.tmp
	sed -e 's/<<</.get_from(/g; s/>>>/)/g' ${SRCFILE} > ${SRCFILE}.tmp
	diff ${SRCFILE} ${SRCFILE}.tmp || true
	mv ${SRCFILE} ${SRCFILE}.bkp
	mv ${SRCFILE}.tmp ${SRCFILE}

clean:
	rm -rf *.o RunMe

libcoarray_cpp.so: coarray_cpp.o
	${MPICC} -g -shared coarray_cpp.o -L/lib -lcaf_mpi -o libcoarray_cpp.so ${MPIOPT}

coarray_cpp.o: ${CPPLIBFILE}
	${MPICC} -c -Wall -g -pedantic -fPIC ${CPPLIBFILE} -o coarray_cpp.o ${MPIOPT}

cleanlib:
	rm -rf *.o *.so

cleancallgrind:
	rm callgrind.*

cleanall: clean cleanlib cleancallgrind