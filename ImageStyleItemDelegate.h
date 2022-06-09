#pragma once

#include <QDebug>
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QStyledItemDelegate>

#include "Constants.h"
#include "StyleBackground.h"
// Eigene Bildanzeigeklasse
// Muss von QStyledItemDelegate abgeleitet werden
class ImageStyleItemDelegate : public QStyledItemDelegate {

  Q_OBJECT

public:
  ImageStyleItemDelegate(QWidget *parent = nullptr);
  // Überschriebene Methode
  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
};
