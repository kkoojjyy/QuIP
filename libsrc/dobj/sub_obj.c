#include "quip_config.h"

#include <stdio.h>
#include <strings.h>	// bzero

#include "quip_prot.h"
#include "data_obj.h"
#include "dobj_private.h"
#include "platform.h"
#ifdef HAVE_OPENCL
#include "ocl_platform.h"
#endif // HAVE_OPENCL

typedef struct {
	const char *	eqd_name;
	Precision *	eqd_prec_p;
	Data_Obj *	eqd_parent;
	Dimension_Set *	eqd_type_dsp;
	Dimension_Set *	eqd_mach_dsp;
	int		eqd_n_per_parent;
	int		eqd_n_per_child;
	int		eqd_bytes_per_parent_elt;
	int		eqd_bytes_per_child_elt;
	dimension_t	eqd_total_child_bytes;
	dimension_t	eqd_total_parent_bytes;
} Equivalence_Data;

#define EQ_NAME(eqd_p)		(eqd_p)->eqd_name
#define EQ_PREC_PTR(eqd_p)	(eqd_p)->eqd_prec_p
#define EQ_PREC_CODE(eqd_p)	PREC_CODE(EQ_PREC_PTR(eqd_p))
#define EQ_PARENT(eqd_p)	(eqd_p)->eqd_parent
#define EQ_TYPE_DIMS(eqd_p)		(eqd_p)->eqd_type_dsp
#define EQ_MACH_DIMS(eqd_p)		(eqd_p)->eqd_mach_dsp
#define EQ_N_PER_PARENT(eqd_p)	(eqd_p)->eqd_n_per_parent
#define EQ_N_PER_CHILD(eqd_p)	(eqd_p)->eqd_n_per_child
#define EQ_BYTES_PER_PARENT_ELT(eqd_p)	(eqd_p)->eqd_bytes_per_parent_elt
#define EQ_BYTES_PER_CHILD_ELT(eqd_p)	(eqd_p)->eqd_bytes_per_child_elt
#define EQ_TOTAL_CHILD_BYTES(eqd_p)	(eqd_p)->eqd_total_child_bytes
#define EQ_TOTAL_PARENT_BYTES(eqd_p)	(eqd_p)->eqd_total_parent_bytes

#define SET_EQ_N_PER_PARENT(eqd_p,v)	(eqd_p)->eqd_n_per_parent = v
#define SET_EQ_N_PER_CHILD(eqd_p,v)	(eqd_p)->eqd_n_per_child = v
#define SET_EQ_BYTES_PER_PARENT_ELT(eqd_p,v)	(eqd_p)->eqd_bytes_per_parent_elt = v
#define SET_EQ_BYTES_PER_CHILD_ELT(eqd_p,v)	(eqd_p)->eqd_bytes_per_child_elt = v

#define n_bytes_per_child_elt	EQ_BYTES_PER_CHILD_ELT(eqd_p)
#define n_bytes_per_parent_elt	EQ_BYTES_PER_PARENT_ELT(eqd_p)
#define n_per_parent	EQ_N_PER_PARENT(eqd_p)
#define n_per_child	EQ_N_PER_CHILD(eqd_p)
#define total_child_bytes	EQ_TOTAL_CHILD_BYTES(eqd_p)
#define total_parent_bytes	EQ_TOTAL_PARENT_BYTES(eqd_p)

/* This used to be declared withing check_posn(),
 * but old sun compiler says "no automatic aggregate initialization"
 */

static incr_t ones[N_DIMENSIONS]={1,1,1,1,1};	/* dummy increments */

static void set_child_increments(Data_Obj *dp, Increment_Set *type_incrs, Increment_Set *mach_incrs)
{
	int i;

	/* Here we copy the increments from the parent -
	 * But if a subscripted dimension is now 1,
	 * then the corresponding increment must
	 * be set to 0...
	 */

	for(i=0;i<N_DIMENSIONS;i++){
		if( OBJ_MACH_DIM(dp,i) == 1 ){
			SET_OBJ_MACH_INC(dp,i,0);
			SET_OBJ_TYPE_INC(dp,i,0);
		} else {
			SET_OBJ_TYPE_INC(dp,i,INCREMENT(type_incrs,i));
			SET_OBJ_MACH_INC(dp,i,INCREMENT(mach_incrs,i));
		}
	}
}

// This was OBJ_MACH_INC but perhaps it should be OBJ_TYPE_INC?
// It probably makes more sense to use type units, but for the sake
// of minimal breakage, we leave it in machine units, and change
// the OpenCL data offset function to make adjustments for complex and quat.

static index_t get_pix_base_offset(index_t *offsets, Data_Obj *parent)
{
	index_t pix_offset;
	int i;

	pix_offset=0L;
	for(i=0;i<N_DIMENSIONS;i++)
		pix_offset += offsets[i] * OBJ_MACH_INC(parent,i);
		//pix_offset += offsets[i] * OBJ_TYPE_INC(parent,i);

	return(pix_offset);
}

/* check one dimension */

#define is_inside(index,which_dim,sub_name,parent)	_is_inside(QSP_ARG  index,which_dim,sub_name,parent)

static int _is_inside( QSP_ARG_DECL  index_t index, int which_dim, const char *sub_name, Data_Obj *parent )
{
	dimension_t pd;		/* parent dim */

	/* if indices are unsigned, the negative test is meaningless... */
	pd = OBJ_TYPE_DIM(parent,which_dim);

	if( /* index < 0 || */ index >= pd){
		sprintf(ERROR_STRING,
"%s offset %d for subobject \"%s\" falls outside of parent \"%s\" (%s count = %d)",
			dimension_name[which_dim],
			index,sub_name,OBJ_NAME(parent),
			dimension_name[which_dim],pd);
		warn(ERROR_STRING);
		sprintf(ERROR_STRING,
			"dim index %d:  parent size = %u",
			which_dim, OBJ_TYPE_DIM(parent,which_dim));
		advise(ERROR_STRING);
		return(0);
	}
	return(1);
}

