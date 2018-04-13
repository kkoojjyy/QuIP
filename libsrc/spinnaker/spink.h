#ifndef _SPINK_H_
#define _SPINK_H_

#include "quip_config.h"
#include "data_obj.h"
#include "fio_api.h"

#ifdef HAVE_LIBSPINNAKER
#include "SpinnakerC.h"
#endif // HAVE_LIBSPINNAKER

#define BOOL	bool8_t

#include "spink_funcs.h"

// jbm made these limits up...
// We should do something sensible here, but it is difficult
// because we don't necessarily know how many buffers we can
// allocate.  USBFS has a default limit of 16MB, but on euler
// we have increased it to 200MB (in /etc/default/grub, see PGR
// TAN, and pointed out by Brian Cha).
// [the above comment originally from the flycap implementation, but should apply to Spinnaker???]
//
#define MIN_N_BUFFERS 2
//#define MAX_N_BUFFERS 1024
#define MAX_N_BUFFERS 64

// Compiler warning C4996 suppressed due to deprecated strcpy() and sprintf()
// functions on Windows platform.
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    #pragma warning(disable : 4996)
#endif

// This macro helps with C-strings.
#define MAX_BUFF_LEN 256

#define MAX_NODE_VALUE_CHARS_TO_PRINT	24	// must be less than LLEN !!

// BUG - if the constant above is changed, these definitions should be changed
// to match...   We could generate them programmatically, but that's extra work
// that we will skip for now.

#define INT_NODE_FMT_STR	"%-24ld"
#define FLT_NODE_FMT_STR	"%-24f"
#define STRING_NODE_FMT_STR	"%-24s"

// a couple of globals...
extern spinCameraList hCameraList;
extern size_t numCameras;
extern int current_node_idx;

// forward declarations
struct spink_map;
struct spink_node;

// a global - used for formatting the print-out of nodes
extern int max_display_name_len;

typedef enum {
	FLOAT_CHUNK_DATA,
	INT_CHUNK_DATA
} Chunk_Data_Type;

typedef struct chunk_data {
	const char *	cd_name;
	Chunk_Data_Type	cd_type;
	union {
		double u_fltval;
		int64_t u_intval;
	} cd_u;
} Chunk_Data;

ITEM_INTERFACE_PROTOTYPES(Chunk_Data,chunk_data)

#define pick_chunk_data(s)	_pick_chunk_data(QSP_ARG  s)
#define new_chunk_data(s)	_new_chunk_data(QSP_ARG  s)
#define list_chunk_datas(fp)	_list_chunk_datas(QSP_ARG  fp)

typedef struct spink_node_type {
	const char *		snt_name;
#ifdef HAVE_LIBSPINNAKER
	spinNodeType		snt_type;
#endif // HAVE_LIBSPINNAKER
	void (*snt_set_func)(QSP_ARG_DECL  struct spink_node *skn_p);
	void (*snt_print_value_func)(QSP_ARG_DECL  struct spink_node *skn_p);
} Spink_Node_Type;

ITEM_INTERFACE_PROTOTYPES(Spink_Node_Type,spink_node_type)
#define init_spink_node_types()	_init_spink_node_types(SINGLE_QSP_ARG)
#define spink_node_type_list()	_spink_node_type_list(SINGLE_QSP_ARG)
#define new_spink_node_type(name)	_new_spink_node_type(QSP_ARG  name)

// It's kind of wasteful to duplicate information that is present
// in the SDK node structure...  but it's not a lot of storage so
// for now we won't worry about it.

typedef struct spink_node {
	const char *		skn_name;
	struct spink_map *	skn_skm_p;
	struct spink_node *	skn_parent;
	int			skn_idx;	// within the parent
	int			skn_level;	// tree depth
	int			skn_flags;
	Spink_Node_Type *	skn_type_p;
	List *			skn_children;
	// We tried using enum values instead of int values, but not all nodes support that...
	int64_t			skn_enum_ival;	// only used by enum_entry nodes...
#define INVALID_ENUM_INT_VALUE	(-1)

//#ifdef HAVE_LIBSPINNAKER
//	spinNodeType		skn_type;
//	// do the handles persist???
//	//spinNodeHandle	skn_handle;
//#endif // HAVE_LIBSPINNAKER
} Spink_Node;

// flag bits
#define NODE_READABLE	1
#define NODE_WRITABLE	2

#define NODE_IS_READABLE(skn_p)	((skn_p)->skn_flags & NODE_READABLE)
#define NODE_IS_WRITABLE(skn_p)	((skn_p)->skn_flags & NODE_WRITABLE)

ITEM_INTERFACE_PROTOTYPES(Spink_Node,spink_node)

