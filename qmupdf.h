#ifndef QMUPDF_H
#define QMUPDF_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "reflowdevice.h"

struct CaracterPDF
{
    int c;
    fz_point Origen;
    fz_quad quad;
    float size;
    fz_font *fuente;
};

struct LineaPDF
{
    int wmode;  /* 0 for horizontal, 1 for vertical */
    fz_point dir;   /* normalized direction of baseline */
    fz_rect bbox;
    QList <CaracterPDF> *Letra;
};

struct BloquePDF
{
    int Tipo;
    fz_rect bbox;
    QList <LineaPDF> *Lineas;
    fz_matrix ImgMatriz;
    fz_image *Imagen;
};

class QMuPDF : public QWidget
{
    Q_OBJECT
public:
    explicit QMuPDF(QWidget *parent = nullptr);
    ~QMuPDF();
    bool AbrirDoc (QString filename); // devuelve true con exito
    QImage *VerPagina (int page);
    void setEscala(float escalax,float escalay);
    float getEscalaX(void);
    float getEscalaY(void);
    int getTotalPaginas (void);
    void CreaPdf (QString filename);
    void VerRecuadros (bool verbox,bool verlin,bool verchar);
    QList <BloquePDF> ExtructuraPagina (int numpagina);
signals:

public slots:

private:
    QWidget *Padre;
    fz_context *ctx;
    fz_document *document;
    fz_stext_page *PaginaTexto;
    fz_stext_page *PaginaTextoTransformada;
    fz_device MiDevice;
    pdf_device *pdfdev;


    pdf_document *pdfdocument;

    float EscalaX;
    float EscalaY;
    float Rotacion;
    bool DibujarBloques;
    bool DibujarLineas;
    bool DibujarLetras;

    QString NombreFichero;
    QString NombreFicheroOut;
    QString Titulo;
    QString Password;
    int TotalPaginas;
    int PaginaActual;
    QImage *ImagenPDF;   // Se guarda la imagen de la pagina PDF para poder pintar encima
    QImage *Render(int pagenumber,float scaleX = 1.0f, float scaleY = 1.0f, float rotation = 0.0f);
    float pix_height; // Alto de la imagen de la pagina
    float pix_width; // Ancho de la imagen de la pagina

    void ExtraerEstructuraTransformada (void);
    void DibujarExtructura (QImage *PaginaDib);
    void DibujaLin (QPainter &paint,fz_stext_block *bloque);
    void DibujaChar (QPainter &paint,fz_stext_line *linea);
    void getTitle();
    void setTitle(QString title,int page);

};

#endif // QMUPDF_H
