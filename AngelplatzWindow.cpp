#include "AngelplatzWindow.h"
#include "ui_AngelplatzWindow.h"

AngelplatzWindow::AngelplatzWindow(qint64 key, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AngelplatzWindow) {
  ui->setupUi(this);

  windowKey = key;

  init();
}

AngelplatzWindow::~AngelplatzWindow() { delete ui; }

void AngelplatzWindow::init() {

  this->showMaximized();

  model = nullptr;

  proxyModel = nullptr;

  if (windowKey < 1)
    ui->actionNeu->setDisabled(true);

  // Einen Label für die ANzeige von Meldungen in der Statusbar erzeugen
  statusLabel = new QLabel(this);

  // Den Text horizontal und vertikal in der Mitte ausrichten
  statusLabel->setAlignment(Qt::AlignCenter);

  // Der 2. Parameter stretch = 1 dehnt den Label
  // über die gesamte StatusBar aus.
  statusBar()->addWidget(statusLabel, 1);

  // Den Event Filter für die tableView installieren
  ui->tableView->installEventFilter(this);

  // Ändert die Text- und Hintergrundfarbe der selektierten Zeile der TableView
  // damit die Markierung beim Fokusverlust sichtbar bleibt.
  QPalette palette = ui->tableView->palette();

  palette.setColor(QPalette::HighlightedText, Qt::white);
  palette.setColor(QPalette::Highlight, QColor(0, 112, 255));

  ui->tableView->setPalette(palette);

  ui->cbFischarten->addItems(FischeDAO::readFischarten());

  ui->cbNiederschlag->addItems(QStringList() << "Niederschlag"
                                             << "Sonnig"
                                             << "Wolkig"
                                             << "Regen"
                                             << "Schnee");

  ui->cbNacht->addItems(QStringList() << "Nacht"
                                      << "Ja"
                                      << "Nein");

  ui->cbParameter->addItems(QStringList() << "Parameter"
                                          << "Länge"
                                          << "Gewicht"
                                          << "Zeit"
                                          << "Temperatur"
                                          << "Windgeschwindigkeit"
                                          << "Luftdruck");

  showParameterFilter(false, false, false);

  if (windowKey > 0)
    readEntry(windowKey);

  showTable();
}

