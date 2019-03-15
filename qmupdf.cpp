#include "qmupdf.h"
#include "info-device.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QPainter>

// Extract structured text from a page. */. fz_stext_page *fz_new_stext_page_from_page(fz_context *ctx, fz_page *page, const fz_stext_options *options);
// fz_stext_page *fz_new_stext_page_from_page(fz_context *ctx, fz_page *page, const fz_stext_options *options);
// fz_stext_page *fz_new_stext_page_from_page_number(fz_context *ctx, fz_document *doc, int number, const fz_stext_options *options);
// fz_stext_page *fz_new_stext_page_from_display_list(fz_context *ctx, fz_display_list *list, const fz_stext_options *options);

//fz_buffer *fz_new_buffer_from_stext_page(fz_context *ctx, fz_stext_page *text);
//fz_buffer *fz_new_buffer_from_page(fz_context *ctx, fz_page *page, const fz_stext_options *options);
//fz_buffer *fz_new_buffer_from_page_number(fz_context *ctx, fz_document *doc, int number, const fz_stext_options *options);
//fz_buffer *fz_new_buffer_from_display_list(fz_context *ctx, fz_display_list *list, const fz_stext_options *options);


QMuPDF::QMuPDF(QWidget *parent) : QWidget (parent)
{
    Padre=parent;
    ImagenPDF=nullptr;
//    scrollArea = elscrollArea;
//    Lienzo = elLienzo;
    pix_width=pix_height=0;

    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    fz_try(ctx){
        fz_register_document_handlers(ctx);
    }
    fz_catch(ctx)
    {
        QMessageBox msgBox;
        QString texto;
        texto.sprintf("No es posible registrar los handlers del documento: %s\n",fz_caught_message(ctx));
        msgBox.setText(texto);
        msgBox.exec();
        fz_drop_context(ctx);
        exit(0);
    }
    NombreFichero.clear();
    NombreFicheroOut="g:/out.pdf";
    TotalPaginas=0;
    PaginaActual=0;
    DibujarBloques=false;
    DibujarLineas=false;
    DibujarLetras=false;
}

QMuPDF::~QMuPDF()
{
    fz_drop_context(ctx);
}

bool QMuPDF::AbrirDoc(QString filename)
{
    QMessageBox msgBox;
    QString texto;

    if(!NombreFichero.isEmpty())
            {
            fz_drop_document(ctx,document);
            return false;
            }

    // Abre el documento PDF, XPS o CBZ.
    fz_try(ctx){
        document= fz_open_document(ctx, filename.toLatin1().data());
    }
    fz_catch(ctx)
    {
        texto.sprintf("No es posible abrir el documento: %s\n",fz_caught_message(ctx));
        msgBox.setText(texto);
        msgBox.exec();
        return false;
    }

    if(document==nullptr)
    {
        texto=(tr("No es posible abrir el documento."));
        msgBox.setText(texto);
        msgBox.exec();
        fz_drop_context(ctx);
        return false;
    }

    if (fz_needs_password(ctx,document)) {
        bool ok;
        QString password;
        do {
            QInputDialog get_password(Padre);
            password = QInputDialog::getText(Padre, tr("Enter password"),
                                             tr("Password:"), QLineEdit::Normal,
                                             "", &ok);
            if (!ok){return false;}
        } while (!fz_authenticate_password(ctx,document,password.toLatin1().data()));
    }

    TotalPaginas=fz_count_pages(ctx, document);
    PaginaActual=0;
    EscalaX=EscalaY=1.0f;
    pix_height=0;
    pix_width=0;
    Rotacion=0;
/*
    //resetting all variable to default while opening new pdf/doc.
    search_count=0;
    m_index=0;

    //disabling previous button when first page is dispalyed
    previousPageAction->setEnabled(false);

    //disabling next button if pdf has only one page
    if(m_numPages==1)
    {
        nextPageAction->setEnabled(false);
    }
    else
        nextPageAction->setEnabled(true);

    zoomInAction->setEnabled(true);
    zoomOutAction->setEnabled(true);

    showPage(0);
*/
    NombreFichero=filename;
//    getTitle();
//    VerPagina(PaginaActual);
    return true;
}

