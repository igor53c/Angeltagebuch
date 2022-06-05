#pragma once

#include <QDebug>
#include <QLabel>
#include <QMainWindow>
#include <QPalette>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QStyle>
#include <QTimer>
#include <QTranslator>
#include <QLibraryInfo>
#include <QActionGroup>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

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
  void on_actionDeutsch_triggered();
  void on_actionEnglisch_triggered();
  void tableView_selectionChanged();
  void tableView_section_resized(int index, int oldSize, int newSize);
  void modifyTableView(const qint64 key,
                       const AngelplatzDialog::EditMode editMode);
  void setColumnAngelplatzWidth(const QList<int> list);

private:
  Ui::MainWindow *ui;
  QLabel *statusLabel;
  AngelplatzDialog *angelplatzDialog;
  AngelplatzWindow *angelplatzWindow;
  QSqlTableModel *model;
  QTranslator* sysTranslator;
  bool sysTranslatorInstalled;
  QTranslator* enTranslator;
  bool enTranslatorInstalled;
  QString currentLanguage;
  QString xmlConfigFile;
  QList<int> columnMainWidth;
  QList<int> columnAngelplatzWidth;

  void init();
  bool openDatabase();
  void setTableViewModel();
  void showTable();
  void showAngelplatzDialog(const qint64 key);
  void showAngelplatzWindow(const qint64 key);
  void deleteEntry(const QModelIndex &index);
  // Aktualisiert die TableView nach dem Hinzufügen eines neuen Datensatzes
  void refreshTableView(const qint64 key);
  void findItemInTableView(const QString &columnName, const QVariant &value);
  // Aktualisiert die TableView nach einer Datensatzänderung
  void updateTableView(const qint64 key);
  void loadLanguage(const QString& language);
  void removeAllTranslators();
  void readXMLSettings(const QString& filename);
  void writeXMLSettings(const QString& filename);
  bool eventFilter(QObject *sender, QEvent *event) override;
  void changeEvent(QEvent* event) override;
  void closeEvent(QCloseEvent *event) override;
};
