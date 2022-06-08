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
  // Initialisierung der ComboBox-Werteliste
  niederschlagList = QStringList() << tr("Sonnig") << tr("Wolkig")
                                   << tr("Regen") << tr("Schnee");
  // Stellen die DateTime-Spinbox auf die aktuelle Zeit ein
  ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
  // Initialisierung der ComboBox
  ui->cbNiederschlag->addItems(niederschlagList);

  ui->cbFischarten->addItems(QStringList() << tr("Fischarten")
                                           << FischeDAO::readFischarten());

  if (dlgKey > 0)
    // Lesen von Daten für einen bestehenden Dialog
    readEntry(dlgKey);
  // Mit Update wird textFischarten zunächst nicht angezeigt. Bei einem neuen
  // Eintrag wird der textFischarten angezeigt
  ui->textFischarten->setVisible(!(dlgKey > 0));
  ui->lblFischartenSpace->setVisible(dlgKey > 0);

  setIsModified(false);
  // Fokuseinstellung
  ui->cbFischarten->setFocus();
  // Hintergrundfarbe ändern
  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, DAOLib::colorBackground());
  this->setPalette(palette);
}

void FischDialog::setIsModified(const bool isModified) {
  this->isModified = isModified;
  // Wenn nichts geändert wird, ist btnSpeichern deaktiviert
  ui->btnSpeichern->setEnabled(isModified);
}

void FischDialog::readEntry(const qint64 key) {
  // Laden von Daten aus der Datenbank für den aktuellen Fisch
  Fisch *fisch = FischeDAO::readFisch(key);
  // Wenn das Laden der Daten fehlgeschlagen ist
  if (fisch == nullptr)
    return;
  // Speichern des Bildlinks in einer globalen Variablen
  imagePath = fisch->getPath();

  if (!imagePath.isEmpty()) {
    // Bildskalierung deaktivieren
    ui->image->setScaledContents(false);
    // Das Bild wird gesetzt, wenn ein Bildlink vorhanden ist
    ui->image->setPixmap(QPixmap::fromImage(QImage(imagePath))
                             .scaled(ui->image->width(), ui->image->height(),
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
  }
  // Speichern den Namen des Angelplatzes in einer globalen Variablen
  angelplatzName = fisch->getAngelplatz();
  // Eingabe von Daten in Dialogfeldern
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
  // Objekt fisch vom Heap löschen
  delete fisch;
}

bool FischDialog::saveEntry() {

  bool retValue = false;
  // Der Name für den Fisch muss gewählt werden
  if (ui->cbFischarten->currentIndex() == 0 &&
      ui->textFischarten->text().isEmpty()) {

    QMessageBox::critical(this, this->windowTitle(), tr("Eingabe fehlt"));
    // Anpassen des Fokus auf ein ungültiges Feld
    ui->textFischarten->setFocus();
    return retValue;
  }
  // Wert für den Namen aus der ComboBox oder aus dem Feld textFischarten
  QString name = ui->cbFischarten->currentIndex() == 0
                     ? ui->textFischarten->text()
                     : ui->cbFischarten->currentText();
  // Lambda-Funktion für update und new
  auto fun = [&](bool value, Cnt::EditMode mode) {
    retValue = value;

    if (retValue)
      // Meldung, wenn sich die Daten in der Datenbank geändert haben
      emit dataModified(dlgKey, mode);
  };
  // Wenn es einen Angelplatz gibt, aktualisieren Sie die Daten und wenn nicht,
  // fügen Sie einen neuen Angelplatz ein
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
  // Nachricht zur Datenspeicherung
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
  // Überprüfen, ob ein Fisch mit diesem Primärschlüssel existiert
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
  // Standard-Bilderweiterungen
  QString defaultFilter =
      tr("Alle Bilddateien (*.jpeg *.jpg *.bmp *.png *.jfif)");
  // Dateiauswahl Dialog
  QString newImagePath = QFileDialog::getOpenFileName(
      this, tr("Bild hochladen"), QDir::currentPath(),
      tr("Alle Dateien (*.*);;") + defaultFilter, &defaultFilter);
  // Ob ein Bild ausgewählt ist
  if (newImagePath.isEmpty())
    return;
  // Speichern des Bildlinks in einer globalen Variablen
  imagePath = newImagePath;
  // Bildskalierung deaktivieren
  ui->image->setScaledContents(false);
  // Das Bild wird gesetzt, wenn ein Bildlink vorhanden ist
  ui->image->setPixmap(QPixmap::fromImage(QImage(imagePath))
                           .scaled(ui->image->width(), ui->image->height(),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation));
}

void FischDialog::closeEvent(QCloseEvent *event) {
  // Prüfen vor dem Beenden, ob die eingegebenen Daten gespeichert werden sollen
  querySave() ? event->accept() : event->ignore();
}

void FischDialog::reject() { close(); }

void FischDialog::on_btnAbbrechen_clicked() { close(); }
// Bei jeder Wertänderung ist isModified = true
void FischDialog::on_btnBildHochladen_clicked() {

  setIsModified(true);

  importImage();
}

void FischDialog::on_btnSpeichern_clicked() {
  // Überprüfen, ob etwas geändert wurde und ob das Speichern erfolgreich war
  if (isModified && !saveEntry())
    return;

  close();
}

void FischDialog::on_cbFischarten_currentIndexChanged(int index) {

  setIsModified(true);
  // Nur für den Anfangswert der ComboBox wird das Feld textFischarten angezeigt
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
// Nach Drücken der Enter-Taste wird automatisch zum nächsten Feld gewechselt
void FischDialog::on_textFischarten_returnPressed() { this->focusNextChild(); }