#define new_spink_node(s)	_new_spink_node(QSP_ARG  s)
#define del_spink_node(skn_p)	_del_spink_node(QSP_ARG  skn_p)
#define get_spink_node(s)	_get_spink_node(QSP_ARG  s)
#define spink_node_of(s)	_spink_node_of(QSP_ARG  s)
#define init_spink_nodes()	_init_spink_nodes(SINGLE_QSP_ARG)
#define list_spink_nodes(fp)	_list_spink_nodes(QSP_ARG  fp)
#define pick_spink_node(s)	_pick_spink_node(QSP_ARG  s)
#define spink_node_list()	_spink_node_list(SINGLE_QSP_ARG)

typedef struct spink_cat {
	const char *	sct_name;
	Spink_Node *	sct_root_p;
} Spink_Category;

ITEM_INTERFACE_PROTOTYPES(Spink_Category,spink_cat)

#define new_spink_cat(s)	_new_spink_cat(QSP_ARG  s)
#define del_spink_cat(p)	_del_spink_cat(QSP_ARG  p)
#define spink_cat_of(s)		_spink_cat_of(QSP_ARG  s)
#define init_spink_cats()	_init_spink_cats(SINGLE_QSP_ARG)
#define list_spink_cats(fp)	_list_spink_cats(QSP_ARG  fp)
#define pick_spink_cat(s)	_pick_spink_cat(QSP_ARG  s)
#define spink_cat_list()	_spink_cat_list(SINGLE_QSP_ARG)


typedef struct spink_enum_val {
	const char *	sev_name;
	size_t		sev_value;
} Spink_Enum_Val;

ITEM_INTERFACE_PROTOTYPES(Spink_Enum_Val,spink_enum_val)

#define new_spink_enum_val(s)		_new_spink_enum_val(QSP_ARG  s)
#define spink_enum_val_of(s)		_spink_enum_val_of(QSP_ARG  s)
#define init_spink_enum_vals()		_init_spink_enum_vals(SINGLE_QSP_ARG)
#define list_spink_enum_vals(fp)	_list_spink_enum_vals(QSP_ARG  fp)
#define pick_spink_enum_val(s)		_pick_spink_enum_val(QSP_ARG  s)
#define spink_enum_val_list()		_spink_enum_val_list(SINGLE_QSP_ARG)


typedef enum {
	INVALID_NODE_MAP,
	CAM_NODE_MAP,
	DEV_NODE_MAP,
	STREAM_NODE_MAP,
	N_NODE_MAP_TYPES
} Node_Map_Type;

struct spink_cam;

typedef struct spink_map {
	const char *		skm_name;
	struct spink_cam *	skm_skc_p;
	Node_Map_Type		skm_type;
	Spink_Node *		skm_root_p;
#ifdef HAVE_LIBSPINNAKER
	// do the maps persist?
	// spinNodeMapHandle	skm_handle;
#endif // HAVE_LIBSPINNAKER
	Item_Context *		skm_node_icp;
	Item_Context *		skm_cat_icp;
} Spink_Map;

ITEM_INTERFACE_PROTOTYPES(Spink_Map,spink_map)

#define new_spink_map(s)	_new_spink_map(QSP_ARG  s)
#define del_spink_map(s)	_del_spink_map(QSP_ARG  s)
#define spink_map_of(s)		_spink_map_of(QSP_ARG  s)
#define list_spink_maps(fp)	_list_spink_maps(QSP_ARG  fp)
#define pick_spink_map(s)	_pick_spink_map(QSP_ARG  s)
#define spink_map_list()	_spink_map_list(SINGLE_QSP_ARG)

#define N_FMT7_MODES	5	// PGR has 32, but why bother?

typedef int Framerate_Mask;

typedef struct spink_cam {
	const char *		skc_name;
	int			skc_sys_idx;
	int			skc_iface_idx;
	struct spink_map *	skc_dev_map;
	struct spink_map *	skc_cam_map;
	struct spink_map *	skc_stream_map;
#ifdef HAVE_LIBSPINNAKER
	spinCamera		skc_current_handle;	// non-NULL if we are holding a handle -
							// set to NULL when released...
//	spinNodeMapHandle	skc_TL_dev_node_map;	// hNodeMapTLDevice
	//spinImage		skc_img_tbl[MAX_N_BUFFERS];
#endif /* HAVE_LIBSPINNAKER */

	unsigned int		skc_cols;
	unsigned int		skc_rows;
	unsigned int		skc_depth;	// bytes per pixel
	int			skc_n_buffers;
	int			skc_newest;
	int			skc_oldest;
	Data_Obj **		skc_frm_dp_tbl;
//	Item_Context *		skc_do_icp;	// data_obj context
//	List *			skc_in_use_lp;	// list of frames...
//	List *			skc_feat_lp;
	u_long			skc_flags;
} Spink_Cam;

