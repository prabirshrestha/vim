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

#include "wasm3/source/wasm3.h"

void
f_wasmeval(typval_T *argvars, typval_T *rettv)
{
    return;
}

#endif
