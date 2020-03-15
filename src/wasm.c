/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * WASM interface by Prabir Shrestha
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
#include "vim.h"
#if defined(WASM) || defined(PROTO)
#include "wasm3.h"
#include "m3_api_wasi.h"
#include "m3_api_libc.h"
#include "m3_api_tracer.h"
#include "m3_env.h"

static IM3Environment wasm_get_environment();
static IM3Runtime wasm_get_runtime();

unsigned char fib32_wasm[] = {
  0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x06, 0x01, 0x60,
  0x01, 0x7f, 0x01, 0x7f, 0x03, 0x02, 0x01, 0x00, 0x07, 0x07, 0x01, 0x03,
  0x66, 0x69, 0x62, 0x00, 0x00, 0x0a, 0x1f, 0x01, 0x1d, 0x00, 0x20, 0x00,
  0x41, 0x02, 0x49, 0x04, 0x40, 0x20, 0x00, 0x0f, 0x0b, 0x20, 0x00, 0x41,
  0x02, 0x6b, 0x10, 0x00, 0x20, 0x00, 0x41, 0x01, 0x6b, 0x10, 0x00, 0x6a,
  0x0f, 0x0b
};
unsigned int fib32_wasm_len = 62;

/*
   let b = readfile('fib.wasm', 'B')
   let m = wasm_module_load(b)
   let f = wasm_func_get(m, 'fib')
   let result = wasm_func_call(f, ["10"])
   echom result
*/

static IM3Environment
wasm_get_environment() {
    static IM3Environment environment = NULL;

    if (environment == NULL) {
	environment = m3_NewEnvironment();
	if (environment == NULL) {
	    semsg("Failed to create WASM environment");
	}
    }

    return environment;
}

static IM3Runtime
wasm_get_runtime() {
    static IM3Runtime runtime = NULL;
    IM3Environment environment = wasm_get_environment();

    if (environment != NULL && runtime == NULL) {
	runtime = m3_NewRuntime(environment, 64 * 1024, NULL);
	if (runtime == NULL) {
	    semsg("Failed to create WASM runtime");
	}
    }

    return runtime;
}

void
f_wasmeval(typval_T *argvars, typval_T *rettv)
{
    M3Result result = m3Err_none;

    uint8_t* wasm = (uint8_t*)fib32_wasm;
    size_t fsize = fib32_wasm_len-1;

    IM3Environment environment = wasm_get_environment();
    if (environment == NULL) {
	return;
    }

    IM3Runtime runtime = wasm_get_runtime();
    if (runtime == NULL) {
	return;
    }

    IM3Module module;
    result = m3_ParseModule(environment, &module, wasm, fsize);
    result = m3_LoadModule(runtime, module);

    IM3Function f;
    result = m3_FindFunction(&f, runtime, "fib");

    const char* i_argv[2] = { "10", NULL };
    result = m3_CallWithArgs(f, 1, i_argv);

    long value = *(uint64_t*)(runtime->stack);

    semsg("wasm: %ld", value);

    return;
}

#endif
