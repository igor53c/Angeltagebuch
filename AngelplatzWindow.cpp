#include "AngelplatzWindow.h"
#include "ui_AngelplatzWindow.h"

AngelplatzWindow::AngelplatzWindow(QList<int> &colAngelplatzWidthList,
                                   QString &angelplatzName, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AngelplatzWindow) {

  ui->setupUi(this);

  this->angelplatzName = angelplatzName;

  this->colAngelplatzWidthList = colAngelplatzWidthList;

  init();
}

AngelplatzWindow::~AngelplatzWindow() { delete ui; }

void AngelplatzWindow::init() {
  // Fenster maximieren
  this->showMaximized();

  model = nullptr;
  proxyModel = nullptr;
  // Einen Label für die Anzeige von Meldungen in der Statusbar erzeugen
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
  palette.setColor(QPalette::Highlight, Cnt::COLOR_HIGHLIGHT);
  // Stellen die Farbe des leeren Teils der Tabelle ein
  palette.setColor(QPalette::Base, DAOLib::colorBackground());
  ui->tableView->setPalette(palette);
  // Stellen den Stil der Tabelle ein
  ui->tableView->setStyleSheet("QHeaderView::section{"
                               "border-top:0px solid #D8D8D8;"
                               "border-left:0px solid #D8D8D8;"
                               "border-right:1px solid #D8D8D8;"
                               "border-bottom: 1px solid #D8D8D8;"
                               "padding:4px;"
                               "font: bold 14px;"
                               "background-color: " +
                               DAOLib::colorBackground().name() +
                               ";}"
                               "QTableView{ border : 1px solid #D8D8D8; }");
  // Hintergrundfarbe ändern für das Fenster
  palette = this->palette();
  palette.setColor(QPalette::Window, DAOLib::colorBackground());
  this->setPalette(palette);

  auto changeBacgroundColor = [&](QMenu *menu) {
    palette = menu->palette();
    palette.setColor(menu->backgroundRole(), DAOLib::colorBackground());
    menu->setPalette(palette);
  };
  // Hintergrundfarbe ändern für Menu
  changeBacgroundColor(ui->menuBearbeiten);
  changeBacgroundColor(ui->menuDatei);
  // Initialisierung globaler Variablen
  listNacht = QStringList() << tr("Ja") << tr("Nein");

  listNiederschlag = QStringList() << tr("Sonnig") << tr("Wolkig")
                                   << tr("Regen") << tr("Schnee");
  // Combobox-Filtern Werte zuweisen
  ui->cbFischarten->addItems(QStringList() << tr("Fischarten")
                                           << FischeDAO::readFischarten());

  ui->cbNiederschlag->addItems(QStringList()
                               << tr("Niederschlag") << listNiederschlag);

  ui->cbNacht->addItems(QStringList() << tr("Nacht") << listNacht);

  ui->cbParameter->addItems(QStringList()
                            << tr("Parameter") << tr("Länge") << tr("Gewicht")
                            << tr("Zeit") << tr("Temperatur")
                            << tr("Windgeschwindigkeit") << tr("Luftdruck"));
  // Der Parameterfilter wird zunächst nicht angezeigt
  showParameterFilter(false, false, false);
  // Wenn Fische von allen Angelplätzen angezeigt werden, ist die Möglichkeit,
  // neue Fische hinzuzufügen, nicht erlaubt
  angelplatzName.isEmpty() ? ui->actionNeu->setVisible(false)
                           // falls nur ein Angelplatz angezeigt wird, werden
                           // die Daten dieses Angelplatzes geladen
                           : readEntry(angelplatzName);

  showTable();
}

