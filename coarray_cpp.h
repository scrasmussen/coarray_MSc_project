#ifndef CAF_CPP_H
#define CAF_CPP_H

#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <string>

/**
 * Internal MPI_Communicator for use by the C++ layer
 * 
 * @author     Jerome LEKIEFFRE
 */
MPI_Comm CACPP_COMM_WORLD=-1;

extern "C" {
	#include "libcaf-gfortran-descriptor.h"
/* Unbuffering C */
	void ersetb(void) { setbuf(stdout, NULL); /* set output to unbuffered */ }

/* TYPES IMPORT */
	/**
	 * Type used to describe different Coarray types
	 * 
	 * @author     OpenCoarrays
	 */
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

	/**
	 * Type used to specify the type of deregistration of a Coarray
	 * 
	 * @author     OpenCoarrays
	 */
	typedef enum caf_deregister_t {
		CAF_DEREGTYPE_COARRAY_DEREGISTER,
		/* Only allowed for allocatable components in derived type coarrays */
		CAF_DEREGTYPE_COARRAY_DEALLOCATE_ONLY
	}caf_deregister_t;

	/**
	 * Black box used as an ID for the Coarray.
	 * MPI_Win in background
	 * 
	 * @author     OpenCoarrays
	 */
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

	/**
	 * Black box used as an ID for a Coarray team.
	 * 
	 * @author     OpenCoarrays
	 */
	typedef void *caf_team_t;

	/**
	 * References to remote components of a derived type.
	 * NOTE: Keep in sync with gcc/libgfortran/caf/libcaf.h.
	 * 
	 * @author     OpenCoarrays
	 */
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
	/**
	 * OpenCoarrays defined MPI communicator
	 * 
	 * @author     OpenCoarrays
	 */
	MPI_Comm CAF_COMM_WORLD;

/* METHODS IMPORT */
	/**
	 * @brief      Initialize the coarray environment
	 *
	 * @param      argc  An integer pointer with the number of arguments passed to the program or NULL
	 * @param      argv  A pointer to an array of strings with the command-line arguments or NULL
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_init(int *argc, char ***argv);
	
	/**
	 * @brief      Called at the end of execution of a Coarray program to finalize the environment
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_finalize(void);

	/**
	 * @brief      Get the status of the image given by the id image of the team given by team
	 *
	 * @param[in]  image  The image ID
	 * @param      team   The team ID
	 *
	 * @return     0 if OK, STAT_STOPPED_IMAGE if image has been stopped and STAT_FAILED_IMAGE if image has executed a FAIL_IMAGE statement
	 * 
	 * @author     OpenCoarrays
	 */
	int _gfortran_caf_image_status(int image, caf_team_t * team);

	/**
	 * @brief      Get an array of image indexes in the current team that have failed
	 *
	 * @param      team  The team ID (Not used, WIP)
	 * @param      kind  The kind of the resulting integer array
	 *
	 * @return     Array of images that have failed in the current team
	 * @author     OpenCoarrays
	 */
	int _gfortran_caf_failed_images(caf_team_t * team, int * kind);

	/**
	 * @brief      Get an array of image indexes in the current team that have stopped
	 *
	 * @param      team  The team ID (Not used, WIP)
	 * @param      kind  The kind of the resulting integer array
	 *
	 * @return     Array of images that have stopped in the current team
	 * 
	 * @author     OpenCoarrays
	 */
	int _gfortran_caf_stopped_images(caf_team_t *team, int *kind);

	/**
	 * @brief      Allocates memory for a coarray and creates a token to identify the coarray.
	 *
	 * @param[in]  size        For normal coarrays, the byte size of the coarray to be allocated; for lock types and event types, the number of elements.
	 * @param[in]  type        One of the caf_register_t types. 
	 * @param      token       An opaque pointer identifying the coarray.
	 * @param      desc        The (pseudo) array descriptor.
	 * @param      stat        For allocatable coarrays, stores the STAT=; may be NULL
	 * @param      errmsg      When an error occurs, this will be set to an error message; may be NULL
	 * @param[in]  errmsg_len  The buffer size of errmsg.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_register(size_t size, caf_register_t type,
		caf_token_t *token, gfc_descriptor_t *desc,
		int *stat, char *errmsg, size_t errmsg_len);
	
	/**
	 * @brief      Called to free or deregister the memory of a coarray.
	 *
	 * @param      token       The token to free
	 * @param[in]  type        The type of action to take for the Coarray
	 * @param      stat        Stores the STAT=; may be NULL
	 * @param      errmsg      When an error occurs, this will be set to an error message; may be NULL
	 * @param[in]  errmsg_len  The buffer size of errmsg
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_deregister(caf_token_t *token,caf_deregister_t type, int *stat, char *errmsg,
		size_t errmsg_len);
	
	// *
	//  * @brief      Used to query the coarray library whether an allocatable component in a derived type coarray is allocated on a remote image.
	//  *
	//  * @param[in]  token        The token identifying the coarray
	//  * @param[in]  image_index  The remote image index
	//  * @param      ref          A chain of references to address the allocatable or pointer component in the derived type coarray. The object reference needs to be a scalar or a full array reference, respectively.
	//  * 
	//  * @author     OpenCoarrays
	 
	// void _gfortran_caf_is_present(caf_token_t token, int image_index, gfc_reference_t *ref);

	/**
	 * @brief      Called to send a scalar, an array section or a whole array from a local to a remote image identified by the image_index.
	 *
	 * @param[in]  token            The token identifying the coarray
	 * @param[in]  offset           The offset by which amount of bytes the actual data is shifted compared to the base address of the coarray.
	 * @param[in]  image_index      The image index of the remote image.
	 * @param      dest             Array descriptor for the remote image for the bounds and the size. The base_addr shall not be accessed.
	 * @param      dst_vector       If not NULL, it contains the vector subscript of the destination array; the values are relative to the dimension triplet of the dest argument.
	 * @param      src              Array descriptor of the local array to be transferred to the remote image
	 * @param[in]  dst_kind         Kind of the destination argument
	 * @param[in]  src_kind         Kind of the source argument
	 * @param[in]  may_require_tmp  The variable is false when it is known at compile time that the dest and src either cannot overlap or overlap (fully or partially) such that walking src and dest in element wise element order (honoring the stride value) will not lead to wrong results. Otherwise, the value is true.
	 * @param      stat             When non-NULL give the result of the operation, i.e., zero on success and non-zero on error. When NULL and an error occurs, then an error message is printed and the program is terminated.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_send(caf_token_t token, size_t offset, int image_index,
		gfc_descriptor_t *dest, caf_vector_t *dst_vector,
		gfc_descriptor_t *src, int dst_kind, int src_kind,
		bool may_require_tmp, int *stat);

	/**
	 * @brief      Called to get an array section or a whole array from a remote, image identified by the image_index.
	 *
	 * @param[in]  token            The token identifying the coarray
	 * @param[in]  offset           The offset by which amount of bytes the actual data is shifted compared to the base address of the coarray.
	 * @param[in]  image_index      The image index of the remote image.
	 * @param      src              Array descriptor for the remote image for the bounds and the size. The base_addr shall not be accessed.
	 * @param      src_vector       If not NULL, it contains the vector subscript of the source array; the values are relative to the dimension triplet of the src argument.
	 * @param      dest             Array descriptor of the local array to store the data retrieved from the remote image
	 * @param[in]  src_kind         The kind of the source argument
	 * @param[in]  dst_kind         The kind of the destination argument
	 * @param[in]  may_require_tmp  The variable is false when it is known at compile time that the dest and src either cannot overlap or overlap (fully or partially) such that walking src and dest in element wise element order (honoring the stride value) will not lead to wrong results. Otherwise, the value is true.
	 * @param      stat             When non-NULL give the result of the operation, i.e., zero on success and non-zero on error. When NULL and an error occurs, then an error message is printed and the program is terminated.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_get(caf_token_t token, size_t offset, int image_index,
		gfc_descriptor_t *src, caf_vector_t *src_vector,
		gfc_descriptor_t *dest, int src_kind, int dst_kind,
		bool may_require_tmp, int *stat);
	
	/**
	 * @brief      Called to send a scalar, an array section or a whole array from a remote image identified by the src_image_index to a remote image identified by the dst_image_index. 
	 *
	 * @param[in]  dst_token        The token identifying the destination coarray
	 * @param[in]  dst_offset       By which amount of bytes the actual data is shifted compared to the base address of the destination coarray
	 * @param[in]  dst_image_index  The destination image index
	 * @param      dest             Array descriptor for the destination remote image for the bounds and the size. The base_addr shall not be accessed.
	 * @param      dst_vector       If not NULL, it contains the vector subscript of the destination array; the values are relative to the dimension triplet of the dest argument.
	 * @param[in]  src_token        The token identifying the source coarray
	 * @param[in]  src_offset       By which amount of bytes the actual data is shifted compared to the base address of the source coarray.
	 * @param[in]  src_image_index  The source image index
	 * @param      src              Array descriptor of the local array to be transferred to the remote image.
	 * @param      src_vector       Array descriptor of the local array to be transferred to the remote image
	 * @param[in]  dst_kind         Kind of the destination argument
	 * @param[in]  src_kind         Kind of the source argument
	 * @param[in]  may_require_tmp  The variable is false when it is known at compile time that the dest and src either cannot overlap or overlap (fully or partially) such that walking src and dest in element wise element order (honoring the stride value) will not lead to wrong results. Otherwise, the value is true.
	 * @param      stat             When non-NULL give the result of the operation, i.e., zero on success and non-zero on error. When NULL and an error occurs, then an error message is printed and the program is terminated.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_sendget(caf_token_t dst_token, size_t dst_offset,
		int dst_image_index, gfc_descriptor_t *dest,
		caf_vector_t *dst_vector, caf_token_t src_token,
		size_t src_offset, int src_image_index,
		gfc_descriptor_t *src, caf_vector_t *src_vector,
		int dst_kind, int src_kind, bool may_require_tmp,
		int *stat);

	/**
	 * @brief      Called to send a scalar, an array section or a whole array, with enhanced referencing options, from a local to a remote image identified by the image_index.
	 *
	 * @param[in]  token              The token identifying the coarray
	 * @param[in]  image_index        The image index of the remote image.
	 * @param      src                Array descriptor of the local array to be transferred to the remote image
	 * @param      refs               The references on the remote array to store the data given by src. Guaranteed to have at least one entry.
	 * @param[in]  dst_kind           Kind of the destination argument
	 * @param[in]  src_kind           Kind of the source argument
	 * @param[in]  may_require_tmp    The variable is false when it is known at compile time that the dest and src either cannot overlap or overlap (fully or partially) such that walking src and dest in element wise element order (honoring the stride value) will not lead to wrong results. Otherwise, the value is true.
	 * @param[in]  dst_reallocatable  Set when the destination is of allocatable or pointer type and the refs will allow reallocation, i.e., the ref is a full array or component ref.
	 * @param      stat               When non-NULL give the result of the operation, i.e., zero on success and non-zero on error. When NULL and an error occurs, then an error message is printed and the program is terminated.
	 * @param[in]  dst_type           Give the type of the destination. When the destination is not an array, than the precise type, e.g. of a component in a derived type, is not known, but provided here.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_send_by_ref(caf_token_t token, int image_index,
		gfc_descriptor_t *src, caf_reference_t *refs,
		int dst_kind, int src_kind, bool may_require_tmp,
		bool dst_reallocatable, int *stat, int dst_type);

	/**
	 * @brief      Called to get a scalar, an array section or a whole array, with enhanced referencing options, from a remote image identified by the image_index. 
	 *
	 * @param[in]  token              The token identifying the coarray
	 * @param[in]  image_index        The image index of the remote image.
	 * @param      refs               The references to apply to the remote structure to get the data.
	 * @param      dst                Array descriptor of the local array to store the data transferred from the remote image. May be reallocated where needed and when DST_REALLOCATABLE allows it.
	 * @param[in]  dst_kind           Kind of the destination argument
	 * @param[in]  src_kind           Kind of the source argument
	 * @param[in]  may_require_tmp    The variable is false when it is known at compile time that the dest and src either cannot overlap or overlap (fully or partially) such that walking src and dest in element wise element order (honoring the stride value) will not lead to wrong results. Otherwise, the value is true.
	 * @param[in]  dst_reallocatable  Set when DST is of allocatable or pointer type and its refs allow reallocation, i.e., the full array or a component is referenced.
	 * @param      stat               When non-NULL give the result of the operation, i.e., zero on success and non-zero on error. When NULL and an error occurs, then an error message is printed and the program is terminated.
	 * @param[in]  src_type           Give the type of the source. When the source is not an array, than the precise type, e.g. of a component in a derived type, is not known, but provided here.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_get_by_ref(caf_token_t token, int image_index,
		caf_reference_t *refs, gfc_descriptor_t *dst,
		int dst_kind, int src_kind, bool may_require_tmp,
		bool dst_reallocatable, int *stat, int src_type);
	
	/**
	 * @brief      Called to send a scalar, an array section or a whole array, whith enhanced referencing options, from a remote image identified by the src_image_index to a remote image identified by the dst_image_index.
	 *
	 * @param[in]  dst_token        The token identifying the destination coarray
	 * @param[in]  dst_image_index  The ID of the destination remote image; must be a positive number.
	 * @param      dst_refs         The references on the remote array to store the data given by the source. Guaranteed to have at least one entry.
	 * @param[in]  src_token        The token identifying the source coarray
	 * @param[in]  src_image_index  The ID of the source remote image; must be a positive number.
	 * @param      src_refs         The references to apply to the remote structure to get the data.
	 * @param[in]  dst_kind         Kind of the destination argument
	 * @param[in]  src_kind         Kind of the source argument
	 * @param[in]  may_require_tmp  The variable is false when it is known at compile time that the dest and src either cannot overlap or overlap (fully or partially) such that walking src and dest in element wise element order (honoring the stride value) will not lead to wrong results. Otherwise, the value is true.
	 * @param      dst_stat         When non-NULL give the result of the send-operation, i.e., zero on success and non-zero on error. When NULL and an error occurs, then an error message is printed and the program is terminated.
	 * @param      src_stat         When non-NULL give the result of the get-operation, i.e., zero on success and non-zero on error. When NULL and an error occurs, then an error message is printed and the program is terminated.
	 * @param[in]  dst_type         Give the type of the destination. When the destination is not an array, than the precise type, e.g. of a component in a derived type, is not known, but provided here.
	 * @param[in]  src_type         Give the type of the source. When the source is not an array, than the precise type, e.g. of a component in a derived type, is not known, but provided here.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_sendget_by_ref(caf_token_t dst_token, int dst_image_index,
		caf_reference_t *dst_refs,
		caf_token_t src_token, int src_image_index,
		caf_reference_t *src_refs, int dst_kind,
		int src_kind, bool may_require_tmp,
		int *dst_stat, int *src_stat, int dst_type,
		int src_type);

	/**
	 * @brief      Acquire a lock on the given image on a scalar locking variable or for the given array element for an array-valued variable. If the aquired_lock is NULL, the function returns after having obtained the lock. If it is non-NULL, then acquired_lock is assigned the value true (one) when the lock could be obtained and false (zero) otherwise. Locking a lock variable which has already been locked by the same image is an error.
	 *
	 * @param[in]  token         The token identifying the coarray
	 * @param[in]  index         Array index; first array index is 0. For scalars, it is always 0.
	 * @param[in]  image_index   The ID of the remote image; must be a positive number.
	 * @param      aquired_lock  If not NULL, it returns whether lock could be obtained.
	 * @param      stat          Stores the STAT=; may be NULL.
	 * @param      errmsg        When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len    The buffer size of errmsg
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_lock(caf_token_t token, size_t index, int image_index,
		int *aquired_lock, int *stat, char *errmsg,
		size_t errmsg_len);

	/**
	 * @brief      Release a lock on the given image on a scalar locking variable or for the given array element for an array-valued variable. Unlocking a lock variable which is unlocked or has been locked by a different image is an error. 
	 *
	 * @param[in]  token        The token identifying the coarray
	 * @param[in]  index        Array index; first array index is 0. For scalars, it is always 0.
	 * @param[in]  image_index  The ID of the remote image; must be a positive number.
	 * @param      stat         For allocatable coarrays, stores the STAT=; may be NULL.
	 * @param      errmsg       When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len   The buffer size of errmsg
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_unlock(caf_token_t token, size_t index, int image_index,
		int *stat, char *errmsg, size_t errmsg_len);

	/**
	 * @brief      Increment the event count of the specified event variable. 
	 *
	 * @param[in]  token        The token identifying the coarray
	 * @param[in]  index        Array index; first array index is 0. For scalars, it is always 0.
	 * @param[in]  image_index  The ID of the remote image; must be a positive number; zero indicates the current image, when accessed noncoindexed.
	 * @param      stat         Stores the STAT=; may be NULL.
	 * @param      errmsg       When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len   The buffer size of errmsg
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_event_post(caf_token_t token, size_t index, int image_index,
		int *stat, char *errmsg, size_t errmsg_len);
	
	/**
	 * @brief      Wait until the event count has reached at least the specified until_count; if so, atomically decrement the event variable by this amount and return. 
	 *
	 * @param[in]  token        The token identifying the coarray
	 * @param[in]  index        Array index; first array index is 0. For scalars, it is always 0.
	 * @param[in]  until_count  The number of events which have to be available before the function retur
	 * @param      stat         Stores the STAT=; may be NULL.
	 * @param      errmsg       When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len   The buffer size of errmsg
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_event_wait(caf_token_t token, size_t index, int until_count,
		int *stat, char *errmsg, size_t errmsg_len);
	
	/**
	 * @brief      Return the event count of the specified event variable. 
	 *
	 * @param[in]  token        The token identifying the coarray
	 * @param[in]  index        Array index; first array index is 0. For scalars, it is always 0.
	 * @param[in]  image_index  The ID of the remote image; must be a positive number; zero indicates the current image when accessed noncoindexed.
	 * @param      count        The number of events currently posted to the event variable.
	 * @param      stat         Stores the STAT=; may be NULL.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_event_query(caf_token_t token, size_t index, int image_index,
		int *count, int *stat);

	/**
	 * @brief      Synchronization of all images in the current team; the program only continues on a given image after this function has been called on all images of the current team. Additionally, it ensures that all pending data transfers of previous segment have completed. 
	 *
	 * @param      stat        Stores the status STAT= and may be NULL.
	 * @param      errmsg      When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len  The buffer size of errmsg
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_sync_all(int *stat, char *errmsg, size_t errmsg_len);

	/**
	 * @brief      Synchronization between the specified images; the program only continues on a given image after this function has been called on all images specified for that image. Note that one image can wait for all other images in the current team (e.g. via sync images(*)) while those only wait for that specific image. Additionally, sync images ensures that all pending data transfers of previous segments have completed. 
	 *
	 * @param[in]  count       The number of images which are provided in the next argument. For a zero-sized array, the value is zero. For sync images (*), the value is -1.
	 * @param      images      An array with the images provided by the user. If count is zero, a NULL pointer is passed.
	 * @param      stat        Stores the status STAT= and may be NULL.
	 * @param      errmsg      When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len  The buffer size of errmsg
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_sync_images(int count, int images[], int *stat, char *errmsg,
		size_t errmsg_len);
	
	/**
	 * @brief      Acts as optimization barrier between different segments. It also ensures that all pending memory operations of this image have been completed. 
	 *
	 * @param      stat        Stores the status STAT= and may be NULL.
	 * @param      errmsg      When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len  The buffer size of errmsg
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_sync_memory(int *stat, char *errmsg, size_t errmsg_len);
	
	/**
	 * @brief      Invoked for an ERROR STOP statement which has an integer argument. The function should terminate the program with the specified exit code. 
	 *
	 * @param[in]  error   The exit status to be used.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_error_stop(int error);

	/**
	 * @brief      Invoked for an ERROR STOP statement which has a string as argument. The function should terminate the program with a nonzero-exit code. 
	 *
	 * @param[in]  string  The error message (not zero terminated)
	 * @param[in]  len     The length of the string
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_error_stop_str(const char *string, size_t len);

	/**
	 * @brief      Invoked for a FAIL IMAGE statement. The function should terminate the current image. 
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_fail_image(void);

	/**
	 * @brief      Assign atomically a value to an integer or logical variable. 
	 *
	 * @param[in]  token        The token identifying the coarray
	 * @param[in]  offset       By which amount of bytes the actual data is shifted compared to the base address of the coarray.
	 * @param[in]  image_index  The ID of the remote image; must be a positive number; zero indicates the current image when used noncoindexed.
	 * @param      value        The value to be assigned, passed by reference.
	 * @param      stat         Stores the status STAT= and may be NULL.
	 * @param[in]  type         The data type, i.e. BT_INTEGER (1) or BT_LOGICAL (2).
	 * @param[in]  kind         The kind value (only 4; always int)
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_atomic_define(caf_token_t token, size_t offset,
		int image_index, void *value, int *stat,
		int type, int kind);

	/**
	 * @brief      Reference atomically a value of a kind-4 integer or logical variable. 
	 *
	 * @param[in]  token        The token identifying the coarray
	 * @param[in]  offset       By which amount of bytes the actual data is shifted compared to the base address of the coarray.
	 * @param[in]  image_index  The ID of the remote image; must be a positive number; zero indicates the current image when used noncoindexed.
	 * @param      value        The variable assigned the atomically referenced variable.
	 * @param      stat         Stores the status STAT= and may be NULL.
	 * @param[in]  type         The data type, i.e. BT_INTEGER (1) or BT_LOGICAL (2).
	 * @param[in]  kind         The kind value (only 4; always int)
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_atomic_ref(caf_token_t token, size_t offset, int image_index,
		void *value, int *stat, int type, int kind);

	/**
	 * @brief      Atomic compare and swap of a kind-4 integer or logical variable. Assigns atomically the specified value to the atomic variable, if the latter has the value specified by the passed condition value. 
	 *
	 * @param[in]  token        The token identifying the coarray
	 * @param[in]  offset       By which amount of bytes the actual data is shifted compared to the base address of the coarray.
	 * @param[in]  image_index  The ID of the remote image; must be a positive number; zero indicates the current image when used noncoindexed.
	 * @param      old          The value which the atomic variable had just before the cas operation.
	 * @param      compare      The value used for comparision.
	 * @param      new_val      The new value for the atomic variable, assigned to the atomic variable, if compare equals the value of the atomic variable.
	 * @param      stat         Stores the status STAT= and may be NULL.
	 * @param[in]  type         The data type, i.e. BT_INTEGER (1) or BT_LOGICAL (2).
	 * @param[in]  kind         The kind value (only 4; always int)
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_atomic_cas(caf_token_t token, size_t offset, int image_index,
		void *old, void *compare, void *new_val,
		int *stat, int type, int kind);

	/**
	 * @brief      Apply an operation atomically to an atomic integer or logical variable. After the operation, old contains the value just before the operation, which, respectively, adds (GFC_CAF_ATOMIC_ADD) atomically the value to the atomic integer variable or does a bitwise AND, OR or exclusive OR between the atomic variable and value; the result is then stored in the atomic variable. 
	 *
	 * @param[in]  op           The operation to be performed; possible values GFC_CAF_ATOMIC_ADD (1), GFC_CAF_ATOMIC_AND (2), GFC_CAF_ATOMIC_OR (3), GFC_CAF_ATOMIC_XOR (4).
	 * @param[in]  token        The token identifying the coarray
	 * @param[in]  offset       By which amount of bytes the actual data is shifted compared to the base address of the coarray.
	 * @param[in]  image_index  The ID of the remote image; must be a positive number; zero indicates the current image when used noncoindexed.
	 * @param      value        The value which the atomic variable had just before the atomic operation.
	 * @param      old          The new value for the atomic variable, assigned to the atomic variable, if compare equals the value of the atomic variable.
	 * @param      stat         Stores the status STAT= and may be NULL.
	 * @param[in]  type         The data type, i.e. BT_INTEGER (1) or BT_LOGICAL (2).
	 * @param[in]  kind         The kind value (only 4; always int)
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_atomic_op(int op, caf_token_t token, size_t offset,
		int image_index, void *value, void *old, int *stat,
		int type, int kind);

	/**
	 * @brief      Distribute a value from a given image to all other images in the team. Has to be called collectively. 
	 *
	 * @param      a             An array descriptor with the data to be broadcasted (on source_image) or to be received (other images).
	 * @param[in]  source_image  The ID of the image from which the data should be broadcasted.
	 * @param      stat          Stores the status STAT= and may be NULL.
	 * @param      errmsg        When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len    The buffer size of errmsg.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_co_broadcast(gfc_descriptor_t *a, int source_image,
		int *stat, char *errmsg, size_t errmsg_len);

	/**
	 * @brief      Calculates for each array element of the variable a the maximum value for that element in the current team; if result_image has the value 0, the result shall be stored on all images, otherwise, only on the specified image. This function operates on numeric values and character strings.
	 *
	 * @param      a             An array descriptor for the data to be processed. On the destination image(s) the result overwrites the old content.
	 * @param[in]  result_image  The ID of the image to which the reduced value should be copied to; if zero, it has to be copied to all images.
	 * @param      stat          Stores the status STAT= and may be NULL.
	 * @param      errmsg        When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  a_len         The string length of argument a
	 * @param[in]  errmsg_len    The buffer size of errmsg.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_co_max(gfc_descriptor_t *a, int result_image, int *stat,
		char *errmsg, int a_len, size_t errmsg_len);

	/**
	 * @brief      Calculates for each array element of the variable a the minimum value for that element in the current team; if result_image has the value 0, the result shall be stored on all images, otherwise, only on the specified image. This function operates on numeric values and character strings.
	 *
	 * @param      a             An array descriptor for the data to be processed. On the destination image(s) the result overwrites the old content.
	 * @param[in]  result_image  The ID of the image to which the reduced value should be copied to; if zero, it has to be copied to all images.
	 * @param      stat          Stores the status STAT= and may be NULL.
	 * @param      errmsg        When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  a_len         The string length of argument a
	 * @param[in]  errmsg_len    The buffer size of errmsg.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_co_min(gfc_descriptor_t *a, int result_image, int *stat,
		char *errmsg, int a_len, size_t errmsg_len);

	/**
	 * @brief      Calculates for each array element of the variable a the sum of all values for that element in the current team; if result_image has the value 0, the result shall be stored on all images, otherwise, only on the specified image. This function operates on numeric values only.
	 *
	 * @param      a             An array descriptor with the data to be processed. On the destination image(s) the result overwrites the old content.
	 * @param[in]  result_image  The ID of the image to which the reduced value should be copied to; if zero, it has to be copied to all images.
	 * @param      stat          Stores the status STAT= and may be NULL.
	 * @param      errmsg        When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len    The buffer size of errmsg.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_co_sum(gfc_descriptor_t *a, int result_image, int *stat,
		char *errmsg, size_t errmsg_len);

	/**
	 * @brief      Calculates for each array element of the variable a the reduction value for that element in the current team; if result_image has the value 0, the result shall be stored on all images, otherwise, only on the specified image. The opr is a pure function doing a mathematically commutative and associative operation.
	 *
	 * @param      a             An array descriptor with the data to be processed. On the destination image(s) the result overwrites the old content.
	 * @param      opr           Function pointer to the reduction function
	 * @param[in]  opr_flags     Flags regarding the reduction function
	 * @param[in]  result_image  The ID of the image to which the reduced value should be copied to; if zero, it has to be copied to all images.
	 * @param      stat          Stores the status STAT= and may be NULL.
	 * @param      errmsg        When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  a_len         The string length of argument a
	 * @param[in]  errmsg_len    The buffer size of errmsg.
	 * 
	 * @author     OpenCoarrays
	 */
	void _gfortran_caf_co_reduce(gfc_descriptor_t *a, void *(*opr)(void *, void *),
		int opr_flags, int result_image, int *stat,
		char *errmsg, int a_len, size_t errmsg_len);
}