void AngelplatzWindow::setTableViewModel() {
  // Evtl. vorhandenes QSqlTableModel löschen
  delete ui->tableView->model();

  model = FischeDAO::readFischeIntoTableModel();

  proxyModel = new SortFilterProxyModel(this);

  // Spaltenüberschriften der Tabelle setzen
  model->setHeaderData(model->record().indexOf("PATH"), Qt::Horizontal, "");
  model->setHeaderData(model->record().indexOf("NAME"), Qt::Horizontal, "Name");
  model->setHeaderData(model->record().indexOf("ANGELPLATZ"), Qt::Horizontal,
                       "Angelplatz");
  model->setHeaderData(model->record().indexOf("LAENGE"), Qt::Horizontal,
                       "Länge\n[ cm ]");
  model->setHeaderData(model->record().indexOf("GEWICHT"), Qt::Horizontal,
                       "Gewicht\n[ g ]");
  model->setHeaderData(model->record().indexOf("ZEIT"), Qt::Horizontal,
                       "Zeit\ndd/MM/yyyy HH:mm");
  model->setHeaderData(model->record().indexOf("TEMPERATUR"), Qt::Horizontal,
                       "Temperatur\n[ °C ]");
  model->setHeaderData(model->record().indexOf("WINDGESCHWINDIGKEIT"),
                       Qt::Horizontal, "Windgeschwindigkeit\n[ km/h ]");
  model->setHeaderData(model->record().indexOf("LUFTDRUCK"), Qt::Horizontal,
                       "Luftdruck\n[ hPa ]");
  model->setHeaderData(model->record().indexOf("IS_NACHT"), Qt::Horizontal,
                       "Nacht");
  model->setHeaderData(model->record().indexOf("NIEDERSCHLAG"), Qt::Horizontal,
                       "Niederschlag");
  model->setHeaderData(model->record().indexOf("INFO"), Qt::Horizontal,
                       "Zusätzliche Information");

  // Aufsteigende Sortierung nach Postleitzahl
  model->sort(model->record().indexOf("NAME"), Qt::AscendingOrder);

  ImageStyleItemDelegate *delegate = new ImageStyleItemDelegate(this);

  ui->tableView->setItemDelegateForColumn(model->record().indexOf("PATH"),
                                          delegate);

  proxyModel->setAngelplatz(windowKey);
  proxyModel->setAngelplatzColumn(model->record().indexOf("ANGELPLATZ"));

  if (!filterFischarten.isEmpty()) {
    proxyModel->setName(filterFischarten);
    proxyModel->setNameColumn(model->record().indexOf("NAME"));
  }

  if (!filterNiederschlag.isEmpty()) {
    proxyModel->setNiederschlag(filterNiederschlag);
    proxyModel->setNiederschlagColumn(model->record().indexOf("NIEDERSCHLAG"));
  }

  if (!filterNacht.isEmpty()) {
    proxyModel->setNacht(filterNacht);
    proxyModel->setNachtColumn(model->record().indexOf("IS_NACHT"));
  }

  if (!filterParameter.isEmpty()) {

    proxyModel->setParameter(filterParameter);

    if (filterParameter == "Länge") {
      proxyModel->setParameterMin(ui->sbMin->value());
      proxyModel->setParameterMax(ui->sbMax->value());
      proxyModel->setParameterColumn(model->record().indexOf("LAENGE"));
    } else if (filterParameter == "Gewicht") {
      proxyModel->setParameterMin(ui->sbMin->value());
      proxyModel->setParameterMax(ui->sbMax->value());
      proxyModel->setParameterColumn(model->record().indexOf("GEWICHT"));
    } else if (filterParameter == "Zeit") {
      proxyModel->setZeitMin(ui->dateTimeMin->dateTime());
      proxyModel->setZeitMax(ui->dateTimeMax->dateTime());
      proxyModel->setParameterColumn(model->record().indexOf("ZEIT"));
    } else if (filterParameter == "Temperatur") {
      proxyModel->setParameterMin(ui->sbMin->value());
      proxyModel->setParameterMax(ui->sbMax->value());
      proxyModel->setParameterColumn(model->record().indexOf("TEMPERATUR"));
    } else if (filterParameter == "Windgeschwindigkeit") {
      proxyModel->setParameterMin(ui->sbMin->value());
      proxyModel->setParameterMax(ui->sbMax->value());
      proxyModel->setParameterColumn(
          model->record().indexOf("WINDGESCHWINDIGKEIT"));
    } else if (filterParameter == "Luftdruck") {
      proxyModel->setParameterMin(ui->sbMin->value());
      proxyModel->setParameterMax(ui->sbMax->value());
      proxyModel->setParameterColumn(model->record().indexOf("LUFTDRUCK"));
    }
  }

  // Das Datenmodel zur tableView zuweisen
  ui->tableView->setModel(proxyModel);

  proxyModel->setSourceModel(model);

  // Die TableView liest initial nur die ersten 256 Datensätze.
  // Das ist im Datenmodell festgelegt.

  // Wenn aber von Anfang an alle Datensätze benötigt werden, kann
  // man mit folgender Anweisung die vollständige Anzahl der Datensätze
  // im Datenmodell ermitteln
  while (model->canFetchMore())
    model->fetchMore();

  // Signal für die Anzeige des aktuellen Datensatzes in der Statusbar
  connect(ui->tableView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &AngelplatzWindow::tableView_selectionChanged);

  // Signal für die Änderung der Spaltenbreite in der TableView
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionResized, this,
          &AngelplatzWindow::tableView_section_resized);

  //  return model;
}

