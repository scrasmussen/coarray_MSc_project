#include "coarray_cpp.h"
#include "/usr/include/libcaf.h"

// Returns the image number (MPI rank + 1)
int this_image(){
	int ierr = MPI_Comm_rank(CACPP_COMM_WORLD, &image_num);
	if (ierr /= 0) error_stop();
	return image_num + 1;
}

// Returns the total number of images
int num_images(){
	int ierr = MPI_Comm_size(CACPP_COMM_WORLD, &num_images)
	if(ierr /= 0) error_stop();
	return &num_images;
};

// Halt the execution of all images
void error_stop(int_32_t stop_code = -1){
	opencoarrays_error_stop(stop_code);
}

// Impose a global execution barrier
void sync_all(){
	opencoarrays_sync_all();
}