/**
 * @brief      Simple link calling the OpenCoarray init function
 *
 * @param      argc  An integer pointer with the number of arguments passed to the program or NULL.
 * @param      argv  A pointer to an array of strings with the command-line arguments or NULL.
 * 
 * @see        _gfortran_caf_init (int *argc, char ***argv)
 * 
 * @author     Jerome LEKIEFFRE
 */
inline void opencoarrays_caf_init(int *argc,char ***argv) {
	_gfortran_caf_init(argc, argv);
}

/**
 * @brief      Simple link calling the OpenCoarray sync_all function
 *
 * @param      stat        Stores the status STAT= and may be NULL.
 * @param      errmsg      When an error occurs, this will be set to an error message; may be NULL.
 * @param[in]  errmsg_len  The buffer size of errmsg
 * 
 * @see        _gfortran_caf_sync_all (int *stat, char *errmsg, size_t errmsg_len)
 * 
 * @author     Jerome LEKIEFFRE
 */
inline void opencoarrays_sync_all(int *stat, char errmsg[], size_t errmsg_len) {
	_gfortran_caf_sync_all(stat, errmsg, errmsg_len);
}

/**
 * @brief      Simple link calling the OpenCoarray error_stop function to halt the program
 *
 * @param[in]  error  The error code
 *  
 * @see        _gfortran_caf_error_stop (int error)
 * 
 * @author     Jerome LEKIEFFRE
 */