/* make sure that a requested subobject fits within the parent */

#define check_inset(parent,offsets,dsp,incrs,name)	_check_inset(QSP_ARG  parent,offsets,dsp,incrs,name)

static int _check_inset( QSP_ARG_DECL  Data_Obj *parent, index_t *offsets, Dimension_Set *dsp, incr_t *incrs, const char *name )
{
	int i;
	int retval=0;

	/* make sure that all the sizes are valid */

	for(i=0;i<N_DIMENSIONS;i++){
		assert( DIMENSION(dsp,i) > 0 );
	}

	for(i=0;i<N_DIMENSIONS;i++){
		index_t extreme_index;

		extreme_index = offsets[i];
		if( ! is_inside(extreme_index,i,name,parent) )
			retval=(-1);

		extreme_index = offsets[i]+incrs[i]*(DIMENSION(dsp,i)-1);
		if( ! is_inside(extreme_index,i,name,parent) )
			retval=(-1);

	}
	return retval;
} /* end check_inset() */

#define check_posn(parent,offsets,dsp,name)	_check_posn(QSP_ARG  parent,offsets,dsp,name)

static int _check_posn( QSP_ARG_DECL  Data_Obj *parent, index_t *offsets, Dimension_Set *dsp, const char *name )
{
	return( check_inset(parent,offsets,dsp,ones,name) );
}

void point_obj_to_ext_data( Data_Obj *dp, void *ptr )
{
	SET_OBJ_DATA_PTR(dp,ptr);
	SET_OBJ_FLAG_BITS(dp,DT_NO_DATA);
}

/* Set family fields for both child and parent
 */

void parent_relationship( Data_Obj *parent, Data_Obj *child )
{
	SET_OBJ_AREA(child,OBJ_AREA(parent));
	SET_OBJ_PARENT(child,parent);

	/* the child is always a new object, so we're not
	 * losing anything here
	 */
	SET_OBJ_CHILDREN(child, NULL);

	if( OBJ_CHILDREN(parent) == NULL )
		SET_OBJ_CHILDREN(parent,new_list());

	addHead(OBJ_CHILDREN(parent),mk_node(child));

	SET_OBJ_FLAGS(child,OBJ_FLAGS(parent));
	SET_OBJ_FLAG_BITS(child,DT_NO_DATA);

	/* Clear the TEMP flag in case
	 * parent is a subscripted object
	 * (not in hash table).
	 *
	 * It is the responsibility of the caller to explicitly
	 * set this flag if needed (see array.c)
	 */

	/* Clear the EXPORTED flag - subobjects not
	 * automatically exported, even if the parent is.
	 */

	CLEAR_OBJ_FLAG_BITS(child,DT_TEMP|DT_EXPORTED);

	// We would like to remove dependencies on OpenGL
	// and cuda...  One solution is to make the flag
	// copying an object method?  Then, if an object
	// is created as a gl buffer, the function pointer
	// can be set...

// this section used to be ifdef'd HAVE_OPENGL?

	if( IS_GL_BUFFER(parent) ){
		// Unaligned_data is an overloaded field here... ?
		SET_OBJ_GL_INFO(child,OBJ_GL_INFO(parent) );
		// These two flag transfer commands
		// used to be ifdef'd HAVE_CUDA...
		xfer_dobj_flag(child,parent,DT_GL_BUF);
		xfer_dobj_flag(child,parent,DT_BUF_MAPPED);
	}

}

// Is pix_offset in machine or type units?  (Important for float/complex)
// Originally it was in machine units, but this broke things for the OpenCL
// implementation.
//

// update_offset should be here too, instead of in veclib2, even though
// it is a platform function...

void default_offset_data_func(QSP_ARG_DECL  Data_Obj *dp, index_t pix_offset )
{
	Data_Obj *parent;

//fprintf(stderr,"default_offset_data_func:  %s, pix_offset = %d\n",
//OBJ_NAME(dp),pix_offset);
	parent = OBJ_PARENT(dp);
	if( IS_BITMAP(parent) ){
		/*
		point_obj_to_ext_data(dp, ((long *)OBJ_DATA_PTR(parent)) +
			((OBJ_BIT0(parent)+pix_offset)>>LOG2_BITS_PER_BITMAP_WORD) );
		*/
		// bitmap offsets are handled by bit0
		point_obj_to_ext_data(dp, ((long *)OBJ_DATA_PTR(parent)) );
		pix_offset=0;
	} else {
		pix_offset *= ELEMENT_SIZE(dp);
		point_obj_to_ext_data(dp, ((char *)OBJ_DATA_PTR(parent)) + pix_offset);
	}
	SET_OBJ_OFFSET(dp,pix_offset);
}

static void set_child_bit0(Data_Obj *dp, index_t *offsets)
{
	int i;
	Data_Obj *parent;

	parent = OBJ_PARENT(dp);

	SET_OBJ_BIT0(dp, OBJ_BIT0(parent) );
	for(i=0;i<N_DIMENSIONS;i++)
		SET_OBJ_BIT0(dp, OBJ_BIT0(dp)
			 + offsets[i] * OBJ_TYPE_INC(parent,i) );
}

