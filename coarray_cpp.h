#ifndef CAF_C_H 
#define CAF_C_H

#include <stdio.h>
#include <string>
#include <vector>
#include <mpi.h>

MPI_Comm CACPP_COMM_WORLD;
int *image_num;
int *num_image;

extern "C" {
	#include "/home/jerome/OpenCoarrays-2.7.1/src/libcaf.h"
	/* Unbuffering C */
	void ersetb(void){
		setbuf(stdout,NULL); /* set output to unbuffered */
	}
	void _gfortran_caf_init(int *argc, char argv[]);
	void _gfortran_caf_finalize();
	void _gfortran_caf_sync_all(int stat, char errmsg[], int unused);
	void _gfortran_caf_error_stop(int32_t stop_code);
	void _gfortran_caf_co_broadcast(int a, int source_image, int stat, int errmsg, int errmsg_len);
	MPI_Comm CAF_COMM_WORLD;
}

void opencoarrays_sync_all(int stat, char errmsg[], int unused){
	_gfortran_caf_sync_all(stat, errmsg, unused);
}

// Halt the execution of all images
void opencoarrays_error_stop(int32_t stop_code = -1){
	_gfortran_caf_error_stop(stop_code);
}

void opencoarrays_co_broadcast(int a, int source_image, int stat, int errmsg, int errmsg_len){
	_gfortran_caf_co_broadcast(a, source_image, stat, errmsg, errmsg_len);
}

void opencoarrays_get(caf_token_t token, size_t offset, int image_index, gfc_descriptor_t *src , caf_vector_t *src_vector __attribute__ ((unused)), gfc_descriptor_t *dest, int src_kind, int dst_kind, bool mrt){
	_gfortran_caf_get(token, offset, image_index, *src , *src_vector __attribute__ ((unused)), *dest, src_kind, dst_kind, mrt)
}

namespace coarraycpp {

	/* Links to the OpenCoarray Library */
	void caf_init(int *argc, char argv[]){
		_gfortran_caf_init(argc, argv);
		CACPP_COMM_WORLD = CAF_COMM_WORLD;
	}

	void caf_finalize(){
		_gfortran_caf_finalize();
	}
	
	// Returns the image number (MPI rank + 1)
	int this_image();

	// Returns the total number of images
	int num_images();

	// Halt the execution of all images
	void error_stop(int32_t stop_code = -1);

	// Impose a global execution barrier
	void sync_all(int stat, char errmsg[], int unused);

	template <class T >
	class coarray{
	public:
		typedef T data_type;
	    coarray();
	    ~coarray();
		void operator=(const data_type& value);
		void operator=(const coarray<T>& coarray);
	    T get_from(int index);
	private:
		data_type Data;
	};

}

#endif