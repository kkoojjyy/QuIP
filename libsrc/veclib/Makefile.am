all:	libvec.a
	cp libvec.a ../../libs

noinst_LIBRARIES = libvec.a

libvec_a_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -D_GNU_SOURCE -Wall -Wmissing-prototypes
# -D__STRICT_ANSI__  this breaks u_long in sys/types.h...

#				sampmenu.c	\
#

#				bm_funcs.c

#				convert.c
#				bitmap.c

libvec_a_SOURCES =				\
				sampmenu.c	\
				vec_args.c	\
				vec_call.c	\
				obj_args.c	\
				warmenu.c	\
				vectbl.c	\
				vl_menu.c	\
				vec_chn.c	\
				chn_menu.c	\
				cksiz.c		\
				cpu_flags.c	\
				lin_util.c	\
				vec_func.c	\
				dispatch.c	\
				platform.c	\
				platform_menu.c	\
				vec_chain.c


# included files (not compiled themselves
#EXTRA_libvec_a_SOURCES =			\
#				conv_vec.c
#				bit_vec.c
#				linear.c

#				dp_common.h
#				sp_common.h
#				new_ops.h
#				method_prot.h
#				nv_undefs.h
#				convert.h
#				bitmap.h
#				calling_args.h

noinst_HEADERS =				\
				my_cpuid.h	\
				nvf.h		\
				nvproto.h	\
				os_check.h	\
				vec_chain.h	\
				veclib_prot.h


fast_sp.s:	fast_sp.c
	$(CC) -I../../include -I../.. $(CPPFLAGS) $(CFLAGS) -S fast_sp.c

