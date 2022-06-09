#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);

  init();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::init() {
  // Fenster maximieren
  this->showMaximized();

  angelplatzDialog = nullptr;
  angelplatzWindow = nullptr;
  model = nullptr;
  proxyModel = nullptr;
  // Abbruch-Button zum Abbrechen der Suche
  cancelButton = new QPushButton(this);
  cancelButton->setFixedSize(QSize(30, 30));
  cancelButton->setIcon(QIcon(":/beenden.png"));
  cancelButton->setIconSize(QSize(15, 15));
  cancelButton->setToolTip(tr("Die Suche abbrechen"));
  // Signal 'clicked' mit eigenen Slot 'onSearch_cancel' verbinden
  connect(cancelButton, &QPushButton::clicked, this,
          &MainWindow::onSearch_cancel);
  // Initial unsichtbar
  cancelButton->setVisible(false);

  statusBar()->addWidget(cancelButton);
  // Erstellen einen Texteintrag in der Statusbar
  textSuchen = new QLineEdit(this);
  // Abmessungen der Texteintrag anpassen
  textSuchen->setFixedSize(QSize(200, 30));
  // Signal 'textChanged' mit eigenen Slot 'onTextSuchen_textChanged' verbinden
  connect(textSuchen, &QLineEdit::textChanged, this,
          &MainWindow::onTextSuchen_textChanged);
  // Initial unsichtbar
  textSuchen->setVisible(false);
  // Der 2. Parameter stretch = 1 dehnt den Label
  // über die gesamte StatusBar aus.
  statusBar()->addWidget(textSuchen, 1);
  // Einen Label für die Anzeige von Meldungen in der Statusbar erzeugen
  statusLabel = new QLabel(this);
  // Den Text horizontal und vertikal in der Mitte ausrichten
  statusLabel->setAlignment(Qt::AlignCenter);
  // Positioniert statusLabel rechts in der Statusbar
  statusBar()->addPermanentWidget(statusLabel);
  // QMargins(left, top, right, bottom)
  statusBar()->setContentsMargins(QMargins(10, 0, 10, 10));
  // Menüeinträge als RadioButtons
  // Die Menüeinträge zur Sprachauswahl sind checkable.
  // Damit immer nur ein Eintrag ausgewählt werden kann
  // (mutually exclusive) müssen sie in einer ActionGroup
  // zusammengefasst werden.
  QActionGroup *actionGroupLanguage = new QActionGroup(this);
  actionGroupLanguage->addAction(ui->actionDeutsch);
  actionGroupLanguage->addAction(ui->actionEnglisch);
  actionGroupLanguage->addAction(ui->actionSerBisch);

  QActionGroup *actionGroupColor = new QActionGroup(this);
  actionGroupColor->addAction(ui->actionWeiss);
  actionGroupColor->addAction(ui->actionGelb);
  actionGroupColor->addAction(ui->actionGraU);
  actionGroupColor->addAction(ui->actionGRn);
  actionGroupColor->addAction(ui->actionRot);
  actionGroupColor->addAction(ui->actionBlau);
  // Übersetzer für Systemtexte erstellen
  sysTranslator = new QTranslator(this);
  sysTranslatorInstalled = false;
  // Übersetzer für Englisch erstellen
  enTranslator = new QTranslator(this);
  enTranslatorInstalled = false;
  // Übersetzer für Serbisch erstellen
  srbTranslator = new QTranslator(this);
  srbTranslatorInstalled = false;
  // Übersetzungsdatei für Englisch aus dem Anwendungsverzeichnis laden
  if (!enTranslator->load(QLocale::English,
                          QDir::currentPath() + QDir::separator() +
                              Cnt::APP_NAME,
                          Cnt::SUFFIX)) {
    delete enTranslator;
    enTranslator = nullptr;
  }
  // Übersetzungsdatei für Serbisch aus dem Anwendungsverzeichnis laden
  if (!srbTranslator->load(QLocale::Croatian,
                           QDir::currentPath() + QDir::separator() +
                               Cnt::APP_NAME,
                           Cnt::SUFFIX)) {
    delete srbTranslator;
    srbTranslator = nullptr;
  }
  // Datenbank öffnen
  DAOLib::connectToDatabase(Cnt::DRIVER, Cnt::DRIVER_NAME, Cnt::SERVER,
                            Cnt::DATABASE_NAME);
  // Setzen von Standardwerten für Spaltenbreiten
  for (int i = 0; i <= Cnt::Angelplaetze::A_INFO; i++)
    mainColWidthList.push_back(Cnt::MAIN_COL_WIDTH);

  for (int i = 0; i <= Cnt::Fische::F_INFO; i++)
    angelplatzColWidthList.push_back(Cnt::ANGELPLATZ_COL_WIDTH);
  // Zuletzt verwendete Sprache aus der XML-Datei lesen
  QString xmlConfigFilePath =
      QDir::homePath() + Cnt::XML_PATH + QApplication::applicationName();
  xmlConfigFile = xmlConfigFilePath + QDir::separator() +
                  QApplication::applicationName() + Cnt::XML;

  readXMLSettings(xmlConfigFile);
  // Den Event Filter für die tableView installieren
  ui->tableView->installEventFilter(this);

  setBackgroundColor(StyleBackground::getColorIndex());
  // Anzeige aller Datensätze aus der Tabelle der Angelplätze
  showTable();
}

