#pragma once

#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "Constants.h"
#include "Fisch.h"
#include "FischeDAO.h"
#include "DAOLib.h"

namespace Ui {
class FischDialog;
}

class FischDialog : public QDialog {
  Q_OBJECT

public:
  explicit FischDialog(QString &angelplatzName, qint64 key,
                       QWidget *parent = nullptr);
  ~FischDialog();

signals:
  // Eigenes Signal
  // Ob sich die Daten in der Datenbank geändert haben
  void dataModified(const qint64 key, const Cnt::EditMode editMode);

private slots:
  void on_btnAbbrechen_clicked();
  void on_btnBildHochladen_clicked();
  void on_btnSpeichern_clicked();
  void on_cbFischarten_currentIndexChanged(int index);
  void on_textFischarten_textChanged(const QString &);
  void on_sbLaenge_valueChanged(int);
  void on_sbGewicht_valueChanged(int);
  void on_dateTimeEdit_dateTimeChanged(const QDateTime &);
  void on_sbTemperatur_valueChanged(int);
  void on_sbWindgeschwindigkeit_valueChanged(int);
  void on_sbLuftdruck_valueChanged(int);
  void on_checkNacht_stateChanged(int);
  void on_cbNiederschlag_currentTextChanged(const QString &);
  void on_textInfo_textChanged();
  void on_textFischarten_returnPressed();

private:
  Ui::FischDialog *ui;
  // Der Name des aktuellen Angelplatzes
  QString angelplatzName;
  // Der Primärschlüssel zum aktuellen Fisch
  qint64 dlgKey;
  // Bildlink für aktuellen Fisch
  QString imagePath;
  // Ob sich Daten geändert haben
  bool isModified;
  // ComboBox-Werteliste
  QStringList niederschlagList;

  void init();
  void setIsModified(const bool isModified);
  void readEntry(const qint64 key);
  bool saveEntry();
  bool querySave();
  bool updateEntry(const QString &name, const qint64 key);
  bool insertEntry(const QString &name);
  void importImage();
  // Überschriebene Methoden
  // Um das Schließne des Dialogs zu überwachen
  void closeEvent(QCloseEvent *event) override;
  // Wegen der ESC-Taste
  void reject() override;
};