QImage *QMuPDF::VerPagina(int page)
{
    QString texto;
    QImage *image=nullptr;

    if (PaginaActual<TotalPaginas)
    {
        if (ImagenPDF!=nullptr) delete ImagenPDF;
        image=Render(page,EscalaX,EscalaY,Rotacion);
        ImagenPDF=image;
        PaginaActual=page;
        ExtraerEstructuraTransformada();
        DibujarExtructura(ImagenPDF);

//        Lienzo->setPixmap(QPixmap::fromImage(ImagenPDF));
//        Lienzo->resize(Lienzo->sizeHint());

//        scrollArea->verticalScrollBar()->setValue(0);
//        setTitle(Titulo,(PaginaActual+1));
        return ImagenPDF;
    }
    else return nullptr;
}

QImage *QMuPDF::Render(int pagenumber,float scaleX, float scaleY, float rotation)
{
    QMessageBox msgBox;
    QString texto;
    QImage *image;

    image=new QImage();
    unsigned char *samples = nullptr;
    unsigned char  *copyed_samples = nullptr;
    int width = 0;
    int height = 0;


    fz_page *page = fz_load_page(ctx, document, pagenumber);
    if(!page)
    {
        texto.sprintf("No es posible presentar la página %d");
        msgBox.setText(texto);
        msgBox.exec();
        fz_drop_page(ctx,page);
        return (image);
    }

    // build transform matrix
    fz_matrix transform = fz_identity;

    transform=fz_rotate(rotation);
    transform=fz_pre_scale(transform, scaleX, scaleY);

    // get transformed page size
    fz_rect bounds;
    fz_irect bbox;
    bounds=fz_bound_page(ctx, page);
    bounds=fz_transform_rect(bounds, transform);
    bbox=fz_round_rect(bounds);

    fz_pixmap *pix =fz_new_pixmap_from_page_number(ctx,document,pagenumber,transform,fz_device_bgr(ctx),1);

    samples = fz_pixmap_samples(ctx, pix);



    width = fz_pixmap_width(ctx, pix);
    height = fz_pixmap_height(ctx, pix);

    /*
    * set pix_height and pix_width to 0 when pdf is opened for the first time or new pdf is open for the first time
    * used while calculating m_scale for rotate_left and rotate_right
    */
    if(pix_height==0 && pix_width==0)
        {
        pix_height=height;
        pix_width=width;
        }


    copyed_samples=samples;

#if QT_VERSION < 0x050200
    // most computers use little endian, so Format_ARGB32 means bgra order
    // note: this is not correct for computers with big endian architecture
    *image = QImage(copyed_samples,
                   width, height, QImage::Format_RGBA8888, nullptr, copyed_samples);
#else
    // with Qt 5.2, Format_RGBA8888 is correct for any architecture
    *image = QImage(copyed_samples,
                   width, height, QImage::Format_ARGB32, nullptr, copyed_samples);
#endif

    fz_drop_page(ctx,page);
    return image;
}

void QMuPDF::getTitle()
{
    int last_index=NombreFichero.lastIndexOf("/");
    Titulo=NombreFichero.mid(last_index+1,NombreFichero.size());
}

void QMuPDF::setTitle(QString title,int page)
{
    QString end_title=QString::number(page)+"/"+QString::number(TotalPaginas);
    Padre->setWindowTitle(title+" - "+end_title);
}

void QMuPDF::setEscala(float escalax,float escalay)
{
    EscalaX=escalax;
    EscalaY=escalay;
}

float QMuPDF::getEscalaX(void)
{
    return EscalaX;
}

float QMuPDF::getEscalaY(void)
{
    return EscalaY;
}

