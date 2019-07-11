CC=g++
FILE=helloworld.cpp

# run `mpic++ -show` and enter that result, minus the g++
MPIOPT=-I/mnt/gpfs0/home/s280353/local/mpich/3.2.1-gcc-trunk/include -L/mnt/gpfs0/home/s280353/local/mpich/3.2.1-gcc-trunk/lib -lmpicxx -Wl,-rpath -Wl,/mnt/gpfs0/home/s280353/local/mpich/3.2.1-gcc-trunk/lib -Wl,--enable-new-dtags -lmpi
# this is the path to the lib64 directory of where I installed Open Coarray
COPT=/mnt/gpfs0/home/s280353/local/ocaf/2.7.3-gcc-trunk-mpich-3.2.1/lib64/libcaf_mpi.a

all: build

build:
	$(CC) $(FILE) $(COPT) $(MPIOPT) -o runMe.exe

run:
	mpiexec -np 2 ./runMe.exe

clean:
	rm -f *~ *.o *.exe a.out