void AngelplatzWindow::showTable() {

  setTableViewModel();

  // Feste Höhe für die Tabellenüberschrift
  ui->tableView->horizontalHeader()->setFixedHeight(50);

  // Alle Spaltenüberschriften linksbündig
  ui->tableView->horizontalHeader()->setDefaultAlignment(
      Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

  // Spalte PRIMARYKEY unsichtbar machen
  ui->tableView->hideColumn(model->record().indexOf("PRIMARYKEY"));
  ui->tableView->hideColumn(model->record().indexOf("ANGELPLATZ"));

  ui->tableView->setColumnWidth(model->record().indexOf("NAME"), 80);
  ui->tableView->setColumnWidth(model->record().indexOf("LAENGE"), 60);
  ui->tableView->setColumnWidth(model->record().indexOf("GEWICHT"), 70);
  ui->tableView->setColumnWidth(model->record().indexOf("ZEIT"), 140);
  ui->tableView->setColumnWidth(model->record().indexOf("TEMPERATUR"), 50);
  ui->tableView->setColumnWidth(model->record().indexOf("WINDGESCHWINDIGKEIT"),
                                70);
  ui->tableView->setColumnWidth(model->record().indexOf("LUFTDRUCK"), 80);
  ui->tableView->setColumnWidth(model->record().indexOf("IS_NACHT"), 60);
  ui->tableView->setColumnWidth(model->record().indexOf("NIEDERSCHLAG"), 100);

  // Aktivieren/Deaktivieren der Komponenten, abhängig davon,
  // ob Datensätze gelesen wurden.
  ui->tableView->setEnabled(proxyModel->rowCount());
  ui->actionNdern->setEnabled(ui->tableView->isEnabled());
  ui->actionLschen->setEnabled(ui->tableView->isEnabled());

  // Erste Zeile auswählen
  ui->tableView->isEnabled() ? ui->tableView->selectRow(0)
                             : statusLabel->setText("Der Datensatz ist leer.");
}

void AngelplatzWindow::readEntry(qint64 key) {
  Angelplatz *angelplatz = AngelplaetzeDAO::readAngelplatz(key);

  if (angelplatz == nullptr)
    return;

  ui->image->setScaledContents(false);

  ui->image->setPixmap(QPixmap::fromImage(QImage(angelplatz->getPath()))
                           .scaled(ui->image->width(), ui->image->height(),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation));

  ui->lblAngelplatzInfo->setText(angelplatz->getName());

  // Objekt plz vom Heap löschen
  delete angelplatz;
}

void AngelplatzWindow::showFischDialog(const qint64 key) {
  // Dialog auf dem Stack erstellen
  FischDialog fischDialog(windowKey, key, this);

  connect(&fischDialog, &FischDialog::dataModified, this,
          &AngelplatzWindow::modifyTableView);

  fischDialog.exec();
}

void AngelplatzWindow::deleteEntry(const QModelIndex &index) {
  // Das Datenmodell der TableView in ein QSqlTableModel konvertieren
  // FischQSqlTableModel *model = getTableViewModel();

  auto sourceIndex = proxyModel->mapToSource(index);

  // Ermitteln des Primärschlüssels in Spalte 'PRIMARYKEY' über den als
  // Parameter übergebenen QModelIndex.
  qint64 key =
      model->record(sourceIndex.row()).value("PRIMARYKEY").toLongLong();

  int msgValue = QMessageBox::question(
      this, this->windowTitle(),
      "Datensatz löschen:\n" +
          model->record(sourceIndex.row()).value("NAME").toString(),
      QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

  if (msgValue == QMessageBox::Cancel)
    return;

  qint64 angelplatzKey = windowKey;

  if (windowKey < 1) {
    Fisch *fisch = FischeDAO::readFisch(key);

    angelplatzKey = fisch->getAngelplatz();

    delete fisch;
  }

  // Löschen der Postleitzahl über den Primärschlüssel
  if (FischeDAO::deleteFisch(key) &&
      AngelplaetzeDAO::changeNumberFische(angelplatzKey, -1)) {

    emit dataModified(angelplatzKey);

    statusLabel->setText("Einträge werden aktualisiert...");
    QApplication::processEvents();

    // Postleitzahlen neu in das Datenmodell einlesen
    showTable();

    int row = (index.row() - 1 < 0) ? 0 : index.row() - 1;

    // Prüfen, ob die ermittelte Zeile selektiert werden kann
    if (ui->tableView->model()->rowCount() >= row)
      ui->tableView->selectRow(row);

    if (ui->tableView->model()->rowCount() == 0)
      statusLabel->setText("Der Datensatz ist leer.");
  }
}

void AngelplatzWindow::refreshTableView(const qint64 key) {

  statusLabel->setText("Einträge werden aktualisiert...");
  QApplication::processEvents();

  AngelplaetzeDAO::changeNumberFische(windowKey, 1);

  emit dataModified(windowKey);

  ui->cbFischarten->clear();
  ui->cbFischarten->addItems(FischeDAO::readFischarten());

  // Postleitzahlen neu in das Datenmodell einlesen
  showTable();

  // Den Cursor auf den neuen Eintrag über den Primärschlüssel positionieren
  findItemInTableView("PRIMARYKEY", QVariant(key));
}

void AngelplatzWindow::findItemInTableView(const QString &columnName,
                                           const QVariant &value) {
  int row;
  bool found = false;

  // Konvertieren des Datenmodells der tableView in ein SqlTableModel
  // FischQSqlTableModel *model = getTableViewModel();

  // Auf die Query/Datensätze innerhalb des Datenmodells zugreifen
  QSqlQuery query = model->query();

  // Spaltenindex des Spaltennamens über den Datensatz des Datenmodells
  // ermitteln und damit prüfen, ob der Spaltenname auch exisitiert. Nur diese
  // Spalte wird nach dem angegebenen Wert durchsucht.

  int colIndex = model->record().indexOf(columnName);

  if (colIndex < 0)
    return;

  // Auf den ersten Eintrag in der Query positionieren
  query.first();

  // Die Zeile auf den Anfangswert der Suche setzen
  row = query.at();

  do {
    // Der Suchbegriff kann irgendwo innerhalb der Spalte enthalten sein.
    // Groß-/Kleinschreibung wird nicht berücksichtigt.
    if (query.value(colIndex).toString().contains(value.toString(),
                                                  Qt::CaseInsensitive)) {
      found = true;
      break;
    }

    row++;
  } while (query.next());

  ui->tableView->selectRow(found ? row : 0);
}

void AngelplatzWindow::updateTableView(const qint64 key) {

  Fisch *fisch = FischeDAO::readFisch(key);

  if (fisch == nullptr)
    return;

  QModelIndex currentIndex = ui->tableView->currentIndex();

  auto sourceIndex = proxyModel->mapToSource(currentIndex);

  QModelIndex index;

  auto changeData = [&](QString column, auto data) {
    // Einen Index erstellen, der auf die Spalte 'PLZ'
    index = model->index(sourceIndex.row(), model->record().indexOf(column));
    // Postleitzahl im Datenmodell über den Index ändern
    model->setData(index, QVariant(data), Qt::EditRole);
    // Die TableView informieren, dass die Postleitzahl geändert wurde und
    // aktualisiert werden muss
    emit model->dataChanged(index, index);
  };

  changeData("PATH", fisch->getPath());
  changeData("NAME", fisch->getName());
  changeData("ANGELPLATZ", fisch->getAngelplatz());
  changeData("LAENGE", fisch->getLaenge());
  changeData("GEWICHT", fisch->getGewicht());
  changeData("ZEIT", fisch->getZeit());
  changeData("TEMPERATUR", fisch->getTemperatur());
  changeData("WINDGESCHWINDIGKEIT", fisch->getWindgeschwindigkeit());
  changeData("LUFTDRUCK", fisch->getLuftdruck());
  changeData("IS_NACHT", fisch->getIsNacht());
  changeData("NIEDERSCHLAG", fisch->getNiederschlag());
  changeData("INFO", fisch->getInfo());

  // Objekt angelplatz vom Heap löschen
  delete fisch;
}

void AngelplatzWindow::showParameterFilter(bool spinBox, bool dateTime,
                                           bool text) {
  ui->sbMin->setVisible(spinBox);
  ui->sbMax->setVisible(spinBox);
  ui->dateTimeMin->setVisible(dateTime);
  ui->dateTimeMax->setVisible(dateTime);
  ui->lblMin->setText(text ? "Min" : "");
  ui->lblMax->setText(text ? "Max" : "");
}

void AngelplatzWindow::modifyTableView(const qint64 key,
                                       const FischDialog::EditMode editMode) {
  switch (editMode) {
  case FischDialog::EditMode::NEW:
    refreshTableView(key);
    break;

  case FischDialog::EditMode::UPDATE:
    updateTableView(key);
    break;
  }
}

void AngelplatzWindow::tableView_section_resized(int index, int, int newSize) {
  // Nur auf die Änderung der 1 Spalte (Bild) reagieren
  if (index == 1) {
    // qDebug() << "newSize: " << newSize;
    ui->tableView->verticalHeader()->setMinimumSectionSize(newSize);
    ui->tableView->verticalHeader()->setMaximumSectionSize(newSize);
  }
}

void AngelplatzWindow::tableView_selectionChanged() {

  int currentRow = ui->tableView->selectionModel()->currentIndex().row() + 1;
  int rowCount = ui->tableView->model()->rowCount();

  QString msg = QString("Datensatz %L1 von %L2").arg(currentRow).arg(rowCount);

  statusLabel->setText(msg);
}

bool AngelplatzWindow::eventFilter(QObject *sender, QEvent *event) {

  if (sender == ui->tableView) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

      // Home-Taste (Pos1)
      if (keyEvent->key() == Qt::Key_Home) {
        ui->tableView->scrollToTop();
        ui->tableView->selectRow(0);
      }
      // End-Taste (Ende)
      else if (keyEvent->key() == Qt::Key_End) {
        ui->tableView->scrollToBottom();
        ui->tableView->selectRow(ui->tableView->model()->rowCount() - 1);
      }
      // Return-Taste zur Auswahl eines Eintrags
      else if (keyEvent->key() == Qt::Key_Return) {
        on_tableView_doubleClicked(ui->tableView->currentIndex());
      }
      // Entf-Taste zum Löschen eines Eintrags
      else if (keyEvent->key() == Qt::Key_Delete) {
        deleteEntry(ui->tableView->currentIndex());
      }
    }
  }

  return QObject::eventFilter(sender, event);
}

