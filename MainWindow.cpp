#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  init();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::init() {

  this->showMaximized();

  angelplatzDialog = nullptr;

  angelplatzWindow = nullptr;

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

  // Datenbank öffnen
  openDatabase();

  // Anzeige aller Datensätze aus der Tabelle der Postleitzahlen
  showTable();
}

bool MainWindow::openDatabase() {
  QString driver = "QODBC";

  QString driverName = "DRIVER={SQL Server}";

  QString server = "localhost\\SQLEXPRESS";

  QString databaseName = "alfatraining";

  bool retValue =
      DAOLib::connectToDatabase(driver, driverName, server, databaseName);

  return retValue;
}

QSqlTableModel *MainWindow::setTableViewModel() {
  // Evtl. vorhandenes QSqlTableModel löschen
  delete ui->tableView->model();

  QSqlTableModel *model = AngelplaetzeDAO::readAngelplaetzeIntoTableModel();

  // Spaltenüberschriften der Tabelle setzen
  model->setHeaderData(model->record().indexOf("PATH"), Qt::Horizontal, "");
  model->setHeaderData(model->record().indexOf("NAME"), Qt::Horizontal, "Name");
  model->setHeaderData(model->record().indexOf("TYPE"), Qt::Horizontal, "Type");
  model->setHeaderData(model->record().indexOf("FISCHE"), Qt::Horizontal, "Fische");
  model->setHeaderData(model->record().indexOf("PLZ"), Qt::Horizontal, "PLZ");
  model->setHeaderData(model->record().indexOf("ORT"), Qt::Horizontal, "Ort");
  model->setHeaderData(model->record().indexOf("LAND"), Qt::Horizontal, "Land");
  model->setHeaderData(model->record().indexOf("INFO"), Qt::Horizontal,
                       "Zusätzliche Information");

  // Aufsteigende Sortierung nach Postleitzahl
  model->sort(model->record().indexOf("NAME"), Qt::AscendingOrder);

  ImageStyleItemDelegate *delegate = new ImageStyleItemDelegate(this);

  ui->tableView->setItemDelegateForColumn(model->record().indexOf("PATH"),
                                          delegate);

  // Das Datenmodel zur tableView zuweisen
  ui->tableView->setModel(model);

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
          &MainWindow::tableView_selectionChanged);

  // Signal für die Änderung der Spaltenbreite in der TableView
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionResized, this,
          &MainWindow::tableView_section_resized);

  return model;
}

void MainWindow::showTable() {

  QSqlTableModel *model = setTableViewModel();

  // Feste Höhe für die Tabellenüberschrift
  ui->tableView->horizontalHeader()->setFixedHeight(50);

  // Alle Spaltenüberschriften linksbündig
  ui->tableView->horizontalHeader()->setDefaultAlignment(
      Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

  // Spalte PRIMARYKEY unsichtbar machen
  ui->tableView->hideColumn(model->record().indexOf("PRIMARYKEY"));

  // Aktivieren/Deaktivieren der Komponenten, abhängig davon,
  // ob Datensätze gelesen wurden.

  ui->tableView->setEnabled(model->rowCount());
  ui->actionNdern->setEnabled(ui->tableView->isEnabled());
  ui->actionLschen->setEnabled(ui->tableView->isEnabled());
  ui->actionMarkierterAngelplatz->setEnabled(ui->tableView->isEnabled());
  ui->actionAlleAngelpltze->setEnabled(ui->tableView->isEnabled());

  // Erste Zeile auswählen
  ui->tableView->isEnabled() ? ui->tableView->selectRow(0)
                             : statusLabel->setText("Der Datensatz ist leer.");
}

void MainWindow::showAngelplatzDialog(const qint64 key) {
  // Dialog auf dem Stack erstellen
  AngelplatzDialog angelplatzDlg(key, this);

  // Die Signale für UPDATE und NEW mit einem Slot verbinden
  connect(&angelplatzDlg, &AngelplatzDialog::dataModified, this,
          &MainWindow::modifyTableView);

  angelplatzDlg.exec();
}

void MainWindow::showAngelplatzWindow(const qint64 key) {
  delete angelplatzWindow;

  angelplatzWindow = new AngelplatzWindow(key, this);

  connect(angelplatzWindow, &AngelplatzWindow::dataModified, this,
          &MainWindow::refreshTableView);

  angelplatzWindow->show();
}

void MainWindow::deleteEntry(const QModelIndex &index) {
  // Das Datenmodell der TableView in ein QSqlTableModel konvertieren
  QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->tableView->model());

  // Ermitteln des Primärschlüssels in Spalte 'PRIMARYKEY' über den als
  // Parameter übergebenen QModelIndex.
  qint64 key = model->record(index.row()).value("PRIMARYKEY").toLongLong();

  int msgValue = QMessageBox::question(
      this, this->windowTitle(),
      "Angelplatz löschen: " +
          model->record(index.row()).value("NAME").toString() +
          "\nAnzahl der zu löschenden Fische: " +
          QString::number(FischeDAO::countFischeInAngelplatz(key)),
      QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

  if (msgValue == QMessageBox::Cancel)
    return;

  // Löschen der Postleitzahl über den Primärschlüssel
  if ((FischeDAO::countFischeInAngelplatz(key) > 0
           ? FischeDAO::deleteFischeInAngelplatz(key)
           : true) &&
      AngelplaetzeDAO::deleteAngelplatz(key)) {

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

void MainWindow::refreshTableView(const qint64 key) {

  statusLabel->setText("Einträge werden aktualisiert...");
  QApplication::processEvents();

  // Postleitzahlen neu in das Datenmodell einlesen
  showTable();

  // Den Cursor auf den neuen Eintrag über den Primärschlüssel positionieren
  findItemInTableView("PRIMARYKEY", QVariant(key));
}

void MainWindow::findItemInTableView(const QString &columnName,
                                     const QVariant &value) {
  int row;
  bool found = false;

  // Konvertieren des Datenmodells der tableView in ein SqlTableModel
  QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->tableView->model());

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
    if (query.value(colIndex).toLongLong() == value.toLongLong()) {
      found = true;
      break;
    }

    row++;
  } while (query.next());

  ui->tableView->selectRow(found ? row : 0);
}

void MainWindow::updateTableView(const qint64 key) {

  Angelplatz *angelplatz = AngelplaetzeDAO::readAngelplatz(key);

  if (angelplatz == nullptr)
    return;

  QModelIndex currentIndex = ui->tableView->currentIndex();

  // Das Datenmodell der TableView in ein QSqlTableModel konvertieren
  QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->tableView->model());

  QModelIndex index;

  auto changeData = [&](QString column, auto data) {
    // Einen Index erstellen, der auf die Spalte 'PLZ'
    index = model->index(currentIndex.row(), model->record().indexOf(column));
    // Postleitzahl im Datenmodell über den Index ändern
    model->setData(index, QVariant(data), Qt::EditRole);
    // Die TableView informieren, dass die Postleitzahl geändert wurde und
    // aktualisiert werden muss
    emit model->dataChanged(index, index);
  };

  changeData("PATH", angelplatz->getPath());
  changeData("NAME", angelplatz->getName());
  changeData("TYPE", angelplatz->getType());
  changeData("Fische", angelplatz->getFische());
  changeData("PLZ", angelplatz->getPlz());
  changeData("ORT", angelplatz->getOrt());
  changeData("LAND", angelplatz->getLand());
  changeData("INFO", angelplatz->getInfo());

  // Objekt angelplatz vom Heap löschen
  delete angelplatz;
}

