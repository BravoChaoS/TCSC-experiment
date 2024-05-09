#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sgx_tgmp.h"

# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"


/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
        {
                SGX_ERROR_UNEXPECTED,
                "Unexpected error occurred.",
                NULL
        },
        {
                SGX_ERROR_INVALID_PARAMETER,
                "Invalid parameter.",
                NULL
        },
        {
                SGX_ERROR_OUT_OF_MEMORY,
                "Out of memory.",
                NULL
        },
        {
                SGX_ERROR_ENCLAVE_LOST,
                "Power transition occurred.",
                "Please refer to the sample \"PowerTransition\" for details."
        },
        {
                SGX_ERROR_INVALID_ENCLAVE,
                "Invalid enclave image.",
                NULL
        },
        {
                SGX_ERROR_INVALID_ENCLAVE_ID,
                "Invalid enclave identification.",
                NULL
        },
        {
                SGX_ERROR_INVALID_SIGNATURE,
                "Invalid enclave signature.",
                NULL
        },
        {
                SGX_ERROR_OUT_OF_EPC,
                "Out of EPC memory.",
                NULL
        },
        {
                SGX_ERROR_NO_DEVICE,
                "Invalid SGX device.",
                "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
        },
        {
                SGX_ERROR_MEMORY_MAP_CONFLICT,
                "Memory map conflicted.",
                NULL
        },
        {
                SGX_ERROR_INVALID_METADATA,
                "Invalid enclave metadata.",
                NULL
        },
        {
                SGX_ERROR_DEVICE_BUSY,
                "SGX device was busy.",
                NULL
        },
        {
                SGX_ERROR_INVALID_VERSION,
                "Enclave version was invalid.",
                NULL
        },
        {
                SGX_ERROR_INVALID_ATTRIBUTE,
                "Enclave was not authorized.",
                NULL
        },
        {
                SGX_ERROR_ENCLAVE_FILE_ACCESS,
                "Can't open enclave file.",
                NULL
        },
};


void test_cpu_performance_with_gmp();
void test_tee_performance_with_gmp();

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret) {
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist / sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if (ret == sgx_errlist[idx].err) {
            if (NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }

    if (idx == ttl)
        printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n",
               ret);
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(void) {
    printf("Starting initialize enclave\n");
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;

    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        return -1;
    }

    printf("Creating enclave succeed\n");

    return 0;
}

/* OCall functions */
long ocall_get_time() {
    long t = clock();
//    printf("ocall: %ld\n", t);
    return t;
}

void test_large_input() {
    sgx_status_t ret;
    clock_t s, e, t;
    long inside_t;

    long begin, end, step, loop;
    begin = 1, end = 70, step = 2e6;
    clock_t inside_t_arr[end + 10], t_arr[end + 10];

    long input_size = end * step;

    uint8_t *input;
    input = (uint8_t *) malloc(input_size);
    for (long i = 0; i < input_size; ++i) {
        input[i] = i % 128;
    }

    for (long j = begin; j <= end; ++j) {
        input_size = j * step;
//        printf("%d\n", input_size);

        s = clock();
        ret = ecall_test_large_input(global_eid, &inside_t, input_size, input);
        e = clock();
        t = e - s;

        if (ret != SGX_SUCCESS) {
            print_error_message(ret);
            break;
        } else {
            if (j % 10 == 0)
                printf("size=%ld: (external)%ld - (internal)%ld = %ld\n", input_size, t, inside_t, t - inside_t);
            inside_t_arr[j] = inside_t;
            t_arr[j] = t;
        }
    }

    if (ret == SGX_SUCCESS) {
        printf("ecall_test_large_input_external(μs): \n");
        printf("input size,enclave transition(μs)\n");
        for (long j = begin; j <= end; ++j) {
            input_size = j * step;
            printf("(%ld,%ld)", input_size, t_arr[j]);
        }
        printf("\n");

        printf("ecall_test_large_input_internal(μs): \n");
        printf("input size,enclave transition(μs)\n");
        for (long j = begin; j <= end; ++j) {
            input_size = j * step;
            printf("(%ld,%ld)", input_size, inside_t_arr[j]);
        }
        printf("\n");


    }

}

