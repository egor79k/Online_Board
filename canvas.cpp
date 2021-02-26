#include "canvas.h"
#include "serverapi.h"

#include <QMouseEvent>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QDebug>

Canvas::Canvas(QWidget* parent, QSize size)
    : QWidget(parent),
      background(size, QImage::Format_ARGB32),
      canv_image(size, QImage::Format_ARGB32)
{
    resize(size);
    background.fill(Qt::white);

    connect(ServerApi::getInstance(), SIGNAL(dataReceived()), this, SLOT(onDataReceived()));
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        startDrawing(event->pos());
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (drawing) {
        drawLineToPoint(event->pos());
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        stopDrawing();
    }
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(event->rect(), background, event->rect());
    painter.drawImage(event->rect(), canv_image, event->rect());
}

void Canvas::startDrawing(const QPoint& pos)
{
    prev_point = pos;
    drawing = true;
}

void Canvas::drawLineToPoint(const QPoint& point)
{
    drawLine(prev_point, point);

    QString sendStr = QString("%1 %2 %3 %4").arg(prev_point.x()).
            arg(prev_point.y()).arg(point.x()).arg(point.y());
    ServerApi::sendData(sendStr.toUtf8());

    prev_point = point;
}

void Canvas::drawLine(const QPoint& first, const QPoint& second)
{
    QPainter painter(&canv_image);
    painter.drawLine(first, second);
    update();
}

void Canvas::stopDrawing()
{
    drawing = false;
}


void Canvas::onDataReceived()
{
    QString data = QString::fromUtf8(ServerApi::readData());
    qDebug() << "Data received: " << data;

    QStringList data_split = data.split(' ');
    Q_ASSERT(data_split.size() == 4);

    QPoint first(data_split.at(0).toInt(), data_split.at(1).toInt());
    QPoint second(data_split.at(2).toInt(), data_split.at(3).toInt());

    drawLine(first, second);
}
