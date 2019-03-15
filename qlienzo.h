#ifndef QLIENZO_H
#define QLIENZO_H

#include <QLabel>
#include <QDebug>

class QLienzo : public QLabel
{
    Q_OBJECT

public:
    QLienzo(QWidget *parent);
    QLienzo(const QString &text, QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
signals:
    void LienzoRatonPulsado(QMouseEvent *event);
    void LienzoRatonSoltado(QMouseEvent *event);
    void LienzoRatonMovido(QMouseEvent *event);
    void LienzoRatonDoble(QMouseEvent *event);
};

#endif // QLIENZO_H
