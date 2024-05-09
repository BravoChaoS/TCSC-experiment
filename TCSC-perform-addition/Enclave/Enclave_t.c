#include "Enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */
#include "sgx_lfence.h" /* for sgx_lfence */

#include <errno.h>
#include <mbusafecrt.h> /* for memcpy_s etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_ENCLAVE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_within_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define ADD_ASSIGN_OVERFLOW(a, b) (	\
	((a) += (b)) < (b)	\
)


typedef struct ms_ecall_main_t {
	long int ms_retval;
	int ms_x;
	int ms_lim;
} ms_ecall_main_t;

typedef struct ms_ecall_test_large_input_t {
	long int ms_retval;
	long int ms_input_size;
	uint8_t* ms_input;
} ms_ecall_test_large_input_t;

typedef struct ms_ecall_test_large_epc_t {
	long int ms_retval;
	long int ms_size;
} ms_ecall_test_large_epc_t;

typedef struct ms_ecall_test_parallel_t {
	long int ms_retval;
	int ms_n;
	unsigned int* ms_input;
	unsigned int* ms_output;
} ms_ecall_test_parallel_t;

typedef struct ms_ecall_test_non_parallel_t {
	unsigned int ms_retval;
	unsigned int ms_uia;
} ms_ecall_test_non_parallel_t;

typedef struct ms_ecall_compute_intensive_task_with_gmp_t {
	unsigned int ms_retval;
} ms_ecall_compute_intensive_task_with_gmp_t;

typedef struct ms_ocall_get_time_t {
	long int ms_retval;
} ms_ocall_get_time_t;

static sgx_status_t SGX_CDECL sgx_ecall_main(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_main_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_main_t* ms = SGX_CAST(ms_ecall_main_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ms->ms_retval = ecall_main(ms->ms_x, ms->ms_lim);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_test_large_input(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_test_large_input_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_test_large_input_t* ms = SGX_CAST(ms_ecall_test_large_input_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	uint8_t* _tmp_input = ms->ms_input;
	long int _tmp_input_size = ms->ms_input_size;
	size_t _len_input = _tmp_input_size;
	uint8_t* _in_input = NULL;

	CHECK_UNIQUE_POINTER(_tmp_input, _len_input);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_input != NULL && _len_input != 0) {
		if ( _len_input % sizeof(*_tmp_input) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_input = (uint8_t*)malloc(_len_input);
		if (_in_input == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_input, _len_input, _tmp_input, _len_input)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}

	ms->ms_retval = ecall_test_large_input(_tmp_input_size, _in_input);

err:
	if (_in_input) free(_in_input);
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_test_large_epc(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_test_large_epc_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_test_large_epc_t* ms = SGX_CAST(ms_ecall_test_large_epc_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ms->ms_retval = ecall_test_large_epc(ms->ms_size);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_test_parallel(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_test_parallel_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_test_parallel_t* ms = SGX_CAST(ms_ecall_test_parallel_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	unsigned int* _tmp_input = ms->ms_input;
	int _tmp_n = ms->ms_n;
	size_t _len_input = _tmp_n;
	unsigned int* _in_input = NULL;
	unsigned int* _tmp_output = ms->ms_output;
	size_t _len_output = _tmp_n;
	unsigned int* _in_output = NULL;

	CHECK_UNIQUE_POINTER(_tmp_input, _len_input);
	CHECK_UNIQUE_POINTER(_tmp_output, _len_output);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_input != NULL && _len_input != 0) {
		if ( _len_input % sizeof(*_tmp_input) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_input = (unsigned int*)malloc(_len_input);
		if (_in_input == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_input, _len_input, _tmp_input, _len_input)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_output != NULL && _len_output != 0) {
		if ( _len_output % sizeof(*_tmp_output) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_output = (unsigned int*)malloc(_len_output)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_output, 0, _len_output);
	}

	ms->ms_retval = ecall_test_parallel(_tmp_n, _in_input, _in_output);
	if (_in_output) {
		if (memcpy_s(_tmp_output, _len_output, _in_output, _len_output)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_input) free(_in_input);
	if (_in_output) free(_in_output);
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_test_non_parallel(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_test_non_parallel_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_test_non_parallel_t* ms = SGX_CAST(ms_ecall_test_non_parallel_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ms->ms_retval = ecall_test_non_parallel(ms->ms_uia);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_compute_intensive_task_with_gmp(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_compute_intensive_task_with_gmp_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_compute_intensive_task_with_gmp_t* ms = SGX_CAST(ms_ecall_compute_intensive_task_with_gmp_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ms->ms_retval = ecall_compute_intensive_task_with_gmp();


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_empty(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_empty();
	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[7];
} g_ecall_table = {
	7,
	{
		{(void*)(uintptr_t)sgx_ecall_main, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_test_large_input, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_test_large_epc, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_test_parallel, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_test_non_parallel, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_compute_intensive_task_with_gmp, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_empty, 0, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[1][7];
} g_dyn_entry_table = {
	1,
	{
		{0, 0, 0, 0, 0, 0, 0, },
	}
};


sgx_status_t SGX_CDECL ocall_get_time(long int* retval)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_get_time_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_get_time_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_get_time_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_get_time_t));
	ocalloc_size -= sizeof(ms_ocall_get_time_t);

	status = sgx_ocall(0, ms);

	if (status == SGX_SUCCESS) {
		if (retval) *retval = ms->ms_retval;
	}
	sgx_ocfree();
	return status;
}

