# E-Bike2 – Übersetzungsverhältnis-Rechner

Eine **Microsoft MFC 2022**-Anwendung zur Berechnung und Darstellung des
Übersetzungsverhältnisses eines E-Bikes mit einem Kettenrad und einer
10-fach-Ritzelkassette.

---

## Funktionen

| Feature | Beschreibung |
|---------|-------------|
| **Übersetzungstabelle** | Zeigt für alle 10 Ritzel: Nr., Zähne, Übersetzungsverhältnis *i* und Entfaltung in m/U |
| **Balkendiagramm** | Visualisiert die Entfaltung aller Gänge mit farbigen Verlaufsbalken |
| **Konfigurierbar** | Kettenrad-Zähne (frei eingebbar) und Radgröße (26", 27,5", 28"/700c, 29") wählbar |
| **Sofort-Berechnung** | Schaltfläche „Berechnen" aktualisiert Tabelle und Grafik sofort |

### Berechnungsformeln

```
Übersetzungsverhältnis:    i  = Z_Kettenrad / Z_Ritzel
Entfaltung (m/Umdrehung):  E  = i × π × d
```

*Z = Zähne, d = Raddurchmesser in Metern*

### Standard-Kassette (10-fach)

`11 – 13 – 15 – 17 – 19 – 21 – 24 – 28 – 34 – 42 Zähne`

---

## Projektstruktur

```
E-Bike2/
├── EBike2.sln              Visual Studio 2022 Solution
├── EBike2.vcxproj          C++ Projektdatei (MFC, v143, Unicode)
├── EBike2.vcxproj.filters  Projektfilter
└── src/
    ├── stdafx.h            Vorkompilierter Header
    ├── stdafx.cpp
    ├── EBike2.h            Anwendungsklasse  (CEBike2App)
    ├── EBike2.cpp
    ├── EBike2Dlg.h         Hauptdialog + CChartCtrl (Balkendiagramm)
    ├── EBike2Dlg.cpp
    ├── Resource.h          Ressourcen-IDs
    ├── EBike2.rc           Dialog-Ressourcen
    └── res/
        └── EBike2.ico      Anwendungssymbol
```

---

## Voraussetzungen

| Werkzeug | Version |
|----------|---------|
| **Visual Studio** | 2022 (v17.x) |
| **MSVC-Toolset** | v143 |
| **Windows SDK** | 10.0 |
| **MFC** | Im Lieferumfang von VS 2022 enthalten |

---

## Erstellen der Anwendung

### Option 1 – Visual Studio IDE

1. `EBike2.sln` in Visual Studio 2022 öffnen
2. Konfiguration wählen: `Debug | x64` oder `Release | x64`
3. **Erstellen → Projektmappe erstellen** (`Strg+Umschalt+B`)
4. Ausgabe: `x64\Debug\EBike2.exe` (bzw. `x64\Release\EBike2.exe`)

### Option 2 – Kommandozeile (MSBuild)

```bat
:: Developer Command Prompt for VS 2022
msbuild EBike2.sln /p:Configuration=Release /p:Platform=x64
```

---

## Ergebnisbeispiel

Für **Kettenrad 38Z** und **Rad 27,5" (584 mm)**:

| Ritzel Nr. | Zähne | Übersetzung i | Entfaltung (m/U) |
|:----------:|:-----:|:-------------:|:----------------:|
| 1          |  11   |    3,455      |    7,38          |
| 2          |  13   |    2,923      |    6,25          |
| 3          |  15   |    2,533      |    5,41          |
| 4          |  17   |    2,235      |    4,77          |
| 5          |  19   |    2,000      |    4,27          |
| 6          |  21   |    1,810      |    3,87          |
| 7          |  24   |    1,583      |    3,38          |
| 8          |  28   |    1,357      |    2,90          |
| 9          |  34   |    1,118      |    2,39          |
| 10         |  42   |    0,905      |    1,93          |

---

## Lizenz

Dieses Projekt steht unter der [MIT License](LICENSE).
