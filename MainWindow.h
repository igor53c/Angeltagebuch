#pragma once

#include <QActionGroup>
#include <QLabel>
#include <QLibraryInfo>
#include <QLineEdit>
#include <QMainWindow>
#include <QPalette>
#include <QPushButton>
#include <QSqlRecord>
#include <QTranslator>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "AngelplaetzeDAO.h"
#include "AngelplatzDialog.h"
#include "AngelplatzSqlTableModel.h"
#include "AngelplatzWindow.h"
#include "Constants.h"
#include "DAOLib.h"
#include "FischeDAO.h"
#include "ImageStyleItemDelegate.h"
#include "SearchProxyModel.h"
#include "StyleBackground.h"

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
  void on_action_Suchen_triggered();
  void on_tableView_doubleClicked(const QModelIndex &index);
  void on_actionMarkierterAngelplatz_triggered();
  void on_actionAlleAngelpltze_triggered();
  void on_actionDeutsch_triggered();
  void on_actionEnglisch_triggered();
  void on_actionSerBisch_triggered();
  void on_actionWeiss_triggered();
  void on_actionGelb_triggered();
  void on_actionGraU_triggered();
  void on_actionGRn_triggered();
  void on_actionRot_triggered();
  void on_actionBlau_triggered();
  // Eigene Slots
  void tableView_selectionChanged();
  void tableView_section_resized(int index, int oldSize, int newSize);
  void modifyTableView(const qint64 key, const Cnt::EditMode editMode);
  // Einstellen des Spaltenbreitenwerts, gesendetes Signal von AngelplatzWindow
  void setColumnAngelplatzWidth(const QList<int> list);
  void onSearch_cancel();
  void onTextSuchen_textChanged(const QString &);

private:
  Ui::MainWindow *ui;
  QLabel *statusLabel;
  AngelplatzDialog *angelplatzDialog;
  AngelplatzWindow *angelplatzWindow;
  AngelplatzSqlTableModel *model;
  SearchProxyModel *proxyModel;
  QTranslator *sysTranslator;
  bool sysTranslatorInstalled;
  QTranslator *enTranslator;
  bool enTranslatorInstalled;
  QTranslator *srbTranslator;
  bool srbTranslatorInstalled;
  QString currentLanguage;
  QString xmlConfigFile;
  QList<int> mainColWidthList;
  QList<int> angelplatzColWidthList;
  QLineEdit *textSuchen;
  QPushButton *cancelButton;

  void init();
  void setBackgroundColor(const int colorIndex);
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
  void loadLanguage(const QString &language);
  void loadBackgroundColor(const int color);
  void removeAllTranslators();
  void readXMLSettings(const QString &filename);
  void writeXMLSettings(const QString &filename);
  // Überschriebene Methoden
  bool eventFilter(QObject *sender, QEvent *event) override;
  void changeEvent(QEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
};
