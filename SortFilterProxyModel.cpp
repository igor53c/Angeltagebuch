#include "SortFilterProxyModel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {}

bool SortFilterProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex &source_parent) const {

  return filteringByAngelplatz(source_row, source_parent)

         && filteringByName(source_row, source_parent)

         && filteringByNiederschlag(source_row, source_parent)

         && filteringByNacht(source_row, source_parent)

         && filteringByParameter(source_row, source_parent);
}

bool SortFilterProxyModel::filteringByAngelplatz(
    int source_row, const QModelIndex &source_parent) const {

  if (angelplatz < 1)
    return true;

  QModelIndex ind =
      sourceModel()->index(source_row, angelplatzColumn, source_parent);

  return sourceModel()->data(ind, Qt::DisplayRole).toLongLong() == angelplatz;
}

bool SortFilterProxyModel::filteringByName(
    int source_row, const QModelIndex &source_parent) const {

  if (name.isEmpty())
    return true;

  QModelIndex ind = sourceModel()->index(source_row, nameColumn, source_parent);

  return sourceModel()->data(ind, Qt::DisplayRole).toString() == name;
}

bool SortFilterProxyModel::filteringByNiederschlag(
    int source_row, const QModelIndex &source_parent) const {

  if (niederschlag.isEmpty())
    return true;

  QModelIndex ind =
      sourceModel()->index(source_row, niederschlagColumn, source_parent);

  return sourceModel()->data(ind, Qt::DisplayRole).toString() == niederschlag;
}

bool SortFilterProxyModel::filteringByNacht(
    int source_row, const QModelIndex &source_parent) const {

  if (nacht.isEmpty())
    return true;

  QModelIndex ind =
      sourceModel()->index(source_row, nachtColumn, source_parent);

  return sourceModel()->data(ind, Qt::DisplayRole).toString() == nacht;
}

bool SortFilterProxyModel::filteringByParameter(
    int source_row, const QModelIndex &source_parent) const {

  if (parameter.isEmpty())
    return true;

  QModelIndex ind =
      sourceModel()->index(source_row, parameterColumn, source_parent);

  return parameter == "Zeit"
             ? sourceModel()->data(ind, Qt::DisplayRole).toDateTime() >=
                       zeitMin &&
                   sourceModel()->data(ind, Qt::DisplayRole).toDateTime() <=
                       zeitMax
             : sourceModel()->data(ind, Qt::DisplayRole).toInt() >=
                       parameterMin &&
                   sourceModel()->data(ind, Qt::DisplayRole).toInt() <=
                       parameterMax;
}

void SortFilterProxyModel::setAngelplatz(qint64 angelplatz) {
  this->angelplatz = angelplatz;
}

void SortFilterProxyModel::setAngelplatzColumn(int angelplatzColumn) {
  this->angelplatzColumn = angelplatzColumn;
}

void SortFilterProxyModel::setName(QString &name) { this->name = name; }

void SortFilterProxyModel::setNameColumn(int nameColumn) {
  this->nameColumn = nameColumn;
}

void SortFilterProxyModel::setNiederschlag(QString &niederschlag) {
  this->niederschlag = niederschlag;
}

void SortFilterProxyModel::setNiederschlagColumn(int niederschlagColumn) {
  this->niederschlagColumn = niederschlagColumn;
}

void SortFilterProxyModel::setNacht(QString &nacht) { this->nacht = nacht; }

void SortFilterProxyModel::setNachtColumn(int nachtColumn) {
  this->nachtColumn = nachtColumn;
}

void SortFilterProxyModel::setParameter(QString &parameter) {
  this->parameter = parameter;
}

void SortFilterProxyModel::setParameterMin(int parameterMin) {
  this->parameterMin = parameterMin;
}

void SortFilterProxyModel::setParameterMax(int parameterMax) {
  this->parameterMax = parameterMax;
}

void SortFilterProxyModel::setParameterColumn(int parameterColumn) {
  this->parameterColumn = parameterColumn;
}

void SortFilterProxyModel::setZeitMin(QDateTime zeitMin) {
  this->zeitMin = zeitMin;
}

void SortFilterProxyModel::setZeitMax(QDateTime zeitMax) {
  this->zeitMax = zeitMax;
}