int QMuPDF::getTotalPaginas (void)
{
    return TotalPaginas;
}

void QMuPDF::ExtraerEstructuraTransformada()
{
    fz_page *Pagina;
    fz_stext_options OpcionesTexto;
    fz_device *DeviceTexto=nullptr;
    fz_matrix transform = fz_identity;
//    fz_cookie *cookie;
    fz_rect bounds;
    fz_irect bbox;
    fz_rect mediabox;

    Pagina = fz_load_page(ctx, document, PaginaActual);
    if(Pagina)
    {
        transform=fz_rotate(Rotacion);
        transform=fz_pre_scale(transform, EscalaX, EscalaY);
        // get transformed page size
        mediabox = fz_bound_page(ctx, Pagina);
        bounds=fz_bound_page(ctx, Pagina);
        bounds=fz_transform_rect(bounds, transform);
        bbox=fz_round_rect(bounds);
        OpcionesTexto.flags=FZ_STEXT_PRESERVE_LIGATURES | FZ_STEXT_PRESERVE_IMAGES | FZ_STEXT_PRESERVE_WHITESPACE;
        fz_try(ctx)
        {
//            PaginaTextoTransformada=fz_new_stext_page_from_page(ctx,Pagina,&OpcionesTexto);
            PaginaTextoTransformada=fz_new_stext_page(ctx,mediabox);
            DeviceTexto=fz_new_stext_device(ctx,PaginaTextoTransformada,&OpcionesTexto);
            fz_run_page(ctx,Pagina,DeviceTexto,transform,nullptr);
            fz_close_device(ctx, DeviceTexto);
            fz_drop_device(ctx, DeviceTexto);
            DeviceTexto = nullptr;
//            PaginaTexto=fz_new_stext_page_from_page(ctx,Pagina,&OpcionesTexto);
        }
        fz_always(ctx)
        {
            fz_drop_device(ctx, DeviceTexto);
        }
        fz_catch(ctx)
        {
            fz_drop_page(ctx, Pagina);
            fz_rethrow(ctx);
        }
    }
    else {
        fz_drop_page(ctx,Pagina);
    }
}

void QMuPDF::DibujarExtructura (QImage *PaginaDib)
{
    QPainter paint;
    fz_rect rectangulo;
    QRect mirect;
    fz_stext_block *Bloque;
    fz_stext_block * BloqueUltimo;
    QColor color(255,0,0);
    QPen MiPen;
    bool seguir=true;

    MiPen.setColor(color);
    MiPen.setStyle(Qt::SolidLine);
    MiPen.setWidth(2);
    Bloque=PaginaTextoTransformada->first_block;
    BloqueUltimo=PaginaTextoTransformada->last_block;
    paint.begin(PaginaDib);
    do {
        rectangulo=Bloque->bbox;
        mirect.setCoords(int(rectangulo.x0),int(rectangulo.y0),int(rectangulo.x1),int(rectangulo.y1));
        paint.setPen(MiPen);
        if (DibujarBloques)  paint.drawRect(mirect);
        DibujaLin(paint,Bloque);
        if (Bloque==BloqueUltimo) seguir=false;
        Bloque=Bloque->next;
    } while (seguir);
    paint.end();
}

void QMuPDF::VerRecuadros (bool verbox,bool verlin,bool verchar)
{
    DibujarBloques=verbox;
    DibujarLineas=verlin;
    DibujarLetras=verchar;
    VerPagina(PaginaActual);
}