void AngelplatzWindow::on_actionBEenden_triggered() { close(); }

void AngelplatzWindow::on_actionNeu_triggered() { showFischDialog(0); }

void AngelplatzWindow::on_actionNdern_triggered() {

  showFischDialog(
      model
          ->record(proxyModel->mapToSource(ui->tableView->currentIndex()).row())
          .value("PRIMARYKEY")
          .toLongLong());
}

void AngelplatzWindow::on_tableView_doubleClicked(const QModelIndex &index) {

  showFischDialog(model->record(proxyModel->mapToSource(index).row())
                      .value("PRIMARYKEY")
                      .toLongLong());
}

void AngelplatzWindow::on_actionLschen_triggered() {
  deleteEntry(ui->tableView->currentIndex());
}

void AngelplatzWindow::on_cbFischarten_currentTextChanged(const QString &text) {

  filterFischarten = QString::compare(text, "Fischarten") ? text : QString();

  showTable();
}

void AngelplatzWindow::on_cbNiederschlag_currentTextChanged(
    const QString &text) {

  filterNiederschlag =
      QString::compare(text, "Niederschlag") ? text : QString();

  showTable();
}

void AngelplatzWindow::on_cbNacht_currentTextChanged(const QString &text) {

  filterNacht = QString::compare(text, "Nacht") ? text : QString();

  showTable();
}