ITEM_INTERFACE_PROTOTYPES(Spink_Cam,spink_cam)

#define new_spink_cam(s)	_new_spink_cam(QSP_ARG  s)
#define del_spink_cam(s)	_del_spink_cam(QSP_ARG  s)
#define spink_cam_of(s)	_spink_cam_of(QSP_ARG  s)
#define list_spink_cams(fp)	_list_spink_cams(QSP_ARG  fp)
#define pick_spink_cam(s)	_pick_spink_cam(QSP_ARG  s)
#define spink_cam_list()	_spink_cam_list(SINGLE_QSP_ARG)

/* flag bits */

#define SPINK_CAM_CONNECTED		1
#define SPINK_CAM_RUNNING		2
#define SPINK_CAM_CAPTURING		4
#define SPINK_CAM_TRANSMITTING		8
#define SPINK_CAM_EVENTS_READY		16

#define IS_CONNECTED(skc_p)	(skc_p->skc_flags & SPINK_CAM_CONNECTED)
#define IS_RUNNING(skc_p)	(skc_p->skc_flags & SPINK_CAM_RUNNING)
#define IS_CAPTURING(skc_p)	(skc_p->skc_flags & SPINK_CAM_CAPTURING)
#define IS_TRANSMITTING(skc_p)	(skc_p->skc_flags & SPINK_CAM_TRANSMITTING)
#define IS_EVENTFUL(skc_p)	(skc_p->skc_flags & SPINK_CAM_EVENTS_READY)

typedef struct spink_interface {
	const char *		ski_name;
	int			ski_idx;
#ifdef HAVE_LIBSPINNAKER
	//spinInterface		ski_handle;
#endif // HAVE_LIBSPINNAKER
} Spink_Interface;

ITEM_INTERFACE_PROTOTYPES(Spink_Interface,spink_interface)
#define new_spink_interface(s)		_new_spink_interface(QSP_ARG  s)
#define del_spink_interface(p)		_del_spink_interface(QSP_ARG  p)
#define spink_interface_of(s)		_spink_interface_of(QSP_ARG  s)
#define list_spink_interfaces(fp)	_list_spink_interfaces(QSP_ARG  fp)
#define pick_spink_interface(s)		_pick_spink_interface(QSP_ARG  s)
#define spink_interface_list()		_spink_interface_list(SINGLE_QSP_ARG)

// information passed to image event handlers...

typedef struct my_event_info {
	Query_Stack *	ei_qsp;
	Spink_Cam *	ei_skc_p;
} Image_Event_Info;

// spink_enum.c
#ifdef HAVE_LIBSPINNAKER
extern int _get_camera_model_name(QSP_ARG_DECL  char *buf, size_t buflen, spinCamera hCam);
#define get_camera_model_name(buf, buflen, map) _get_camera_model_name(QSP_ARG  buf, buflen, map)
extern int _get_camera_vendor_name(QSP_ARG_DECL  char *buf, size_t buflen, spinCamera hCam);
#define get_camera_vendor_name(buf, buflen, hCam) _get_camera_vendor_name(QSP_ARG  buf, buflen, hCam)

extern int _get_interface_name(QSP_ARG_DECL  char *buf, size_t buflen, spinInterface hInterface);
#define get_interface_name(buf, buflen, hInterface)	_get_interface_name(QSP_ARG  buf, buflen, hInterface)

extern int _lookup_spink_node(QSP_ARG_DECL Spink_Node *skn_p, spinNodeHandle *hdl_p);
#define lookup_spink_node(skn_p, hdl_p) _lookup_spink_node(QSP_ARG skn_p, hdl_p)
extern void _print_interface_name(QSP_ARG_DECL  spinNodeHandle hInterfaceDisplayName);
#define print_interface_name(hInterfaceDisplayName)	_print_interface_name(QSP_ARG  hInterfaceDisplayName)
extern int _get_spink_cam_list(QSP_ARG_DECL  spinInterface hInterface, spinCameraList *hCamList_p, size_t *num_p);
#define get_spink_cam_list(hInterface, hCamList_p, num_p)	_get_spink_cam_list(QSP_ARG  hInterface, hCamList_p, num_p)
extern int _release_spink_interface_list( QSP_ARG_DECL  spinInterfaceList *hInterfaceList_p );
#define release_spink_interface_list( hInterfaceList_p )	_release_spink_interface_list( QSP_ARG  hInterfaceList_p )
extern int _get_spink_interface_from_list(QSP_ARG_DECL  spinInterface *hInterface_p, spinInterfaceList hInterfaceList, int idx );
extern int _get_spink_model_name(QSP_ARG_DECL   spinNodeMapHandle hNodeMapTLDevice, char *buf, size_t *len_p );
extern int _print_indexed_spink_cam_info(QSP_ARG_DECL   spinCameraList hCameraList, int idx );
#define get_spink_interface_from_list(hInterface_p, hInterfaceList, idx ) _get_spink_interface_from_list(QSP_ARG  hInterface_p, hInterfaceList, idx )
#define get_spink_model_name(hNodeMapTLDevice, buf, len_p ) _get_spink_model_name(QSP_ARG   hNodeMapTLDevice, buf, len_p )
#define print_indexed_spink_cam_info(hCameraList, idx ) _print_indexed_spink_cam_info(QSP_ARG   hCameraList, idx )

