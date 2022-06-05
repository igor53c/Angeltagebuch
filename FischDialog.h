#pragma once

#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "Fisch.h"
#include "FischeDAO.h"

namespace Ui {
class FischDialog;
}

class FischDialog : public QDialog {
  Q_OBJECT

public:
  explicit FischDialog(qint64 angelplatzKey, qint64 key,
                       QWidget *parent = nullptr);
  ~FischDialog();

  enum EditMode { NEW, UPDATE };

signals:
  void dataModified(const qint64 key, const EditMode editMode);

private slots:
  void on_btnAbbrechen_clicked();
  void on_btnBildHochladen_clicked();
  void on_btnSpeichern_clicked();
  void on_cbFischarten_currentTextChanged(const QString &text);
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

private:
  Ui::FischDialog *ui;
  qint64 angelplatzKey;
  qint64 dlgKey;
  QString imagePath;
  bool isModified;

  void init();
  void readEntry(qint64 key);
  bool saveEntry();
  bool querySave();
  bool updateEntry(QString &name, qint64 key);
  bool insertEntry(QString &name);
  void importImage();
  void closeEvent(QCloseEvent *event) override;
  void reject() override;
};