void AngelplatzWindow::setTableViewModel() {
  // Evtl. vorhandenes FischeSqlTableModel löschen
  delete model;
  // Evtl. vorhandenes SortFilterProxyModel löschen
  delete proxyModel;

  model =
      FischeDAO::readFischeIntoTableModel(listNacht, listNiederschlag, this);
  // Verwenden ein Proxymodel, um Daten zu filtern
  proxyModel = new SortFilterProxyModel(this);
  // Spaltenüberschriften der Tabelle setzen
  model->setHeaderData(model->record().indexOf(Cnt::PATH), Qt::Horizontal,
                       QString());
  model->setHeaderData(model->record().indexOf(Cnt::NAME), Qt::Horizontal,
                       tr("Name"));
  model->setHeaderData(model->record().indexOf(Cnt::ANGELPLATZ), Qt::Horizontal,
                       tr("Angelplatz"));
  model->setHeaderData(model->record().indexOf(Cnt::LAENGE), Qt::Horizontal,
                       tr("Länge\n[ cm ]"));
  model->setHeaderData(model->record().indexOf(Cnt::GEWICHT), Qt::Horizontal,
                       tr("Gewicht\n[ g ]"));
  model->setHeaderData(model->record().indexOf(Cnt::ZEIT), Qt::Horizontal,
                       tr("Zeit\ndd/MM/yyyy HH:mm"));
  model->setHeaderData(model->record().indexOf(Cnt::TEMPERATUR), Qt::Horizontal,
                       tr("Temperatur\n[ °C ]"));
  model->setHeaderData(model->record().indexOf(Cnt::WINDGESCHWINDIGKEIT),
                       Qt::Horizontal, tr("Windgeschwindigkeit\n[ km/h ]"));
  model->setHeaderData(model->record().indexOf(Cnt::LUFTDRUCK), Qt::Horizontal,
                       tr("Luftdruck\n[ hPa ]"));
  model->setHeaderData(model->record().indexOf(Cnt::IS_NACHT), Qt::Horizontal,
                       tr("Nacht"));
  model->setHeaderData(model->record().indexOf(Cnt::NIEDERSCHLAG),
                       Qt::Horizontal, tr("Niederschlag"));
  model->setHeaderData(model->record().indexOf(Cnt::INFO), Qt::Horizontal,
                       tr("Zusätzliche Information"));
  // Aufsteigende Sortierung nach Name
  model->sort(model->record().indexOf(Cnt::NAME), Qt::AscendingOrder);
  // Verwenden eigenes QStyledItemDelegate, um Bilder anzuzeigen
  ImageStyleItemDelegate *delegate = new ImageStyleItemDelegate(this);
  // Bilder nur für die PATH-Spalte anzeigen
  ui->tableView->setItemDelegateForColumn(model->record().indexOf(Cnt::PATH),
                                          delegate);
  // Anfangswerte für die Tabellenfilterung zuweisen
  proxyModel->setAngelplatz(angelplatzName);
  proxyModel->setAngelplatzColumn(model->record().indexOf(Cnt::ANGELPLATZ));

  if (!filterFischarten.isEmpty()) {
    proxyModel->setName(filterFischarten);
    proxyModel->setNameColumn(model->record().indexOf(Cnt::NAME));
  }

  if (!filterNiederschlag.isEmpty()) {
    proxyModel->setNiederschlag(filterNiederschlag);
    proxyModel->setNiederschlagColumn(
        model->record().indexOf(Cnt::NIEDERSCHLAG));
  }

  if (!filterNacht.isEmpty()) {
    proxyModel->setNacht(filterNacht);
    proxyModel->setNachtColumn(model->record().indexOf(Cnt::IS_NACHT));
  }

  proxyModel->setParameter(filterParameter);
  proxyModel->setParameterMin(ui->sbMin->value());
  proxyModel->setParameterMax(ui->sbMax->value());
  proxyModel->setZeitMin(ui->dateTimeMin->dateTime());
  proxyModel->setZeitMax(ui->dateTimeMax->dateTime());
  // Passen Sie je nach Wahl des Filterparameters den Spaltenindex an
  switch (filterParameter) {
  case Cnt::Parameter::P_LAENGE:
    proxyModel->setParameterColumn(model->record().indexOf(Cnt::LAENGE));
    break;
  case Cnt::Parameter::P_GEWICHT:
    proxyModel->setParameterColumn(model->record().indexOf(Cnt::GEWICHT));
    break;
  case Cnt::Parameter::P_ZEIT:
    proxyModel->setParameterColumn(model->record().indexOf(Cnt::ZEIT));
    break;
  case Cnt::Parameter::P_TEMPERATUR:
    proxyModel->setParameterColumn(model->record().indexOf(Cnt::TEMPERATUR));
    break;
  case Cnt::Parameter::P_WINDGESCHWINDIGKEIT:
    proxyModel->setParameterColumn(
        model->record().indexOf(Cnt::WINDGESCHWINDIGKEIT));
    break;
  case Cnt::Parameter::P_LUFTDRUCK:
    proxyModel->setParameterColumn(model->record().indexOf(Cnt::LUFTDRUCK));
    break;
  }
  // Das Datenmodel zur tableView zuweisen
  ui->tableView->setModel(proxyModel);

  proxyModel->setSourceModel(model);
  // Übernahme aller Daten in das Datenmodell
  while (model->canFetchMore())
    model->fetchMore();
  // Signal für die Anzeige des aktuellen Datensatzes in der Statusbar
  connect(ui->tableView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &AngelplatzWindow::tableView_selectionChanged);
  // Signal für die Änderung der Spaltenbreite in der TableView
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionResized, this,
          &AngelplatzWindow::tableView_section_resized);
}