// more enclaves? large enclave?
void test_large_epc() {

    sgx_status_t ret;
    clock_t s, e, t;
    long tee_t;

    int begin, end, step, size;
    begin = 1, end = 70, step = 2e6;
    clock_t tee_arr[end + 10], cpu_arr[end + 10];

    uint8_t *arr = (uint8_t *) malloc(end * step);

    for (int j = begin; j <= end; ++j) {
        size = j * step;
//        printf("%d\n", size);

        s = clock();

        long sum = 0;
        for (int i = 0; i < size; ++i) {
            arr[i] = arr[size - i] = i & 7;
            sum += arr[i];
            sum %= size;
        }
        e = clock();
        t = sum + e;
        t -= sum + s;

        ret = ecall_test_large_epc(global_eid, &tee_t, size);

        if (ret != SGX_SUCCESS) {
            print_error_message(ret);
        } else {
            if (j % 10 == 0) printf("size=%ld: (tee)%ld - (cpu)%ld = %ld\n", size, tee_t, t, tee_t - t);
            tee_arr[j] = tee_t;
            cpu_arr[j] = t;
        }
    }

    if (ret == SGX_SUCCESS) {
        printf("ecall_test_large_epc_tee(μs): \n");
        printf("input size,enclave transition(μs)\n");
        for (int j = begin; j <= end; ++j) {
            size = j * step;
            printf("(%d,%ld)", size, tee_arr[j]);
        }
        printf("\n");

        printf("ecall_test_large_epc_cpu(μs): \n");
        printf("input size,enclave transition(μs)\n");
        for (int j = begin; j <= end; ++j) {
            size = j * step;
            printf("(%d,%ld)", size, cpu_arr[j]);
        }
        printf("\n");
    }

}

long net_overload = 500000; // 1s

void test_long_time_execution();

void test_parallel() {
    sgx_status_t ret;
    clock_t s, e, t;

    int begin, end, delt;
    begin = delt = 1, end = 20;

    printf("ecall_test_parallel(ms): \n");
    printf("input size,enclave transition(ms)\n");

    int n = end;
    long v;

    unsigned int input[n + 10], output[n + 10];
    for (int i = 0; i < n; ++i) {
        input[i] = i + 10;
    }
//    s = clock();
//    ecall_test_parallel(global_eid, &v, n * 4, input, output);
//    e = clock();
//    t = e - s;
//    printf("%d,%ld\n", n, t + net_overload);

    for (int j = begin; j <= end; j += delt) {
        n = j;
//        printf("%d\n", n);

        s = clock();

        ret = ecall_test_parallel(global_eid, &v, n * 4, input, output);

        e = clock();
        t = e - s;

        if (ret != SGX_SUCCESS) {
            print_error_message(ret);
        } else {
//            printf("ecall_test_large_input(μs): (outside)%ld - (inside)%ld = %ld\n", t, inside_t, t - inside_t);
            printf("(%d,%lf)\n", n, n / (((double) t + net_overload) / CLOCKS_PER_SEC));
        }
    }
}

void test_non_parallel() {
    sgx_status_t ret;
    clock_t s, e, t;

    int begin, end, delt;
    begin = delt = 1, end = 20;

    printf("ecall_test_non_parallel(ms): \n");
    printf("input size,enclave transition(ms)\n");

    int n = end;

    unsigned int input[n + 10], output[n + 10];
    for (int i = 0; i < n; ++i) {
        input[i] = i + 10;
    }

    for (int j = begin; j <= end; j += delt) {
        n = j;

        t = 0;
        for (int i = 0; i < n; ++i) {
            s = clock();
            ret = ecall_test_non_parallel(global_eid, &output[i], input[i]);
            e = clock();
            t += e - s + net_overload;
        }


        if (ret != SGX_SUCCESS) {
            print_error_message(ret);
        } else {
//            printf("ecall_test_large_input(μs): (outside)%ld - (inside)%ld = %ld\n", t, inside_t, t - inside_t);
            printf("(%d,%lf)\n", n, n / ((double) t / CLOCKS_PER_SEC));
        }
    }
}


