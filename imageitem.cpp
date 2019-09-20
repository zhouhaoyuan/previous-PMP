#include "imageitem.h"

ImageItem::ImageItem()
{

}

ImageItem::~ImageItem()
{

}

QRectF ImageItem::boundingRect() const
{
    QRect rect = this->pixmap().rect();
    //return QRectF(rect);
    return QRectF(0,0,rect.width(),rect.height());
}


QPainterPath ImageItem::shape() const
{
    QPainterPath path;
    path.addRect(-10, -20, 20, 40);
    return path;
}

void ImageItem::setMyPixmap(QPixmap pixmap,int width,int height)
{
    pixmap= pixmap.scaled(width,height,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    setPixmap(pixmap);
}


void ImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPixmap pixmap = this->pixmap();
    QRect rect = pixmap.rect();

    painter->drawPixmap(rect,pixmap);
}