extern int _spink_node_is_readable(QSP_ARG_DECL  spinNodeHandle hdl);
#define spink_node_is_readable(hdl)	_spink_node_is_readable(QSP_ARG  hdl)
extern int _spink_node_is_writable(QSP_ARG_DECL  spinNodeHandle hdl);
#define spink_node_is_writable(hdl)	_spink_node_is_writable(QSP_ARG  hdl)
extern int _spink_node_is_available(QSP_ARG_DECL  spinNodeHandle hdl);
#define spink_node_is_available(hdl)	_spink_node_is_available(QSP_ARG  hdl)
extern int _spink_node_is_implemented(QSP_ARG_DECL  spinNodeHandle hdl);
#define spink_node_is_implemented(hdl)	_spink_node_is_implemented(QSP_ARG  hdl)

extern int _spink_release_cam(QSP_ARG_DECL  Spink_Cam *skc_p);
#define spink_release_cam(skc_p) _spink_release_cam(QSP_ARG  skc_p)

extern int _release_spink_cam_list(QSP_ARG_DECL   spinCameraList *hCamList_p );
#define release_spink_cam_list(hCamList_p )	_release_spink_cam_list(QSP_ARG   hCamList_p )
extern int _fetch_spink_map(QSP_ARG_DECL  spinInterface hInterface, spinNodeMapHandle *hMap_p);
#define fetch_spink_map(hInterface, hMap_p) _fetch_spink_map(QSP_ARG  hInterface, hMap_p)

extern int _get_spink_interfaces(QSP_ARG_DECL  spinSystem hSystem, spinInterfaceList *hInterfaceList_p, size_t *numInterfaces_p);
extern int _get_spink_cameras(QSP_ARG_DECL  spinSystem hSystem, spinCameraList *hCameraList_p, size_t *num_p );

#define get_spink_interfaces(hSystem, hInterfaceList_p, numInterfaces_p) _get_spink_interfaces(QSP_ARG  hSystem, hInterfaceList_p, numInterfaces_p)
#define get_spink_cameras(hSystem, hCameraList_p, num_p ) _get_spink_cameras(QSP_ARG  hSystem, hCameraList_p, num_p )

extern int _get_spink_interface_cameras(QSP_ARG_DECL  spinInterface hInterface);
#define get_spink_interface_cameras(hInterface)	_get_spink_interface_cameras(QSP_ARG  hInterface)

extern void _report_spink_error(QSP_ARG_DECL  spinError error, const char *whence );
#define report_spink_error(error,whence)	_report_spink_error(QSP_ARG  error, whence )

// spink_node_map.c

extern void _report_node_access_error(QSP_ARG_DECL  spinNodeHandle hNode, const char *w);
#define report_node_access_error(hNode, w) _report_node_access_error(QSP_ARG  hNode, w)

extern int _get_display_name_len(QSP_ARG_DECL  spinNodeHandle hdl);
#define get_display_name_len(hdl) _get_display_name_len(QSP_ARG  hdl)

extern void _insure_current_camera(QSP_ARG_DECL  Spink_Cam *skc_p);
extern int _release_current_camera(QSP_ARG_DECL  int verbose);
#define insure_current_camera(skc_p) _insure_current_camera(QSP_ARG  skc_p)
#define release_current_camera(v) _release_current_camera(QSP_ARG  v)


extern int _get_node_map_handle(QSP_ARG_DECL  spinNodeMapHandle *hMap_p,Spink_Map *skm_p, const char *whence);
#define get_node_map_handle(hMap_p,skm_p,w) _get_node_map_handle(QSP_ARG  hMap_p,skm_p,w)

extern int _traverse_by_node_handle(QSP_ARG_DECL  spinNodeHandle hCategoryNode, int level, int (*func)(QSP_ARG_DECL spinNodeHandle hNode, int level) );
#define traverse_by_node_handle(hCategoryNode, level, func ) _traverse_by_node_handle(QSP_ARG  hCategoryNode, level, func )

