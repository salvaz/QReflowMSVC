#ifndef VENTANAPRINCIPAL_H
#define VENTANAPRINCIPAL_H

#include <QMainWindow>
#include <QLabel>
#include <QBoxLayout>
#include <QCheckBox>
#include "qmupdf.h"
#include "qlienzo.h"

class VentanaPrincipal : public QMainWindow
{
    Q_OBJECT

public:
    VentanaPrincipal(QWidget *parent = nullptr);
    ~VentanaPrincipal();
private:
    QMuPDF *MiPDF;
// Acciones del menĂº y barra de herramientas
    QAction *openAction;
    QAction *infoAction;
    QAction *saveAction;
    QAction *saveasAction;
    QAction *zoominAction;
    QAction *zoomoutAction;
    QAction *siguientepagAction;
    QAction *anteriorpagAction;
    QAction *exitAction;
// Objetos de la pantalla principal
    QMenu *fileMenu;
    QMenu *vistaMenu;
    QToolBar *BarraHerramientas;
    QToolBar *OpcVista;
    QStatusBar *BarraEstado;
    QTabWidget *MiTab;
    QWidget *TabVisor;
    QBoxLayout *TabLayVisor;

    QLienzo *Lienzo;     // Label donde se representa la pagina PDF visualizada
    QScrollArea *scrollArea; // Barras de Scroll para el lienzo
    QImage *ImagePDF;
    QBoxLayout *boxlay;
    QWidget *WindowsCentral;
    QCheckBox *CheckBlock;
    QCheckBox *CheckLinea;
    QCheckBox *CheckLetra;

    QString NombreFichero;

    void CrearAcciones (void);
    void CrearMenu (void);
    void CrearBarraHerramientas (void);
    void CrearGui (void);

    float Escala;
    int PaginaActual;
    int PaginasTotales;
private slots:
    void AbrirDocumento (void);
//    void ImportarFichero (void);
    void GuardarDocumento (void);
    void GuardarComoDocumento (void);
    void ZoomInImagen (void);
    void ZoomOutImagen (void);
    void PaginaAnterior (void);
    void PaginaSiguiente (void);
    void CerrarPrograma (void);
    void VerPagina (int page);
    void CambiaVerRecuadros (int estado);
    void RatonPresionado (QMouseEvent *event);
};

#endif // VENTANAPRINCIPAL_H