Data_Obj *
_mk_subseq( QSP_ARG_DECL  const char *name, Data_Obj *parent, index_t *offsets, Dimension_Set *dsp )
{
	Data_Obj *dp;
	index_t pix_offset;

	if( check_posn(parent,offsets,dsp,name) < 0 ) return(NULL);

	dp = new_dobj(name);
	if( dp==NULL ) return(NULL);

	SET_OBJ_SHAPE(dp,ALLOC_SHAPE);

	if( set_obj_dimensions(dp,dsp,OBJ_PREC_PTR(parent)) < 0 ){
		rls_shape( OBJ_SHAPE(dp) );
		del_dobj(dp);
		return(NULL);
	}

	/* this must be called before setup_dp, because
	 * flags are first copied from parent
	 */
	parent_relationship(parent,dp);

	set_child_increments(dp,OBJ_TYPE_INCS(parent),OBJ_MACH_INCS(parent));
	pix_offset = get_pix_base_offset(offsets,parent);

	dp = setup_dp(dp,OBJ_PREC_PTR(parent));
	if( dp==NULL ){
		/* BUG? where does the cleanup happen? */
		return(dp);
	}

	// Clear the volatile flag if set...
	// This is necessary because volatile objects are deleted in ascii_menu.c,
	// to deal with platform copies - probably better to introduce a new flag.
	CLEAR_OBJ_FLAG_BITS(dp,DT_VOLATILE);

	// In openCL, the "data_ptr" is actually a pointer to the memory buffer,
	// object, not the address of the data itself.  (This appears to be a private
	// structure, so we don't have access to the innards.)
	// Instead, we have to call clCreateSubBuffer to get a sub-buffer with
	// the required offset.  Unfortunately, we can't call this multiple times,
	// so if we are creating a sub-object of a sub-object, we have to go back
	// and get the buffer from the ultimate owner of the data.
	//
	// We address this by using adding an offset parameter to the data object,
	// which is passed to all OpenCL kernel calls.  A bit wasteful when not
	// needed, but guaranteed to work.  The offset is passed to the kernel
	// and is in units of the basic type (complex, not float!)
	// 
	( * PF_OFFSET_DATA_FN(OBJ_PLATFORM(parent)) ) (QSP_ARG  dp, pix_offset );

#ifdef REMOVE_BAD
	// This has to be part of the platform-specific function !!!
	SET_OBJ_OFFSET(dp,pix_offset);		/* offset was in bytes */
						/* but now is in elements! */
#endif // REMOVE_BAD

	if( IS_BITMAP(parent) ) set_child_bit0(dp,offsets);

	return(dp);
} /* end mk_subseq() */

Data_Obj * _make_subsamp( QSP_ARG_DECL  const char *name, Data_Obj *parent,
		Dimension_Set *dsp, index_t *offsets, incr_t *incrs )
{
	Data_Obj *dp;
	index_t pix_offset;	/* can be neg if image runs backwards... */
	Increment_Set mis, *new_mach_isp=(&mis);
	Increment_Set tis, *new_type_isp=(&tis);
	int i;

	// Defn of this macro doesn't copy any data, 2nd arg not used???
	//INIT_INCSET_PTR_FROM_OBJ(new_mach_isp,new_mach_incset)
	//INIT_INCSET_PTR_FROM_OBJ(new_type_isp,new_type_incset)

	//INIT_INCSET_PTR(new_mach_isp)
	//INIT_INCSET_PTR(new_type_isp)

	if( check_inset(parent,offsets,dsp,incrs,name) < 0 )
		return(NULL);

	dp = new_dobj(name);
	if( dp==NULL )
		return(NULL);

	// init_dp uses AUTO_SHAPE...
	if( init_dp(dp,dsp,OBJ_PREC_PTR(parent) ) == NULL ){
		del_dobj(dp);
		return(NULL);
	}

	/* this must be called before setup_dp, because
	 * flags are first copied from parent
	 */
	parent_relationship(parent,dp);

	/* setup dp sets the increments as if the object were contiguous */
	for(i=0;i<N_DIMENSIONS;i++){
		SET_INCREMENT(new_mach_isp,i, OBJ_MACH_INC(parent,i) * incrs[i] );
		SET_INCREMENT(new_type_isp,i, OBJ_TYPE_INC(parent,i) * incrs[i] );
	}
	set_child_increments(dp,new_type_isp, new_mach_isp);


	// did init_dp already call setup_dp?? YES
	// BUT we need to reset the flags!?
	// dp = setup_dp(dp,OBJ_PREC_PTR(parent));

	// We might want to not use AUTO_SHAPE - for example, if we subsample
	// a column vector that we want to treat as an image?
	if( auto_shape_flags( OBJ_SHAPE(dp) ) < 0 )
		return(NULL);

	check_contiguity(dp);		// almost sure not contiguous if subsample!

	if( dp==NULL )
		return(dp);

	/* pix_offset can be negative if the parent image is a
	 * reversed image...
	 */

	pix_offset = get_pix_base_offset(offsets,parent);

	( * PF_OFFSET_DATA_FN(OBJ_PLATFORM(parent)) ) (QSP_ARG  dp, pix_offset );

	//SET_OBJ_OFFSET(dp,pix_offset);

	if( IS_BITMAP(dp) ) set_child_bit0(dp,offsets);

	return(dp);
} /* end mk_subsamp */

Data_Obj * _mk_ilace( QSP_ARG_DECL  Data_Obj *parent, const char *name, int parity )
{
	Data_Obj *dp;
	Dimension_Set ds1, *dsp=(&ds1);
	int i;
	index_t offset;

	dp=new_dobj(name);
	if( dp==NULL ) return(NULL);

	SET_OBJ_SHAPE(dp,ALLOC_SHAPE);

	DIMSET_COPY(dsp , OBJ_TYPE_DIMS(parent) );
	SET_DIMENSION(dsp,2,DIMENSION(dsp,2) / 2);

	if( set_obj_dimensions(dp,dsp,OBJ_PREC_PTR(parent)) < 0 ){
		rls_shape( OBJ_SHAPE(dp) );
		del_dobj(dp);
		return(NULL);
	}

	/* this must be called before setup_dp, because
	 * flags are first copied from parent
	 */
	parent_relationship(parent,dp);

	for(i=0;i<N_DIMENSIONS;i++){
		SET_OBJ_TYPE_INC(dp,i,OBJ_TYPE_INC(parent,i));
		SET_OBJ_MACH_INC(dp,i,OBJ_MACH_INC(parent,i));
	}
	SET_OBJ_ROW_INC(dp,OBJ_ROW_INC(dp) * 2);

	/* Even parity gets us the first set of lines, but by convention
	 * in video terminology line numbering starts with 1, and the first set
	 * of lines is referred to as the "odd" field.  So the scripts have to
	 * reverse this, it is kind of ugly and would be nice to hide it.
	 * However, historical inertia prevents us from doing it!?
	 */

	if( parity & 1 )
		offset = OBJ_ROW_INC(parent);
	else
		offset = 0;

	( * PF_OFFSET_DATA_FN(OBJ_PLATFORM(parent)) ) (QSP_ARG  dp, offset );

	return(dp);
} // mk_ilace

