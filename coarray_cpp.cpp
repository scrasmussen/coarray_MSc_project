#include "coarray_cpp.h"

// Returns the image number (MPI rank + 1)
int coarraycpp::this_image(){
	int image_num;
	int ierr = MPI_Comm_rank(CACPP_COMM_WORLD, &image_num);
	if (ierr != 0) coarraycpp::error_stop(int32_t(-1));
	return (image_num+1);
}

// Returns the total number of images
int coarraycpp::num_images(){
	int num_image;
	int ierr = MPI_Comm_size(CACPP_COMM_WORLD, &num_image);
	if(ierr != 0) coarraycpp::error_stop(int32_t(-1));
	return num_image;
}

void coarraycpp::caf_init(int *argc, char ***argv){
	opencoarrays_caf_init(argc, argv);
	CACPP_COMM_WORLD = CAF_COMM_WORLD;
}


void coarraycpp::caf_finalize(){ _gfortran_caf_finalize(); }

// Halt the execution of all images
void coarraycpp::error_stop(int32_t stop_code){
	opencoarrays_error_stop(stop_code);
}

// Impose a global execution barrier
void coarraycpp::sync_all(int *stat, char errmsg[], size_t errmsg_len){
	opencoarrays_sync_all(stat, errmsg, errmsg_len);
}

/* ############## COARRAY CLASS ############## */

template<class T>
coarraycpp::coarray<T>::coarray(){
	size_t errmsg_len;
	char *errmsg;
	this.token = new caf_token_t;
	this.descriptor = new gfc_descriptor_t;
	this.size = sizeof(T);
	// Fixed value for proof of concept, will need to be dynamically selected in the future
	this.type = CAF_REGTYPE_COARRAY_ALLOC;
	_gfortran_caf_register(this.size, this.type, &this.token, &this.descriptor, &this.stat, errmsg, errmsg_len);
}

template<class T>
void coarraycpp::coarray<T>::operator=(const T& val) {
	this.value = val;
}

// template<class T>
// template<class U>
// void coarraycpp::coarray<T>::operator=(const U& val){
// 	this.value = static_cast<T>(val);
// }

template<class T>
void coarraycpp::coarray<T>::operator=(const coarraycpp::coarray<T>& coarray){
	this.value = coarray.get_value();
}

template<class T>
T coarraycpp::coarray<T>::get_value(){
	return this.value;
}

template<class T>
T& coarraycpp::coarray<T>::get_from(int image_index){
	caf_token_t token;
	size_t offset;
	gfc_descriptor_t src, dest;
	caf_vector_t src_vector;
	int src_kind, dst_kind;
	bool may_require_tmp = true;
	opencoarrays_get(token, offset,image_index,&src,&src_vector,&this.descriptor,src_kind ,dst_kind, may_require_tmp, this.stat);
	return NULL;
}

template<class T>
T coarraycpp::coarray<T>::operator[](void){
	return this.value;
}

// int get_int(int src, int dest, int image_index, int* offset, bool mrt){
// 	std::cout << "Remote access of coarrays not yet supported" << std::endl;
// 	error_stop();
// }