inline void opencoarrays_error_stop(int32_t error = -1) {
	_gfortran_caf_error_stop(error);
}

/**
 * @brief      Link calling the OpenCoarraus
 *
 * @param[in]  token            The token identifying the Coarray
 * @param[in]  offset           The number of bytes by which the data is shifted compare to the base address of the Coarray
 * @param[in]  image_index      The remote image index
 * @param      src              Fortran Array descriptor to describe the bounds and the size of the required data. Accessing its Base_addr may cause issues.
 * @param      src_vector       Can be NULL. Contains the Vector subscript of the source array
 * @param      dest             Fortran Array descriptor of the local array to store the incomming value
 * @param[in]  src_kind         Kind of the source argument (4 = Integer)
 * @param[in]  dst_kind         Kind of the destination argument (4 = Integer)
 * @param[in]  may_require_tmp  Set to false when you are sure that dest and src either cannot overlap or overlap such that walking src and dest in element wise element order will not lead to wrong results. Otherwise true.
 * @param      stat             When non-NULL give the result of the operation, i.e., zero on success and non-zero on error. When NULL and an error occurs, then an error message is printed and the program is terminated.
 * 
 * @see        _gfortran_caf_get (caf_token_t token, size_t offset, int image_index, gfc_descriptor_t *src, caf_vector_t *src_vector, gfc_descriptor_t *dest, int src_kind, int dst_kind, bool may_require_tmp, int *stat)
 * 
 * @author     Jerome LEKIEFFRE
 */
