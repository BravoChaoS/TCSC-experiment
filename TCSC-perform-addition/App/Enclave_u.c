#include "Enclave_u.h"
#include <errno.h>

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

static sgx_status_t SGX_CDECL Enclave_ocall_get_time(void* pms)
{
	ms_ocall_get_time_t* ms = SGX_CAST(ms_ocall_get_time_t*, pms);
	ms->ms_retval = ocall_get_time();

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Enclave = {
	1,
	{
		(void*)Enclave_ocall_get_time,
	}
};
sgx_status_t ecall_main(sgx_enclave_id_t eid, long int* retval, int x, int lim)
{
	sgx_status_t status;
	ms_ecall_main_t ms;
	ms.ms_x = x;
	ms.ms_lim = lim;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_test_large_input(sgx_enclave_id_t eid, long int* retval, long int input_size, uint8_t* input)
{
	sgx_status_t status;
	ms_ecall_test_large_input_t ms;
	ms.ms_input_size = input_size;
	ms.ms_input = input;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_test_large_epc(sgx_enclave_id_t eid, long int* retval, long int size)
{
	sgx_status_t status;
	ms_ecall_test_large_epc_t ms;
	ms.ms_size = size;
	status = sgx_ecall(eid, 2, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_test_parallel(sgx_enclave_id_t eid, long int* retval, int n, unsigned int* input, unsigned int* output)
{
	sgx_status_t status;
	ms_ecall_test_parallel_t ms;
	ms.ms_n = n;
	ms.ms_input = input;
	ms.ms_output = output;
	status = sgx_ecall(eid, 3, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_test_non_parallel(sgx_enclave_id_t eid, unsigned int* retval, unsigned int uia)
{
	sgx_status_t status;
	ms_ecall_test_non_parallel_t ms;
	ms.ms_uia = uia;
	status = sgx_ecall(eid, 4, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_compute_intensive_task_with_gmp(sgx_enclave_id_t eid, unsigned int* retval)
{
	sgx_status_t status;
	ms_ecall_compute_intensive_task_with_gmp_t ms;
	status = sgx_ecall(eid, 5, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_empty(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 6, &ocall_table_Enclave, NULL);
	return status;
}

