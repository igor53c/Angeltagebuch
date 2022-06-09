#include "SearchProxyModel.h"

SearchProxyModel::SearchProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {}

bool SearchProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex &source_parent) const {

  if (searchTerm.isEmpty())
    return true;
  // Ob der Suchbegriff in einer Spalte steht
  for (int i = 0; i <= Cnt::Angelplaetze::A_INFO; i++)
    if (sourceModel()
            ->data(sourceModel()->index(source_row, i, source_parent))
            .toString()
            .contains(searchTerm, Qt::CaseInsensitive))
      return true;

  return false;
}

void SearchProxyModel::setSearchTerm(const QString &searchTerm) {
  this->searchTerm = searchTerm;
}