/* When we relocate a subimage, we also have to relocate any subimages of
 * the subimage!!!  This is why each object has to remember its offset
 * in dt_offset...  We used to keep this in bytes, but as OpenCL needs
 * to do something different, we now keep the offset in elements.
 *
 * The update_offset function should update the offset of a child object
 * relative to its parent...
 */

#define relocate_children(dp)	_relocate_children(QSP_ARG  dp)

static void _relocate_children(QSP_ARG_DECL  Data_Obj *dp )
{
	Node *np;
	Data_Obj *child;

	np=QLIST_HEAD( OBJ_CHILDREN(dp) );
	while(np!=NULL){
		child = (Data_Obj *)NODE_DATA(np);
		( * PF_UPDATE_OFFSET_FN(OBJ_PLATFORM(dp)) ) (QSP_ARG  child );

		if( OBJ_CHILDREN(child) != NULL )
			relocate_children(child);
		np = NODE_NEXT(np);
	}
}

/* Relocate a subimage within the parent - assumes same data type.
 * Checks for valid offsets.
 */

int _relocate_with_offsets( QSP_ARG_DECL  Data_Obj *dp, index_t *offsets )
{
	index_t os;

	if( OBJ_PARENT(dp) == NULL ){
		sprintf(ERROR_STRING,
	"__relocate:  object \"%s\" is not a subimage",
			OBJ_NAME(dp));
		warn(ERROR_STRING);
		return(-1);
	}
		
	if( check_posn(OBJ_PARENT(dp),offsets,
		OBJ_TYPE_DIMS(dp),OBJ_NAME(dp)) < 0 ){

		sprintf(ERROR_STRING,
			"bad relocation info for %s",OBJ_NAME(dp));
		warn(ERROR_STRING);
		return(-1);
	}
	os = get_pix_base_offset(offsets,OBJ_PARENT(dp));

	( * PF_OFFSET_DATA_FN(OBJ_PLATFORM(OBJ_PARENT(dp))) ) (QSP_ARG  dp, os );
	//SET_OBJ_OFFSET(dp,os);

	/*
	 * Need to recompute the data pointers of any children
	 */

	if( OBJ_CHILDREN(dp) != NULL )
		relocate_children(dp);

	return(0);
} /* __relocate */

/* relocate position of the subimage */
int _relocate( QSP_ARG_DECL  Data_Obj *dp, index_t xos, index_t yos,index_t tos )
{
	index_t offsets[N_DIMENSIONS];

	offsets[0]=0L;
	offsets[1]=xos;
	offsets[2]=yos;
	offsets[3]=tos;
	offsets[4]=0L;

	return( relocate_with_offsets(dp,offsets) );
}


Data_Obj *
_mk_subimg( QSP_ARG_DECL  Data_Obj *parent, index_t xos,index_t yos, const char *name, dimension_t rows,dimension_t cols )
{
	index_t offsets[N_DIMENSIONS];
	Dimension_Set ds1, *dsp=(&ds1);

	offsets[0]=0L;	SET_DIMENSION(dsp,0,OBJ_TYPE_DIM(parent,0));
	offsets[1]=xos;	SET_DIMENSION(dsp,1,cols);
	offsets[2]=yos;	SET_DIMENSION(dsp,2,rows);
	offsets[3]=0L;	SET_DIMENSION(dsp,3,OBJ_TYPE_DIM(parent,3));
	offsets[4]=0L;	SET_DIMENSION(dsp,4,OBJ_TYPE_DIM(parent,4));

	return(mk_subseq(name,parent,offsets,dsp));
}


Data_Obj *
_mk_substring( QSP_ARG_DECL  Data_Obj *parent, index_t sos,const char *name, dimension_t len )
{
	index_t offsets[N_DIMENSIONS];
	Dimension_Set ds1, *dsp=(&ds1);

	offsets[0]=sos;	SET_DIMENSION(dsp,0,len);
	offsets[1]=0L;	SET_DIMENSION(dsp,1,OBJ_TYPE_DIM(parent,1));
	offsets[2]=0L;	SET_DIMENSION(dsp,2,OBJ_TYPE_DIM(parent,2));
	offsets[3]=0L;	SET_DIMENSION(dsp,3,OBJ_TYPE_DIM(parent,3));
	offsets[4]=0L;	SET_DIMENSION(dsp,4,OBJ_TYPE_DIM(parent,4));

	return(mk_subseq(name,parent,offsets,dsp));
}

Data_Obj * _nmk_subimg( QSP_ARG_DECL  Data_Obj *parent, index_t xos,index_t yos, const char *name, dimension_t rows,dimension_t cols,dimension_t tdim )
{
	index_t offsets[N_DIMENSIONS];
	Dimension_Set ds1, *dsp=(&ds1);

	offsets[0]=0L;	SET_DIMENSION(dsp,0,tdim);
	offsets[1]=xos;	SET_DIMENSION(dsp,1,cols);
	offsets[2]=yos;	SET_DIMENSION(dsp,2,rows);
	offsets[3]=0L;	SET_DIMENSION(dsp,3,OBJ_TYPE_DIM(parent,3));
	offsets[4]=0L;	SET_DIMENSION(dsp,4,OBJ_TYPE_DIM(parent,4));

	return(mk_subseq(name,parent,offsets,dsp));
}

/* get_machine_dimensions - utility function to support make_equivalence
 */

#define get_machine_dimensions(eqd_p) _get_machine_dimensions(QSP_ARG  eqd_p)

