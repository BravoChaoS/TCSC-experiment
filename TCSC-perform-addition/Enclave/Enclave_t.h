#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "stdio.h"
#include "sgx_tgmp.h"
#include "time.h"
#include "sgx_tseal.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

long int ecall_main(int x, int lim);
long int ecall_test_large_input(long int input_size, uint8_t* input);
long int ecall_test_large_epc(long int size);
long int ecall_test_parallel(int n, unsigned int* input, unsigned int* output);
unsigned int ecall_test_non_parallel(unsigned int uia);
unsigned int ecall_compute_intensive_task_with_gmp(void);
void ecall_empty(void);

sgx_status_t SGX_CDECL ocall_get_time(long int* retval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
