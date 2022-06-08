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
  // Anfangswert für die Anzahl der Fische
  fische = 0;

  if (dlgKey > 0)
    // Lesen von Daten für einen bestehenden Dialog
    readEntry(dlgKey);

  setIsModified(false);
  // Fokuseinstellung
  ui->textName->setFocus();
  // Hintergrundfarbe ändern
  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, DAOLib::colorBackground());
  this->setPalette(palette);
}

void AngelplatzDialog::setIsModified(const bool isModified) {

  this->isModified = isModified;
  // Wenn nichts geändert wird, ist btnSpeichern deaktiviert
  ui->btnSpeichern->setEnabled(isModified);
}

void AngelplatzDialog::readEntry(const qint64 key) {
  // Laden von Daten aus der Datenbank für den aktuellen Angelplatz
  Angelplatz *angelplatz = AngelplaetzeDAO::readAngelplatz(key);
  // Wenn das Laden der Daten fehlgeschlagen ist
  if (angelplatz == nullptr)
    return;
  // Speichern des Bildlinks in einer globalen Variablen
  imagePath = angelplatz->getPath();

  if (!imagePath.isEmpty()) {
    // Bildskalierung deaktivieren
    ui->image->setScaledContents(false);
    // das Bild wird gesetzt, wenn ein Bildlink vorhanden ist
    ui->image->setPixmap(QPixmap::fromImage(QImage(imagePath))
                             .scaled(ui->image->width(), ui->image->height(),
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
  }
  // Speichern die Anzahl der Fische in der globalen Variablen
  fische = angelplatz->getFische();
  // Eingabe von Daten in Dialogfeldern
  ui->textName->setText(angelplatz->getName());
  ui->textType->setText(angelplatz->getType());
  ui->textPlz->setText(angelplatz->getPlz());
  ui->textOrt->setText(angelplatz->getOrt());
  ui->textLand->setText(angelplatz->getLand());
  ui->textInfo->setText(angelplatz->getInfo());
  // Objekt angelplatz vom Heap löschen
  delete angelplatz;
}

bool AngelplatzDialog::saveEntry() {

  bool retValue = false;
  // Wenn die eingegebenen Daten nicht gültig sind
  if (!entryIsValid())
    return retValue;
  // Lambda-Funktion für update und new
  auto fun = [&](bool value, Cnt::EditMode mode) {
    retValue = value;

    if (retValue)
      // Meldung, wenn sich die Daten in der Datenbank geändert haben
      emit dataModified(dlgKey, mode);
  };
  // Wenn es einen Angelplatz gibt, aktualisieren Sie die Daten und wenn nicht,
  // fügen Sie einen neuen Angelplatz ein
  dlgKey > 0 ? fun(updateEntry(dlgKey), Cnt::EditMode::UPDATE)
             : fun(insertEntry(), Cnt::EditMode::NEW);
  // Modified Flag
  isModified = !retValue;

  return retValue;
}

bool AngelplatzDialog::querySave() {

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

bool AngelplatzDialog::entryIsValid() {

  bool retValue = true;
  // Lambda-Funktion für Nachrichten
  auto message = [&](QLineEdit *lineEdit, QString text) {
    QMessageBox::critical(this, this->windowTitle(), text);
    // Anpassen des Fokus auf ein ungültiges Feld
    lineEdit->setFocus();
    retValue = false;
  };
  // Namensfeld darf nicht leer sein
  if (ui->textName->text().isEmpty())
    message(ui->textName, tr("Eingabe fehlt"));
  // Das Typfeld darf nicht leer sein
  else if (ui->textType->text().isEmpty())
    message(ui->textType, tr("Eingabe fehlt"));
  // Der Name des Angelplatzes muss eindeutig sein
  else if (AngelplaetzeDAO::angelplatzExistsWithName(ui->textName->text(),
                                                     dlgKey))
    message(ui->textName, tr("Der Angelplatz ist bereits vorhanden"));

  return retValue;
}

bool AngelplatzDialog::updateEntry(const qint64 key) {
  // Überprüfen, ob ein Angelplatz mit diesem Primärschlüssel existiert
  if (!AngelplaetzeDAO::angelplatzExists(key))
    return false;

  QString oldName = AngelplaetzeDAO::readAngelplatzName(key);

  return AngelplaetzeDAO::updateAngelplatz(
             key, imagePath, ui->textName->text(), ui->textType->text(), fische,
             ui->textPlz->text(), ui->textOrt->text(), ui->textLand->text(),
             ui->textInfo->toPlainText()) &&
         // Update und Fischdaten mit dem Namen des Angelplatzes
         FischeDAO::updateFischeWithAngelplatz(oldName, ui->textName->text());
}

bool AngelplatzDialog::insertEntry() {

  return AngelplaetzeDAO::insertAngelplatz(
      imagePath, ui->textName->text(), ui->textType->text(), fische,
      ui->textPlz->text(), ui->textOrt->text(), ui->textLand->text(),
      ui->textInfo->toPlainText());
}

void AngelplatzDialog::importImage() {
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

void AngelplatzDialog::closeEvent(QCloseEvent *event) {
    // Prüfen vor dem Beenden, ob die eingegebenen Daten gespeichert werden sollen
    querySave() ? event->accept() : event->ignore();
}

void AngelplatzDialog::reject() { close(); }
// Bei jeder Wertänderung ist isModified = true
void AngelplatzDialog::on_btnBildHochladen_clicked() {

  setIsModified(true);

  importImage();
}

void AngelplatzDialog::on_btnAbbrechen_clicked() { close(); }

void AngelplatzDialog::on_btnSpeichern_clicked() {
  // Überprüfen, ob etwas geändert wurde und ob das Speichern erfolgreich war
  if (isModified && !saveEntry())
    return;

  close();
}

void AngelplatzDialog::on_textName_textChanged(const QString &) {

  setIsModified(true);
}

void AngelplatzDialog::on_textType_textChanged(const QString &) {

  setIsModified(true);
}

void AngelplatzDialog::on_textPlz_textChanged(const QString &) {

  setIsModified(true);
}

void AngelplatzDialog::on_textOrt_textChanged(const QString &) {

  setIsModified(true);
}

void AngelplatzDialog::on_textLand_textChanged(const QString &) {

  setIsModified(true);
}

void AngelplatzDialog::on_textInfo_textChanged() { setIsModified(true); }
// Nach Drücken der Enter-Taste wird automatisch zum nächsten Feld gewechselt
void AngelplatzDialog::on_textName_returnPressed() { this->focusNextChild(); }

void AngelplatzDialog::on_textType_returnPressed() { this->focusNextChild(); }

void AngelplatzDialog::on_textPlz_returnPressed() { this->focusNextChild(); }

void AngelplatzDialog::on_textOrt_returnPressed() { this->focusNextChild(); }

void AngelplatzDialog::on_textLand_returnPressed() { this->focusNextChild(); }
