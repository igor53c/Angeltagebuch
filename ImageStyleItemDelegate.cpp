#include "ImageStyleItemDelegate.h"

ImageStyleItemDelegate::ImageStyleItemDelegate(QWidget *parent)
    : QStyledItemDelegate(parent) {}

void ImageStyleItemDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const {
  // Um eine Hintergrundfarbe für das Bild zu setzen
  painter->fillRect(option.rect, QColor(255, 255, 255));

  // Namen der Bilddatei über den Index lesen
  QString imageFile = index.data().toString();

  if (imageFile.isEmpty())
    return;

  // Über den Dateinamen ein Image erstellen
  QImage image(imageFile);

  // Das Image in eine Pixmap kovertieren
  QPixmap pixmap = QPixmap::fromImage(image);

  // Die Abmessungen des Bildes ermitteln
  QRect rect = pixmap.rect();

  int w = rect.width();
  int h = rect.height();

  // Das Seitenverhältnis errechnen
  double ratio = static_cast<double>(h) / w;

  if (ratio > 1) {
    h = option.rect.height();
    w = h / ratio;
  } else {
    w = option.rect.width();
    h = w * ratio;
  }

  // Das Bild skalieren
  pixmap.scaled(w, h, Qt::KeepAspectRatio);

  w > h ? painter->drawPixmap(
              QRect(option.rect.left(), option.rect.top() + (w - h) / 2, w, h),
              pixmap)
        : painter->drawPixmap(
              QRect(option.rect.left() + (h - w) / 2, option.rect.top(), w, h),
              pixmap);
}