void MainWindow::setBackgroundColor(const int colorIndex) {
  // Stellen den Stil der Tabelle ein
  ui->tableView->setStyleSheet(
      "QHeaderView::section{"
      "border-top: 0px solid #D8D8D8;"
      "border-left: 0px solid #D8D8D8;"
      "border-right: 1px solid #D8D8D8;"
      "border-bottom: 1px solid #D8D8D8;"
      "padding: 4px;"
      "font: bold 18px;"
      "background-color: " +
      Cnt::COLOR_BACKGROUND[colorIndex].name() +
      ";}"
      "QTableView{ "
      "border : 1px solid #D8D8D8;" +
      // Stellen die Farbe des leeren Teils der Tabelle ein
      "background : " + Cnt::COLOR_BACKGROUND[colorIndex].name() +
      ";selection-background-color: " +
      // Ändert die Text- und Hintergrundfarbe der selektierten Zeile der
      // TableView damit die Markierung beim Fokusverlust sichtbar bleibt.
      Cnt::COLOR_HIGHLIGHT[colorIndex].name() +
      ";selection-color: " + Cnt::COLOR_WHITE.name() + ";};");
  // Hintergrundfarbe ändern für das Fenster
  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Cnt::COLOR_BACKGROUND[colorIndex]);
  this->setPalette(palette);

  auto changeBacgroundColor = [&](QMenu *menu) {
    palette = menu->palette();
    palette.setColor(menu->backgroundRole(), Cnt::COLOR_BACKGROUND[colorIndex]);
    menu->setPalette(palette);
  };
  // Hintergrundfarbe ändern für Menu
  changeBacgroundColor(ui->menuAngelplatz);
  changeBacgroundColor(ui->menuBearbeiten);
  changeBacgroundColor(ui->menuDaten);
  changeBacgroundColor(ui->menuExtras);
  changeBacgroundColor(ui->menuSprache);
  changeBacgroundColor(ui->menuHintergrund);
}

void MainWindow::setTableViewModel() {
  // Evtl. vorhandenes QSqlTableModel löschen
  delete model;
  // Evtl. vorhandenes SortFilterProxyModel löschen
  delete proxyModel;

  model = AngelplaetzeDAO::readAngelplaetzeIntoTableModel(this);
  // Verwenden ein Proxymodel, um Daten zu suchen
  proxyModel = new SearchProxyModel(this);
  // Spaltenüberschriften der Tabelle setzen
  model->setHeaderData(model->record().indexOf(Cnt::PATH), Qt::Horizontal,
                       QString());
  model->setHeaderData(model->record().indexOf(Cnt::NAME), Qt::Horizontal,
                       tr("Name"));
  model->setHeaderData(model->record().indexOf(Cnt::TYPE), Qt::Horizontal,
                       tr("Typ"));
  model->setHeaderData(model->record().indexOf(Cnt::FISCHE), Qt::Horizontal,
                       tr("Fische"));
  model->setHeaderData(model->record().indexOf(Cnt::PLZ), Qt::Horizontal,
                       tr("PLZ"));
  model->setHeaderData(model->record().indexOf(Cnt::ORT), Qt::Horizontal,
                       tr("Ort"));
  model->setHeaderData(model->record().indexOf(Cnt::LAND), Qt::Horizontal,
                       tr("Land"));
  model->setHeaderData(model->record().indexOf(Cnt::INFO), Qt::Horizontal,
                       tr("Zusätzliche Information"));
  // Aufsteigende Sortierung nach Postleitzahl
  model->sort(model->record().indexOf(Cnt::NAME), Qt::AscendingOrder);
  // Verwenden eigenes QStyledItemDelegate, um Bilder anzuzeigen
  ImageStyleItemDelegate *delegate = new ImageStyleItemDelegate(this);
  // Bilder nur für die PATH-Spalte anzeigen
  ui->tableView->setItemDelegateForColumn(model->record().indexOf(Cnt::PATH),
                                          delegate);
  // Anfangssuchwerte festlegen
  proxyModel->setSearchTerm(textSuchen->text());
  // Das Datenmodel zur tableView zuweisen
  ui->tableView->setModel(proxyModel);

  proxyModel->setSourceModel(model);
  // Übernahme aller Daten in das Datenmodell
  while (model->canFetchMore())
    model->fetchMore();
  // Signal für die Anzeige des aktuellen Datensatzes in der Statusbar
  connect(ui->tableView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &MainWindow::tableView_selectionChanged);
  // Signal für die Änderung der Spaltenbreite in der TableView
  connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionResized, this,
          &MainWindow::tableView_section_resized);
}

