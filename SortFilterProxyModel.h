#pragma once

#include <QObject>
#include <QSortFilterProxyModel>
#include <QDateTime>

class SortFilterProxyModel : public QSortFilterProxyModel {

public:
  SortFilterProxyModel(QObject *parent = nullptr);

  bool filterAcceptsRow(int source_row,
                        const QModelIndex &source_parent) const override;

  void setAngelplatz(qint64 angelplatz);
  void setAngelplatzColumn(int angelplatzColumn);
  void setName(QString& name);
  void setNameColumn(int nameColumn);
  void setNiederschlag(QString& niederschlag);
  void setNiederschlagColumn(int niederschlagColumn);
  void setNacht(QString& nacht);
  void setNachtColumn(int nachtColumn);
  void setParameter(QString& parameter);
  void setParameterMin(int parameterMin);
  void setParameterMax(int parameterMax);
  void setParameterColumn(int parameterColumn);
  void setZeitMin(QDateTime zeitMin);
  void setZeitMax(QDateTime zeitMax);

private:
  qint64 angelplatz;
  int angelplatzColumn;
  QString name;
  int nameColumn;
  QString niederschlag;
  int niederschlagColumn;
  QString nacht;
  int nachtColumn;
  QString parameter;
  int parameterMin;
  int parameterMax;
  int parameterColumn;
  QDateTime zeitMin;
  QDateTime zeitMax;

  bool filteringByAngelplatz(int source_row, const QModelIndex &source_parent) const;
  bool filteringByName(int source_row, const QModelIndex &source_parent) const;
  bool filteringByNiederschlag(int source_row, const QModelIndex &source_parent) const;
  bool filteringByNacht(int source_row, const QModelIndex &source_parent) const;
  bool filteringByParameter(int source_row, const QModelIndex &source_parent) const;
};
