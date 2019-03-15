#ifndef REFLOWDEVICE_H
#define REFLOWDEVICE_H

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pdf_device_s pdf_device;

typedef struct gstate_s gstate;

struct gstate_s
{
    /* The first few entries aren't really graphics state things, but
     * they are recorded here as they are fundamentally intertwined with
     * the push/pulling of the gstates. */
    fz_buffer *buf;
    void (*on_pop)(fz_context*,pdf_device*,void *);
    void *on_pop_arg;

    /* The graphics state proper */
    fz_matrix ctm;
    fz_colorspace *colorspace[2];
    float color[2][4];
    float alpha[2];
    fz_stroke_state *stroke_state;
    int font;
    int text_rendering_mode;
    int knockout;
};

typedef struct alpha_entry_s alpha_entry;

struct alpha_entry_s
{
    float alpha;
    int stroke;
};

typedef struct group_entry_s group_entry;

struct group_entry_s
{
    int alpha;
    int isolated;
    int knockout;
    fz_colorspace *colorspace;
    pdf_obj *ref;
};

struct pdf_device_s
{
    fz_device super;

    pdf_document *doc;
    pdf_obj *resources;

    int in_text;

    int num_forms;
    int num_smasks;

    int num_gstates;
    int max_gstates;
    gstate *gstates;

    int num_imgs;
    int max_imgs;
    int *image_indices;

    int num_cid_fonts;
    int max_cid_fonts;
    fz_font **cid_fonts;

    int num_alphas;
    int max_alphas;
    alpha_entry *alphas;

    int num_groups;
    int max_groups;
    group_entry *groups;
};

extern fz_device UnPdfDevice;

//static void Mipdf_dev_fill_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm,
//        fz_colorspace *colorspace, const float *color, float alpha, const fz_color_params *color_params);
//void Mipdf_dev_fill_path(fz_context *, fz_device *, const fz_path *, int even_odd, fz_matrix, fz_colorspace *, const float *color, float alpha, const fz_color_params *);
//void Mipdf_dev_stroke_path(fz_context *, fz_device *, const fz_path *, const fz_stroke_state *, fz_matrix, fz_colorspace *, const float *color, float alpha, const fz_color_params *);
//void Mipdf_dev_clip_path(fz_context *, fz_device *, const fz_path *, int even_odd, fz_matrix, fz_rect scissor);
//void Mipdf_dev_clip_stroke_path(fz_context *, fz_device *, const fz_path *, const fz_stroke_state *, fz_matrix, fz_rect scissor);

void Mipdf_dev_fill_text(fz_context *, fz_device *, const fz_text *, fz_matrix, fz_colorspace *, const float *color, float alpha, const fz_color_params *);
void Mipdf_dev_stroke_text(fz_context *ctx, fz_device *dev, const fz_text *text, const fz_stroke_state *stroke, fz_matrix ctm,fz_colorspace *colorspace, const float *color, float alpha, const fz_color_params *color_params);
//void Mipdf_dev_clip_text(fz_context *, fz_device *, const fz_text *, fz_matrix, fz_rect scissor);
//void Mipdf_dev_clip_stroke_text(fz_context *, fz_device *, const fz_text *, const fz_stroke_state *, fz_matrix, fz_rect scissor);
//void Mipdf_dev_ignore_text(fz_context *, fz_device *, const fz_text *, fz_matrix );

//void Mipdf_dev_fill_shade(fz_context *, fz_device *, fz_shade *shd, fz_matrix ctm, float alpha, const fz_color_params *color_params);
//void Mipdf_dev_fill_image(fz_context *, fz_device *, fz_image *img, fz_matrix ctm, float alpha, const fz_color_params *color_params);
//void Mipdf_dev_fill_image_mask(fz_context *, fz_device *, fz_image *img, fz_matrix ctm, fz_colorspace *, const float *color, float alpha, const fz_color_params *color_params);
//void Mipdf_dev_clip_image_mask(fz_context *, fz_device *, fz_image *img, fz_matrix ctm, fz_rect scissor);

//void Mipdf_dev_pop_clip(fz_context *, fz_device *);

//void Mipdf_dev_begin_mask(fz_context *, fz_device *, fz_rect area, int luminosity, fz_colorspace *, const float *bc, const fz_color_params *);
//void Mipdf_dev_end_mask(fz_context *, fz_device *);
//void Mipdf_dev_begin_group(fz_context *, fz_device *, fz_rect area, fz_colorspace *cs, int isolated, int knockout, int blendmode, float alpha);
//void Mipdf_dev_end_group(fz_context *, fz_device *);

//int Mipdf_dev_begin_tile(fz_context *, fz_device *, fz_rect area, fz_rect view, float xstep, float ystep, fz_matrix ctm, int id);
//void Mipdf_dev_end_tile(fz_context *, fz_device *);

//void Mipdf_dev_render_flags(fz_context *, fz_device *, int set, int clear);
//void Mipdf_dev_set_default_colorspaces(fz_context *, fz_device *, fz_default_colorspaces *);

//void Mipdf_dev_begin_layer(fz_context *, fz_device *, const char *layer_name);
//void Mipdf_dev_end_layer(fz_context *, fz_device *);


void CambiarLlamadas (pdf_device *pdfdev);
void RestaurarLlamadas (pdf_device *pdfdev);

#ifdef __cplusplus
}
#endif


#endif // REFLOWDEVICE_H