void AngelplatzWindow::on_cbParameter_currentTextChanged(const QString &text) {

  if (text == "Parameter") {

    showParameterFilter(false, false, false);

    filterParameter = QString();
  } else {

    auto setMinMax = [&](int min, int max) {
      ui->sbMin->setRange(min, max);
      ui->sbMin->setValue(min);
      ui->sbMax->setRange(min, max);
      ui->sbMax->setValue(max);
    };

    if (text == "Länge") {

      setMinMax(FischeDAO::getMinParameter("LAENGE").toInt(),
                FischeDAO::getMaxParameter("LAENGE").toInt());

    } else if (text == "Gewicht") {

      setMinMax(FischeDAO::getMinParameter("GEWICHT").toInt(),
                FischeDAO::getMaxParameter("GEWICHT").toInt());

    } else if (text == "Zeit") {

      QDateTime min = FischeDAO::getMinParameter("ZEIT").toDateTime();
      QDateTime max = FischeDAO::getMaxParameter("ZEIT").toDateTime();

      ui->dateTimeMin->setDateTimeRange(min, max);
      ui->dateTimeMin->setDateTime(min);
      ui->dateTimeMax->setDateTimeRange(min, max);
      ui->dateTimeMax->setDateTime(max);

    } else if (text == "Temperatur") {

      setMinMax(FischeDAO::getMinParameter("TEMPERATUR").toInt(),
                FischeDAO::getMaxParameter("TEMPERATUR").toInt());

    } else if (text == "Windgeschwindigkeit") {

      setMinMax(FischeDAO::getMinParameter("WINDGESCHWINDIGKEIT").toInt(),
                FischeDAO::getMaxParameter("WINDGESCHWINDIGKEIT").toInt());

    } else if (text == "Luftdruck") {

      setMinMax(FischeDAO::getMinParameter("LUFTDRUCK").toInt(),
                FischeDAO::getMaxParameter("LUFTDRUCK").toInt());
    }

    text == "Zeit" ? showParameterFilter(false, true, true)
                   : showParameterFilter(true, false, true);

    filterParameter = text;
  }

  showTable();
}

void AngelplatzWindow::on_sbMin_valueChanged(int) { showTable(); }

void AngelplatzWindow::on_dateTimeMin_dateTimeChanged(const QDateTime &) {
  showTable();
}

void AngelplatzWindow::on_sbMax_valueChanged(int) { showTable(); }

void AngelplatzWindow::on_dateTimeMax_dateTimeChanged(const QDateTime &) {
  showTable();
}