static void _get_machine_dimensions(QSP_ARG_DECL  Equivalence_Data *eqd_p)
{
	DIMSET_COPY( EQ_MACH_DIMS(eqd_p), EQ_TYPE_DIMS(eqd_p));

	if( BITMAP_PRECISION( EQ_PREC_CODE(eqd_p) ) ){
		if( DIMENSION(EQ_TYPE_DIMS(eqd_p),0) != 1 )
			error1("get_machine_dimensions:  Sorry, don't handle multi-component bitmaps");
		SET_DIMENSION(EQ_MACH_DIMS(eqd_p),0,1);

		// round number of columns up
		// BUG for a bitmap, we should represent it as a row vector?
		// But that might not be OK of we have a rare legal
		// non-contiguous bitmap...
		SET_DIMENSION(EQ_MACH_DIMS(eqd_p),1,N_BITMAP_WORDS(DIMENSION(EQ_TYPE_DIMS(eqd_p),1)));
	} else if( COMPLEX_PRECISION( EQ_PREC_CODE(eqd_p)) ){
		// complex can't have a component dimension
		if( DIMENSION(EQ_TYPE_DIMS(eqd_p),0) != 1 ){
			sprintf(ERROR_STRING,
		"Sorry, complex images must have component dimension (%d) equal to 1",
				DIMENSION(EQ_TYPE_DIMS(eqd_p),0));
			error1(ERROR_STRING);
		}
		SET_DIMENSION(EQ_MACH_DIMS(eqd_p),0,2);
	} else if( QUAT_PRECISION(EQ_PREC_CODE(eqd_p)) ){
		if( DIMENSION(EQ_TYPE_DIMS(eqd_p),0) != 1 )
			error1("Sorry, complex quaternion images must have component dimension equal to 1");
		SET_DIMENSION(EQ_MACH_DIMS(eqd_p),0,4);
	}
}

#define get_n_per_child(eqd_p) _get_n_per_child(QSP_ARG  eqd_p )

static int _get_n_per_child(QSP_ARG_DECL  Equivalence_Data *eqd_p)
{
	incr_t n_contig;
	int parent_dim;

	SET_EQ_N_PER_CHILD(eqd_p, n_bytes_per_child_elt / n_bytes_per_parent_elt);
	/* new size is larger, first increment must be 1 */

	/* Find the largest number of contiguous elements in the parent */
	n_contig=1;
	for(parent_dim=0;parent_dim<N_DIMENSIONS;parent_dim++)
		if( OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim) == n_contig )
			n_contig *= OBJ_MACH_DIM(EQ_PARENT(eqd_p),parent_dim);
	/* n_contig is the number of contiguous machine elements in the parent... */

	if( n_contig < EQ_N_PER_CHILD(eqd_p) ){
		sprintf(ERROR_STRING,
	"get_n_per_child:  parent object %s n_contig = %d < %d, can't cast to %s",
			OBJ_NAME(EQ_PARENT(eqd_p)),n_contig,EQ_N_PER_CHILD(eqd_p),PREC_NAME(EQ_PREC_PTR(eqd_p)));
		warn(ERROR_STRING);
		return -1;
	}
	return 0;
}

// Figure out the relation between the element sizes

#define compare_element_sizes(eqd_p) _compare_element_sizes(QSP_ARG  eqd_p)

static int _compare_element_sizes(QSP_ARG_DECL  Equivalence_Data *eqd_p)
{
	n_bytes_per_child_elt = PREC_SIZE(EQ_PREC_PTR(eqd_p));
	n_bytes_per_parent_elt = PREC_SIZE(OBJ_PREC_PTR(EQ_PARENT(eqd_p)));

	/* Now we know how many bits in each basic element.
	 * Figure out how many elements of one makes up an element of the other.
	 * The results end up in n_per_parent and n_per_child.
	 *
	 * Bitmaps are a special case...
	 */
	SET_EQ_N_PER_PARENT(eqd_p,1);
	SET_EQ_N_PER_CHILD(eqd_p,1);

	/* Case 1:  child element size is greater than parent element size - casting up */
	if( n_bytes_per_child_elt > n_bytes_per_parent_elt ) {
		assert(n_bytes_per_child_elt % n_bytes_per_parent_elt == 0 );
		if( get_n_per_child(eqd_p) < 0 )
			return -1;
	} else if( n_bytes_per_child_elt < n_bytes_per_parent_elt ) {
		/* Case 2:  child element size is less than parent element size - casting down */
		//n_per_parent = OBJ_PREC_MACH_SIZE( parent ) / PREC_SIZE( prec & MACH_PREC_MASK ) ;
		assert(n_bytes_per_parent_elt % n_bytes_per_child_elt == 0 );
		n_per_parent = n_bytes_per_parent_elt / n_bytes_per_child_elt ;
	}
	return 0;
}

#define check_eq_size_match(eqd_p) _check_eq_size_match(QSP_ARG  eqd_p )

static int _check_eq_size_match(QSP_ARG_DECL  Equivalence_Data *eqd_p)
{
	int child_dim;

	total_child_bytes = 1;
	assert( EQ_TYPE_DIMS(eqd_p) != NULL );
	for(child_dim=0;child_dim<N_DIMENSIONS;child_dim++)
		total_child_bytes *= DIMENSION( EQ_TYPE_DIMS(eqd_p),child_dim);

	if( EQ_PREC_CODE(eqd_p) == PREC_BIT ){
		/* convert number of bits to number of words */
		total_child_bytes += BITS_PER_BITMAP_WORD - 1;
		total_child_bytes /= BITS_PER_BITMAP_WORD;
		total_child_bytes *= PREC_MACH_SIZE( EQ_PREC_PTR(eqd_p) );
	} else {
		total_child_bytes *= PREC_SIZE( EQ_PREC_PTR(eqd_p) );
	}


	total_parent_bytes = ELEMENT_SIZE( EQ_PARENT(eqd_p) ) *
				OBJ_N_MACH_ELTS( EQ_PARENT(eqd_p) );

	if( total_child_bytes != total_parent_bytes){
		sprintf(ERROR_STRING,
	"make_equivalence %s:  total requested size (%d bytes) does not match parent %s (%d bytes)",
			EQ_NAME(eqd_p),total_child_bytes,
			OBJ_NAME( EQ_PARENT(eqd_p) ),total_parent_bytes);
		warn(ERROR_STRING);
		return -1;
	}
	return 0;
}