extern int _print_camera_nodes(QSP_ARG_DECL  Spink_Cam *skc_p);
#define print_camera_nodes(skc_p) _print_camera_nodes(QSP_ARG  skc_p)

extern int _get_node_value_string(QSP_ARG_DECL  char *buf, size_t *buflen_p, spinNodeHandle hNode );
#define get_node_value_string(buf, buflen_p, hNode ) _get_node_value_string(QSP_ARG  buf, buflen_p, hNode )
extern int _print_value_node(QSP_ARG_DECL  spinNodeHandle hNode, unsigned int level);
#define print_value_node(hNode, level) _print_value_node(QSP_ARG  hNode, level)
extern int _get_display_name(QSP_ARG_DECL  char *buf, size_t *len_p, spinNodeHandle hdl);
#define get_display_name(buf, len_p, hdl) _get_display_name(QSP_ARG  buf, len_p, hdl)
extern int _get_node_name(QSP_ARG_DECL  char *buf, size_t *len_p, spinNodeHandle hdl);
#define get_node_name(buf, len_p, hdl) _get_node_name(QSP_ARG  buf, len_p, hdl)
extern int _get_string_node_string(QSP_ARG_DECL  char *buf, size_t *buflen_p, spinNodeHandle hNode );
#define get_string_node_string(buf, buflen_p, hNode ) _get_string_node_string(QSP_ARG  buf, buflen_p, hNode )
extern int _print_string_node(QSP_ARG_DECL  spinNodeHandle hNode, unsigned int level);
#define print_string_node(hNode, level) _print_string_node(QSP_ARG  hNode, level)

extern void _list_nodes_from_map(QSP_ARG_DECL  Spink_Map *skm_p);
#define list_nodes_from_map(skm_p) _list_nodes_from_map(QSP_ARG  skm_p)
extern void _print_spink_node_info(QSP_ARG_DECL Spink_Node *skn_p, int level);
#define print_spink_node_info(skn_p,level) _print_spink_node_info(QSP_ARG skn_p,level)

// spink_acq.c

extern int _set_acquisition_continuous(QSP_ARG_DECL  Spink_Cam *skc_p);
#define set_acquisition_continuous(skc_p) _set_acquisition_continuous(QSP_ARG  skc_p)

extern void _enable_image_events(QSP_ARG_DECL  Spink_Cam *skc_p, void (*func)(spinImage,void *));
#define enable_image_events(skc_p,f) _enable_image_events(QSP_ARG  skc_p,f)

extern void _set_camera_node(QSP_ARG_DECL  Spink_Cam *skc_p, const char *node_name, const char *entry_name);
#define set_camera_node(skc_p, node_name, entry_name) _set_camera_node(QSP_ARG  skc_p, node_name, entry_name)

extern void _set_n_spink_buffers(QSP_ARG_DECL  Spink_Cam *skc_p, int n);
#define set_n_spink_buffers(skc_p, n) _set_n_spink_buffers(QSP_ARG  skc_p, n)
extern void _show_n_buffers(QSP_ARG_DECL  Spink_Cam *skc_p);
#define show_n_buffers(skc_p) _show_n_buffers(QSP_ARG  skc_p)

extern int _next_spink_image(QSP_ARG_DECL  spinImage *img_p, Spink_Cam *skc_p);
#define next_spink_image(img_p, skc_p) _next_spink_image(QSP_ARG  img_p, skc_p)
extern int _spink_start_capture(QSP_ARG_DECL  Spink_Cam *skc_p);
extern int _spink_stop_capture(QSP_ARG_DECL  Spink_Cam *skc_p);

#define spink_start_capture(skc_p) _spink_start_capture(QSP_ARG  skc_p)
#define spink_stop_capture(skc_p) _spink_stop_capture(QSP_ARG  skc_p)

extern int _spink_test_acq(QSP_ARG_DECL  Spink_Cam *skc_p);
#define spink_test_acq(skc_p) _spink_test_acq(QSP_ARG  skc_p)

#endif // HAVE_LIBSPINNAKER

// spink_menu.c

extern void _select_spink_map(QSP_ARG_DECL  Spink_Map *skm_p);
#define select_spink_map(skm_p) _select_spink_map(QSP_ARG  skm_p)


// spink_util.c