void MainWindow::showTable() {

  setTableViewModel();
  // Feste Höhe für die Tabellenüberschrift
  ui->tableView->horizontalHeader()->setFixedHeight(Cnt::MAIN_HEADER_HEIGHT);
  // Anpassen der Spaltenbreiten der vorherigen Einstellung des Benutzers
  for (int i = 0; i < mainColWidthList.size(); i++)
    ui->tableView->setColumnWidth(i, mainColWidthList[i]);
  // Spalte PRIMARYKEY unsichtbar machen
  ui->tableView->hideColumn(model->record().indexOf(Cnt::PRIMARYKEY));
  // Aktivieren/Deaktivieren der Komponenten, abhängig davon,
  // ob Datensätze gelesen wurden.
  ui->tableView->setEnabled(proxyModel->rowCount());
  ui->actionNdern->setEnabled(ui->tableView->isEnabled());
  ui->actionLschen->setEnabled(ui->tableView->isEnabled());
  ui->actionMarkierterAngelplatz->setEnabled(ui->tableView->isEnabled());
  ui->actionAlleAngelpltze->setEnabled(ui->tableView->isEnabled());
  // Erste Zeile auswählen, oder eine Meldung ausgeben,
  // dass der Datensatz ist leer
  ui->tableView->isEnabled()
      ? ui->tableView->selectRow(0)
      : statusLabel->setText(tr("Der Datensatz ist leer."));
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
  // Evtl. vorhandenes AngelplatzWindow löschen
  delete angelplatzWindow;

  QString angelplatzName = AngelplaetzeDAO::readAngelplatzName(key);

  angelplatzWindow =
      new AngelplatzWindow(angelplatzColWidthList, angelplatzName, this);
  // Meldung, ob sich die Daten in der Datenbank geändert haben
  connect(angelplatzWindow, &AngelplatzWindow::dataModified, this,
          &MainWindow::refreshTableView);
  // Meldung, wenn sich die Spaltenbreite ändert
  connect(angelplatzWindow, &AngelplatzWindow::columnWidthModified, this,
          &MainWindow::setColumnAngelplatzWidth);

  angelplatzWindow->show();
}

