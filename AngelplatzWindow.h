#pragma once

#include <QDebug>
#include <QMainWindow>
#include <QSqlRecord>
#include <QSqlTableModel>

#include "AngelplaetzeDAO.h"
#include "Angelplatz.h"
#include "AngelplatzDialog.h"
#include "FischDialog.h"
#include "FischeDAO.h"
#include "ImageStyleItemDelegate.h"
#include "SortFilterProxyModel.h"
#include "FischeSqlTableModel.h"

namespace Ui {
class AngelplatzWindow;
}

class AngelplatzWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit AngelplatzWindow(QList<int> columnAngelplatzWidth,
                            QString &angelplatzName, QWidget *parent = nullptr);
  ~AngelplatzWindow();

signals:
  void dataModified(const qint64 key);
  void columnWidthModified(const QList<int> list);

private slots:
  void on_actionBEenden_triggered();
  void on_actionNeu_triggered();
  void on_actionNdern_triggered();
  void on_actionLschen_triggered();
  void on_cbFischarten_currentTextChanged(const QString &text);
  void on_cbNiederschlag_currentTextChanged(const QString &text);
  void on_cbNacht_currentTextChanged(const QString &text);
  void on_cbParameter_currentIndexChanged(int index);
  void on_sbMin_valueChanged(int);
  void on_dateTimeMin_dateTimeChanged(const QDateTime &);
  void on_sbMax_valueChanged(int);
  void on_dateTimeMax_dateTimeChanged(const QDateTime &);
  void on_tableView_doubleClicked(const QModelIndex &index);
  void modifyTableView(const qint64 key, const FischDialog::EditMode editMode);
  void tableView_section_resized(int index, int oldSize, int newSize);
  void tableView_selectionChanged();

  private:
  Ui::AngelplatzWindow *ui;
  QString angelplatzName;
  QLabel *statusLabel;
  FischeSqlTableModel *model;
  SortFilterProxyModel *proxyModel;
  QString filterFischarten;
  QString filterNiederschlag;
  QString filterNacht;
  int filterParameter;
  QList<int> columnAngelplatzWidth;
  QStringList listNacht;
  QStringList listNiederschlag;

  void init();
  void setTableViewModel();
  void showTable();
  void readEntry(const QString &name);
  void showFischDialog(const qint64 key);
  void deleteEntry(const QModelIndex &index);
  void refreshTableView(const qint64 key);
  void findItemInTableView(const QString &columnName, const QVariant &value);
  // Aktualisiert die TableView nach einer Datensatzänderung
  void updateTableView(const qint64 key);
  void showParameterFilter(bool spinBox, bool dateTime, bool text);
  // Event filter für die TableView
  bool eventFilter(QObject *sender, QEvent *event) override;
  // Überschriebene Methoden
  void closeEvent(QCloseEvent *event) override;
};
