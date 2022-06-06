#pragma once

#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "AngelplaetzeDAO.h"

namespace Ui {
class AngelplatzDialog;
}

class AngelplatzDialog : public QDialog {
  Q_OBJECT

public:
  explicit AngelplatzDialog(qint64 key, QWidget *parent = nullptr);
  ~AngelplatzDialog();

  enum EditMode { NEW, UPDATE };

signals:
  void dataModified(const qint64 key,
                    const AngelplatzDialog::EditMode editMode);

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
  qint64 dlgKey;
  QString imagePath;
  bool isModified;
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
  void closeEvent(QCloseEvent *event) override;
  void reject() override;
};
