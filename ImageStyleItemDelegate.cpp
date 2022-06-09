#include "ImageStyleItemDelegate.h"

ImageStyleItemDelegate::ImageStyleItemDelegate(QWidget *parent)
    : QStyledItemDelegate(parent) {}

void ImageStyleItemDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const {
  // Um eine Hintergrundfarbe für das Bild zu setzen
  painter->fillRect(option.rect, StyleBackground::colorBackground());
  // Namen der Bilddatei über den Index lesen
  QString imageFile = index.data().toString();
  // Überprüfen, ob der Bildlink geladen ist
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
  // Prüfen, ob die Breite oder Höhe höher ist,
  // und stellen einen höheren Wert für den Standard ein
  if (ratio > 1) {
    h = option.rect.height();
    w = h / ratio;
  } else {
    w = option.rect.width();
    h = w * ratio;
  }
  // Das Bild skalieren
  pixmap.scaled(w, h, Qt::KeepAspectRatio);
  // Platzieren das Bild immer in der Mitte des Feldes
  painter->drawPixmap(
      w > h ? QRect(option.rect.left(), option.rect.top() + (w - h) / 2, w, h)
            : QRect(option.rect.left() + (h - w) / 2, option.rect.top(), w, h),
      pixmap);
}
