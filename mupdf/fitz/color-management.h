#ifndef MUPDF_FITZ_COLOR_MANAGEMENT_H
#define MUPDF_FITZ_COLOR_MANAGEMENT_H

#include "mupdf/fitz/colorspace.h"
#include "mupdf/fitz/context.h"
#include "mupdf/fitz/pixmap.h"

/*
	MuPDF can either run with or without color management. By default
	MuPDF runs without color management. To enable color management,
	a color management engine must be given to the context.

	The context will then create one 'instance' of this engine per
	cloned context. Every instance is tied to the particular context
	in which it is created.

	Profiles and links can be shared between instances.
*/

/*
	Create a new instance of the color
	management engine, tied to the given context.
*/
typedef fz_cmm_instance *(fz_cmm_new_instance_fn)(fz_context *ctx);

/*
	Drop a given instance of the color
	management engine. No further calls will be made to this
	instance.
*/
typedef void (fz_cmm_drop_instance_fn)(fz_cmm_instance *instance);

/*
	Transform a pixmap according
	to a link.
*/
typedef void (fz_cmm_transform_pixmap_fn)(fz_cmm_instance *ctx, fz_icclink *link, fz_pixmap *dst, fz_pixmap *src);

/*
	Transform some color values according
	to a link.
*/
typedef void (fz_cmm_transform_color_fn)(fz_cmm_instance *ctx, fz_icclink *link, unsigned short *dst, const unsigned short *src);

/*
	Create a new link between icc profiles.
*/
typedef void (fz_cmm_init_link_fn)(fz_cmm_instance *ctx, fz_icclink *link, const fz_iccprofile *dst, int dst_extras, const fz_iccprofile *src, int src_extras, const fz_iccprofile *prf, const fz_color_params *rend, int cmm_flags, int num_bytes, int copy_spots);

typedef void (fz_cmm_fin_link_fn)(fz_cmm_instance *ctx, fz_icclink *link);

/*
	Create the cmm specific data for the given
	profile. The cmm handle is stored to profile->cmm_handle.
*/
typedef void (fz_cmm_init_profile_fn)(fz_cmm_instance *ctx, fz_iccprofile *profile);

/*
	Drop the cmm specific data for the given
	profile.
*/
typedef void (fz_cmm_fin_profile_fn)(fz_cmm_instance *ctx, fz_iccprofile *profile);

/*
	Encapsulate details for a given color management engine into a single
	structure.
*/
struct fz_cmm_engine_s {
	fz_cmm_new_instance_fn *new_instance;
	fz_cmm_drop_instance_fn *drop_instance;
	fz_cmm_transform_pixmap_fn *transform_pixmap;
	fz_cmm_transform_color_fn *transform_color;
	fz_cmm_init_link_fn *init_link;
	fz_cmm_fin_link_fn *fin_link;
	fz_cmm_init_profile_fn *init_profile;
	fz_cmm_fin_profile_fn *fin_profile;
};

const fz_cmm_engine *fz_get_cmm_engine(fz_context *ctx);

void fz_set_cmm_engine(fz_context *ctx, const fz_cmm_engine *engine);

/*
	Currently we only provide a single color management
	engine, based on a (modified) LCMS2.

	An unmodified LCMS2 should work too, but only when restricted
	to a single thread.
*/
extern fz_cmm_engine fz_cmm_engine_lcms;

#endif
