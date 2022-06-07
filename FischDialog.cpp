#include "FischDialog.h"
#include "ui_FischDialog.h"

FischDialog::FischDialog(QString &angelplatzName, qint64 key, QWidget *parent)
    : QDialog(parent), ui(new Ui::FischDialog) {

  ui->setupUi(this);

  this->angelplatzName = angelplatzName;

  dlgKey = key;

  init();
}

FischDialog::~FischDialog() { delete ui; }

void FischDialog::init() {

  niederschlagList = QStringList() << tr("Sonnig") << tr("Wolkig")
                                   << tr("Regen") << tr("Schnee");

  ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

  ui->cbNiederschlag->addItems(niederschlagList);

  ui->cbFischarten->addItems(QStringList() << tr("Fischarten")
                                           << FischeDAO::readFischarten());

  if (dlgKey > 0)
    readEntry(dlgKey);

  ui->textFischarten->setVisible(!(dlgKey > 0));

  ui->lblFischartenSpace->setVisible(dlgKey > 0);

  setIsModified(false);

  ui->cbFischarten->setFocus();
}

void FischDialog::setIsModified(const bool isModified) {
  this->isModified = isModified;
  ui->btnSpeichern->setEnabled(isModified);
}

void FischDialog::readEntry(const qint64 key) {

  Fisch *fisch = FischeDAO::readFisch(key);

  if (fisch == nullptr)
    return;

  imagePath = fisch->getPath();

  if (!imagePath.isEmpty()) {

    ui->image->setScaledContents(false);

    ui->image->setPixmap(QPixmap::fromImage(QImage(imagePath))
                             .scaled(ui->image->width(), ui->image->height(),
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
  }

  angelplatzName = fisch->getAngelplatz();

  ui->cbFischarten->setCurrentText(fisch->getName());
  ui->sbLaenge->setValue(fisch->getLaenge());
  ui->sbGewicht->setValue(fisch->getGewicht());
  ui->dateTimeEdit->setDateTime(fisch->getZeit());
  ui->sbTemperatur->setValue(fisch->getTemperatur());
  ui->sbWindgeschwindigkeit->setValue(fisch->getWindgeschwindigkeit());
  ui->sbLuftdruck->setValue(fisch->getLuftdruck());
  ui->checkNacht->setChecked(fisch->getIsNacht());
  ui->cbNiederschlag->setCurrentText(
      niederschlagList[fisch->getNiederschlag()]);
  ui->textInfo->setText(fisch->getInfo());

  // Objekt plz vom Heap löschen
  delete fisch;
}

bool FischDialog::saveEntry() {

  bool retValue = false;

  if (ui->cbFischarten->currentIndex() == 0 &&
      ui->textFischarten->text().isEmpty()) {

    QMessageBox::critical(this, this->windowTitle(), tr("Eingabe fehlt"));
    ui->textFischarten->setFocus();
    return retValue;
  }

  QString name = ui->cbFischarten->currentIndex() == 0
                     ? ui->textFischarten->text()
                     : ui->cbFischarten->currentText();

  auto fun = [&](bool value, Cnt::EditMode mode) {
    retValue = value;

    if (retValue)
      emit dataModified(dlgKey, mode);
  };

  dlgKey > 0 ? fun(updateEntry(name, dlgKey), Cnt::EditMode::UPDATE)
             : fun(insertEntry(name), Cnt::EditMode::NEW);

  // Modified Flag
  isModified = !retValue;

  return retValue;
}

bool FischDialog::querySave() {

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

bool FischDialog::updateEntry(const QString &name, const qint64 key) {

  if (!FischeDAO::fischExists(key))
    return false;

  return FischeDAO::updateFisch(
      key, imagePath, name, angelplatzName, ui->sbLaenge->value(),
      ui->sbGewicht->value(), ui->dateTimeEdit->dateTime(),
      ui->sbTemperatur->value(), ui->sbWindgeschwindigkeit->value(),
      ui->sbLuftdruck->value(), ui->checkNacht->isChecked(),
      ui->cbNiederschlag->currentIndex(), ui->textInfo->toPlainText());
}

bool FischDialog::insertEntry(const QString &name) {

  return FischeDAO::insertFisch(
      imagePath, name, angelplatzName, ui->sbLaenge->value(),
      ui->sbGewicht->value(), ui->dateTimeEdit->dateTime(),
      ui->sbTemperatur->value(), ui->sbWindgeschwindigkeit->value(),
      ui->sbLuftdruck->value(), ui->checkNacht->isChecked(),
      ui->cbNiederschlag->currentIndex(), ui->textInfo->toPlainText());
}

void FischDialog::importImage() {
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

void FischDialog::closeEvent(QCloseEvent *event) {

  querySave() ? event->accept() : event->ignore();
}

void FischDialog::reject() { close(); }

void FischDialog::on_btnAbbrechen_clicked() { close(); }

void FischDialog::on_btnBildHochladen_clicked() {

  setIsModified(true);

  importImage();
}

void FischDialog::on_btnSpeichern_clicked() {

  if (isModified && !saveEntry())
    return;

  close();
}

void FischDialog::on_cbFischarten_currentIndexChanged(int index) {

  setIsModified(true);

  ui->textFischarten->setVisible(index == 0);

  ui->lblFischartenSpace->setVisible(index != 0);
}

void FischDialog::on_textFischarten_textChanged(const QString &) {
  setIsModified(true);
}

void FischDialog::on_sbLaenge_valueChanged(int) { setIsModified(true); }

void FischDialog::on_sbGewicht_valueChanged(int) { setIsModified(true); }

void FischDialog::on_dateTimeEdit_dateTimeChanged(const QDateTime &) {
  setIsModified(true);
}

void FischDialog::on_sbTemperatur_valueChanged(int) { setIsModified(true); }

void FischDialog::on_sbWindgeschwindigkeit_valueChanged(int) {
  setIsModified(true);
}

void FischDialog::on_sbLuftdruck_valueChanged(int) { setIsModified(true); }

void FischDialog::on_checkNacht_stateChanged(int) { setIsModified(true); }

void FischDialog::on_cbNiederschlag_currentTextChanged(const QString &) {
  setIsModified(true);
}

void FischDialog::on_textInfo_textChanged() { setIsModified(true); }

void FischDialog::on_textFischarten_returnPressed() { this->focusNextChild(); }