inline void opencoarrays_get(caf_token_t token, size_t offset, int image_index,
	gfc_descriptor_t *src, caf_vector_t *src_vector,
	gfc_descriptor_t *dest, int src_kind, int dst_kind,
	bool may_require_tmp, int *stat) {
	_gfortran_caf_get(token, offset, image_index, src, src_vector, dest,
		src_kind, dst_kind, may_require_tmp, stat);
}

/**
 * Namespace to encapsulate Coarraycpp specific functions
 * @author     Jerome LEKIEFFRE
 */
namespace coarraycpp {

	/**
	 * @brief      Initialises the Coarray environment
	 *
	 * @param      argc  The argc
	 * @param      argv  The argv
	 * 
	 * @see        opencoarrays_caf_init(int *argc,char ***argv)
	 * @see        _gfortran_caf_init (int *argc, char ***argv)
	 * 
	 * @author     Jerome LEKIEFFRE
	 */
	void caf_init(int *argc, char ***argv);

	/**
	 * @brief      Terminates the Coarray environment
	 * 
	 * @see        _gfortran_caf_finalize()
	 * 
	 * @author     Jerome LEKIEFFRE
	 */
	void caf_finalize();

	/**
	 * @brief      Returns the image ID (MPI rank + 1)
	 *
	 * @return     The image ID
	 * 
	 * @author     Jerome LEKIEFFRE
	 */
	int this_image();

