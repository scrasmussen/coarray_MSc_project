#ifndef CAF_C_H 
#define CAF_C_H 

#include <stdio.h>
#include <string>
#include <vector>
#include "mpi.h"


#ifdef COMPILER_SUPPORTS_CAF_INTRINSICS
	void caf_init(int *argc, char argv[]){
		_caf_extensions_init();
	}
	void caf_finalize(int *argc, char argv[]){
		_caf_extensions_finalize();
	}
	void opencoarrays_sync_all(){
		_caf_extensions_sync_all();
	}
	void opencoarrays_error_stop(int32_t stop_code = -1){
		_caf_extensions_error_stop();
	}
	void opencoarrays_co_broadcast(int a, int source_image, int stat, int errmsg, int errmsg_len){
		_caf_extensions_co_broadcast();
	}
#else
	void caf_init(int *argc, char argv[]){
		_gfortran_caf_init();
	}
	void caf_finalize(int *argc, char argv[]){
		_gfortran_caf_finalize();
	}
	void opencoarrays_sync_all(){
		_gfortran_caf_sync_all();
	}
	void opencoarrays_error_stop(int32_t stop_code = -1){
		_gfortran_caf_error_stop();
	}
	void opencoarrays_co_broadcast(int a, int source_image, int stat, int errmsg, int errmsg_len){
		_gfortran_caf_co_broadcast();
	}
#endif

// Returns the image number (MPI rank + 1)
int this_image();

// Returns the total number of images
int num_images();

// Halt the execution of all images
void error_stop(int32_t stop_code = -1);

// Impose a global execution barrier
void sync_all();


template <class T >
class coarray{
public:
	typedef T data_type;
    coarray();
    ~coarray();
    
private:



};
/*
CPP: coarray<type [] []> nom
FORTRAN: type(len=MAX_STRING) :: nom[*] [*] = possessed by every process
acces: nom(index1,index2)[num_process]
*/
#endif