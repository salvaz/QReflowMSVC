#include "qlienzo.h"
#include <QMouseEvent>

QLienzo::QLienzo(QWidget *parent):QLabel (parent)
{
    setMouseTracking(false); // habilita el evento de mover el raton sobre el Lienzo

}

QLienzo::QLienzo(const QString &text, QWidget *parent):QLabel(text,parent)
{

}

void QLienzo::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "Mouse move.." << event->pos() << "---> " << event->x() << "," << event->y();
    emit LienzoRatonMovido(event);
}

void QLienzo::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << "Doble click.." << event->pos() << "---> " << event->x() << "," << event->y();
    emit LienzoRatonDoble(event);
 }

void QLienzo::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse press.." << event->pos()  << "---> " << event->x() << "," << event->y();
    emit LienzoRatonPulsado(event);
}

void QLienzo::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Mouse release.." << event->pos() << "---> " << event->x() << "," << event->y();
    LienzoRatonSoltado(event);
}