	/**
	 * @brief      Returns the amount of images of the current team
	 *
	 * @return     Number of images of the current team
	 * 
	 * @author     Jerome LEKIEFFRE
	 */
	int num_images();

	/**
	 * @brief      Halts the program with the error code given
	 *
	 * @param[in]  stop_code  The stop code
	 * 
	 * @see        opencoarrays_error_stop(int32_t error)
	 * @see        _gfortran_caf_error_stop (int error)
	 * 
	 * @author     Jerome LEKIEFFRE
	 */
	void error_stop(int32_t stop_code);

	/**
	 * @brief      Imposes a global execution barrier
	 *
	 * @param      stat        Stores the status STAT= and may be NULL.
	 * @param      errmsg      When an error occurs, this will be set to an error message; may be NULL.
	 * @param[in]  errmsg_len  The buffer size of errmsg
	 * 
	 * @see        opencoarrays_sync_all(int *stat, char errmsg[], size_t errmsg_len)
	 * @see        _gfortran_caf_sync_all (int *stat, char *errmsg, size_t errmsg_len)
	 * 
	 * @author     Jerome LEKIEFFRE
	 */
	void sync_all(int *stat = NULL, char errmsg[] = NULL, size_t errmsg_len = 0);

	/**
	 * @brief      Class for coarray Object.
	 *
	 * @tparam     T     Data type of the Coarray
	 * 
	 * @author     Jerome LEKIEFFRE
	 */
	template <class T>
	class coarray {
	private:
		/* PARAMETERS */
		/**
		 * Actual local value of the Coarray
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		T value;
	public:

		/**
		 * @brief      Constructs the Coarray.
		 * 
		 * @see
		 */
		coarray();

