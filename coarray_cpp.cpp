#include "coarray_cpp.h"
//#include "/usr/include/libcaf.h"

// Returns the image number (MPI rank + 1)
int this_image(){
	int ierr = MPI_Comm_rank(CACPP_COMM_WORLD, image_num);
	if (ierr != 0) coarraycpp::error_stop(-1);
	return (*image_num+1);
}

// Returns the total number of images
int num_images(){
	int ierr = MPI_Comm_size(CACPP_COMM_WORLD, num_image);
	if(ierr != 0) coarraycpp::error_stop(-1);
	return *num_image;
};

// Halt the execution of all images
void coarraycpp::error_stop(int32_t stop_code){
	opencoarrays_error_stop(stop_code);
}

// Impose a global execution barrier
void coarraycpp::sync_all(int stat, char errmsg[], int unused){
	opencoarrays_sync_all(stat, errmsg, unused);
}

template<class T>
void coarraycpp::coarray<T>::operator=(const data_type& value){
	this.value = value;
}

template<class T>
void coarraycpp::coarray<T>::operator=(const coarraycpp::coarray<T>& coarray){
	this.value = coarray.value;
}

template<class T>
T coarraycpp::coarray<T>::get_from(int index){
	
};