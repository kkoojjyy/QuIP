// all_mixed_float_kernels.m4 BEGIN - can be same precision or mixed precision

my_include(`veclib/fast_defs.m4')
my_include(`veclib/gen_mixed_float_calls.m4')

ifdef(`BUILD_FOR_CUDA',`
my_include(`veclib/flen_defs.m4')
my_include(`veclib/gen_mixed_float_calls.m4')
')
dnl what about elen and slen???

my_include(`veclib/eqsp_defs.m4')
my_include(`veclib/gen_mixed_float_calls.m4')

my_include(`veclib/slow_defs.m4')
my_include(`veclib/gen_mixed_float_calls.m4')

// all_mixed_float_kernels.m4 DONE