extern void _fetch_chunk_data(QSP_ARG_DECL  Chunk_Data *cd_p, Data_Obj *dp);
#define fetch_chunk_data(cd_p, dp) _fetch_chunk_data(QSP_ARG  cd_p, dp)
extern void _enable_chunk_data(QSP_ARG_DECL  Spink_Cam *skc_p, Chunk_Data *cd_p);
#define enable_chunk_data(skc_p, cd_p) _enable_chunk_data(QSP_ARG  skc_p, cd_p)
extern void _format_chunk_data(QSP_ARG_DECL  char *buf, Chunk_Data *cd_p);
#define format_chunk_data(buf, cd_p) _format_chunk_data(QSP_ARG  buf, cd_p)

extern void _pop_map_contexts(SINGLE_QSP_ARG_DECL);
extern void _push_map_contexts(QSP_ARG_DECL  Spink_Map *skm_p);
#define pop_map_contexts() _pop_map_contexts(SINGLE_QSP_ARG)
#define push_map_contexts(skm_p) _push_map_contexts(QSP_ARG  skm_p)

extern void _print_map_tree(QSP_ARG_DECL  Spink_Map *skm_p);
#define print_map_tree(skm_p) _print_map_tree(QSP_ARG  skm_p)

extern void _print_cat_tree(QSP_ARG_DECL  Spink_Category *sct_p);
#define print_cat_tree(sct_p) _print_cat_tree(QSP_ARG  sct_p)

extern Spink_Node_Type *_find_type_by_code(QSP_ARG_DECL  spinNodeType type);
#define find_type_by_code(type) _find_type_by_code(QSP_ARG  type)

extern Item_Context * _pop_spink_node_context(SINGLE_QSP_ARG_DECL);
extern void _push_spink_node_context(QSP_ARG_DECL  Item_Context *icp);
#define pop_spink_node_context() _pop_spink_node_context(SINGLE_QSP_ARG)
#define push_spink_node_context(icp) _push_spink_node_context(QSP_ARG  icp)

extern Item_Context * _pop_spink_cat_context(SINGLE_QSP_ARG_DECL);
extern void _push_spink_cat_context(QSP_ARG_DECL  Item_Context *icp);
#define pop_spink_cat_context() _pop_spink_cat_context(SINGLE_QSP_ARG)
#define push_spink_cat_context(icp) _push_spink_cat_context(QSP_ARG  icp)

extern void _release_spink_cam_system(SINGLE_QSP_ARG_DECL);
#define release_spink_cam_system() _release_spink_cam_system(SINGLE_QSP_ARG)

extern int is_fmt7_mode(QSP_ARG_DECL  Spink_Cam *scp, int idx );
extern int set_fmt7_mode(QSP_ARG_DECL  Spink_Cam *scp, int idx );
extern int set_std_mode(QSP_ARG_DECL  Spink_Cam *fcp, int idx);

//////////////////////////
typedef struct pgr_property_type {
	const char *		name;
#ifdef HAVE_LIBSPINNAKER
	/*
	fc2PropertyType		type_code;
	fc2PropertyInfo 	info;
	fc2Property 		prop;
	*/
#endif // HAVE_LIBSPINNAKER
} Spink_Cam_Property_Type;

ITEM_INTERFACE_PROTOTYPES(Spink_Cam_Property_Type,pgr_prop)

#define pick_pgr_prop(p)	_pick_pgr_prop(QSP_ARG  p)
#define get_pgr_prop(p)		_get_pgr_prop(QSP_ARG  p)
#define new_pgr_prop(p)		_new_pgr_prop(QSP_ARG  p)
#define pgr_prop_list()		_pgr_prop_list(SINGLE_QSP_ARG)

typedef struct fly_frame {
	const char *		pf_name;
#ifdef HAVE_LIBSPINNAKER
	//dc1394video_frame_t *	pf_framep;
#endif
	Data_Obj *		pf_dp;
} Fly_Frame;

typedef struct pgr_prop_val {
	int	pv_is_abs;
	union {
		int	u_i;
		float	u_f;
	} pv_u;
} Spink_Cam_Prop_Val;


#define N_EII_PROPERTIES	10

#ifdef HAVE_LIBSPINNAKER

typedef struct _myEmbeddedImageInfo {
	/*
	fc2EmbeddedImageInfoProperty prop_tbl[N_EII_PROPERTIES];
	*/
} myEmbeddedImageInfo;


#define DECLARE_NAMED_PARAM(stem,type,short_stem,cap_stem)	\
								\
typedef struct named_##stem {					\
	const char *		short_stem##_name;		\
	type			short_stem##_value;		\
} Named_##cap_stem;

#else	// ! HAVE_LIBSPINNAKER

#define DECLARE_NAMED_PARAM(stem,type,short_stem,cap_stem)	\
								\
typedef struct named_##stem {					\
	const char *		short_stem##_name;		\
} Named_##cap_stem;

#endif	// ! HAVE_LIBSPINNAKER

