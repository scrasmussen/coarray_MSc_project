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

	/* TYPES IMPORT */
	typedef enum caf_register_t;
	typedef enum caf_deregister_t;
	typedef void* caf_token_t;
	typedef void * caf_team_t;
	typedef struct caf_teams_list;
	typedef struct caf_used_teams_list;
	typedef struct caf_vector_t;
	typedef enum caf_array_ref_t;
	typedef struct caf_reference_t;

	/* GLOBAL VARIABLE IMPORT */
	MPI_Comm CAF_COMM_WORLD;

	/* METHODS IMPORT */
	void _gfortran_caf_init(int *argc, char ***argv);
	void _gfortran_caf_finalize(void);
	
	void _gfortran_caf_this_image(int distance);
	void _gfortran_caf_num_images(int distance, int failed);
	
	void _gfortran_caf_register(size_t size, caf_register_t type, caf_token_t *token, int *stat,gfc_descriptor_t *desc, int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_deregister(caf_token_t *token, int *stat, char *errmsg, size_t errmsg_len);
	
	void _gfortran_caf_caf_get(caf_token_t token, size_t offset, int image_index, gfc_descriptor_t *src, caf_vector_t *src_vector, gfc_descriptor_t *dest, int src_kind, int dst_kind, bool may_require_tmp, int *stat);
	void _gfortran_caf_caf_send(caf_token_t token, size_t offset, int image_index, gfc_descriptor_t *dest, caf_vector_t *dst_vector, gfc_descriptor_t *src, int dst_kind, int src_kind, bool may_require_tmp, int *stat);
	void _gfortran_caf_caf_sendget(caf_token_t dst_token, size_t dst_offset, int dst_image_index, gfc_descriptor_t *dest, caf_vector_t *dst_vector, caf_token_t src_token, size_t src_offset, int src_image_index, gfc_descriptor_t *src, caf_vector_t *src_vector, int dst_kind, int src_kind, bool may_require_tmp, int *stat);
	
	void _gfortran_caf_get_by_ref(caf_token_t token, int image_index, gfc_descriptor_t *dst, caf_reference_t *refs, int dst_kind, int src_kind, bool may_require_tmp, bool dst_reallocatable, int *stat, int src_type);
	void _gfortran_caf_send_by_ref(caf_token_t token, int image_index, gfc_descriptor_t *src, caf_reference_t *refs, int dst_kind, int src_kind, bool may_require_tmp, bool dst_reallocatable, int *stat, int dst_type);
	void _gfortran_caf_sendget_by_ref(caf_token_t dst_token, int dst_image_index, caf_reference_t *dst_refs, caf_token_t src_token, int src_image_index, caf_reference_t *src_refs, int dst_kind, int src_kind, bool may_require_tmp, int *dst_stat, int *src_stat, int dst_type, int src_type);
	
	void _gfortran_caf_is_present(caf_token_t token, int image_index, gfc_reference_t *ref);
	
	void _gfortran_caf_co_broadcast(gfc_descriptor_t *a, int source_image, int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_co_max(gfc_descriptor_t *a, int result_image, int *stat, char *errmsg, int a_len, size_t errmsg_len);
	void _gfortran_caf_co_min(gfc_descriptor_t *a, int result_image, int *stat, char *errmsg, int a_len, size_t errmsg_len);
	void _gfortran_caf_co_reduce(gfc_descriptor_t *a, void * (*opr) (void *, void *), int opr_flags, int result_image, int *stat, char *errmsg, int a_len, size_t errmsg_len);
	
	void _gfortran_caf_co_sum(gfc_descriptor_t *a, int result_image, int *stat, char *errmsg, size_t errmsg_len);
	
	void _gfortran_caf_sync_all(int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_sync_images(int count, int images[], int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_sync_memory(int *stat, char *errmsg, size_t errmsg_len);

	void _gfortran_caf_stop_str(const char *, size_t QUIETARG);

	void _gfortran_caf_stop(int QUIETARG);
	void _gfortran_caf_error_stop_str(const char *string, size_t len);

	void _gfortran_caf_error_stop(int error);

	void _gfortran_caf_fail_image(void);

	void _gfortran_caf_form_team(int, caf_team_t *, int);
	void _gfortran_caf_change_team(caf_team_t *, int);
	void _gfortran_caf_end_team(caf_team_t *);
	void _gfortran_caf_sync_team(caf_team_t *, int);
	void _gfortran_caf_team_number(caf_team_t *);

	void _gfortran_caf_image_status(int image, caf_team_t * team);
	void _gfortran_caf_failed_images(caf_team_t * team, int * kind);
	void _gfortran_caf_stopped_images(caf_team_t * team, int * kind);

	void _gfortran_caf_atomic_define(caf_token_t token, size_t offset, int image_index, void *value, int *stat, int type, int kind);
	void _gfortran_caf_atomic_ref(caf_token_t token, size_t offset, int image_index, void *value, int *stat, int type, int kind);
	void _gfortran_caf_atomic_cas(caf_token_t token, size_t offset, int image_index, void *old, void *compare, void *new_val, int *stat, int type, int kind);
	void _gfortran_caf_atomic_op(int op, caf_token_t token, size_t offset, int image_index, void *value, void *old, int *stat, int type, int kind);
	
	void _gfortran_caf_lock(caf_token_t token, size_t index, int image_index, int *aquired_lock, int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_unlock(caf_token_t token, size_t index, int image_index, int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_event_post(caf_token_t token, size_t index, int image_index, int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_event_wait(caf_token_t token, size_t index, int until_count, int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_event_query(caf_token_t token, size_t index, int image_index, int *count, int *stat);

	MPI_Fint _gfortran_caf_get_communicator(caf_team_t *);
}

void opencoarrays_caf_init(){
	_gfortran_caf_init(NULL, NULL);
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