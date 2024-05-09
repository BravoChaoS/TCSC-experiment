#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */

#include "stdio.h"
#include "sgx_tgmp.h"
#include "time.h"
#include "sgx_tseal.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OCALL_GET_TIME_DEFINED__
#define OCALL_GET_TIME_DEFINED__
long int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_get_time, (void));
#endif

sgx_status_t ecall_main(sgx_enclave_id_t eid, long int* retval, int x, int lim);
sgx_status_t ecall_test_large_input(sgx_enclave_id_t eid, long int* retval, long int input_size, uint8_t* input);
sgx_status_t ecall_test_large_epc(sgx_enclave_id_t eid, long int* retval, long int size);
sgx_status_t ecall_test_parallel(sgx_enclave_id_t eid, long int* retval, int n, unsigned int* input, unsigned int* output);
sgx_status_t ecall_test_non_parallel(sgx_enclave_id_t eid, unsigned int* retval, unsigned int uia);
sgx_status_t ecall_compute_intensive_task_with_gmp(sgx_enclave_id_t eid, unsigned int* retval);
sgx_status_t ecall_empty(sgx_enclave_id_t eid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