/* make_equivalence
 *
 * Make an object of arbirary shape, which points to the data area
 * of an existing object.  It should not be necessary that the
 * parent object be contiguous as long as the dimensions
 * and increments of the new object can be set appropriately.
 *
 * A common use is to cast between a long integer to 4 bytes,
 * and vice-versa.  For example, we can copy byte images faster
 * if we do long word transfers.  (Note:  that particular
 * example has been implemented inside of vmov, so is no longer
 * very relevant.)  As long as the bytes in a row
 * are contiguous, and the number of bytes is a multiple of 4,
 * then it doesn't matter if the rows are contiguous.  Calculation
 * of the increments is tricky, however...
 *
 * Correct calculation of the increments has not yet been
 * implemented.  BUG
 *
 * It is not clear if the above comment is accurate and up-to-date;
 * We need to develope a comprehensive test suite!
 *
 * Here is an outline of the general strategy:  if parent and child
 * are different types, we compute n_per_child and n_per_parent,
 * which represent the number of elements of the other object which
 * make up one element of the given object.  For example, if we
 * are casting a byte image (the parent) to a long image (the child)
 * then n_per_parent is 1 (actually 1/4!?), and n_per_child is 4.
 * Then, starting at the bottom, we count elements up to the next
 * dimension boundary (parent or child).  When we reach a dimension
 * boundary of the child, we have to set the child increment.
 * If this boundary coincides with a dimension boundary of the parent,
 * then we can use the parent increment (which doesn't have to be
 * the same dimension).  If we reach a parent dimension boundary
 * in the middle of a child dimension, then the parent has to be
 * contiguous or there will be trouble.
 *
 * There is clearly a problem here with bitmaps...
 * And what about complex?
 * We are trying to fix this with typ instead of mach...
 *
 * Type/Mach dimensions and increments...
 *
 * For complex:  type dimension and inc are number of cpx numbers:
 *	r1 i1 r2 i2 r3 i3	type_dim = 3 mach_dim = 6
 *	R1 I1 R2 I2 R3 I3	type_inc = 1 mach_inc = 2 (col)
 *
 * But for bitmaps, type dimension boundaries may not align
 * with word boundaries; example uses 4 bit words:
 *
 *	bbbb bb		type_dim = 6  mach_dim = 2 ?
 *	bb bbbb		type_inc = 1 (col), 6 (row)  mach_inc = 1 (col), 2 (row)
 *
 * Because bitmaps can include don't-care bits at the end, the dimensions
 * may not be an exact match...
 */

