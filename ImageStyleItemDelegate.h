#pragma once

#include <QDebug>
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QStyledItemDelegate>

// Muss von QStyledItemDelegate abgeleitet werden
class ImageStyleItemDelegate : public QStyledItemDelegate {

  Q_OBJECT

public:
  ImageStyleItemDelegate(QWidget *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
};