void MainWindow::deleteEntry(const QModelIndex &index) {
  // Quellmodellindex
  auto sourceIndex = proxyModel->mapToSource(index);
  // Ermitteln des Primärschlüssels in Spalte 'PRIMARYKEY' über den als
  // Parameter übergebenen QModelIndex.
  qint64 key =
      model->record(sourceIndex.row()).value(Cnt::PRIMARYKEY).toLongLong();

  QString angelplatzName = AngelplaetzeDAO::readAngelplatzName(key);

  int msgValue = QMessageBox::question(
      this, this->windowTitle(),
      tr("Angelplatz löschen: ") +
          model->record(sourceIndex.row()).value(Cnt::NAME).toString() +
          tr("\nAnzahl der zu löschenden Fische: ") +
          QString::number(FischeDAO::countFischeInAngelplatz(angelplatzName)),
      QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

  if (msgValue == QMessageBox::Cancel)
    return;
  // Löschen und Fische, die sich am Angelplatz befinden
  if ((FischeDAO::countFischeInAngelplatz(angelplatzName) > 0
           ? FischeDAO::deleteFischeInAngelplatz(angelplatzName)
           : true) &&
      // Löschen den Angelplatz über den Primärschlüssel
      AngelplaetzeDAO::deleteAngelplatz(key)) {

    statusLabel->setText(tr("Einträge werden aktualisiert..."));
    QApplication::processEvents();
    // Die Angelplätze im Datenmodell erneut lesen
    showTable();

    int row = (index.row() - 1 < 0) ? 0 : index.row() - 1;

    // Prüfen, ob die ermittelte Zeile selektiert werden kann
    if (ui->tableView->model()->rowCount() >= row)
      ui->tableView->selectRow(row);

    if (ui->tableView->model()->rowCount() == 0)
      statusLabel->setText(tr("Der Datensatz ist leer."));
  }
}

void MainWindow::refreshTableView(const qint64 key) {

  statusLabel->setText(tr("Einträge werden aktualisiert..."));
  QApplication::processEvents();
  // Die Angelplätze im Datenmodell erneut lesen
  showTable();
  // Den Cursor auf den neuen Eintrag über den Primärschlüssel positionieren
  findItemInTableView(Cnt::PRIMARYKEY, QVariant(key));
}

void MainWindow::findItemInTableView(const QString &columnName,
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

void MainWindow::updateTableView(const qint64 key) {

  Angelplatz *angelplatz = AngelplaetzeDAO::readAngelplatz(key);

  if (angelplatz == nullptr)
    return;

  auto sourceIndex = proxyModel->mapToSource(ui->tableView->currentIndex());

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

  changeData(Cnt::PATH, angelplatz->getPath());
  changeData(Cnt::NAME, angelplatz->getName());
  changeData(Cnt::TYPE, angelplatz->getType());
  changeData(Cnt::FISCHE, angelplatz->getFische());
  changeData(Cnt::PLZ, angelplatz->getPlz());
  changeData(Cnt::ORT, angelplatz->getOrt());
  changeData(Cnt::LAND, angelplatz->getLand());
  changeData(Cnt::INFO, angelplatz->getInfo());
  // Objekt angelplatz vom Heap löschen
  delete angelplatz;
}

void MainWindow::modifyTableView(const qint64 key,
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

void MainWindow::setColumnAngelplatzWidth(const QList<int> list) {

  angelplatzColWidthList = list;
}

void MainWindow::onSearch_cancel() {

  textSuchen->setVisible(false);

  cancelButton->setVisible(false);
  // Löschen den angeforderten Text
  textSuchen->setText(QString());
  // Tabelle ohne Filterung anzeigen
  showTable();
}

void MainWindow::onTextSuchen_textChanged(const QString &) {
  // Aktualisieren die Tabellenansicht
  showTable();
}

void MainWindow::tableView_selectionChanged() {
  // Anzeigen, welcher Datensatz ausgewählt ist
  statusLabel->setText(
      QString(tr("Datensatz %L1 von %L2"))
          .arg(ui->tableView->selectionModel()->currentIndex().row() + 1)
          .arg(ui->tableView->model()->rowCount()));
}

void MainWindow::tableView_section_resized(int index, int, int newSize) {
  // Nur auf die Änderung der PATH Spalte (Bild) reagieren
  if (index == model->record().indexOf(Cnt::PATH))
    ui->tableView->verticalHeader()->setDefaultSectionSize(newSize);
  // Wertänderung in der globalen Variablen
  if (mainColWidthList.size() > index)
    mainColWidthList[index] = newSize;
}

void MainWindow::loadLanguage(const QString &language) {

  removeAllTranslators();

  bool sysLoaded = false;

  if (language.toLower() == Cnt::EN && enTranslator != nullptr) {
    // Englische Systemtexte laden
    sysLoaded =
        sysTranslator->load(Cnt::QTBASE_ + language,
                            QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    // Übersetzer für Englisch installieren
    enTranslatorInstalled = QApplication::installTranslator(enTranslator);
    ui->actionEnglisch->setChecked(true);
    currentLanguage = language;
  } else if (language.toLower() == Cnt::SRB && srbTranslator != nullptr) {
    // Serbische Systemtexte laden
    sysLoaded =
        sysTranslator->load(Cnt::QTBASE_ + language,
                            QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    // Übersetzer für Serbisch installieren
    srbTranslatorInstalled = QApplication::installTranslator(srbTranslator);
    ui->actionSerBisch->setChecked(true);
    currentLanguage = language;
  } else {
    // Standardsprache ist Deutsch
    // Deutsche Systemtexte laden
    sysLoaded = sysTranslator->load(
        Cnt::QTBASE_DE, QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    ui->actionDeutsch->setChecked(true);
    currentLanguage = QString();
  }

  if (sysLoaded)
    sysTranslatorInstalled = QApplication::installTranslator(sysTranslator);
}

void MainWindow::loadBackgroundColor(const int color) {

  StyleBackground::setColorIndex(color);

  switch (color) {
  case Cnt::Color::WHITE:
    ui->actionWeiss->setChecked(true);
    break;
  case Cnt::Color::YELLOW:
    ui->actionGelb->setChecked(true);
    break;
  case Cnt::Color::GRAY:
    ui->actionGraU->setChecked(true);
    break;
  case Cnt::Color::GREEN:
    ui->actionGRn->setChecked(true);
    break;
  case Cnt::Color::RED:
    ui->actionRot->setChecked(true);
    break;
  case Cnt::Color::BLUE:
    ui->actionBlau->setChecked(true);
    break;
  }
}

void MainWindow::removeAllTranslators() {

  if (enTranslatorInstalled) {
    QApplication::removeTranslator(enTranslator);
    enTranslatorInstalled = false;
  }

  if (srbTranslatorInstalled) {
    QApplication::removeTranslator(srbTranslator);
    srbTranslatorInstalled = false;
  }

  QApplication::removeTranslator(sysTranslator);
  sysTranslatorInstalled = false;

  // Aktuelle Sprache mit leerer Zeichenkette initialisieren
  // (Standardsprache Deutsch)
  currentLanguage = QString();
}

void MainWindow::readXMLSettings(const QString &filename) {

  QString language;

  int colorBackground = 0;
  // Datei öffnen
  QFile file(filename);

  if (!file.open(QFile::ReadOnly | QFile::Text))
    return;

  QXmlStreamReader xmlReader(&file);

  do {
    // Startelement lesen
    if (xmlReader.readNextStartElement()) {
      if (xmlReader.name() == QString(Cnt::SETTINGS)) {
        do {
          // Alle Einträge des Startelements lesen
          xmlReader.readNext();
          // Ist das Ende des aktuelle Startelements erreicht ?
          if (xmlReader.isEndElement())
            break;
          // Alle gespeicherten Werte aus XML eingeben
          if (xmlReader.name() == QString(Cnt::LANGUAGE))
            language = xmlReader.readElementText();

          for (int i = 0; i < mainColWidthList.size(); i++)
            if (xmlReader.name() == QString(Cnt::COL_MAIN).arg(i))
              mainColWidthList[i] = xmlReader.readElementText().toInt();

          for (int i = 0; i < angelplatzColWidthList.size(); i++)
            if (xmlReader.name() == QString(Cnt::COL_ANGELPLATZ).arg(i))
              angelplatzColWidthList[i] = xmlReader.readElementText().toInt();

          if (xmlReader.name() == QString(Cnt::BACKGROUND))
            colorBackground = xmlReader.readElementText().toInt();

        } while (!xmlReader.atEnd());
      }
    }
  } while (!xmlReader.atEnd());

  file.close();

  loadLanguage(language);

  loadBackgroundColor(colorBackground);
}

void MainWindow::writeXMLSettings(const QString &filename) {

  // Prüfen, ob das Verzeichnis existiert, wenn nicht erstellen.
  QFileInfo fi(filename);
  // Nur den Verzeichnisnamen aus dem Dateinamen extrahieren
  QString xmlConfigFilePath = fi.absolutePath();

  QDir dir(xmlConfigFilePath);
  // QDir.mkpath() erstellt alle nicht existierende Verzeichnisse.
  if (!dir.exists() && !dir.mkpath(xmlConfigFilePath)) {
    QMessageBox::critical(this, this->windowTitle(),
                          tr("Das Verzeichnis '") + xmlConfigFilePath +
                              tr("' konnte nicht erstellt werden"));
    return;
  }
  // Datei öffnen
  QFile file(filename);

  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::critical(this, this->windowTitle(), file.errorString());
    return;
  }

  QXmlStreamWriter xmlWriter(&file);
  // Autom. Formatierung der XML-Datei
  xmlWriter.setAutoFormatting(true);
  // Schreibt die XML-Version 1.0 und die Information für die
  // Kodierung/Dekodierung (UTF-8)
  xmlWriter.writeStartDocument();
  // Schreibt das Startelement (Knoten) der XML-Datei <Settings>
  xmlWriter.writeStartElement(Cnt::SETTINGS);
  // Schreibt das Textelement (Attribut) welches zu diesem Knoten gehört
  // <Language>en</Language>
  xmlWriter.writeTextElement(Cnt::LANGUAGE, currentLanguage);
  // Schreiben aller Attribute für Spaltenbreiten (MainWindow)
  for (int i = 0; i < mainColWidthList.size(); i++)
    xmlWriter.writeTextElement(QString(Cnt::COL_MAIN).arg(i),
                               QString::number(mainColWidthList[i]));
  // Schreiben aller Attribute für Spaltenbreiten (AngelplatzWindow)
  for (int i = 0; i < angelplatzColWidthList.size(); i++)
    xmlWriter.writeTextElement(QString(Cnt::COL_ANGELPLATZ).arg(i),
                               QString::number(angelplatzColWidthList[i]));
  // Schreiben die ausgewählte Hintergrundfarbe
  xmlWriter.writeTextElement(Cnt::BACKGROUND,
                             QString::number(StyleBackground::getColorIndex()));
  // Beendet den Knoten </Settings>
  xmlWriter.writeEndElement();

  file.close();
}

bool MainWindow::eventFilter(QObject *sender, QEvent *event) {

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

// Das changeEvent wird durch installTranslator() ausgelöst
// Wird aufgerufen, wenn die Sparche geändert wurde.
void MainWindow::changeEvent(QEvent *event) {

  if (event->type() == QEvent::LanguageChange)
    ui->retranslateUi(this);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  // Schließen der Datenbank
  DAOLib::closeConnection();
  // Daten in XML schreiben
  writeXMLSettings(xmlConfigFile);

  event->accept();
}

void MainWindow::on_actionBEenden_triggered() { close(); }

void MainWindow::on_actionNeu_triggered() { showAngelplatzDialog(0); }

void MainWindow::on_actionLschen_triggered() {
  deleteEntry(ui->tableView->currentIndex());
}

void MainWindow::on_action_Suchen_triggered() {

  cancelButton->setVisible(true);

  textSuchen->setVisible(true);
  // Fokuseinstellung
  textSuchen->setFocus();
}

void MainWindow::on_actionNdern_triggered() {

  showAngelplatzDialog(
      model
          ->record(proxyModel->mapToSource(ui->tableView->currentIndex()).row())
          .value(Cnt::PRIMARYKEY)
          .toLongLong());
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index) {

  showAngelplatzWindow(model->record(proxyModel->mapToSource(index).row())
                           .value(Cnt::PRIMARYKEY)
                           .toLongLong());
}

void MainWindow::on_actionMarkierterAngelplatz_triggered() {
  // Über den als Parameter übergebenen QModelIndex der selektierten Zeile
  // den Wert der Spalte PRIMARYKEY ermitten und als Parameter an die
  // Methode showAngelplatzWindow() übergeben.
  showAngelplatzWindow(
      model
          ->record(proxyModel->mapToSource(ui->tableView->currentIndex()).row())
          .value(Cnt::PRIMARYKEY)
          .toLongLong());
}

void MainWindow::on_actionAlleAngelpltze_triggered() {
  // Anzeigedaten für alle Angelplätze (key = 0)
  showAngelplatzWindow(0);
}

void MainWindow::on_actionDeutsch_triggered() {
  // Standardsprache einstellen (Deutsch)
  loadLanguage(QString());

  showTable();
}

void MainWindow::on_actionEnglisch_triggered() {

  loadLanguage(Cnt::EN);

  showTable();
}

void MainWindow::on_actionSerBisch_triggered() {

  loadLanguage(Cnt::SRB);

  showTable();
}

void MainWindow::on_actionWeiss_triggered() {

  loadBackgroundColor(Cnt::Color::WHITE);

  setBackgroundColor(Cnt::Color::WHITE);
}

void MainWindow::on_actionGelb_triggered() {

  loadBackgroundColor(Cnt::Color::YELLOW);

  setBackgroundColor(Cnt::Color::YELLOW);
}

void MainWindow::on_actionGraU_triggered() {

  loadBackgroundColor(Cnt::Color::GRAY);

  setBackgroundColor(Cnt::Color::GRAY);
}

void MainWindow::on_actionGRn_triggered() {

  loadBackgroundColor(Cnt::Color::GREEN);

  setBackgroundColor(Cnt::Color::GREEN);
}

void MainWindow::on_actionRot_triggered() {

  loadBackgroundColor(Cnt::Color::RED);

  setBackgroundColor(Cnt::Color::RED);
}

void MainWindow::on_actionBlau_triggered() {

  loadBackgroundColor(Cnt::Color::BLUE);

  setBackgroundColor(Cnt::Color::BLUE);
}