void QMuPDF::DibujaLin (QPainter &paint,fz_stext_block *bloque)
{
    fz_rect rectangulo;
    QRect mirect;
    fz_stext_line *Linea;
    fz_stext_line * LineaUltimo;
    QColor color(0,255,0);
    QPen MiPen;
    bool seguir=true;

    MiPen.setColor(color);
    MiPen.setStyle(Qt::SolidLine);
    MiPen.setWidth(2);
    if (bloque->type==FZ_STEXT_BLOCK_TEXT)
    {
        Linea=bloque->u.t.first_line;
        LineaUltimo=bloque->u.t.last_line;
        do {
            rectangulo=Linea->bbox;
            mirect.setCoords(int(rectangulo.x0),int(rectangulo.y0),int(rectangulo.x1),int(rectangulo.y1));
            paint.setPen(MiPen);
            if (DibujarLineas) paint.drawRect(mirect);
            DibujaChar(paint,Linea);
            if (Linea==LineaUltimo) seguir=false;
            Linea=Linea->next;
        } while (seguir);
    }
}

void QMuPDF::DibujaChar(QPainter &paint, fz_stext_line *linea)
{
    fz_rect rectangulo;
    QRect mirect;
    fz_stext_char *Letra;
    fz_stext_char * LetraUltimo;
    QColor color(0,0,255);
    QPen MiPen;
    bool seguir=true;

    MiPen.setColor(color);
    MiPen.setStyle(Qt::SolidLine);
    MiPen.setWidth(2);
    Letra=linea->first_char;
    LetraUltimo=linea->last_char;
        do {
            rectangulo=fz_rect_from_quad(Letra->quad);
            mirect.setCoords(int(rectangulo.x0),int(rectangulo.y0),int(rectangulo.x1),int(rectangulo.y1));
            paint.setPen(MiPen);
            if (DibujarLetras) paint.drawRect(mirect);
            if (Letra==LetraUltimo) seguir=false;
            Letra=Letra->next;
        } while (seguir);
}

void QMuPDF::CreaPdf(QString filename)
{
    fz_document_writer *writer;
    fz_rect mediabox;
    fz_page *pagina;
    fz_device *dev = nullptr;

    NombreFicheroOut=filename;
    mediabox.x0=0;
    mediabox.x1=200;
    mediabox.y0=0;
    mediabox.y1=300;
    writer=fz_new_document_writer(ctx,NombreFicheroOut.toLatin1().data(),"pdf","clean sanitize");
    for (int kk=50;kk<60;kk++)
    {
        pagina = fz_load_page(ctx, document, kk);
        fz_try(ctx)
        {
            mediabox = fz_bound_page(ctx, pagina);
            dev=fz_begin_page(ctx,writer,mediabox);
            pdfdev=reinterpret_cast<pdf_device *>(dev);
            CambiarLlamadas(pdfdev);
            fz_run_page(ctx, pagina, dev, fz_identity, nullptr);
            pdfdev->super.fill_text=UnPdfDevice.fill_text;
            RestaurarLlamadas(pdfdev);
            fz_end_page(ctx,writer);
        }
        fz_always(ctx)
        {
            fz_drop_device(ctx,dev);
            fz_drop_page(ctx, pagina);
        }
        fz_catch(ctx)
            fz_rethrow(ctx);
    }
    fz_close_document_writer(ctx,writer);
    fz_drop_document_writer(ctx,writer);
}