/* Application entry */
int SGX_CDECL main(int argc, char *argv[]) {
    (void) (argc);
    (void) (argv);


    /* Initialize the enclave */
    clock_t s, e;

    s = clock();
    if (initialize_enclave() < 0) {
        printf("Enter a character before exit ...\n");
        getchar();
        return -1;
    }
    e = clock();
    printf("%ld", e - s);

//    test_long_time_execution();
    test_large_input();
//    test_large_epc();
//    test_parallel();
//    test_non_parallel();
    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);

    printf("Info: SampleEnclave successfully returned.\n");

    return 0;
}


unsigned int compute_intensive_task_with_gmp() {
    mpz_t a, b, result;
    gmp_randstate_t state;
    unsigned int q = UINT32_MAX, out;

    // Initialize random state with a seed
    unsigned long int seed = time(NULL); // Use current time as seed
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

// 运行计算任务，并记录不同时间点的执行效率
void test_long_time_execution() {
    test_tee_performance_with_gmp();
    test_cpu_performance_with_gmp();
}

void test_cpu_performance_with_gmp() {
    int total_duration = 100; // 100秒
    int interval = 1; // 10秒
    time_t start_time, current_time, last_interval_time;
    int total_iterations = 0;
    int iterations_since_last_interval = 0;

    start_time = last_interval_time = time(NULL);

    do {
        compute_intensive_task_with_gmp();
        total_iterations++;
        iterations_since_last_interval++;

        current_time = time(NULL);
        if (difftime(current_time, last_interval_time) >= interval) {
            // 输出每10秒的执行效率
//            printf("Execution rate at %lf seconds: %d tasks/sec\n",
//                   difftime(current_time, start_time),
//                   iterations_since_last_interval / interval);

            printf("(%d,%d)",
                   (int)difftime(current_time, start_time),
                   iterations_since_last_interval / interval);

            // 重置迭代数和时间
            iterations_since_last_interval = 0;
            last_interval_time = current_time;
        }
    } while (difftime(current_time, start_time) < total_duration);

    // 输出最后一个间隔的执行效率，即使它不满10秒
    if (iterations_since_last_interval > 0) {
        int remaining_seconds = difftime(current_time, last_interval_time);
//        printf("Execution rate for the last %d seconds: %d tasks/sec\n",
//               remaining_seconds, iterations_since_last_interval / remaining_seconds);
        printf("(%d,%d)\n",
               remaining_seconds, iterations_since_last_interval / remaining_seconds);
    }

    printf("Total completed iterations: %d\n", total_iterations);
}

void test_tee_performance_with_gmp() {
    int total_duration = 100; // 100秒
    int interval = 1; // 10秒
    time_t start_time, current_time, last_interval_time;
    int total_iterations = 0;
    int iterations_since_last_interval = 0;

    start_time = last_interval_time = time(NULL);
    int ret = SGX_SUCCESS;
    unsigned int result;
    do {

        ret = ecall_compute_intensive_task_with_gmp(global_eid, &result);

        total_iterations++;
        iterations_since_last_interval++;

        current_time = time(NULL);
        if (difftime(current_time, last_interval_time) >= interval) {
            // 输出每10秒的执行效率
//            printf("Execution rate at %lf seconds: %d tasks/sec\n",
//                   difftime(current_time, start_time),
//                   iterations_since_last_interval / interval);

            printf("(%d,%d)",
                   (int)difftime(current_time, start_time),
                   iterations_since_last_interval / interval);

            // 重置迭代数和时间
            iterations_since_last_interval = 0;
            last_interval_time = current_time;
        }
    } while (difftime(current_time, start_time) < total_duration);

    // 输出最后一个间隔的执行效率，即使它不满10秒
    if (iterations_since_last_interval > 0) {
        int remaining_seconds = difftime(current_time, last_interval_time);
//        printf("Execution rate for the last %d seconds: %d tasks/sec\n",
//               remaining_seconds, iterations_since_last_interval / remaining_seconds);
        printf("(%d,%d)\n",
               remaining_seconds, iterations_since_last_interval / remaining_seconds);
    }

    printf("Total completed iterations: %d\n", total_iterations);
}