void MainWindow::modifyTableView(const qint64 key,
                                 const AngelplatzDialog::EditMode editMode) {
  switch (editMode) {
  case AngelplatzDialog::EditMode::NEW:
    refreshTableView(key);
    break;

  case AngelplatzDialog::EditMode::UPDATE:
    updateTableView(key);
    break;
  }
}

void MainWindow::tableView_selectionChanged() {

  int currentRow = ui->tableView->selectionModel()->currentIndex().row() + 1;
  int rowCount = ui->tableView->model()->rowCount();

  QString msg = QString("Datensatz %L1 von %L2").arg(currentRow).arg(rowCount);

  statusLabel->setText(msg);
}

void MainWindow::tableView_section_resized(int index, int, int newSize) {
  // Nur auf die Änderung der 1 Spalte (Bild) reagieren
  if (index == 1) {
    // qDebug() << "newSize: " << newSize;
    ui->tableView->verticalHeader()->setMinimumSectionSize(newSize);
    ui->tableView->verticalHeader()->setMaximumSectionSize(newSize);
  }
}

bool MainWindow::eventFilter(QObject *sender, QEvent *event) {

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

void MainWindow::on_actionBEenden_triggered() { close(); }

void MainWindow::on_actionNeu_triggered() { showAngelplatzDialog(0); }

void MainWindow::on_actionLschen_triggered() {
  deleteEntry(ui->tableView->currentIndex());
}

void MainWindow::on_actionNdern_triggered() {
  // Das Datenmodell der TableView in ein QSqlTableModel konvertieren
  QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->tableView->model());

  // Den Index der selektierten Zeile ermitteln
  QModelIndex index = ui->tableView->currentIndex();

  showAngelplatzDialog(
      model->record(index.row()).value("PRIMARYKEY").toLongLong());
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index) {
  // Das Datenmodell der TableView in ein QSqlTableModel konvertieren
  QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->tableView->model());

  // Über den als Parameter übergebenen QModelIndex der selektierten Zeile
  // den Wert der Spalte PRIMARYKEY ermitten und als Parameter an die
  // Methode showPLZDialog() übergeben.

  showAngelplatzWindow(
      model->record(index.row()).value("PRIMARYKEY").toLongLong());
}

void MainWindow::on_actionMarkierterAngelplatz_triggered() {
  // Das Datenmodell der TableView in ein QSqlTableModel konvertieren
  QSqlTableModel *model = static_cast<QSqlTableModel *>(ui->tableView->model());

  // Über den als Parameter übergebenen QModelIndex der selektierten Zeile
  // den Wert der Spalte PRIMARYKEY ermitten und als Parameter an die
  // Methode showPLZDialog() übergeben.

  showAngelplatzWindow(model->record(ui->tableView->currentIndex().row())
                           .value("PRIMARYKEY")
                           .toLongLong());
}

void MainWindow::on_actionAlleAngelpltze_triggered() {
  showAngelplatzWindow(-1);
}

void MainWindow::closeEvent(QCloseEvent *event) {

  DAOLib::closeConnection();
  event->accept();
}
