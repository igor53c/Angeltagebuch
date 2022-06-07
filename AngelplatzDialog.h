#pragma once

#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "AngelplaetzeDAO.h"
#include "Constants.h"
#include "FischeDAO.h"

namespace Ui {
class AngelplatzDialog;
}

class AngelplatzDialog : public QDialog {
  Q_OBJECT

public:
  explicit AngelplatzDialog(qint64 key, QWidget *parent = nullptr);
  ~AngelplatzDialog();

signals:
  // Eigenes Signal
  // Ob sich die Daten in der Datenbank geändert haben
  void dataModified(const qint64 key, const Cnt::EditMode editMode);

private slots:
  void on_btnAbbrechen_clicked();
  void on_btnSpeichern_clicked();
  void on_btnBildHochladen_clicked();
  void on_textName_textChanged(const QString &);
  void on_textType_textChanged(const QString &);
  void on_textPlz_textChanged(const QString &);
  void on_textOrt_textChanged(const QString &);
  void on_textLand_textChanged(const QString &);
  void on_textInfo_textChanged();
  void on_textName_returnPressed();
  void on_textType_returnPressed();
  void on_textPlz_returnPressed();
  void on_textOrt_returnPressed();
  void on_textLand_returnPressed();

private:
  Ui::AngelplatzDialog *ui;
  // Der Primärschlüssel des aktuellen Angelplatzes
  qint64 dlgKey;
  // Bildlink für aktuellen Angelplatz
  QString imagePath;
  // Bb sich Daten geändert haben
  bool isModified;
  // Anzahl der Fische an diesem Angelplatz
  int fische;

  void init();
  void setIsModified(const bool isModified);
  void readEntry(const qint64 key);
  bool saveEntry();
  bool querySave();
  bool entryIsValid();
  bool updateEntry(const qint64 key);
  bool insertEntry();
  void importImage();
  // Überschriebene Methoden
  // Um das Schließne des Dialogs zu überwachen
  void closeEvent(QCloseEvent *event) override;
  // Wegen der ESC-Taste
  void reject() override;
};