QList <BloquePDF> QMuPDF::ExtructuraPagina (int numpagina)
{
    fz_page *Pagina;
    fz_info_page *paginastext=nullptr;
    fz_info_options OpcionesTexto;
    fz_device *DeviceTexto=nullptr;
    fz_matrix transform = fz_identity;
//    fz_cookie *cookie;
    fz_rect bounds;
    fz_irect bbox;
    fz_rect mediabox;

    Pagina = fz_load_page(ctx, document, numpagina);
    if(Pagina)
    {
        transform=fz_rotate(Rotacion);
        transform=fz_pre_scale(transform, EscalaX, EscalaY);
        // get transformed page size
        mediabox = fz_bound_page(ctx, Pagina);
        bounds=fz_bound_page(ctx, Pagina);
        bounds=fz_transform_rect(bounds, transform);
        bbox=fz_round_rect(bounds);

        OpcionesTexto.flags=FZ_INFO_PRESERVE_LIGATURES | FZ_INFO_PRESERVE_IMAGES | FZ_INFO_PRESERVE_WHITESPACE;
        fz_try(ctx)
        {
            paginastext=fz_new_info_page(ctx,mediabox);
            DeviceTexto=fz_new_info_device(ctx,paginastext,&OpcionesTexto);
            fz_run_page(ctx,Pagina,DeviceTexto,transform,nullptr);
            fz_close_device(ctx, DeviceTexto);
            fz_drop_device(ctx, DeviceTexto);
            DeviceTexto = nullptr;
        }
        fz_always(ctx)
        {
            fz_drop_device(ctx, DeviceTexto);
        }
        fz_catch(ctx)
        {
            fz_drop_page(ctx, Pagina);
            fz_rethrow(ctx);
        }
    }
    else {
        fz_drop_page(ctx,Pagina);
    }
    fz_info_block *mibloque;
    fz_info_block *mibloqueultimo;
    fz_info_line *milinea;
    fz_info_line *milineaultima;
    fz_info_char *michar;
    fz_info_char *micharultimo;
    QList <BloquePDF> *MiLista;
    QList <LineaPDF> *MiLinea;
    QList <CaracterPDF> *MiLetra;
    BloquePDF bloquepdf;
    LineaPDF lineapdf;
    CaracterPDF charpdf;
    bool seguirbloque=true;
    bool seguirlinea=true;
    bool seguirchar=true;

    MiLista=new QList <BloquePDF>;
    mibloque=paginastext->first_block;
    mibloqueultimo=paginastext->last_block;
    do {
        bloquepdf.Tipo=mibloque->type;
        bloquepdf.bbox=mibloque->bbox;
        if (bloquepdf.Tipo==0)  // Es un bloque texto y tiene lineas
        {
            seguirlinea=true;
            MiLinea =new QList <LineaPDF>;
            milinea=mibloque->u.t.first_line;
            milineaultima=mibloque->u.t.last_line;
            do {
                lineapdf.dir=milinea->dir;
                lineapdf.bbox=milinea->bbox;
                lineapdf.wmode=milinea->wmode;

                seguirchar=true;
                MiLetra=new QList <CaracterPDF>;
                michar=milinea->first_char;
                micharultimo=milinea->last_char;
                do {
                    charpdf.c=michar->c;
                    charpdf.quad=michar->quad;
                    charpdf.size=michar->size;
                    charpdf.Origen=michar->origin;
                    charpdf.fuente=michar->font;
                    MiLetra->append(charpdf);
                    if (michar==micharultimo) seguirchar=false;
                    michar=michar->next;
                } while (seguirchar);


                lineapdf.Letra=MiLetra;
                MiLinea->append(lineapdf);
                if (milinea==milineaultima) seguirlinea=false;
                milinea=milinea->next;
            } while (seguirlinea);
        bloquepdf.Lineas=MiLinea;
        }
        else {      // Es una imagen no hay que dibujar sus lineas
            bloquepdf.ImgMatriz=mibloque->u.i.transform;
            bloquepdf.Imagen=mibloque->u.i.image;
        }
        MiLista->append(bloquepdf);
        if (mibloque==mibloqueultimo) seguirbloque=false;
        mibloque=mibloque->next;
    } while (seguirbloque);

    fz_drop_info_page(ctx,paginastext);

    return *MiLista;
}
/*
QList <BloquePDF> QMuPDF::ExtructuraPagina (int numpagina)
{
    fz_page *Pagina;
    fz_stext_page *paginastext=nullptr;
    fz_stext_options OpcionesTexto;
    fz_device *DeviceTexto=nullptr;
    fz_matrix transform = fz_identity;
//    fz_cookie *cookie;
    fz_rect bounds;
    fz_irect bbox;
    fz_rect mediabox;

    Pagina = fz_load_page(ctx, document, numpagina);
    if(Pagina)
    {
        transform=fz_rotate(Rotacion);
        transform=fz_pre_scale(transform, EscalaX, EscalaY);
        // get transformed page size
        mediabox = fz_bound_page(ctx, Pagina);
        bounds=fz_bound_page(ctx, Pagina);
        bounds=fz_transform_rect(bounds, transform);
        bbox=fz_round_rect(bounds);

        OpcionesTexto.flags=FZ_STEXT_PRESERVE_LIGATURES | FZ_STEXT_PRESERVE_IMAGES | FZ_STEXT_PRESERVE_WHITESPACE;
        fz_try(ctx)
        {
            paginastext=fz_new_stext_page(ctx,mediabox);
            DeviceTexto=fz_new_stext_device(ctx,paginastext,&OpcionesTexto);
            fz_run_page(ctx,Pagina,DeviceTexto,transform,nullptr);
            fz_close_device(ctx, DeviceTexto);
            fz_drop_device(ctx, DeviceTexto);
            DeviceTexto = nullptr;
//            PaginaTexto=fz_new_stext_page_from_page(ctx,Pagina,&OpcionesTexto);
        }
        fz_always(ctx)
        {
            fz_drop_device(ctx, DeviceTexto);
        }
        fz_catch(ctx)
        {
            fz_drop_page(ctx, Pagina);
            fz_rethrow(ctx);
        }
    }
    else {
        fz_drop_page(ctx,Pagina);
    }
    fz_stext_block *mibloque;
    fz_stext_block *mibloqueultimo;
    fz_stext_line *milinea;
    fz_stext_line *milineaultima;
    fz_stext_char *michar;
    fz_stext_char *micharultimo;
    QList <BloquePDF> *MiLista;
    QList <LineaPDF> *MiLinea;
    QList <CaracterPDF> *MiLetra;
    BloquePDF bloquepdf;
    LineaPDF lineapdf;
    CaracterPDF charpdf;
    bool seguirbloque=true;
    bool seguirlinea=true;
    bool seguirchar=true;

    MiLista=new QList <BloquePDF>;
    mibloque=paginastext->first_block;
    mibloqueultimo=paginastext->last_block;
    do {
        bloquepdf.Tipo=mibloque->type;
        bloquepdf.bbox=mibloque->bbox;
        if (bloquepdf.Tipo==0)  // Es un bloque texto y tiene lineas
        {
            seguirlinea=true;
            MiLinea =new QList <LineaPDF>;
            milinea=mibloque->u.t.first_line;
            milineaultima=mibloque->u.t.last_line;
            do {
                lineapdf.dir=milinea->dir;
                lineapdf.bbox=milinea->bbox;
                lineapdf.wmode=milinea->wmode;

                seguirchar=true;
                MiLetra=new QList <CaracterPDF>;
                michar=milinea->first_char;
                micharultimo=milinea->last_char;
                do {
                    charpdf.c=michar->c;
                    charpdf.quad=michar->quad;
                    charpdf.size=michar->size;
                    charpdf.Origen=michar->origin;
                    charpdf.fuente=michar->font;
                    MiLetra->append(charpdf);
                    if (michar==micharultimo) seguirchar=false;
                    michar=michar->next;
                } while (seguirchar);


                lineapdf.Letra=MiLetra;
                MiLinea->append(lineapdf);
                if (milinea==milineaultima) seguirlinea=false;
                milinea=milinea->next;
            } while (seguirlinea);
        bloquepdf.Lineas=MiLinea;
        }
        else {      // Es una imagen no hay que dibujar sus lineas
            bloquepdf.ImgMatriz=mibloque->u.i.transform;
            bloquepdf.Imagen=mibloque->u.i.image;
        }
        MiLista->append(bloquepdf);
        if (mibloque==mibloqueultimo) seguirbloque=false;
        mibloque=mibloque->next;
    } while (seguirbloque);

    fz_drop_stext_page(ctx,paginastext);

    return *MiLista;
}

*/
