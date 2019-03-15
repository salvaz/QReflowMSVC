#include "reflowdevice.h"

extern "C"
{
/*
static void Mipdf_dev_fill_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm,
        fz_colorspace *colorspace, const float *color, float alpha, const fz_color_params *color_params)
{
    UnPdfDevice.fill_text(ctx,dev,text,ctm,colorspace,color,alpha,color_params);
}
*/

fz_device UnPdfDevice;

void Mipdf_dev_fill_text(fz_context *ctx, fz_device *dev, const fz_text *text, fz_matrix ctm,
        fz_colorspace *colorspace, const float *color, float alpha, const fz_color_params *color_params)
{
    fz_text_span *span;
    fz_text_item *it;

    for (span = text->head; span; span = span->next)
    {
        for (int i = 0; i < span->len; ++i)
        {
            it = &span->items[i];
            if (it->gid < 0)
                continue;
            it->x=it->x+ctm.e;
            it->y=it->y+40;
        }
    }
    UnPdfDevice.fill_text(ctx,dev,text,ctm,colorspace,color,alpha,color_params);
}

void Mipdf_dev_stroke_text(fz_context *ctx, fz_device *dev, const fz_text *text, const fz_stroke_state *stroke, fz_matrix ctm,
                           fz_colorspace *colorspace, const float *color, float alpha, const fz_color_params *color_params)
{
    UnPdfDevice.stroke_text(ctx,dev,text,stroke,ctm,colorspace,color,alpha,color_params);
}

void CambiarLlamadas (pdf_device *pdfdev)
{
    UnPdfDevice.fill_text=pdfdev->super.fill_text;
    pdfdev->super.fill_text=Mipdf_dev_fill_text;
    UnPdfDevice.stroke_text=pdfdev->super.stroke_text;
    pdfdev->super.stroke_text=Mipdf_dev_stroke_text;
}

void RestaurarLlamadas (pdf_device *pdfdev)
{
    pdfdev->super.fill_text=UnPdfDevice.fill_text;
    pdfdev->super.stroke_text=UnPdfDevice.stroke_text;
}

}

