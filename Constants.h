#pragma once

#include <QColor>
#include <QString>

namespace Cnt {

enum EditMode { NEW, UPDATE };
// Filterparameter ComboBox
enum Parameter {
  P_PARAMETER,
  P_LAENGE,
  P_GEWICHT,
  P_ZEIT,
  P_TEMPERATUR,
  P_WINDGESCHWINDIGKEIT,
  P_LUFTDRUCK
};
// Standardwerte Dimensionstabelle
inline constexpr int MAIN_COL_WIDTH = 150;
inline constexpr int MAIN_HEADER_HEIGHT = 50;
inline constexpr int ANGELPLATZ_COL_WIDTH = 100;
inline constexpr int ANGELPLATZ_HEADER_HEIGHT = 50;
// Die Farbe der markierten Zeile in der Tabelle
inline constexpr QColor COLOR_HIGHLIGHT = QColor(0, 112, 255);
// Spaltennamen in der Fische-Tabellendatenbank
inline const QString PRIMARYKEY = "PRIMARYKEY";
inline const QString PATH = "PATH";
inline const QString NAME = "NAME";
inline const QString ANGELPLATZ = "ANGELPLATZ";
inline const QString LAENGE = "LAENGE";
inline const QString GEWICHT = "GEWICHT";
inline const QString ZEIT = "ZEIT";
inline const QString TEMPERATUR = "TEMPERATUR";
inline const QString WINDGESCHWINDIGKEIT = "WINDGESCHWINDIGKEIT";
inline const QString LUFTDRUCK = "LUFTDRUCK";
inline const QString IS_NACHT = "IS_NACHT";
inline const QString NIEDERSCHLAG = "NIEDERSCHLAG";
inline const QString INFO = "INFO";
// Spaltennamen in der Angelplaetze-Tabellendatenbank
inline const QString TYPE = "TYPE";
inline const QString FISCHE = "FISCHE";
inline const QString PLZ = "PLZ";
inline const QString ORT = "ORT";
inline const QString LAND = "LAND";
// Verbindung zum MS SQL Server
inline const QString DRIVER = "QODBC";
inline const QString DRIVER_NAME = "DRIVER={SQL Server}";
inline const QString SERVER = "localhost\\SQLEXPRESS";
inline const QString DATABASE_NAME = "alfatraining";
// Übersetzerdaten
inline const QString APP_NAME = "Angeltagebuch";
inline const QString SUFFIX = "_";
inline const QString EN = "en";
inline const QString QTBASE_ = "qtbase_";
inline const QString QTBASE_DE = "qtbase_de";
// Daten für XML-Datei
inline const QString XML_PATH = "/AppData/Local/";
inline const QString XML = ".xml";
inline const QString SETTINGS = "Settings";
inline const QString LANGUAGE = "Language";
inline const QString COL_MAIN = "ColumnMain%1";
inline const QString COL_ANGELPLATZ = "ColumnAngelplatz%1";

} // namespace Cnt
