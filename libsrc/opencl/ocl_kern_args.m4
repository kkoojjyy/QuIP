
include(`../../include/veclib/gen_kern_args.m4')

define(`SET_KERNEL_SEPARATOR',`')

dnl SHOW_KERNEL_ARG(type,value)

define(`SHOW_KERNEL_ARG(type,value)',`

fprintf(stderr,"Setting kernel arg %d, with size %ld\n",
ki_idx,sizeof($1));
fprintf(stderr,"_SHOW_KERNEL_ARG( %s , %s )\n",#$1,#$2);
if( !strcmp(#$1,"float") ) fprintf(stderr,"\tfloat arg = %g\n",
* ((float *)$2));
else if( !strcmp(#$1,"int") ) fprintf(stderr,"\tint arg = %d\n",
* ((int *)$2));
else if( !strcmp(#$1,"uint32_t") ) fprintf(stderr,"\tuint32_t arg = %d\n",
* ((uint32_t *)$2));
else if( !strcmp(#$1,"int32_t") ) fprintf(stderr,"\tint32_t arg = %d\n",
* ((int32_t *)$2));
else if( !strcmp(#$1,"void *") ) fprintf(stderr,"\tptr arg = 0x%lx (0x%lx)\n",
(u_long)$2,(u_long)(*((void **)$2)));
else if( !strcmp(#$1,"bitmap_word") ) fprintf(stderr,"\tbitmap word arg = 0x%lx\n",
(/*bitmap_word*/u_long)$2);
else if( !strcmp(#$1,"dim5") ) fprintf(stderr,"\tdim5 arg = %d %d %d %d %d\n",
((dim5 *)$2)->d5_dim[0],((dim5 *)$2)->d5_dim[1],((dim5 *)$2)->d5_dim[2],((dim5 *)$2)->d5_dim[3],((dim5 *)$2)->d5_dim[4]); 
else fprintf(stderr,"\t_SHOW_KERNEL_ARG:  unhandled case for type %s\n",#$1);

/*else if( !strcmp(#$1,"dim3") ) fprintf(stderr,"\tdim3 arg = %d %d %d\n",
((dim3 *)$2)->x,((dim3 *)$2)->y,((dim3 *)$2)->z);
*/
')

dnl SET_KERNEL_ARG(type,value)

define(`SET_KERNEL_ARG',`_SET_KERNEL_ARG(kernel[pd_idx],$1,$2)')
define(`SET_KERNEL_ARG_1',`_SET_KERNEL_ARG(kernel1[pd_idx],$1,$2)')
define(`SET_KERNEL_ARG_2',`_SET_KERNEL_ARG(kernel2[pd_idx],$1,$2)')

define(`SET_KERNEL_ARGS_FAST',SET_KERNEL_ARGS_FAST_$1$2$3$4)

dnl _SET_KERNEL_ARG(kernel,type,value)
define(`_SET_KERNEL_ARG',`
	status = clSetKernelArg($1,	ki_idx++, sizeof($2), $3);
	if( status != CL_SUCCESS )
		report_ocl_error(DEFAULT_QSP_ARG  status, "clSetKernelArg" );
')


define(`SET_KERNEL_ARGS_DEST_OFFSET',`SET_KERNEL_ARG( OCL_OFFSET_TYPE, &VA_DEST_OFFSET(vap) )')

define(`SET_KERNEL_ARGS_SRC1_OFFSET',`SET_KERNEL_ARG( OCL_OFFSET_TYPE, &VA_SRC1_OFFSET(vap) )')

define(`SET_KERNEL_ARGS_SRC2_OFFSET',`SET_KERNEL_ARG( OCL_OFFSET_TYPE, &VA_SRC2_OFFSET(vap) )')

define(`SET_KERNEL_ARGS_SRC3_OFFSET',`SET_KERNEL_ARG( OCL_OFFSET_TYPE, &VA_SRC3_OFFSET(vap) )')

define(`SET_KERNEL_ARGS_SRC4_OFFSET',`SET_KERNEL_ARG( OCL_OFFSET_TYPE, &VA_SRC4_OFFSET(vap) )')

define(`SET_KERNEL_ARGS_SBM_OFFSET',`SET_KERNEL_ARG( OCL_OFFSET_TYPE, &VA_SBM_OFFSET(vap) )')

define(`SET_KERNEL_ARGS_DBM_OFFSET',`SET_KERNEL_ARG( OCL_OFFSET_TYPE, &VA_DBM_OFFSET(vap) )')

define(`SET_KERNEL_ARGS_DBM_GPU_INFO',`SET_KERNEL_ARG( void *, &VA_DBM_GPU_INFO_PTR(vap) )')

define(`SET_KERNEL_ARGS_FAST_SRC2',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC2(vap))) )')

dnl SET_KERNEL_ARGS_FAST_CONV_DEST(t)
define(`SET_KERNEL_ARGS_FAST_CONV_DEST',`SET_KERNEL_ARGS_FAST_1')

define(`SET_KERNEL_ARGS_SLOW',SET_KERNEL_ARGS_SLOW_$1$2$3$4)
define(`SET_KERNEL_ARGS_EQSP',SET_KERNEL_ARGS_EQSP_$1$2$3$4)
define(`SET_KERNEL_ARGS_FAST',SET_KERNEL_ARGS_FAST_$1$2$3$4)

define(`SET_KERNEL_ARGS_SLOW_SIZE',`/*SHOW_SLOW_SIZE(vap)*/ SET_KERNEL_ARG(dim5, & VA_SLOW_SIZE(vap))')

define(`SET_KERNEL_ARGS_SLOW_SIZE_OFFSET',`')

define(`SET_KERNEL_ARGS_FAST_1',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_DEST(vap))) )')

define(`SET_KERNEL_ARGS_FAST_CPX_1',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_DEST(vap))) )')

define(`SET_KERNEL_ARGS_FAST_QUAT_1',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_DEST(vap))) )')

// BUG - need to make sure consistent with expected args???

dnl `SET_KERNEL_ARGS_FAST_NOCC_SETUP'

define(`SET_KERNEL_ARGS_FAST_NOCC_SETUP',`

	SET_KERNEL_ARG_1(void *,&dst_values)
	SET_KERNEL_ARG_1(void *,&dst_counts)
	SET_KERNEL_ARG_1(void *,&orig_src_values)
	SET_KERNEL_ARG_1(void *,&indices)
	SET_KERNEL_ARG_1( uint32_t, &len1 )
	SET_KERNEL_ARG_1( uint32_t, &len2 )
')

dnl `SET_KERNEL_ARGS_FAST_NOCC_HELPER'

define(`SET_KERNEL_ARGS_FAST_NOCC_HELPER',`

	ki_idx=0;
	SET_KERNEL_ARG_2(void *,&dst_values)
	SET_KERNEL_ARG_2(void *,&dst_counts)
	SET_KERNEL_ARG_2(void *,&src_values)
	SET_KERNEL_ARG_2(void *,&src_counts)
	SET_KERNEL_ARG_2(void *,&indices)
	SET_KERNEL_ARG_2( uint32_t, &len1 )
	SET_KERNEL_ARG_2( uint32_t, &len2 )
	SET_KERNEL_ARG_2( uint32_t, &stride )
')

define(`SET_KERNEL_ARGS_FAST_PROJ_2V_HELPER',`

	ki_idx=0;
fprintf(stderr,"set_kernal_args_fast_proj_2v_helper:  dst_values = 0x%lx, src_values = 0x%lx, len1 = %d, len2 = %d\n",
(long)dst_values,(long)src_values,len1,len2);
	SET_KERNEL_ARG_2(void *,&dst_values)
	SET_KERNEL_ARG_2(void *,&src_values)
	SET_KERNEL_ARG_2( uint32_t, &len1 )
	SET_KERNEL_ARG_2( uint32_t, &len2 )
')

define(`SET_KERNEL_ARGS_FAST_PROJ_2V_SETUP',`

fprintf(stderr,"set_kernel_args_fast_proj_2v_setup:  dst_values = 0x%lx, orig_src_values = 0x%lx, len1 = %d, len2 = %d\n",
(long)dst_values,(long)orig_src_values,len1,len2);
	SET_KERNEL_ARG_1(void *,&dst_values)
	SET_KERNEL_ARG_1(void *,&orig_src_values)
	SET_KERNEL_ARG_1( uint32_t, &len1 )
	SET_KERNEL_ARG_1( uint32_t, &len2 )
')

define(`SET_KERNEL_ARGS_FAST_PROJ_3V_HELPER',`

	ki_idx=0;
fprintf(stderr,"set_kernel_args_fast_proj_3v_helper:  setting dst_values\n");
	SET_KERNEL_ARG_2(void *,&dst_values)
fprintf(stderr,"set_kernel_args_fast_proj_3v_helper:  setting src1_values\n");
	SET_KERNEL_ARG_2(void *,&src1_values)
fprintf(stderr,"set_kernel_args_fast_proj_3v_helper:  setting src2_values\n");
fprintf(stderr,"src2_values = 0x%lx, &src2_values = 0x%lx\n",(long)src2_values,(long)(&src2_values));
fprintf(stderr,"pd_idx = %d,  kernel2[pd_idx] = 0x%lx\n",pd_idx,(long)kernel2[pd_idx]);
	SET_KERNEL_ARG_2(void *,&src2_values)
fprintf(stderr,"set_kernel_args_fast_proj_3v_helper:  setting len1\n");
	SET_KERNEL_ARG_2( uint32_t, &len1 )
fprintf(stderr,"set_kernel_args_fast_proj_3v_helper:  setting len2\n");
	SET_KERNEL_ARG_2( uint32_t, &len2 )
')

define(`SET_KERNEL_ARGS_FAST_PROJ_3V_SETUP',`

	SET_KERNEL_ARG_1(void *,&dst_values)
	SET_KERNEL_ARG_1(void *,&orig_src1_values)
	SET_KERNEL_ARG_1(void *,&orig_src2_values)
	SET_KERNEL_ARG_1( uint32_t, &len1 )
	SET_KERNEL_ARG_1( uint32_t, &len2 )
')

define(`SET_KERNEL_ARGS_FAST_INDEX_SETUP',`

	SET_KERNEL_ARG_1(void *,&indices)
	SET_KERNEL_ARG_1(void *,&src1_values)
	SET_KERNEL_ARG_1(void *,&src2_values)
	SET_KERNEL_ARG_1( uint32_t, &len1 )
	SET_KERNEL_ARG_1( uint32_t, &len2 )
')


dnl should this be src1_values???  or orig_values?

define(`SET_KERNEL_ARGS_FAST_INDEX_HELPER',`

	ki_idx=0;
	SET_KERNEL_ARG_2(void *,&indices)
	SET_KERNEL_ARG_2(void *,&idx1_values)
	SET_KERNEL_ARG_2(void *,&idx2_values)
	SET_KERNEL_ARG_2(void *,&src1_values)
	SET_KERNEL_ARG_2( uint32_t, &len1 )
	SET_KERNEL_ARG_2( uint32_t, &len2 )
')




dnl BUG - can we insure that the arg order matches VFUNC???

define(`SET_KERNEL_ARGS_FAST_SBM',`SET_KERNEL_ARG(void *,&(VA_SBM_PTR(vap)))
					SET_KERNEL_ARG(int,&(VA_SBM_BIT0(vap)))
')

dnl BUG incset is not increment!?
define(`SET_KERNEL_ARGS_EQSP_SBM',`SET_KERNEL_ARG(void *,&(VA_SBM_PTR(vap)))
					SET_KERNEL_ARG(int,&(VA_SBM_BIT0(vap)))
					SET_KERNEL_ARG(int,&VA_SBM_EQSP_INC(vap))
')

define(`SET_KERNEL_ARGS_SLOW_SBM',`SET_KERNEL_ARG(void *,&(VA_SBM_PTR(vap)))
					SET_KERNEL_ARG(int,&(VA_SBM_BIT0(vap)))
					SET_KERNEL_ARG(dim5,&sbm_vwxyz_incr)
')



dnl I don't think that "fast" bitmaps can include a bit0 parameter, unless it is a multiple of the word size!?
dnl If so, then they will need the gpu_info arg, which is no longer "fast" !?

dnl BUG - how can we be sure that these definitions are consistent with the kernels?

dnl	SET_KERNEL_ARG( void *, &VA_DBM_GPU_INFO_PTR(vap) )

define(`SET_KERNEL_ARGS_FAST_DBM',`SET_KERNEL_ARG(void *,&(VA_DBM_PTR(vap)))')

define(`SET_KERNEL_ARGS_EQSP_DBM',`SET_KERNEL_ARG(void *,&(VA_DBM_PTR(vap)))')


dnl BUG - there doesn't seem to be anything that enforces these definitions to match what is done elsewhere?
define(`SET_KERNEL_ARGS_SLOW_DBM',`SET_KERNEL_ARG(void *,&(VA_DBM_PTR(vap)))')


dnl SRC1

define(`SET_KERNEL_ARGS_FAST_SRC1',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC1(vap))) )')
define(`SET_KERNEL_ARGS_FAST_CPX_SRC1',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC1(vap))) )')
define(`SET_KERNEL_ARGS_FAST_QUAT_SRC1',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC1(vap))) )')

define(`SET_KERNEL_ARGS_FAST_SRC2',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC2(vap))) )')
define(`SET_KERNEL_ARGS_FAST_CPX_SRC2',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC2(vap))) )')
define(`SET_KERNEL_ARGS_FAST_QUAT_SRC2',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC2(vap))) )')

define(`SET_KERNEL_ARGS_FAST_SRC3',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC3(vap))) )')
define(`SET_KERNEL_ARGS_FAST_CPX_SRC3',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC3(vap))) )')
define(`SET_KERNEL_ARGS_FAST_QUAT_SRC3',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC3(vap))) )')

define(`SET_KERNEL_ARGS_FAST_SRC4',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC4(vap))) )')
define(`SET_KERNEL_ARGS_FAST_CPX_SRC4',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC4(vap))) )')
define(`SET_KERNEL_ARGS_FAST_QUAT_SRC4',`SET_KERNEL_ARG(void *,&(VARG_PTR( VA_SRC4(vap))) )')

define(`SET_KERNEL_ARGS_EQSP_SRC1',`GEN_ARGS_EQSP_SRC1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_CPX_SRC1',`GEN_ARGS_EQSP_SRC1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QUAT_SRC1',`GEN_ARGS_EQSP_SRC1(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_SRC1',`GEN_ARGS_SLOW_SRC1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_CPX_SRC1',`GEN_ARGS_SLOW_SRC1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QUAT_SRC1',`GEN_ARGS_SLOW_SRC1(SET_KERNEL_ARGS)')


dnl SET_KERNEL_ARGS_EQSP_CONV(t)
define(`SET_KERNEL_ARGS_EQSP_CONV',`GEN_ARGS_EQSP_CONV(SET_KERNEL_ARGS,$1)')
define(`SET_KERNEL_ARGS_EQSP_1',`GEN_ARGS_EQSP_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_2',`GEN_ARGS_EQSP_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_3',`GEN_ARGS_EQSP_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_EQSP_CPX_1',`GEN_ARGS_EQSP_CPX_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_CPX_2',`GEN_ARGS_EQSP_CPX_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_CPX_3',`GEN_ARGS_EQSP_CPX_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_EQSP_QUAT_1',`GEN_ARGS_EQSP_QUAT_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QUAT_2',`GEN_ARGS_EQSP_QUAT_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QUAT_3',`GEN_ARGS_EQSP_QUAT_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_CPX_3',`GEN_ARGS_FAST_CPX_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_CPX_3',`GEN_ARGS_EQSP_CPX_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_CPX_3',`GEN_ARGS_SLOW_CPX_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_QUAT_3',`GEN_ARGS_FAST_QUAT_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QUAT_3',`GEN_ARGS_EQSP_QUAT_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QUAT_3',`GEN_ARGS_SLOW_QUAT_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_CCR_3',`GEN_ARGS_FAST_CCR_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_CCR_3',`GEN_ARGS_EQSP_CCR_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_CCR_3',`GEN_ARGS_SLOW_CCR_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_QQR_3',`GEN_ARGS_FAST_QQR_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QQR_3',`GEN_ARGS_EQSP_QQR_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QQR_3',`GEN_ARGS_SLOW_QQR_3(SET_KERNEL_ARGS)')



define(`SET_KERNEL_ARGS_FAST_CR_1S_2',`GEN_ARGS_FAST_CR_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_CR_1S_2',`GEN_ARGS_EQSP_CR_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_CR_1S_2',`GEN_ARGS_SLOW_CR_1S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_QR_1S_2',`GEN_ARGS_FAST_QR_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QR_1S_2',`GEN_ARGS_EQSP_QR_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QR_1S_2',`GEN_ARGS_SLOW_QR_1S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_RC_2',`GEN_ARGS_FAST_RC_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_RC_2',`GEN_ARGS_EQSP_RC_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_RC_2',`GEN_ARGS_SLOW_RC_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_RQ_2',`GEN_ARGS_FAST_RQ_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_RQ_2',`GEN_ARGS_EQSP_RQ_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_RQ_2',`GEN_ARGS_SLOW_RQ_2(SET_KERNEL_ARGS)')

dnl SET_KERNEL_ARGS_SLOW_CONV(dest_type)
define(`SET_KERNEL_ARGS_SLOW_CONV',`GEN_ARGS_SLOW_CONV(SET_KERNEL_ARGS,$1)')
define(`SET_KERNEL_ARGS_SLOW_1',`GEN_ARGS_SLOW_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_2',`GEN_ARGS_SLOW_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_3',`GEN_ARGS_SLOW_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_CPX_1',`GEN_ARGS_SLOW_CPX_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_CPX_2',`GEN_ARGS_SLOW_CPX_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_CPX_3',`GEN_ARGS_SLOW_CPX_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_CCR_3',`GEN_ARGS_SLOW_CCR_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QQR_3',`GEN_ARGS_SLOW_QQR_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_QUAT_1',`GEN_ARGS_SLOW_QUAT_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QUAT_2',`GEN_ARGS_SLOW_QUAT_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QUAT_3',`GEN_ARGS_SLOW_QUAT_3(SET_KERNEL_ARGS)')


define(`SET_KERNEL_ARGS_FAST_1S_1',`GEN_ARGS_FAST_1S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_1S_1',`GEN_ARGS_EQSP_1S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_1S_1',`GEN_ARGS_SLOW_1S_1(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_CPX_1S_1',`GEN_ARGS_FAST_CPX_1S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_CPX_1S_1',`GEN_ARGS_EQSP_CPX_1S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_CPX_1S_1',`GEN_ARGS_SLOW_CPX_1S_1(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_QUAT_1S_1',`GEN_ARGS_FAST_QUAT_1S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QUAT_1S_1',`GEN_ARGS_EQSP_QUAT_1S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QUAT_1S_1',`GEN_ARGS_SLOW_QUAT_1S_1(SET_KERNEL_ARGS)')


define(`SET_KERNEL_ARGS_FAST_2S_1',`GEN_ARGS_FAST_2S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_2S_1',`GEN_ARGS_EQSP_2S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_2S_1',`GEN_ARGS_SLOW_2S_1(SET_KERNEL_ARGS)')

define(`UNIMP_SK( mname )',`fprintf(stderr,"Oops:  Need to implement %s!?\n",#mname);')


define(`SET_KERNEL_ARGS_1S',`SET_KERNEL_SCALAR_ARG(0)')

define(`SET_KERNEL_ARGS_CPX_1S',`SET_KERNEL_SCALAR_ARG(0)')

define(`SET_KERNEL_ARGS_QUAT_1S',`SET_KERNEL_SCALAR_ARG(0)')

define(`SET_KERNEL_ARGS_2S',`SET_KERNEL_SCALAR_ARG(0) SET_KERNEL_SCALAR_ARG(1)')

define(`SET_KERNEL_ARGS_CPX_2S',`SET_KERNEL_CPX_SCALAR_ARG(0) SET_KERNEL_CPX_SCALAR_ARG(1)')

define(`SET_KERNEL_ARGS_QUAT_2S',`SET_KERNEL_QUAT_SCALAR_ARG(0) SET_KERNEL_QUAT_SCALAR_ARG(1)')

define(`SET_KERNEL_ARGS_3S',`SET_KERNEL_SCALAR_ARG(0) SET_KERNEL_SCALAR_ARG(1) SET_KERNEL_SCALAR_ARG(2)')

dnl SET_KERNEL_SCALAR_ARG(scalar_idx)
define(`SET_KERNEL_SCALAR_ARG',`SET_KERNEL_ARG(std_type,vap->va_sval[$1] )')
define(`SET_KERNEL_CPX_SCALAR_ARG',`SET_KERNEL_ARG(std_cpx,vap->va_sval[$1] )')
define(`SET_KERNEL_QUAT_SCALAR_ARG',`SET_KERNEL_ARG(std_quat,vap->va_sval[$1] )')


dnl BUG need to figure out which dimension is the one?
define(`SET_KERNEL_ARGS_EQSP_INC1',`SET_KERNEL_ARG(int,&VA_DEST_EQSP_INC(vap))')
define(`SET_KERNEL_ARGS_EQSP_INC2',`SET_KERNEL_ARG(int,&VA_SRC1_EQSP_INC(vap))')
define(`SET_KERNEL_ARGS_EQSP_INC3',`SET_KERNEL_ARG(int,&VA_SRC2_EQSP_INC(vap))')
define(`SET_KERNEL_ARGS_EQSP_INC4',`SET_KERNEL_ARG(int,&VA_SRC3_EQSP_INC(vap))')
define(`SET_KERNEL_ARGS_EQSP_INC5',`SET_KERNEL_ARG(int,&VA_SRC4_EQSP_INC(vap))')

dnl BUG?  do we need DIM3 increments in Vector_Args ???
dnl Now we have dim5 sizes and increments in Vector_Args!

define(`SET_KERNEL_ARGS_SLOW_INC1',`SET_KERNEL_ARG(dim5,&dst_vwxyz_incr)	/* SHOW_INCR(&dst_vwxyz_incr) */')
define(`SET_KERNEL_ARGS_SLOW_INC2',`SET_KERNEL_ARG(dim5,&s1_vwxyz_incr)')
define(`SET_KERNEL_ARGS_SLOW_INC3',`SET_KERNEL_ARG(dim5,&s2_vwxyz_incr)')
define(`SET_KERNEL_ARGS_SLOW_INC4',`SET_KERNEL_ARG(dim5,&s3_vwxyz_incr)')
define(`SET_KERNEL_ARGS_SLOW_INC5',`SET_KERNEL_ARG(dim5,&s4_vwxyz_incr)')

define(`SET_KERNEL_ARGS_FAST_1S_2',`GEN_ARGS_FAST_1S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_CPX_1S_2',`GEN_ARGS_FAST_CPX_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_CPX_1S_2',`GEN_ARGS_EQSP_CPX_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_CPX_1S_2',`GEN_ARGS_SLOW_CPX_1S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_QUAT_1S_2',`GEN_ARGS_FAST_QUAT_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QUAT_1S_2',`GEN_ARGS_EQSP_QUAT_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QUAT_1S_2',`GEN_ARGS_SLOW_QUAT_1S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_3',`GEN_ARGS_FAST_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_5',`GEN_ARGS_FAST_5(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_1S_4',`GEN_ARGS_FAST_1S_4(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_2S_3',`GEN_ARGS_FAST_2S_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_3S_2',`GEN_ARGS_FAST_3S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_EQSP_5',`GEN_ARGS_EQSP_5(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_1S_4',`GEN_ARGS_EQSP_1S_4(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_2S_3',`GEN_ARGS_EQSP_2S_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_3S_2',`GEN_ARGS_EQSP_3S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_5',`GEN_ARGS_SLOW_5(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_1S_4',`GEN_ARGS_SLOW_1S_4(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_2S_3',`GEN_ARGS_SLOW_2S_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_3S_2',`GEN_ARGS_SLOW_3S_2(SET_KERNEL_ARGS)')


define(`SET_KERNEL_ARGS_FAST_SBM_3',`GEN_ARGS_FAST_SBM_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_SBM_CPX_3',`GEN_ARGS_FAST_SBM_CPX_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_SBM_QUAT_3',`GEN_ARGS_FAST_SBM_QUAT_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_EQSP_SBM_3',`GEN_ARGS_EQSP_SBM_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_SBM_CPX_3',`GEN_ARGS_EQSP_SBM_CPX_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_SBM_QUAT_3',`GEN_ARGS_EQSP_SBM_QUAT_3(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_SBM_3',`GEN_ARGS_SLOW_SBM_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_SBM_CPX_3',`GEN_ARGS_SLOW_SBM_CPX_3(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_SBM_QUAT_3',`GEN_ARGS_SLOW_SBM_QUAT_3(SET_KERNEL_ARGS)')


define(`SET_KERNEL_ARGS_FAST_SBM_1S_2',`GEN_ARGS_FAST_SBM_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_SBM_CPX_1S_2',`GEN_ARGS_FAST_SBM_CPX_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_SBM_QUAT_1S_2',`GEN_ARGS_FAST_SBM_QUAT_1S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_EQSP_SBM_1S_2',`GEN_ARGS_EQSP_SBM_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_SBM_CPX_1S_2',`GEN_ARGS_EQSP_SBM_CPX_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_SBM_QUAT_1S_2',`GEN_ARGS_EQSP_SBM_QUAT_1S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_SBM_1S_2',`GEN_ARGS_SLOW_SBM_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_SBM_CPX_1S_2',`GEN_ARGS_SLOW_SBM_CPX_1S_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_SBM_QUAT_1S_2',`GEN_ARGS_SLOW_SBM_QUAT_1S_2(SET_KERNEL_ARGS)')



define(`SET_KERNEL_ARGS_FAST_SBM_2S_1',`GEN_ARGS_FAST_SBM_2S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_SBM_CPX_2S_1',`GEN_ARGS_FAST_SBM_CPX_2S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_SBM_QUAT_2S_1',`GEN_ARGS_FAST_SBM_QUAT_2S_1(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_EQSP_SBM_2S_1',`GEN_ARGS_EQSP_SBM_2S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_SBM_CPX_2S_1',`GEN_ARGS_EQSP_SBM_CPX_2S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_SBM_QUAT_2S_1',`GEN_ARGS_EQSP_SBM_QUAT_2S_1(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_SBM_2S_1',`GEN_ARGS_SLOW_SBM_2S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_SBM_CPX_2S_1',`GEN_ARGS_SLOW_SBM_CPX_2S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_SBM_QUAT_2S_1',`GEN_ARGS_SLOW_SBM_QUAT_2S_1(SET_KERNEL_ARGS)')




define(`SET_KERNEL_ARGS_FAST_CONV',`GEN_ARGS_FAST_CONV(SET_KERNEL_ARGS,$1)')
define(`SET_KERNEL_ARGS_FAST_2',`GEN_ARGS_FAST_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_CPX_2',`GEN_ARGS_FAST_CPX_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_QUAT_2',`GEN_ARGS_FAST_QUAT_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_CR_2',`GEN_ARGS_FAST_CR_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_QR_2',`GEN_ARGS_FAST_QR_2(SET_KERNEL_ARGS)')

dnl define(`SET_KERNEL_ARGS_EQSP_2',`GEN_ARGS_EQSP_2(SET_KERNEL_ARGS)')
dnl define(`SET_KERNEL_ARGS_EQSP_CPX_2',`GEN_ARGS_EQSP_CPX_2(SET_KERNEL_ARGS)')
dnl define(`SET_KERNEL_ARGS_EQSP_QUAT_2',`GEN_ARGS_EQSP_QUAT_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_EQSP_CR_2',`GEN_ARGS_EQSP_CR_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_QR_2',`GEN_ARGS_EQSP_QR_2(SET_KERNEL_ARGS)')

dnl define(`SET_KERNEL_ARGS_SLOW_2',`GEN_ARGS_SLOW_2(SET_KERNEL_ARGS)')
dnl define(`SET_KERNEL_ARGS_SLOW_CPX_2',`GEN_ARGS_SLOW_CPX_2(SET_KERNEL_ARGS)')
dnl define(`SET_KERNEL_ARGS_SLOW_QUAT_2',`GEN_ARGS_SLOW_QUAT_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_CR_2',`GEN_ARGS_SLOW_CR_2(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_QR_2',`GEN_ARGS_SLOW_QR_2(SET_KERNEL_ARGS)')


// All eqsp args


define(`SET_KERNEL_ARGS_EQSP_1S_2',`GEN_ARGS_EQSP_1S_2(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_3S_1',`GEN_ARGS_SLOW_3S_1(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_1S_2',`GEN_ARGS_SLOW_1S_2(SET_KERNEL_ARGS)')

// All flen args


define(`SET_KERNEL_ARGS_FAST_DBM_2SRCS',`GEN_ARGS_FAST_DBM_2SRCS(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_DBM_1S_1SRC',`GEN_ARGS_FAST_DBM_1S_1SRC(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_FAST_DBM_1S_',`GEN_ARGS_FAST_DBM_1S_(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_EQSP_DBM_2SRCS',`GEN_ARGS_EQSP_DBM_2SRCS(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_DBM_1S_1SRC',`GEN_ARGS_EQSP_DBM_1S_1SRC(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_DBM_1S_',`GEN_ARGS_EQSP_DBM_1S_(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_SLOW_DBM_2SRCS',`GEN_ARGS_SLOW_DBM_2SRCS(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_DBM_1S_1SRC',`GEN_ARGS_SLOW_DBM_1S_1SRC(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_DBM_1S_',`GEN_ARGS_SLOW_DBM_1S_(SET_KERNEL_ARGS)')

define(`SET_KERNEL_ARGS_FAST_DBM_SBM',`GEN_ARGS_FAST_DBM_SBM(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_EQSP_DBM_SBM',`GEN_ARGS_EQSP_DBM_SBM(SET_KERNEL_ARGS)')
define(`SET_KERNEL_ARGS_SLOW_DBM_SBM',`GEN_ARGS_SLOW_DBM_SBM(SET_KERNEL_ARGS)')


define(`SET_KERNEL_ARGS_FAST_CPX_3V_T1',`SET_KERNEL_ARGS_FAST_CPX_3')

