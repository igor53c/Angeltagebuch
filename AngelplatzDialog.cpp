#include "AngelplatzDialog.h"
#include "ui_AngelplatzDialog.h"

AngelplatzDialog::AngelplatzDialog(qint64 key, QWidget *parent)
    : QDialog(parent), ui(new Ui::AngelplatzDialog) {
  ui->setupUi(this);

  dlgKey = key;

  init();
}

AngelplatzDialog::~AngelplatzDialog() { delete ui; }

void AngelplatzDialog::init() {

  fische = 0;

  if (dlgKey > 0)
    readEntry(dlgKey);

  isModified = false;
}

void AngelplatzDialog::readEntry(qint64 key) {

  Angelplatz *angelplatz = AngelplaetzeDAO::readAngelplatz(key);

  if (angelplatz == nullptr)
    return;

  imagePath = angelplatz->getPath();

  if (!imagePath.isEmpty()) {

    ui->image->setScaledContents(false);

    ui->image->setPixmap(QPixmap::fromImage(QImage(imagePath))
                             .scaled(ui->image->width(), ui->image->height(),
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
  }

  fische = angelplatz->getFische();

  ui->textName->setText(angelplatz->getName());
  ui->textType->setText(angelplatz->getType());
  ui->textPlz->setText(angelplatz->getPlz());
  ui->textOrt->setText(angelplatz->getOrt());
  ui->textLand->setText(angelplatz->getLand());
  ui->textInfo->setText(angelplatz->getInfo());

  // Objekt plz vom Heap löschen
  delete angelplatz;
}

bool AngelplatzDialog::saveEntry() {

  bool retValue = false;

  if (!entryIsValid())
    return retValue;

  auto fun = [&](bool value, EditMode mode) {
    retValue = value;

    if (retValue)
      emit dataModified(dlgKey, mode);
  };

  dlgKey > 0 ? fun(updateEntry(dlgKey), EditMode::UPDATE)
             : fun(insertEntry(), EditMode::NEW);

  // Modified Flag
  isModified = !retValue;

  return retValue;
}

bool AngelplatzDialog::querySave() {

  bool retValue = false;

  // Wenn keine Änderungen vorgenommen wurden, den Dialog schließen
  if (!isModified)
    return true;

  int msgValue = QMessageBox::warning(
      this, this->windowTitle(),
      tr("Daten wurden geändert.\nSollen die Änderungen gespeichert werden?"),
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
      QMessageBox::Cancel);

  // Änderungen verwerfen und den Dialog schließen
  if (msgValue == QMessageBox::Discard)
    retValue = true;
  // Abbrechen und zurück in den Dialog
  else if (msgValue == QMessageBox::Cancel)
    retValue = false;
  // Speichern und wenn erfolgreich, den Dialog schließen
  else
    retValue = saveEntry();

  return retValue;
}

bool AngelplatzDialog::entryIsValid() {

  bool retValue = true;

  auto message = [&](QLineEdit *lineEdit, QString text) {
    QMessageBox::critical(this, this->windowTitle(), text);
    lineEdit->setFocus();
    retValue = false;
  };

  if (ui->textName->text().isEmpty())
    message(ui->textName, tr("Eingabe fehlt"));
  else if (ui->textType->text().isEmpty())
    message(ui->textType, tr("Eingabe fehlt"));
  else if (dlgKey < 1 &&
           AngelplaetzeDAO::angelplatzExists(ui->textName->text()))
    message(ui->textName, tr("Der Angelplatz ist bereits vorhanden"));

  return retValue;
}

bool AngelplatzDialog::updateEntry(qint64 key) {
  // Vor dem UPDATE prüfen, oder der TIMESTAMP des Datensatzes in der
  // Zwischenzeit von einem Benutzer geändert wurde

  Angelplatz *angelplatz = AngelplaetzeDAO::readAngelplatz(key);

  if (angelplatz == nullptr)
    return false;

  delete angelplatz;

  return AngelplaetzeDAO::updateAngelplatz(
      key, imagePath, ui->textName->text(), ui->textType->text(), fische,
      ui->textPlz->text(), ui->textOrt->text(), ui->textLand->text(),
      ui->textInfo->toPlainText());
}

bool AngelplatzDialog::insertEntry() {
  return AngelplaetzeDAO::insertAngelplatz(
      imagePath, ui->textName->text(), ui->textType->text(), fische,
      ui->textPlz->text(), ui->textOrt->text(), ui->textLand->text(),
      ui->textInfo->toPlainText());
}

void AngelplatzDialog::importImage() {
  // Gibt das Heimverzeichnis des Benutzers zurück
  QString defaultFilter =
      tr("Alle Bilddateien (*.jpeg *.jpg *.bmp *.png *.jfif)");

  // Dateiauswahl Dialog
  QString newImagePath = QFileDialog::getOpenFileName(
      this, tr("Bild hochladen"), QDir::currentPath(),
      tr("Alle Dateien (*.*);;") + defaultFilter, &defaultFilter);

  if (newImagePath.isEmpty())
    return;

  imagePath = newImagePath;

  ui->image->setScaledContents(false);

  ui->image->setPixmap(QPixmap::fromImage(QImage(imagePath))
                           .scaled(ui->image->width(), ui->image->height(),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation));
}

void AngelplatzDialog::on_btnBildHochladen_clicked() {

  isModified = true;

  importImage();
}

void AngelplatzDialog::on_btnAbbrechen_clicked() { close(); }

void AngelplatzDialog::on_btnSpeichern_clicked() {

  if (isModified && !saveEntry())
    return;

  close();
}

void AngelplatzDialog::on_textName_textChanged(const QString &) {

  isModified = true;
}

void AngelplatzDialog::on_textType_textChanged(const QString &) {

  isModified = true;
}

void AngelplatzDialog::on_textPlz_textChanged(const QString &) {

  isModified = true;
}

void AngelplatzDialog::on_textOrt_textChanged(const QString &) {

  isModified = true;
}

void AngelplatzDialog::on_textLand_textChanged(const QString &) {

  isModified = true;
}

void AngelplatzDialog::on_textInfo_textChanged() { isModified = true; }

void AngelplatzDialog::closeEvent(QCloseEvent *event) {

  querySave() ? event->accept() : event->ignore();
}

void AngelplatzDialog::reject() { close(); }
