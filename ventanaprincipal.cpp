#include "ventanaprincipal.h"
#include "qmupdf.h"
#include <QFileDialog>
#include <QApplication>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QMouseEvent>
#include <QDebug>

// Qt::Key_PageUp

VentanaPrincipal::VentanaPrincipal(QWidget *parent)
    : QMainWindow(parent)
{
//    this->setWindowTitle("Reorganizar documentos");
//    this->setWindowIcon(QIcon(":/Iconos/Reflow"));
    CrearAcciones();
    CrearBarraHerramientas();
    CrearMenu();
    CrearGui();
    MiPDF=new QMuPDF(this);

    zoominAction->setEnabled(false);
    zoomoutAction->setEnabled(false);
    saveAction->setEnabled(false);
    saveasAction->setEnabled(false);
    anteriorpagAction->setEnabled(false);
    siguientepagAction->setEnabled(false);

    PaginaActual=0;
    PaginasTotales=0;
    Escala=1.0f;
    NombreFichero.clear();
    connect(Lienzo,&QLienzo::LienzoRatonPulsado,this,&VentanaPrincipal::RatonPresionado);
}

VentanaPrincipal::~VentanaPrincipal()
{
    if (MiPDF) delete (MiPDF);
}

void VentanaPrincipal::CrearAcciones(void)
{
    openAction=new QAction(tr("&Abrir"), this);
    openAction->setIcon(QIcon(":/Iconos/Abrir"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Abrir un nuevo documento"));
    connect(openAction, &QAction::triggered, this, &VentanaPrincipal::AbrirDocumento);

    saveAction = new QAction(tr("&Guardar"), this);
    saveAction->setIcon(QIcon(":/Iconos/Save"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Guardar el documento"));
    connect(saveAction, &QAction::triggered, this, &VentanaPrincipal::GuardarDocumento);

    saveasAction = new QAction(tr("Guardar como"), this);
    saveasAction->setIcon(QIcon(":/Iconos/Saveas"));
    saveasAction->setShortcut(QKeySequence::SaveAs);
    saveasAction->setStatusTip(tr("Guardar un nuevo documento"));
    connect(saveasAction, &QAction::triggered, this, &VentanaPrincipal::GuardarComoDocumento);

    zoominAction = new QAction(tr("Aumentar el zoom"), this);
    zoominAction->setIcon(QIcon(":/Iconos/ZoomIn"));
    zoominAction->setShortcut(QKeySequence::ZoomIn);
    zoominAction->setStatusTip(tr("Aumentar el zoom un 10%"));
    connect(zoominAction, &QAction::triggered, this, &VentanaPrincipal::ZoomInImagen);

    zoomoutAction = new QAction(tr("Disminuir el zoom"), this);
    zoomoutAction->setIcon(QIcon(":/Iconos/ZoomOut"));
    zoomoutAction->setShortcut(QKeySequence::ZoomOut);
    zoomoutAction->setStatusTip(tr("Disminuir el zoom un 10%"));
    connect(zoomoutAction, &QAction::triggered, this, &VentanaPrincipal::ZoomOutImagen);

    anteriorpagAction = new QAction(tr("&Pagina anterior"), this);
    anteriorpagAction->setIcon(QIcon(":/Iconos/AnteriorPag"));
    anteriorpagAction->setShortcut(QKeySequence::PreviousChild);
    anteriorpagAction->setStatusTip(tr("Ver la página anterior"));
    connect(anteriorpagAction, &QAction::triggered, this, &VentanaPrincipal::PaginaAnterior);

    siguientepagAction = new QAction(tr("&Pagina siguiente"), this);
    siguientepagAction->setIcon(QIcon(":/Iconos/SiguientePag"));
    siguientepagAction->setShortcut(QKeySequence::NextChild);
    siguientepagAction->setStatusTip(tr("Ver la página siguiente"));
    connect(siguientepagAction, &QAction::triggered, this, &VentanaPrincipal::PaginaSiguiente);

    exitAction = new QAction(tr("&Salir"), this);
    exitAction->setIcon(QIcon(":/Iconos/Salir"));
    exitAction->setShortcut(QKeySequence::Close);
    exitAction->setStatusTip(tr("Cerrar el programa"));
    connect(exitAction, &QAction::triggered, this, &VentanaPrincipal::CerrarPrograma);
}

void VentanaPrincipal::CrearMenu(void)
{

//    menuBar()->addMenu(tr("                "));
    fileMenu = menuBar()->addMenu(tr("&Archivo"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveasAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    vistaMenu = menuBar()->addMenu(tr("&Vista"));
    vistaMenu->addAction(zoominAction);
    vistaMenu->addAction(zoomoutAction);
}

void VentanaPrincipal::CrearBarraHerramientas(void)
    {
    BarraHerramientas = addToolBar("Barra Principal");
    BarraHerramientas->addAction(openAction);
    BarraHerramientas->addAction(saveAction);
    BarraHerramientas->addSeparator();
    BarraHerramientas->addAction(zoominAction);
    BarraHerramientas->addAction(zoomoutAction);
    BarraHerramientas->addSeparator();
    BarraHerramientas->addAction(anteriorpagAction);
    BarraHerramientas->addAction(siguientepagAction);
    BarraHerramientas->addSeparator();
    BarraHerramientas->addAction(exitAction);
    addToolBar(Qt::LeftToolBarArea,BarraHerramientas);
//    BarraHerramientas->addSeparator();

    CheckBlock=new QCheckBox(tr("Bloques"),this);
    CheckLinea=new QCheckBox(tr("Lineas"),this);
    CheckLetra=new QCheckBox(tr("Letras"),this);
    OpcVista=addToolBar(tr("Opciones Vista"));
    OpcVista->addWidget(new QLabel(tr("Ver : ")));
    OpcVista->addWidget(CheckBlock);
    OpcVista->addWidget(CheckLinea);
    OpcVista->addWidget(CheckLetra);
    addToolBar(Qt::TopToolBarArea,OpcVista);
    connect(CheckBlock,&QCheckBox::stateChanged, this, &VentanaPrincipal::CambiaVerRecuadros);
    connect(CheckLinea,&QCheckBox::stateChanged, this, &VentanaPrincipal::CambiaVerRecuadros);
    connect(CheckLetra,&QCheckBox::stateChanged, this, &VentanaPrincipal::CambiaVerRecuadros);

    BarraEstado = statusBar();
    BarraEstado->setVisible(true);
    }

void VentanaPrincipal::CrearGui(void)
{
    MiTab=new QTabWidget(this);
    TabVisor=new QWidget();
    TabLayVisor=new QBoxLayout(QBoxLayout::LeftToRight);
    scrollArea = new QScrollArea;
    scrollArea->setAlignment(Qt::AlignCenter);
    Lienzo = new QLienzo(this);
    Lienzo->setStyleSheet("QLabel { background-color : white;}");
    scrollArea->setWidget(Lienzo);
    TabLayVisor->addWidget(scrollArea);
    TabVisor->setLayout(TabLayVisor);
    MiTab->addTab(TabVisor,QIcon(":/Iconos/TabVisor"),"Visor");
    this->setCentralWidget(MiTab);
    this->setWindowState(Qt::WindowMaximized);
/*
    WindowsCentral=new QWidget(this);
    boxlay=new QBoxLayout(QBoxLayout::LeftToRight,this);
    scrollArea = new QScrollArea;
    scrollArea->setAlignment(Qt::AlignCenter);

    Lienzo = new QLienzo(this);
    Lienzo->setStyleSheet("QLabel { background-color : white;}");
    scrollArea->setWidget(Lienzo);
    boxlay->addWidget(scrollArea);
    WindowsCentral->setLayout(boxlay);
    this->setCentralWidget(WindowsCentral);
//    this->setCentralWidget(scrollArea);
    this->setWindowState(Qt::WindowMaximized);
*/
}

void VentanaPrincipal::CerrarPrograma(void)
{
    qApp->quit();
}

void VentanaPrincipal::CambiaVerRecuadros (int estado)
{
    bool bolbox,bollin,bolchar;

    bolbox=CheckBlock->isChecked();
    bollin=CheckLinea->isChecked();
    bolchar=CheckLetra->isChecked();
    MiPDF->VerRecuadros(bolbox,bollin,bolchar);
    VerPagina(PaginaActual);
}

void VentanaPrincipal::AbrirDocumento()
{
    QMessageBox msgBox;
    QString texto;
    QString filename;

    filename = QFileDialog::getOpenFileName(this,tr("Abrir documento PDF/XPS"), ".", "PDF (*.pdf);;XPS (*.xps)");
    if(filename.isEmpty())
    {
        msgBox.setText(tr("No se ha seleccionado ningún fichero."));
        msgBox.exec();
    }
    else if (MiPDF->AbrirDoc(filename))
    {
        VerPagina(0);
        zoominAction->setEnabled(true);
        zoomoutAction->setEnabled(true);
        PaginasTotales=MiPDF->getTotalPaginas();
        if (PaginasTotales) siguientepagAction->setEnabled(true);
        saveAction->setEnabled(true);
        saveasAction->setEnabled(true);
    }
    else {
        zoominAction->setEnabled(false);
        zoomoutAction->setEnabled(false);
        saveAction->setEnabled(false);
        saveasAction->setEnabled(false);
    }
}

void VentanaPrincipal::GuardarDocumento (void)
{
    if (NombreFichero.isEmpty())
    {
        GuardarComoDocumento();
    }
    else {
        MiPDF->CreaPdf(NombreFichero);
    }
}

void VentanaPrincipal::GuardarComoDocumento (void)
{
    QMessageBox msgBox;
    QString texto;
    QString filename;

    filename = QFileDialog::getSaveFileName(this,tr("Guardar documento PDF convertido"), ".", "PDF (*.pdf)");
    if(filename.isEmpty())
    {
        msgBox.setText(tr("No se ha seleccionado ningún fichero."));
        msgBox.exec();
    }
    else {
        NombreFichero=filename;
        MiPDF->CreaPdf(NombreFichero);
    }
}

void VentanaPrincipal::ZoomInImagen (void)
{
    QList <BloquePDF> MiLista;

    Escala=MiPDF->getEscalaX();
    if (Escala>=5.0f)
    {
        zoominAction->setEnabled(false);
        return;
    }
    Escala+=0.4f;
    MiPDF->setEscala(Escala,Escala);
    zoomoutAction->setEnabled(true);
    VerPagina(PaginaActual);
    MiLista=MiPDF->ExtructuraPagina(PaginaActual);

    while (!MiLista.isEmpty())
    {
        delete MiLista.takeFirst().Lineas;
    }
}

void VentanaPrincipal::ZoomOutImagen (void)
{
    Escala=MiPDF->getEscalaX();
    if (Escala<=0.4f)
    {
        zoomoutAction->setEnabled(false);
        return;
    }
    Escala-=0.4f;
    MiPDF->setEscala(Escala,Escala);
    zoominAction->setEnabled(true);
    VerPagina(PaginaActual);
}

void VentanaPrincipal::PaginaAnterior (void)
{
    if (PaginaActual > 0) {
        --PaginaActual;
        VerPagina(PaginaActual);
        siguientepagAction->setEnabled(true);
    }

    //enabling and disablign the next/previous button based on the current page index
    if(PaginaActual==0)
    {
        anteriorpagAction->setEnabled(false);
    }
    if(PaginaActual!=PaginasTotales-1 && !(PaginasTotales))
    {
        siguientepagAction->setEnabled(true);
    }

}

void VentanaPrincipal::PaginaSiguiente (void)
{
    if (PaginaActual < PaginasTotales-1) {
        ++PaginaActual;
        VerPagina(PaginaActual);
//        MiPDF->EstructuraPagina();
        anteriorpagAction->setEnabled(true);
    }

    if(PaginaActual==PaginasTotales-1)
    {
        siguientepagAction->setEnabled(false);
    }
    if(PaginaActual!=0 && !(PaginasTotales))
    {
        anteriorpagAction->setEnabled(true);
    }
}

void VentanaPrincipal::VerPagina (int page)
{
    QImage *image;

    image=MiPDF->VerPagina(page);
    Lienzo->setPixmap(QPixmap::fromImage(*image));
    Lienzo->resize(Lienzo->sizeHint());

    scrollArea->verticalScrollBar()->setValue(0);
//    setTitle(Titulo,(PaginaActual+1));
}


void VentanaPrincipal::RatonPresionado (QMouseEvent *event)
{
    qDebug() << "Ventana principal presionado.." << event->pos() << "---> " << event->x() << "," << event->y();
}
