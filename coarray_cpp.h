#ifndef CAF_C_H 
#define CAF_C_H 

#include <stdio.h>
#include <string>
#include <vector>
#include "mpi.h"
#include "/usr/include/libcaf.h"

class coarray<typename T>{
public:
	
	int this_image(){
		int ierr = MPI_Comm_rank(CACPP_COMM_WORLD, &image_num);
		if (ierr /= 0) error_stop();
		return image_num + 1;
	}

	int num_images(){
		int ierr = MPI_Comm_size(CACPP_COMM_WORLD, &num_images)
		if(ierr /= 0) error_stop();
		return &num_images

	};

	void error_stop(int_32_t stop_code = -1){
		opencoarrays_error_stop(stop_code)
	}

	void sync_all(){
		opencoarrays_sync_all()
	}


private:

};
/*
CPP: coarray<type [] []> nom
FORTRAN: type(len=MAX_STRING) :: nom[*] [*] = possessed by every process
acces: nom(index1,index2)[num_process]
*/
#endif