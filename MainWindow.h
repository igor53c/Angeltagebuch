#pragma once

#include <QDebug>
#include <QLabel>
#include <QMainWindow>
#include <QPalette>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QStyle>
#include <QTimer>

#include "AngelplaetzeDAO.h"
#include "AngelplatzDialog.h"
#include "AngelplatzWindow.h"
#include "DAOLib.h"
#include "FischeDAO.h"
#include "ImageStyleItemDelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:

  void on_actionBEenden_triggered();
  void on_actionNeu_triggered();
  void on_actionLschen_triggered();
  void on_actionNdern_triggered();
  void on_tableView_doubleClicked(const QModelIndex &index);
  void on_actionMarkierterAngelplatz_triggered();
  void on_actionAlleAngelpltze_triggered();
  void tableView_selectionChanged();
  void tableView_section_resized(int index, int oldSize, int newSize);
  void modifyTableView(const qint64 key,
                       const AngelplatzDialog::EditMode editMode);

private:
  Ui::MainWindow *ui;
  QLabel *statusLabel;
  AngelplatzDialog *angelplatzDialog;
  AngelplatzWindow *angelplatzWindow;

  void init();
  bool openDatabase();
  QSqlTableModel *setTableViewModel();
  void showTable();
  void showAngelplatzDialog(const qint64 key);
  void showAngelplatzWindow(const qint64 key);
  void deleteEntry(const QModelIndex &index);
  // Aktualisiert die TableView nach dem Hinzufügen eines neuen Datensatzes
  void refreshTableView(const qint64 key);
  void findItemInTableView(const QString &columnName, const QVariant &value);
  // Aktualisiert die TableView nach einer Datensatzänderung
  void updateTableView(const qint64 key);
  bool eventFilter(QObject *sender, QEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
};
