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

/*
 * usage:
 *
 *	let blob = readfile('./fib.wasm', 'B')
 *	let module = wasm_load(blob)
 *
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
f_wasm_load(typval_T *argvars, typval_T *rettv)
{
    blob_T *blob = NULL;
    M3Result result = m3Err_none;

    if (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	if (blob == NULL)
	    return;
    }

    void* wasm = blob->bv_ga.ga_data;
    size_t fsize = blob->bv_ga.ga_len - 1;

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
    if (result) {
	return;
    }
    result = m3_LoadModule(runtime, module);
    if (result) {
	return;
    }

    IM3Function f;
    result = m3_FindFunction(&f, runtime, "fib");

    const char* i_argv[2] = { "10", NULL };
    result = m3_CallWithArgs(f, 1, i_argv);
    if (result) {
	return;
    }

    // TODO: should return module instead

    long value = *(uint64_t*)(runtime->stack);

    rettv->vval.v_number = value;
}

#endif