		/**
		 * @brief      Destroys the object.
		 */
		~coarray();

		/* PARAMETERS */

		/**
		 * Either the byte size of the coarray or for lock and event types the nb of elements
		 */
		size_t size;
		
		/**
		 * The type of the coarray
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		caf_register_t type;
		
		/**
		 * ID of the coarray
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		caf_token_t token;
		
		/**
		 * Actual status of the coarray
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		int stat;
		
		/* SPECIFIC MEHODS */
		/**
		 * @brief      Allows setting of the local value of a Coarray.
		 *
		 * @param[in]  value  The value
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		void operator=(const T& value);
		
		/**
		 * @brief      Allow the copy of a Coarray
		 *
		 * @param[in]  coarray  The coarray
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		void operator=(const coarray<T>& coarray);
		
		/**
		 * @brief      Gets the value.
		 *
		 * @return     The value.
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		T get_value();

		/**
		 * @brief      Gets the from.
		 *
		 * @param[in]  image_index  The image index
		 *
		 * @return     The from.
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		T get_from(int image_index);
		
		/**
		 * Fortran Array descriptor of the coarray value
		 * 
		 * @author     Jerome LEKIEFFRE
		 */
		gfc_descriptor_t descriptor;
	}; // end of class coarray

}  // end of namespace coarraycpp

#endif