typedef struct named_video_mode {
	const char *		nvm_name;
#ifdef HAVE_LIBSPINNAKER
	/*
	fc2VideoMode		nvm_value;
	*/
#endif
	int			nvm_width;
	int			nvm_height;
	int			nvm_depth;
} Named_Video_Mode;

DECLARE_NAMED_PARAM(framerate,/*fc2FrameRate*/int,nfr,Frame_Rate)
DECLARE_NAMED_PARAM(grab_mode,/*fc2GrabMode*/int,ngm,Grab_Mode)
DECLARE_NAMED_PARAM(bus_speed,/*fc2BusSpeed*/int,nbs,Bus_Speed)
DECLARE_NAMED_PARAM(bw_allocation,/*fc2BandwidthAllocation*/int,nba,Bandwidth_Allocation)
DECLARE_NAMED_PARAM(interface,/*fc2InterfaceType*/int,nif,Interface)

typedef struct named_feature {
	const char *		nft_name;
#ifdef HAVE_LIBSPINNAKER
	//dc1394feature_t 	nft_feature;
#endif
} Named_Feature;

typedef struct named_trigger_mode {
	const char *		ntm_name;
#ifdef HAVE_LIBSPINNAKER
	//dc1394trigger_mode_t	ntm_mode;
#endif
} Named_Trigger_Mode;

extern const char *eii_prop_names[];


extern const char *eii_prop_names[];

extern int pick_grab_mode(QSP_ARG_DECL Spink_Cam *fcp, const char *pmpt);

// These can be declared unconditionally if they don't refer to any PGR structs...

extern void show_grab_mode(QSP_ARG_DECL  Spink_Cam *fcp);
extern void set_grab_mode(QSP_ARG_DECL  Spink_Cam *fcp, int idx);
extern void set_buffer_obj(QSP_ARG_DECL  Spink_Cam *fcp, Data_Obj *dp);
extern void set_eii_property(QSP_ARG_DECL  Spink_Cam *fcp, int idx, int yesno );
extern void show_fmt7_modes(QSP_ARG_DECL  Spink_Cam *fcp);
extern int set_n_buffers(QSP_ARG_DECL  Spink_Cam *fcp, int n );
extern void set_fmt7_size(QSP_ARG_DECL  Spink_Cam *fcp, int w, int h );

extern void list_spink_cam_properties(QSP_ARG_DECL  Spink_Cam *fcp);
extern void refresh_spink_cam_properties(QSP_ARG_DECL  Spink_Cam *fcp);

#ifdef HAVE_LIBSPINNAKER
extern void refresh_property_info(QSP_ARG_DECL  Spink_Cam *fcp, Spink_Cam_Property_Type *t);
extern void show_property_info(QSP_ARG_DECL  Spink_Cam *fcp, Spink_Cam_Property_Type *t);
extern void refresh_property_value(QSP_ARG_DECL  Spink_Cam *fcp, Spink_Cam_Property_Type *t);
extern void show_property_value(QSP_ARG_DECL  Spink_Cam *fcp, Spink_Cam_Property_Type *t);

extern void set_prop_value(QSP_ARG_DECL  Spink_Cam *fcp, Spink_Cam_Property_Type *t, Spink_Cam_Prop_Val *vp);
extern void set_prop_auto(QSP_ARG_DECL  Spink_Cam *fcp, Spink_Cam_Property_Type *t, BOOL auto_state);
extern unsigned int read_register(QSP_ARG_DECL  Spink_Cam *fcp, unsigned int addr);
extern void write_register(QSP_ARG_DECL  Spink_Cam *fcp, unsigned int addr, unsigned int val);
//extern void report_fc2_error(QSP_ARG_DECL   fc2Error error, const char *whence );

//extern dc1394video_mode_t pick_video_mode(QSP_ARG_DECL  Spink_Cam *fcp, const char *pmpt);
//extern dc1394video_mode_t pick_fmt7_mode(QSP_ARG_DECL  Spink_Cam *fcp, const char *pmpt);
extern int is_fmt7_mode(QSP_ARG_DECL  Spink_Cam *fcp, int idx);
extern int set_fmt7_mode(QSP_ARG_DECL  Spink_Cam *fcp, int idx );
extern int check_buffer_alignment(QSP_ARG_DECL  Spink_Cam *fcp);
//extern void report_feature_info(QSP_ARG_DECL  Spink_Cam *fcp, dc1394feature_t id );
//extern const char *name_for_trigger_mode(dc1394trigger_mode_t mode);
#endif	// HAVE_LIBSPINNAKER

extern int _get_spink_cam_names(QSP_ARG_DECL  Data_Obj *dp );
#define get_spink_cam_names(dp ) _get_spink_cam_names(QSP_ARG  dp )

