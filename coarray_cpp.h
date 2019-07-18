#ifndef CAF_CPP_H
#define CAF_CPP_H

#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

MPI_Comm CACPP_COMM_WORLD;

extern "C" {
	#include "libcaf-gfortran-descriptor.h"
/* Unbuffering C */
	void ersetb(void) { setbuf(stdout, NULL); /* set output to unbuffered */ }

/* TYPES IMPORT */
	/* indicates which kind of coarray variable should be registered. */
	typedef enum caf_register_t {
		CAF_REGTYPE_COARRAY_STATIC,
		CAF_REGTYPE_COARRAY_ALLOC,
		CAF_REGTYPE_LOCK_STATIC,
		CAF_REGTYPE_LOCK_ALLOC,
		CAF_REGTYPE_CRITICAL,
		CAF_REGTYPE_EVENT_STATIC,
		CAF_REGTYPE_EVENT_ALLOC,
		CAF_REGTYPE_COARRAY_ALLOC_REGISTER_ONLY,
		CAF_REGTYPE_COARRAY_ALLOC_ALLOCATE_ONLY
	}caf_register_t;

	/* Allows to specify type of deregistration */
	typedef enum caf_deregister_t {
		CAF_DEREGTYPE_COARRAY_DEREGISTER,
		/* Only allowed for allocatable components in derived type coarrays */
		CAF_DEREGTYPE_COARRAY_DEALLOCATE_ONLY
	}caf_deregister_t;

	/* Type void * on compiler side, can be any type on lib side */
	typedef void *caf_token_t;

	/*  */
	typedef struct caf_vector_t {
		size_t nvec;
		union {
			struct {
				void *vector;
				int kind;
			} v;
			struct {
				ptrdiff_t lower_bound, upper_bound, stride;
			} triplet;
		} u;
	} caf_vector_t;

	// typedef void *caf_team_t;

	/* References to remote components of a derived type.
   Keep in sync with gcc/libgfortran/caf/libcaf.h.  */
	typedef struct caf_reference_t {
		/* A pointer to the next ref or NULL.  */
		struct caf_reference_t *next;
		/* The type of the reference.  */
		/* caf_ref_type_t, replaced by int to allow specification in fortran FE.  */
		int type;
		/* The size of an item referenced in bytes.  I.e. in an array ref this is
		the factor to advance the array pointer with to get to the next item.
		For component refs this gives just the size of the element referenced.  */
		size_t item_size;
		union {
			struct {
				/* The offset (in bytes) of the component in the derived type.  */
				ptrdiff_t offset;
				/* The offset (in bytes) to the caf_token associated with this
				component.  NULL, when not allocatable/pointer ref.  */
				ptrdiff_t caf_token_offset;
			} c;
			struct {
			/* The mode of the array ref.  See CAF_ARR_REF_*.  */
			/* caf_array_ref_t, replaced by unsigend char to allow specification in
			fortran FE.  */
				unsigned char mode[GFC_MAX_DIMENSIONS];
				/* The type of a static array.  Unset for array's with descriptors.  */
				int static_array_type;
				/* Subscript refs (s) or vector refs (v).  */
				union {
					struct {
						/* The start and end boundary of the ref and the stride.  */
						ptrdiff_t start, end, stride;
					} s;
					struct {
						/* nvec entries of kind giving the elements to reference.  */
						void *vector;
						/* The number of entries in vector.  */
						size_t nvec;
						/* The integer kind used for the elements in vector.  */
						int kind;
					} v;
				} dim[GFC_MAX_DIMENSIONS];
			} a;
		} u;
	} caf_reference_t;

/* GLOBAL VARIABLE IMPORT */
	MPI_Comm CAF_COMM_WORLD;

/* METHODS IMPORT */
	void _gfortran_caf_init(int *argc, char ***argv);
	void _gfortran_caf_finalize(void);

	void _gfortran_caf_this_image(int distance);
	void _gfortran_caf_num_images(int distance, int failed);

	void _gfortran_caf_register(size_t size, caf_register_t type,
		caf_token_t *token, gfc_descriptor_t *desc,
		int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_deregister(caf_token_t *token, int *stat, char *errmsg,
		size_t errmsg_len);

	void _gfortran_caf_get(caf_token_t token, size_t offset, int image_index,
		gfc_descriptor_t *src, caf_vector_t *src_vector,
		gfc_descriptor_t *dest, int src_kind, int dst_kind,
		bool may_require_tmp, int *stat);
	void _gfortran_caf_send(caf_token_t token, size_t offset, int image_index,
		gfc_descriptor_t *dest, caf_vector_t *dst_vector,
		gfc_descriptor_t *src, int dst_kind, int src_kind,
		bool may_require_tmp, int *stat);
	void _gfortran_caf_sendget(caf_token_t dst_token, size_t dst_offset,
		int dst_image_index, gfc_descriptor_t *dest,
		caf_vector_t *dst_vector, caf_token_t src_token,
		size_t src_offset, int src_image_index,
		gfc_descriptor_t *src, caf_vector_t *src_vector,
		int dst_kind, int src_kind, bool may_require_tmp,
		int *stat);

	// void _gfortran_caf_get_by_ref(caf_token_t token, int image_index,
	// 	gfc_descriptor_t *dst, caf_reference_t *refs,
	// 	int dst_kind, int src_kind, bool may_require_tmp,
	// 	bool dst_reallocatable, int *stat, int src_type);
	// void _gfortran_caf_send_by_ref(caf_token_t token, int image_index,
	// 	gfc_descriptor_t *src, caf_reference_t *refs,
	// 	int dst_kind, int src_kind, bool may_require_tmp,
	// 	bool dst_reallocatable, int *stat, int dst_type);
	// void _gfortran_caf_sendget_by_ref(caf_token_t dst_token, int dst_image_index,
	// 	caf_reference_t *dst_refs,
	// 	caf_token_t src_token, int src_image_index,
	// 	caf_reference_t *src_refs, int dst_kind,
	// 	int src_kind, bool may_require_tmp,
	// 	int *dst_stat, int *src_stat, int dst_type,
	// 	int src_type);

	void _gfortran_caf_co_broadcast(gfc_descriptor_t *a, int source_image,
		int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_co_max(gfc_descriptor_t *a, int result_image, int *stat,
		char *errmsg, int a_len, size_t errmsg_len);
	void _gfortran_caf_co_min(gfc_descriptor_t *a, int result_image, int *stat,
		char *errmsg, int a_len, size_t errmsg_len);
	void _gfortran_caf_co_reduce(gfc_descriptor_t *a, void *(*opr)(void *, void *),
		int opr_flags, int result_image, int *stat,
		char *errmsg, int a_len, size_t errmsg_len);

	void _gfortran_caf_co_sum(gfc_descriptor_t *a, int result_image, int *stat,
		char *errmsg, size_t errmsg_len);

	void _gfortran_caf_sync_all(int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_sync_images(int count, int images[], int *stat, char *errmsg,
		size_t errmsg_len);
	void _gfortran_caf_sync_memory(int *stat, char *errmsg, size_t errmsg_len);

	void _gfortran_caf_stop_str(const char *, size_t QUIETARG);

	void _gfortran_caf_stop(int QUIETARG);
	void _gfortran_caf_error_stop_str(const char *string, size_t len);

	void _gfortran_caf_error_stop(int error);

	void _gfortran_caf_fail_image(void);

	// void _gfortran_caf_form_team(int, caf_team_t *, int);
	// void _gfortran_caf_change_team(caf_team_t *, int);
	// void _gfortran_caf_end_team(caf_team_t *);
	// void _gfortran_caf_sync_team(caf_team_t *, int);
	// void _gfortran_caf_team_number(caf_team_t *);

	// void _gfortran_caf_image_status(int image, caf_team_t *team);
	// void _gfortran_caf_failed_images(caf_team_t *team, int *kind);
	// void _gfortran_caf_stopped_images(caf_team_t *team, int *kind);

	void _gfortran_caf_atomic_define(caf_token_t token, size_t offset,
		int image_index, void *value, int *stat,
		int type, int kind);
	void _gfortran_caf_atomic_ref(caf_token_t token, size_t offset, int image_index,
		void *value, int *stat, int type, int kind);
	void _gfortran_caf_atomic_cas(caf_token_t token, size_t offset, int image_index,
		void *old, void *compare, void *new_val,
		int *stat, int type, int kind);
	void _gfortran_caf_atomic_op(int op, caf_token_t token, size_t offset,
		int image_index, void *value, void *old, int *stat,
		int type, int kind);

	void _gfortran_caf_lock(caf_token_t token, size_t index, int image_index,
		int *aquired_lock, int *stat, char *errmsg,
		size_t errmsg_len);
	void _gfortran_caf_unlock(caf_token_t token, size_t index, int image_index,
		int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_event_post(caf_token_t token, size_t index, int image_index,
		int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_event_wait(caf_token_t token, size_t index, int until_count,
		int *stat, char *errmsg, size_t errmsg_len);
	void _gfortran_caf_event_query(caf_token_t token, size_t index, int image_index,
		int *count, int *stat);

	// MPI_Fint _gfortran_caf_get_communicator(caf_team_t *);
}

void opencoarrays_caf_init(int *argc,char ***argv) {
	_gfortran_caf_init(argc, argv);
}

void opencoarrays_sync_all(int *stat = NULL, char errmsg[] = NULL, size_t errmsg_len = 0) {
	_gfortran_caf_sync_all(stat, errmsg, errmsg_len);
}

// Halt the execution of all images
void opencoarrays_error_stop(int32_t stop_code = -1) {
	_gfortran_caf_error_stop(stop_code);
}

void opencoarrays_co_broadcast(gfc_descriptor_t *a, int source_image, int *stat,
	char *errmsg, size_t errmsg_len) {
	_gfortran_caf_co_broadcast(a, source_image, stat, errmsg, errmsg_len);
}

void opencoarrays_get(caf_token_t token, size_t offset, int image_index,
	gfc_descriptor_t *src, caf_vector_t *src_vector,
	gfc_descriptor_t *dest, int src_kind, int dst_kind,
	bool may_require_tmp, int *stat) {
	_gfortran_caf_get(token, offset, image_index, src, src_vector, dest,
		src_kind, dst_kind, may_require_tmp, stat);
}

namespace coarraycpp {
/* Links to the OpenCoarray Library */
	void caf_init(int *argc, char ***argv);

	void caf_finalize();

// Returns the image number (MPI rank + 1)
	int this_image();

// Returns the total number of images
	int num_images();

// Halt the execution of all images
	void error_stop(int32_t stop_code = -1);

// Impose a global execution barrier
	void sync_all(int *stat = NULL, char errmsg[] = NULL, size_t errmsg_len = 0);

	template <class T>
		class coarray {
		public:
			typedef T data_type;
			coarray();
			~coarray();
			void operator=(data_type &value);
			void operator=(coarray<T> &coarray);
			data_type& get_from(int image_index);
			size_t size;					// Either the byte size of the coarray or for lock and event types the nb of elements
			caf_register_t type;			// The type of the coarray
			caf_token_t token;				// ID of the coarray
			gfc_descriptor_t descriptor;	// Descriptor of the coarray
			int stat;						// Status of the coarray
	}; // end of class coarray

}  // end of namespace coarraycpp

#endif