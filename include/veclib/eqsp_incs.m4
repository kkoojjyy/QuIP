// Declare increments

// Now we set the increments in the vector_args struct?

dnl DECL_EQSP_INCRS(bitmap,vectors)
define(`DECL_EQSP_INCRS',DECL_EQSP_INCRS_$1$2)

define(`DECL_EQSP_INCRS_1',`')
define(`DECL_EQSP_INCRS_SRC1',`')
define(`DECL_EQSP_INCRS_SRC2',`')
define(`DECL_EQSP_INCRS_SRC3',`')
define(`DECL_EQSP_INCRS_SRC4',`')
define(`DECL_EQSP_INCRS_SBM',`')
dnl	BUG?  these need to be able to have increments???
define(`DECL_EQSP_INCRS_SBM1',`')
define(`DECL_EQSP_INCRS_SBM2',`')
define(`DECL_EQSP_INCRS_DBM',`')
define(`DECL_EQSP_INCRS_DBM_SBM',`')
define(`DECL_EQSP_INCRS_DBM_2SBM',`')
define(`DECL_EQSP_INCRS_DBM_1SBM',`')
define(`DECL_EQSP_INCRS_2',`DECL_EQSP_INCRS_1 DECL_EQSP_INCRS_SRC1')
define(`DECL_EQSP_INCRS_CONV',`DECL_EQSP_INCRS_1 DECL_EQSP_INCRS_SRC1')
define(`DECL_EQSP_INCRS_LUTMAP_B',`DECL_EQSP_INCRS_2')
define(`DECL_EQSP_INCRS_LUTMAP_S',`DECL_EQSP_INCRS_2')
define(`DECL_EQSP_INCRS_3',`DECL_EQSP_INCRS_2 DECL_EQSP_INCRS_SRC2')
define(`DECL_EQSP_INCRS_4',`DECL_EQSP_INCRS_3 DECL_EQSP_INCRS_SRC3')
define(`DECL_EQSP_INCRS_5',`DECL_EQSP_INCRS_4 DECL_EQSP_INCRS_SRC4')
define(`DECL_EQSP_INCRS_2SRCS',`DECL_EQSP_INCRS_SRC1 DECL_EQSP_INCRS_SRC2')

define(`DECL_EQSP_INCRS_SBM_1',`DECL_EQSP_INCRS_1 DECL_EQSP_INCRS_SBM')
define(`DECL_EQSP_INCRS_SBM_2',`DECL_EQSP_INCRS_2 DECL_EQSP_INCRS_SBM')
define(`DECL_EQSP_INCRS_SBM_3',`DECL_EQSP_INCRS_3 DECL_EQSP_INCRS_SBM')
define(`DECL_EQSP_INCRS_DBM_',`DECL_EQSP_INCRS_DBM')
define(`DECL_EQSP_INCRS_DBM_1SRC',`DECL_EQSP_INCRS_SRC1 DECL_EQSP_INCRS_DBM')
define(`DECL_EQSP_INCRS_DBM_2SRCS',`DECL_EQSP_INCRS_2SRCS DECL_EQSP_INCRS_DBM')