extern int get_spink_cam_video_mode_strings(QSP_ARG_DECL  Data_Obj *dp, Spink_Cam *fcp);
extern int get_spink_cam_framerate_strings(QSP_ARG_DECL  Data_Obj *dp, Spink_Cam *fcp);
extern void push_spink_cam_context(QSP_ARG_DECL  Spink_Cam *fcp);
extern void pop_spink_cam_context(SINGLE_QSP_ARG_DECL);
extern int init_spink_cam_system(SINGLE_QSP_ARG_DECL);
extern int start_firewire_transmission(QSP_ARG_DECL  Spink_Cam * fcp, int buf_size );
extern Data_Obj * _grab_spink_cam_frame(QSP_ARG_DECL  Spink_Cam * fcp );
#define grab_spink_cam_frame(fcp ) _grab_spink_cam_frame(QSP_ARG  fcp )

extern Data_Obj * grab_newest_firewire_frame(QSP_ARG_DECL  Spink_Cam * fcp );
extern int reset_spink_cam(QSP_ARG_DECL  Spink_Cam * fcp );
extern void list_spink_cam_trig( QSP_ARG_DECL  Spink_Cam * fcp );
extern void release_oldest_frame(QSP_ARG_DECL  Spink_Cam *fcp);
extern void report_spink_cam_bandwidth(QSP_ARG_DECL  Spink_Cam *fcp);
extern void list_spink_cam_framerates(QSP_ARG_DECL  Spink_Cam *fcp);
extern int list_spink_cam_video_modes(QSP_ARG_DECL  Spink_Cam *fcp);
extern void show_spink_cam_framerate(QSP_ARG_DECL  Spink_Cam *fcp);
extern void show_spink_cam_video_mode(QSP_ARG_DECL  Spink_Cam *fcp);
extern int pick_spink_cam_framerate(QSP_ARG_DECL  Spink_Cam *fcp, const char *pmpt);
extern int set_framerate(QSP_ARG_DECL  Spink_Cam *fcp, int mode_index);
extern void print_spink_cam_info(QSP_ARG_DECL  Spink_Cam *fcp);
extern int list_spink_cam_features(QSP_ARG_DECL  Spink_Cam *fcp);
extern int get_feature_choices(Spink_Cam *fcp, const char ***chp);
extern void get_spink_cam_features(Spink_Cam *fcp);

/* cam_ctl.c */
#ifdef HAVE_LIBSPINNAKER
//extern void describe_dc1394_error( QSP_ARG_DECL  dc1394error_t e );
//extern int is_auto_capable( dc1394feature_info_t *feat_p );
//extern int set_spink_cam_framerate(Spink_Cam *fcp, dc1394framerate_t framerate );
//extern int set_spink_cam_trigger_polarity(Spink_Cam *fcp,
//	dc1394trigger_polarity_t polarity);
//extern int set_spink_cam_trigger_mode(Spink_Cam *fcp, dc1394trigger_mode_t mode);
//extern int set_spink_cam_trigger_source(Spink_Cam *fcp, dc1394trigger_source_t source);
//extern int set_iso_speed(Spink_Cam *fcp, dc1394speed_t speed);
#endif /* HAVE_LIBSPINNAKER */
extern int set_spink_cam_bmode(Spink_Cam *, int);
extern int power_on_spink_cam(Spink_Cam *fcp);
extern int power_off_spink_cam(Spink_Cam *fcp);
extern int set_spink_cam_temperature(Spink_Cam *fcp, int temp);
extern int set_spink_cam_white_balance(Spink_Cam *fcp, int wb);
extern int set_spink_cam_white_shading(Spink_Cam *fcp, int val);

extern const char *name_of_indexed_video_mode(int idx);

/* stream_fly.c */
extern void fly_set_async_record(int flag);
extern int fly_get_async_record(void);
extern void stream_record(QSP_ARG_DECL  Image_File *ifp,int32_t n_frames, Spink_Cam *fcp);
extern COMMAND_FUNC( flycap_wait_record );
extern COMMAND_FUNC( flycap_halt_record );
extern Image_File * _get_file_for_recording(QSP_ARG_DECL  const char *name,
		int n_frames,Spink_Cam *fcp);
#define get_file_for_recording(name,n_f,fcp)	_get_file_for_recording(QSP_ARG  name,n_f,fcp)

typedef int spinkError;
typedef int spinkPropertyType;
typedef int spinkMode;
typedef int spinkContext;

//#define DEBUG_MSG(m)	fprintf(stderr,"%s\n",#m);
#define DEBUG_MSG(m)

#ifndef HAVE_LIBSPINNAKER
#endif // ! HAVE_LIBSPINNAKER

#endif // _SPINK_H_