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


inline void coarraycpp::caf_finalize(){ _gfortran_caf_finalize(); }

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
	if(CACPP_COMM_WORLD == -1){
		coarraycpp::caf_init(NULL,NULL);
	}
	size_t errmsg_len = 0;
	char *errmsg = NULL;
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
	// Check if token is a good MPI_Win
	// check token manipulations
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
	std::cout << std::endl << " ## Image : " << coarraycpp::this_image() << " Address: " << this->descriptor.base_addr << std::endl; //DEBUG
}

template<class T>
T coarraycpp::coarray<T>::get_value(){
	return this->value;
}

template<class T>
T coarraycpp::coarray<T>::get_from(int image_index){
	if(image_index == coarraycpp::this_image())
		return this->value; // if the call is from this_image() to this_image()
	int src_kind = 1, dst_kind = 1;
	gfc_descriptor_t descriptorsrc;
	descriptorsrc.base_addr = new T[1];
	descriptorsrc.offset = 0;
	descriptorsrc.dtype.elem_len = sizeof(T);
	descriptorsrc.dtype.version = 0;
	descriptorsrc.dtype.rank = 1;
	descriptorsrc.dtype.type = 5;
	descriptorsrc.dtype.attribute = 0;
	descriptorsrc.span = 1;
	descriptorsrc.dim[0]._stride = 1;
	descriptorsrc.dim[0].lower_bound = 0;
	descriptorsrc.dim[0]._ubound = 0;
	gfc_descriptor_t descriptordest;
	descriptordest.base_addr = new T[1];
	descriptordest.offset = 0;
	descriptordest.dtype.elem_len = sizeof(T);
	descriptordest.dtype.version = 0;
	descriptordest.dtype.rank = 1;
	descriptordest.dtype.type = 5;
	descriptordest.dtype.attribute = 0;
	descriptordest.span = 1;
	descriptordest.dim[0]._stride = 1;
	descriptordest.dim[0].lower_bound = 0;
	descriptordest.dim[0]._ubound = 0;
	bool may_require_tmp = true;
	std::cout << "Ho hello" << std::endl;
	opencoarrays_get(this->token, descriptorsrc.offset,image_index,&descriptorsrc,NULL,&descriptordest,src_kind ,dst_kind, may_require_tmp, NULL);
	std::cout << "Ho hello again" << std::endl;
	T val = *(T *)descriptordest.base_addr;
	std::cout << "Received value = " << val << std::endl;
	return val;
}

// template<class T>
// T coarraycpp::coarray<T>::operator[](char){
// 	return this->value;
// }

template<class T>
coarraycpp::coarray<T>::~coarray(){
	char errmsg;
	size_t errmsg_len = 0;
	_gfortran_caf_deregister(&this->token, &this->stat, &errmsg, errmsg_len);
	_gfortran_caf_finalize();
}

// int get_int(int src, int dest, int image_index, int* offset, bool mrt){
// 	std::cout << "Remote access of coarrays not yet supported" << std::endl;
// 	error_stop();
// }