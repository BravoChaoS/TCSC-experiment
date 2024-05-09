#include "Enclave.h"
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>
#include <stdlib.h>
#include <sgx_trts.h>

long ecall_main(int x, int lim){
    long s, e, t;
    mpz_t  a, b;
    mpz_init_set_si(a, 78839);
    mpz_init(b);
    uint8_t *mem;
    int size = 1000000; // 10,000,000
    mem = (uint8_t *) malloc(size);
    mem[lim] = 1;
    int q = (int)1e9 + 7;

    ocall_get_time(&s);
    while(lim--) {
//        mpz_pow_ui(b, a, x);
//        mpz_mod_ui(b, b, q);
        ocall_get_time(&t);
    }
    ocall_get_time(&e);
    return e - s;
}


long ecall_test_large_input(long input_size, uint8_t *input) {
    long s, e, ret;
    ocall_get_time(&s);
    long sum = 0;
    for (long i = 0; i < input_size; ++i) {
        sum += input[i];
        sum %= input_size;
    }
    ocall_get_time(&e);
    ret = sum + e;
    ret -= sum + s;
    return ret;
}

long ecall_test_large_epc(long size) {
    long s, e, ret;
    ocall_get_time(&s);
    long sum = 0;

    uint8_t arr[size + 10];

    for (int i = 0; i < size; ++i) {
        arr[i] = arr[size - i] = i & 7;
        sum += arr[i];
        sum %= size;
    }

    ocall_get_time(&e);
    ret = sum + e;
    ret -= sum + s;
    return ret;
}


long ecall_test_parallel(int size, unsigned int *input, unsigned int *output) {

    int n = size / 4;
    mpz_t a, b;
    mpz_init(a);
    mpz_init(b);
    unsigned int q = UINT32_MAX;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 1000; ++j) {
            mpz_set_ui(a, input[i]);
            mpz_pow_ui(b, a, 100000);
            mpz_mod_ui(b, b, q);
            mpz_export(&output[i], 0, -1, sizeof output[i], 0, 0, b);
        }
    }

    return 0;
}


unsigned int ecall_test_non_parallel(unsigned int uia) {

    mpz_t a, b;
    mpz_init(b);
    unsigned int q = UINT32_MAX, out;

    for (int j = 0; j < 1000; ++j) {
        mpz_init_set_ui(a, uia);
        mpz_pow_ui(b, a, 100000);
        mpz_mod_ui(b, b, q);
        mpz_export(&out, 0, -1, sizeof out, 0, 0, b);
    }

    return out;
}

unsigned int ecall_compute_intensive_task_with_gmp() {
    mpz_t a, b, result;
    gmp_randstate_t state;
    unsigned int q = UINT32_MAX, out;

    // Initialize random state with a seed
    unsigned long int seed;
    sgx_read_rand((unsigned char*)&seed, sizeof(seed));
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);

    // Initialize numbers and set random values
    mpz_inits(a, b, result, NULL);
    mpz_urandomb(a, state, 1024);  // Random number with 1024 bits
    mpz_urandomb(b, state, 1024);

    // Perform some arithmetic operations
    mpz_mul(result, a, b);
    mpz_add(result, a, b);
    mpz_sub(result, a, b);

    mpz_mod_ui(result, result, q);
    mpz_export(&out, 0, -1, sizeof out, 0, 0, result);

    // Clean up
    mpz_clears(a, b, result, NULL);
    gmp_randclear(state);
    return out;
}

void ecall_empty(){
    int a = 1 + 2;
}