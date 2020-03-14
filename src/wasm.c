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

unsigned char fib32_wasm[] = {
  0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x06, 0x01, 0x60,
  0x01, 0x7f, 0x01, 0x7f, 0x03, 0x02, 0x01, 0x00, 0x07, 0x07, 0x01, 0x03,
  0x66, 0x69, 0x62, 0x00, 0x00, 0x0a, 0x1f, 0x01, 0x1d, 0x00, 0x20, 0x00,
  0x41, 0x02, 0x49, 0x04, 0x40, 0x20, 0x00, 0x0f, 0x0b, 0x20, 0x00, 0x41,
  0x02, 0x6b, 0x10, 0x00, 0x20, 0x00, 0x41, 0x01, 0x6b, 0x10, 0x00, 0x6a,
  0x0f, 0x0b
};
unsigned int fib32_wasm_len = 62;

void
f_wasmeval(typval_T *argvars, typval_T *rettv)
{
    M3Result result = m3Err_none;

    uint8_t* wasm = (uint8_t*)fib32_wasm;
    size_t fsize = fib32_wasm_len-1;

    IM3Environment env = m3_NewEnvironment();
    IM3Runtime runtime = m3_NewRuntime(env, 8192, NULL);

    IM3Module module;
    result = m3_ParseModule(env, &module, wasm, fsize);
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