void AngelplatzWindow::showTable() {

  setTableViewModel();
  // Feste Höhe für die Tabellenüberschrift
  ui->tableView->horizontalHeader()->setFixedHeight(
      Cnt::ANGELPLATZ_HEADER_HEIGHT);
  // Anpassen der Spaltenbreiten der vorherigen Einstellung des Benutzers
  for (int i = 0; i < colAngelplatzWidthList.size(); i++)
    ui->tableView->setColumnWidth(i, colAngelplatzWidthList[i]);
  // Spalte PRIMARYKEY unsichtbar machen
  ui->tableView->hideColumn(model->record().indexOf(Cnt::PRIMARYKEY));
  // Aktivieren/Deaktivieren der Komponenten, abhängig davon,
  // ob Datensätze gelesen wurden.
  ui->tableView->setEnabled(proxyModel->rowCount());
  ui->actionNdern->setEnabled(ui->tableView->isEnabled());
  ui->actionLschen->setEnabled(ui->tableView->isEnabled());
  // Erste Zeile auswählen, oder eine Meldung ausgeben,
  // dass der Datensatz ist leer
  ui->tableView->isEnabled()
      ? ui->tableView->selectRow(0)
      : statusLabel->setText(tr("Der Datensatz ist leer."));
}

void AngelplatzWindow::readEntry(const QString &name) {

  QString path = AngelplaetzeDAO::readAngelplatzPath(name);

  if (!path.isEmpty()) {
    // Bildskalierung deaktivieren
    ui->image->setScaledContents(false);
    // Das Bild wird gesetzt, wenn ein Bildlink vorhanden ist
    ui->image->setPixmap(QPixmap::fromImage(QImage(path))
                             .scaled(ui->image->width(), ui->image->height(),
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
  }
  // Der Name des Angelplatzes unter dem Bild
  ui->lblAngelplatzInfo->setText(name);
}

void AngelplatzWindow::showFischDialog(const qint64 key) {
  // Dialog auf dem Stack erstellen
  FischDialog fischDialog(angelplatzName, key, this);
  // Die Signale für UPDATE und NEW mit einem Slot verbinden
  connect(&fischDialog, &FischDialog::dataModified, this,
          &AngelplatzWindow::modifyTableView);

  fischDialog.exec();
}

void AngelplatzWindow::deleteEntry(const QModelIndex &index) {
  // Quellmodellindex
  auto sourceIndex = proxyModel->mapToSource(index);
  // Ermitteln des Primärschlüssels in Spalte 'PRIMARYKEY' über den als
  // Parameter übergebenen QModelIndex.
  qint64 key =
      model->record(sourceIndex.row()).value(Cnt::PRIMARYKEY).toLongLong();

  int msgValue = QMessageBox::question(
      this, this->windowTitle(),
      tr("Datensatz löschen:\n") +
          model->record(sourceIndex.row()).value(Cnt::NAME).toString(),
      QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

  if (msgValue == QMessageBox::Cancel)
    return;
  // Falls angelplatzName leer ist, nimmt einen neuen Wert an
  QString newAngelplatzName = angelplatzName;

  if (angelplatzName.isEmpty())
    newAngelplatzName = FischeDAO::readFischAngelplatz(key);

  // Löschen den Fisch über den Primärschlüssel
  if (FischeDAO::deleteFisch(key) &&
      AngelplaetzeDAO::changeNumberFische(newAngelplatzName, -1)) {
    // Meldung, wenn sich die Daten in der Datenbank geändert haben
    emit dataModified(AngelplaetzeDAO::readAngelplatzKey(newAngelplatzName));

    statusLabel->setText(tr("Einträge werden aktualisiert..."));
    QApplication::processEvents();
    // Die Fische im Datenmodell erneut lesen
    showTable();

    int row = (index.row() - 1 < 0) ? 0 : index.row() - 1;
    // Prüfen, ob die ermittelte Zeile selektiert werden kann
    if (ui->tableView->model()->rowCount() >= row)
      ui->tableView->selectRow(row);

    if (ui->tableView->model()->rowCount() == 0)
      statusLabel->setText(tr("Der Datensatz ist leer."));
  }
}

void AngelplatzWindow::refreshTableView(const qint64 key) {

  statusLabel->setText(tr("Einträge werden aktualisiert..."));
  QApplication::processEvents();
  // Falls ein neuer Wert in die Datenbank eingefügt wird
  AngelplaetzeDAO::changeNumberFische(angelplatzName, 1);
  // Meldung, wenn sich die Daten in der Datenbank geändert haben
  emit dataModified(AngelplaetzeDAO::readAngelplatzKey(angelplatzName));
  // Combobox Fischarten aktualisieren
  ui->cbFischarten->clear();
  ui->cbFischarten->addItems(QStringList() << tr("Fischarten")
                                           << FischeDAO::readFischarten());
  // Die Fische im Datenmodell erneut lesen
  showTable();
  // Den Cursor auf den neuen Eintrag über den Primärschlüssel positionieren
  findItemInTableView(Cnt::PRIMARYKEY, QVariant(key));
}

void AngelplatzWindow::findItemInTableView(const QString &columnName,
                                           const QVariant &value) {
  int row;
  bool found = false;
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
    if (query.value(colIndex).toLongLong() == value.toLongLong()) {
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
    // Erstellen einen Index, der auf eine bestimmte Spalte zeigt
    index = model->index(sourceIndex.row(), model->record().indexOf(column));
    // Die Variable Column im Datenmodell über den Index ändern
    model->setData(index, QVariant(data), Qt::EditRole);
    // Die TableView informieren, dass die Werte geändert wurde und
    // aktualisiert werden muss
    emit model->dataChanged(index, index);
  };

  changeData(Cnt::PATH, fisch->getPath());
  changeData(Cnt::NAME, fisch->getName());
  changeData(Cnt::ANGELPLATZ, fisch->getAngelplatz());
  changeData(Cnt::LAENGE, fisch->getLaenge());
  changeData(Cnt::GEWICHT, fisch->getGewicht());
  changeData(Cnt::ZEIT, fisch->getZeit());
  changeData(Cnt::TEMPERATUR, fisch->getTemperatur());
  changeData(Cnt::WINDGESCHWINDIGKEIT, fisch->getWindgeschwindigkeit());
  changeData(Cnt::LUFTDRUCK, fisch->getLuftdruck());
  changeData(Cnt::IS_NACHT, fisch->getIsNacht());
  changeData(Cnt::NIEDERSCHLAG, fisch->getNiederschlag());
  changeData(Cnt::INFO, fisch->getInfo());
  // Objekt fisch vom Heap löschen
  delete fisch;
}

void AngelplatzWindow::showParameterFilter(const bool spinBox,
                                           const bool dateTime,
                                           const bool text) {
  ui->sbMin->setVisible(spinBox);
  ui->sbMax->setVisible(spinBox);
  ui->dateTimeMin->setVisible(dateTime);
  ui->dateTimeMax->setVisible(dateTime);
  ui->lblMin->setText(text ? "Min" : QString());
  ui->lblMax->setText(text ? "Max" : QString());
}

void AngelplatzWindow::modifyTableView(const qint64 key,
                                       const Cnt::EditMode editMode) {
  switch (editMode) {
  case Cnt::EditMode::NEW:
    refreshTableView(key);
    break;

  case Cnt::EditMode::UPDATE:
    updateTableView(key);
    break;
  }
}

void AngelplatzWindow::tableView_section_resized(int index, int, int newSize) {
  // Nur auf die Änderung der PATH Spalte (Bild) reagieren
  if (index == model->record().indexOf(Cnt::PATH))
    ui->tableView->verticalHeader()->setDefaultSectionSize(newSize);
  // Wertänderung in der globalen Variablen
  if (colAngelplatzWidthList.size() > index)
    colAngelplatzWidthList[index] = newSize;
  // Meldung, dass sich die Daten der Spaltenbreite geändert haben
  emit columnWidthModified(colAngelplatzWidthList);
}

void AngelplatzWindow::tableView_selectionChanged() {
  // Anzeigen, welcher Datensatz ausgewählt ist
  statusLabel->setText(
      QString(tr("Datensatz %L1 von %L2"))
          .arg(ui->tableView->selectionModel()->currentIndex().row() + 1)
          .arg(ui->tableView->model()->rowCount()));
}

bool AngelplatzWindow::eventFilter(QObject *sender, QEvent *event) {

  if (sender == ui->tableView) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

      switch (keyEvent->key()) {
        // Home-Taste (Pos1)
      case Qt::Key_Home:
        ui->tableView->scrollToTop();
        ui->tableView->selectRow(0);
        break;
        // End-Taste (Ende)
      case Qt::Key_End:
        ui->tableView->scrollToBottom();
        ui->tableView->selectRow(ui->tableView->model()->rowCount() - 1);
        break;
        // Return-Taste zur Auswahl eines Eintrags
      case Qt::Key_Return:
        on_tableView_doubleClicked(ui->tableView->currentIndex());
        break;
        // Entf-Taste zum Löschen eines Eintrags
      case Qt::Key_Delete:
        deleteEntry(ui->tableView->currentIndex());
        break;
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
          .value(Cnt::PRIMARYKEY)
          .toLongLong());
}

void AngelplatzWindow::on_tableView_doubleClicked(const QModelIndex &index) {

  showFischDialog(model->record(proxyModel->mapToSource(index).row())
                      .value(Cnt::PRIMARYKEY)
                      .toLongLong());
}

void AngelplatzWindow::on_actionLschen_triggered() {
  deleteEntry(ui->tableView->currentIndex());
}

void AngelplatzWindow::on_cbFischarten_currentTextChanged(const QString &text) {

  filterFischarten =
      QString::compare(text, tr("Fischarten")) ? text : QString();

  showTable();
}

void AngelplatzWindow::on_cbNiederschlag_currentTextChanged(
    const QString &text) {

  filterNiederschlag =
      QString::compare(text, tr("Niederschlag")) ? text : QString();

  showTable();
}

void AngelplatzWindow::on_cbNacht_currentTextChanged(const QString &text) {

  filterNacht = QString::compare(text, tr("Nacht")) ? text : QString();

  showTable();
}

void AngelplatzWindow::on_cbParameter_currentIndexChanged(int index) {
  // Festlegen des SpinBox-Werts nach Auswahl eines anderen Parametertyps
  auto setMinMax = [&](const QString &col) {
    int min = FischeDAO::getMinParameter(col, angelplatzName).toInt();
    int max = FischeDAO::getMaxParameter(col, angelplatzName).toInt();

    ui->sbMin->setRange(min, max);
    ui->sbMin->setValue(min);
    ui->sbMax->setRange(min, max);
    ui->sbMax->setValue(max);
  };
  // Mindest- und Höchstwerte einstellen
  switch (index) {
  case Cnt::Parameter::P_LAENGE:
    setMinMax(Cnt::LAENGE);
    break;
  case Cnt::Parameter::P_GEWICHT:
    setMinMax(Cnt::GEWICHT);
    break;
  case Cnt::Parameter::P_ZEIT: {
    QDateTime min =
        FischeDAO::getMinParameter(Cnt::ZEIT, angelplatzName).toDateTime();
    QDateTime max =
        FischeDAO::getMaxParameter(Cnt::ZEIT, angelplatzName).toDateTime();

    ui->dateTimeMin->setDateTimeRange(min, max);
    ui->dateTimeMin->setDateTime(min);
    ui->dateTimeMax->setDateTimeRange(min, max);
    ui->dateTimeMax->setDateTime(max);
  } break;
  case Cnt::Parameter::P_TEMPERATUR:
    setMinMax(Cnt::TEMPERATUR);
    break;
  case Cnt::Parameter::P_WINDGESCHWINDIGKEIT:
    setMinMax(Cnt::WINDGESCHWINDIGKEIT);
    break;
  case Cnt::Parameter::P_LUFTDRUCK:
    setMinMax(Cnt::LUFTDRUCK);
    break;
  }
  // Je nach Wahl der ComboBox, was angezeigt werden soll
  index == 0   ? showParameterFilter(false, false, false)
  : index == 3 ? showParameterFilter(false, true, true)
               : showParameterFilter(true, false, true);
  // Welcher Parametertyp zum Filtern ausgewählt wird
  filterParameter = index;

  showTable();
}
// Aktualisieren Sie die Tabellenanzeige nach jeder Wertänderung
void AngelplatzWindow::on_sbMin_valueChanged(int) { showTable(); }

void AngelplatzWindow::on_dateTimeMin_dateTimeChanged(const QDateTime &) {
  showTable();
}

void AngelplatzWindow::on_sbMax_valueChanged(int) { showTable(); }

void AngelplatzWindow::on_dateTimeMax_dateTimeChanged(const QDateTime &) {
  showTable();
}