Data_Obj *_make_equivalence( QSP_ARG_DECL  const char *name, Data_Obj *parent, Dimension_Set *dsp, Precision * prec_p )
{
	Data_Obj *newdp;
	const char *s;
	int parent_dim,child_dim;
	dimension_t	prev_parent_mach_elts;
	incr_t child_mach_inc;
	incr_t new_mach_inc[N_DIMENSIONS];
	int multiplier, divisor;
	Dimension_Set ds1;
	Equivalence_Data eqd1, *eqd_p=(&eqd1);

	bzero(eqd_p,sizeof(*eqd_p));
	eqd1.eqd_name = name;
	eqd1.eqd_parent = parent;
	eqd1.eqd_prec_p = prec_p;
	eqd1.eqd_type_dsp = dsp;
	eqd1.eqd_mach_dsp = (&ds1);

	/* If we are casting to a larger machine type (e.g. byte to long)
	 * We have to have at least 4 bytes contiguous.
	 */

	/* Step 1
	 *
	 * Figure out how the elements match up.
	 */

	if( compare_element_sizes(eqd_p) < 0 )
		return NULL;

	/* first make sure the total size matches */

	/* We need the machine dimensions of the new object */
	// They should only differ for complex, quaternion, color and bit?
	get_machine_dimensions(eqd_p);

	if( check_eq_size_match(eqd_p /*new_dsp,prec_p,parent*/) < 0 )
		return NULL;

	/* Now we need to see if we can come up with
	 * a new set of increments that works.
	 * 
	 * We have two kinds of problems:
	 * If we are coalescing rows in a subimage, then we won't be able
	 * to do this with a single increment and should fail.
	 *
	 * But if the parent image is an evenly spaced component image,
	 * or a column, then we shouldn't have a problem...
	 *
	 * "n_per_child" is the number of elements from the parent object
	 * that are combined to make up one of the child elements, if we
	 * are casting up in size, one otherwise.
	 *	ppPPppPPppPP
	 *	ccccCCCCcccc	two parent elts for each child elt
	 *
	 * "n_per_parent" is the number of elements of the
	 * child that are formed from a single element of the parent, if
	 * we are casting down in size, or 1 otherwise.
	 *	ppppPPPPpppp
	 *	ccCCccCCccCC	two child elts for each parent elt
	 *
	 * We maintain a count of how many elements we have for each, and add
	 * the next dimension to the one which is smaller.
	 *
	 * We have a special problem with bitmaps, because the machine unit
	 * is a word, but we count the dimensions in bits.  Remember we pad
	 * rows to be an integral number of words...
	 *
	 * 
	 */

fprintf(stderr,"n_per_child = %d,  n_per_parent = %d\n",n_per_child,n_per_parent);
	if( n_per_parent > 1 ){
		// If we have multiple child elements per single
		// parent element, they have to be contiguous
		child_mach_inc = 1;
	} else {
		/* The size of a child element is greater than or equal to
		 * the size of a parent element.
		 * Set child_mach_inc to the smallest non-zero increment of the parent.
		 * (What is the sense of that?)
		 * The parent has to have contiguous elements to fill a child
		 * element, but could then skip...
		 */
		int i;

		child_mach_inc=0;
		i=0;
		while( child_mach_inc==0 && i < N_DIMENSIONS ){
			child_mach_inc = OBJ_MACH_INC(parent,i);
			i++;
		}
		if( child_mach_inc == 0 ){
			if( total_child_bytes == total_parent_bytes &&
				total_child_bytes == PREC_SIZE(prec_p) )
				child_mach_inc=1;
		}

		assert( child_mach_inc != 0 );

		/* Is this correct in all cases?  If multiple parent elements make up one child
		 * element, the contiguity check should have been performed above.  If they are the same
		 * size then we are ok.
		 */
	}

	child_dim=0;	/* index of current child dimension/increment */
	parent_dim=0;	/* index of current parent dimension/increment */

	// Set the total number of bytes equal to the bits
	// in the first element.

	// If the parent is a bitmap, this is NOT the number of machine elts!?
	total_child_bytes = PREC_MACH_SIZE( prec_p );
	total_child_bytes *= DIMENSION(EQ_MACH_DIMS(eqd_p),0);

	total_parent_bytes = OBJ_PREC_MACH_SIZE( EQ_PARENT(eqd_p) );
	total_parent_bytes *= OBJ_MACH_DIM(EQ_PARENT(eqd_p),0);


	/* total_parent_bytes, total_child_bytes count the number of elements of the parent
	 * that have been used at the current level of the dimension counters.
	 *
	 * child_mach_inc is the current increment to get to the next child element...
	 * it is the parent increment unless we are casting to a smaller type.
	 *
	 * But what about bitmaps???  BUGGY!?
	 * the dsp arg refers to type dimension, not machine dimension...
	 */

	while( parent_dim < N_DIMENSIONS || child_dim < N_DIMENSIONS ){
//fprintf(stderr,"make_equivalence:  parent_dim = %d (%d), total_parent_bytes = %d, child_dim = %d (%d), total_child_bytes = %d\n",
//parent_dim,OBJ_MACH_DIM(parent,parent_dim),total_parent_bytes,child_dim,DIMENSION(new_dsp,child_dim),total_child_bytes);
		if( total_parent_bytes == total_child_bytes ){
			/* increase the child dimension */
			new_mach_inc[child_dim] = child_mach_inc;
			child_dim++;
			if( child_dim < N_DIMENSIONS )
				total_child_bytes *= DIMENSION(EQ_MACH_DIMS(eqd_p),child_dim);
			/* increase the parent dimension */
			parent_dim++;
			if( parent_dim < N_DIMENSIONS ){
				total_parent_bytes *= OBJ_MACH_DIM(EQ_PARENT(eqd_p),parent_dim);

				/* BUG - need to make sure that the math comes out even, no fractions or truncations */
				if( OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim) != 0 ){
					child_mach_inc = n_per_parent * OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim) / n_per_child;
				}
			}
		} else if( total_parent_bytes > total_child_bytes ){
			/* Increment the child dimension only.
			 */
			if( child_dim >= N_DIMENSIONS ){
				warn("make_equivalence:  giving up!?");
				return(NULL);
			}
			/* increase the child dimension */
			new_mach_inc[child_dim] = child_mach_inc;
			child_dim++;
			if( child_dim < N_DIMENSIONS )
				total_child_bytes *= DIMENSION(EQ_MACH_DIMS(eqd_p),child_dim);
			/* Increasing the increment assumes that the spacing
			 * within the parent is even at this larger size.
			 * This is guaranteed it the new child size is LTE
			 * the parent size.  Otherwise, we will have to increase
			 * the parent dim next time round, and that is when
			 * we will check it.
			 */
			/* assumes child is evenly-spaced between the new dimension and the old */ 
			if( new_mach_inc[child_dim-1] > 0 )
				child_mach_inc = DIMENSION(EQ_MACH_DIMS(eqd_p),child_dim-1) * new_mach_inc[child_dim-1];
		} else { /* total_parent_bytes < total_child_bytes */
			/* Increment the parent dimension WITHOUT incrementing the child.
			 *
			 * The parent MUST be even-spaced across the dimensions
			 */
			/* increase the parent dimension */
			parent_dim++;
			prev_parent_mach_elts = total_parent_bytes;
			if( parent_dim < N_DIMENSIONS ){
				total_parent_bytes *= OBJ_MACH_DIM(EQ_PARENT(eqd_p),parent_dim);
			} else if( PREC_CODE(prec_p) == PREC_BIT ){
fprintf(stderr,"child_dim = %d\n",child_dim);
				total_child_bytes = (total_child_bytes + BITS_PER_BITMAP_WORD -1 ) / BITS_PER_BITMAP_WORD;
fprintf(stderr,"total_child_bytes = %d\n",total_child_bytes);
			} else {
				sprintf(ERROR_STRING,"make_equivalence:  can't fit objects");
				warn(ERROR_STRING);
				sprintf(ERROR_STRING,"total_parent_bytes = %d   total_child_bytes = %d",
					total_parent_bytes,total_child_bytes);
				advise(ERROR_STRING);
				return(NULL);
			}
			/* When we bring in more parent elements
			 * to fill in a child dimension, we have to insure that
			 * the spacing remains even.
			 *
			 * This test is like the contiguity test - but we only care
			 * if we already were using more than 1 element from the parent...
			 *
			 * We only check for non-zero parent increments.
			 * But we probably should remember the last non-zero increment?
			 */

			if( prev_parent_mach_elts > 1 && OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim) != 0 &&
							OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim-1) != 0 &&
					OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim) !=
					(incr_t)(OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim-1)
					* OBJ_MACH_DIM(EQ_PARENT(eqd_p),parent_dim-1)) ){
				sprintf(ERROR_STRING,
						"make_equivalence:  problem with unevenly spaced parent object %s",OBJ_NAME(EQ_PARENT(eqd_p)));
				warn(ERROR_STRING);
				sprintf(ERROR_STRING,"%s inc[%d] (%d) != inc[%d] (%d) * dim[%d] (%d)",
					OBJ_NAME(EQ_PARENT(eqd_p)),parent_dim,OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim),
					parent_dim-1,OBJ_MACH_INC(EQ_PARENT(eqd_p),parent_dim-1),
					parent_dim-1,OBJ_MACH_DIM(EQ_PARENT(eqd_p),parent_dim-1));
				advise(ERROR_STRING);

				sprintf(ERROR_STRING,"prev_parent_mach_elts = %d, total_parent_bytes = %d, total_child_bytes = %d",
					prev_parent_mach_elts,total_parent_bytes,total_child_bytes);
				advise(ERROR_STRING);

				return(NULL);
			}

		}
	}

	newdp=new_dobj(name);

	if( newdp == NULL ){
		warn("couldn't create equivalence");
		return(NULL);
	}

	s = OBJ_NAME(newdp);	/* save, while we overwrite temporarily */
	//*newdp = *parent;	/* copy all of the fields... */
	OBJ_COPY_FROM(newdp, parent);	// newdp points to parent's shape...
	// after the copy, the shape pointer is the same as the parent's...
	DUP_OBJ_SHAPE(newdp,parent);

	SET_OBJ_NAME(newdp,s);
	SET_OBJ_OFFSET(newdp,0);

	if( set_obj_dimensions(newdp,dsp,prec_p) ){
		rls_shape( OBJ_SHAPE(newdp) );
		del_dobj(newdp);
		return(NULL);
	}

	if( IS_CONTIGUOUS(parent) )
		make_contiguous(newdp);
	else if( IS_EVENLY_SPACED(parent) ){
		incr_t	parent_mach_inc=0,
			parent_type_inc=0;
			/* autoinit to elim compiler warning */
		dimension_t pdim;

		/* We insist that the parent is evenly spaced...
		 *
		 * We need to figure out what the increment should be...
		 * we need to keep track of which dimension of the parent we are in...
		 *
		 * One case:  cast a column to an image.
		 *	in the child, the column increment is the rowinc of the parent
		 *	the rowinc of the child is the column inc * the number of columns.
		 *	Once we find the lowest increment, we are good to go.
		 *
		 * 2nd case:  cast an image to a row or column...
		 *	the increment is the column increment of the parent.
		 */
		pdim=1;
		/* find the basic parent increment */
		for(parent_dim=0;parent_dim<N_DIMENSIONS;parent_dim++){
			//pdim *= OBJ_MACH_DIM(parent,parent_dim);	/* how many elements we are up to */
			pdim *= OBJ_TYPE_DIM(parent,parent_dim);	/* how many elements we are up to */
			if( pdim > 1 ){
				parent_mach_inc = OBJ_MACH_INC(parent,parent_dim);
				parent_type_inc = OBJ_TYPE_INC(parent,parent_dim);
				parent_dim=N_DIMENSIONS;			/* break out of loop */
			}
		}

		SET_OBJ_MACH_INC(newdp,0,parent_mach_inc);
		SET_OBJ_TYPE_INC(newdp,0,parent_type_inc);
		for(child_dim=1;child_dim<N_DIMENSIONS;child_dim++){
			SET_OBJ_MACH_INC(newdp,child_dim,OBJ_MACH_INC(newdp,child_dim-1) * OBJ_MACH_DIM(newdp,child_dim-1));
			SET_OBJ_TYPE_INC(newdp,child_dim,OBJ_TYPE_INC(newdp,child_dim-1) * OBJ_TYPE_DIM(newdp,child_dim-1));
		}
	} else {	/* Not evenly spaced or contiguous... */
		//
		for(child_dim=0;child_dim<N_DIMENSIONS;child_dim++){
			SET_OBJ_MACH_INC(newdp,child_dim,new_mach_inc[child_dim]);
		}
	}

	/* Now we have the machine increments.
	 * Copy them to the type increments, and adjust if necessary.
	 */
	if( COMPLEX_PRECISION(PREC_CODE(prec_p)) )
		divisor=2;
	else if( QUAT_PRECISION(PREC_CODE(prec_p)) )
		divisor=4;
	else	divisor=1;

	if( BITMAP_PRECISION(PREC_CODE(prec_p)) )
		multiplier = BITS_PER_BITMAP_WORD;
	else	multiplier = 1;

	for(child_dim=0;child_dim<N_DIMENSIONS;child_dim++){
		SET_OBJ_TYPE_INC(newdp,child_dim,multiplier * OBJ_MACH_INC(newdp,child_dim) / divisor);
	}

	/* Where should we adjust the row increment when the parent is a bitmap? */


	/* this must be called before setup_dp,
	 * because flags are first copied from parent
	 */
	parent_relationship(parent,newdp);

	newdp=setup_dp(newdp,prec_p);
	assert( newdp != NULL );


	/* If the parent is a bitmap, but the equivalence is not, then
	 * we need to clear the bitmap flag...
	 * We also need to adjust the row increment...
	 */

	if( IS_BITMAP(newdp) && PREC_CODE(prec_p) != PREC_BIT ){
		CLEAR_OBJ_FLAG_BITS(newdp,DT_BIT);
	}

	return(newdp);
} /* end make_equivalence */

void propagate_flag_to_children(Data_Obj *dp, uint32_t flags_to_set )
{
	Data_Obj *child;
	Node *np;

	if( OBJ_CHILDREN(dp) == NULL ) return;

	np = QLIST_HEAD( OBJ_CHILDREN(dp) );
	while( np != NULL ){
		child = (Data_Obj *)NODE_DATA(np);

		SET_OBJ_FLAG_BITS(child,flags_to_set);

		if( OBJ_CHILDREN(dp) != NULL ) propagate_flag_to_children(child,flags_to_set);

		np = NODE_NEXT(np);
	}
}

