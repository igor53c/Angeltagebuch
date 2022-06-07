#pragma once

#include <QDateTime>
#include <QObject>
#include <QSortFilterProxyModel>

#include "Constants.h"

class SortFilterProxyModel : public QSortFilterProxyModel {

public:
  SortFilterProxyModel(QObject *parent = nullptr);
  // Überschriebene Methode
  bool filterAcceptsRow(int source_row,
                        const QModelIndex &source_parent) const override;
  // Setters
  void setAngelplatz(const QString &angelplatz);
  void setAngelplatzColumn(const int angelplatzColumn);
  void setName(const QString &name);
  void setNameColumn(const int nameColumn);
  void setNiederschlag(const QString &niederschlag);
  void setNiederschlagColumn(const int niederschlagColumn);
  void setNacht(const QString &nacht);
  void setNachtColumn(const int nachtColumn);
  void setParameter(const int parameter);
  void setParameterMin(const int parameterMin);
  void setParameterMax(const int parameterMax);
  void setParameterColumn(const int parameterColumn);
  void setZeitMin(const QDateTime &zeitMin);
  void setZeitMax(const QDateTime &zeitMax);

private:
  QString angelplatz;
  int angelplatzColumn;
  QString name;
  int nameColumn;
  QString niederschlag;
  int niederschlagColumn;
  QString nacht;
  int nachtColumn;
  int parameter;
  int parameterMin;
  int parameterMax;
  int parameterColumn;
  QDateTime zeitMin;
  QDateTime zeitMax;

  bool filteringByAngelplatz(int source_row,
                             const QModelIndex &source_parent) const;
  bool filteringByName(int source_row, const QModelIndex &source_parent) const;
  bool filteringByNiederschlag(int source_row,
                               const QModelIndex &source_parent) const;
  bool filteringByNacht(int source_row, const QModelIndex &source_parent) const;
  bool filteringByParameter(int source_row,
                            const QModelIndex &source_parent) const;
};
