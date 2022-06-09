#pragma once

#include <QObject>
#include <QSortFilterProxyModel>

#include "Constants.h"

class SearchProxyModel : public QSortFilterProxyModel {
public:
  explicit SearchProxyModel(QObject *parent = nullptr);
  // Überschriebene Methode
  bool filterAcceptsRow(int source_row,
                        const QModelIndex &source_parent) const override;
  // Setter
  void setSearchTerm(const QString &searchTerm);

private:
  QString searchTerm;
};
