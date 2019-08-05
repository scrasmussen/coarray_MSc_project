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
	this->token = new caf_token_t;
	this->descriptor.base_addr = NULL;
	this->descriptor.offset = 0;
	this->descriptor.dtype.elem_len = sizeof(T);
	this->descriptor.dtype.version = 0;
	this->descriptor.dtype.rank = 0;
	this->descriptor.dtype.type = 5;
	this->descriptor.dtype.attribute = 0;
	this->descriptor.span = 1;
	this->descriptor.dim[0]._stride = 1;
	this->descriptor.dim[0].lower_bound = 0;
	this->descriptor.dim[0]._ubound = 0;
	this->size = sizeof(T);
	// Fixed value for proof of concept, will need to be dynamically selected in the future
	this->type = CAF_REGTYPE_COARRAY_ALLOC;
	_gfortran_caf_register(this->size, this->type, &this->token, &this->descriptor, &this->stat, errmsg, errmsg_len);
}

template<class T>
void coarraycpp::coarray<T>::operator=(const T& val) {
	this->value = val;
	this->size = sizeof(T);
	this->type = CAF_REGTYPE_COARRAY_ALLOC;
	this->descriptor.base_addr = &this->value;
	this->descriptor.offset = 0;
	this->descriptor.dtype.elem_len = sizeof(T);
	this->descriptor.dtype.version = 0;
	this->descriptor.dtype.rank = 0;
	this->descriptor.dtype.type = 5;
	this->descriptor.dtype.attribute = 0;
	this->descriptor.span = 1;
	this->descriptor.dim[0]._stride = 1;
	this->descriptor.dim[0].lower_bound = 0;
	this->descriptor.dim[0]._ubound = 0;
	this->token = new caf_token_t;
}

// template<class T>
// template<class U>
// void coarraycpp::coarray<T>::operator=(const U& val){
// 	this->value = static_cast<T>(val);
// }

template<class T>
void coarraycpp::coarray<T>::operator=(const coarraycpp::coarray<T>& coarray){
	this->value = coarray.get_value();
	this->descriptor = coarray.descriptor;
	this->size = coarray.size;
	this->type = coarray.type;
	this->token = coarray.token;
}

template<class T>
T coarraycpp::coarray<T>::get_value(){
	return this->value;
}

template<class T>
T coarraycpp::coarray<T>::get_from(int image_index){
	int src_kind = 5, dst_kind = 5;
	gfc_descriptor_t descriptor;
	descriptor.offset = 0;
	descriptor.dtype.elem_len = sizeof(T);
	descriptor.dtype.version = 0;
	descriptor.dtype.rank = 0;
	descriptor.dtype.type = 5;
	descriptor.dtype.attribute = 0;
	descriptor.span = 1;
	descriptor.dim[0]._stride = 1;
	descriptor.dim[0].lower_bound = 0;
	descriptor.dim[0]._ubound = 0;
	bool may_require_tmp = true;
	opencoarrays_get(this->token, descriptor.offset,image_index,&descriptor,NULL,&descriptor,src_kind ,dst_kind, may_require_tmp, &this->stat);
	return *(T *)descriptor.base_addr;
}

template<class T>
T coarraycpp::coarray<T>::operator[](char){
	return this->value;
}

template<class T>
coarraycpp::coarray<T>::~coarray(){
	// Compilermindblower
}

// int get_int(int src, int dest, int image_index, int* offset, bool mrt){
// 	std::cout << "Remote access of coarrays not yet supported" << std::endl;
// 	error_stop();
// }