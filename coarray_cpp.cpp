#include "coarray_cpp.h"

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
}

void coarraycpp::caf_init(){
	opencoarrays_caf_init();
	CACPP_COMM_WORLD = CAF_COMM_WORLD;
}


void coarraycpp::caf_finalize(){ _gfortran_caf_finalize(); }

// Halt the execution of all images
void coarraycpp::error_stop(int32_t stop_code){
	opencoarrays_error_stop(stop_code);
}

// Impose a global execution barrier
void coarraycpp::sync_all(int *stat = NULL, char errmsg[] = NULL, int unused = NULL){
	opencoarrays_sync_all(stat, errmsg, unused);
}

template<class T>
void coarraycpp::coarray<T>::operator=(coarraycpp::coarray<T>::data_type& value){
	this.value = value;
}

template<class T>
void coarraycpp::coarray<T>::operator=(coarraycpp::coarray<T>& coarray){
	this.value = coarray.value;
}

// template<class T>
// T coarraycpp::coarray<T>::get_from(int image_index){
// 	opencoarrays_get(caf_token_t token, size_t offset, int image_index, gfc_descriptor_t *src, caf_vector_t *src_vector, gfc_descriptor_t *dest, int src_kind, int dst_kind, bool may_require_tmp, int *stat);
// 	return NULL;
// }