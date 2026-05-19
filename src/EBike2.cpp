// EBike2.cpp : Definiert das Klassenverhalten für die Anwendung
//
// E-Bike Übersetzungsverhältnis-Rechner
// Berechnet die Übersetzung und Entfaltung für eine 10-fach-Kassette
// und stellt die Ergebnisse als Tabelle und Balkendiagramm dar.
//
#include "stdafx.h"
#include "EBike2.h"
#include "EBike2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Visuelles Manifest für Common Controls v6 (Windows Visual Styles)
#pragma comment(linker, \
    "/manifestdependency:\"type='win32' " \
    "name='Microsoft.Windows.Common-Controls' " \
    "version='6.0.0.0' " \
    "processorArchitecture='*' " \
    "publicKeyToken='6595b64144ccf1df' " \
    "language='*'\"")

// ===========================================================
// CEBike2App
// ===========================================================

BEGIN_MESSAGE_MAP(CEBike2App, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CEBike2App::CEBike2App()
    : m_gdiplusToken(0)
{
    // Restart-Manager-Unterstützung
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

// Das einzige CEBike2App-Objekt
CEBike2App theApp;

// -----------------------------------------------------------
BOOL CEBike2App::InitInstance()
{
    // Common Controls initialisieren (erforderlich für Windows XP+)
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC  = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    // Shell-Manager (für TreeView/ListView mit Shell-Icons)
    CShellManager* pShellManager = new CShellManager;

    // Modernen Windows-Visual-Manager aktivieren
    CMFCVisualManager::SetDefaultManager(
        RUNTIME_CLASS(CMFCVisualManagerWindows));

    // Registry-Schlüssel für Einstellungen
    SetRegistryKey(_T("EBike2"));

    // Hauptdialog anzeigen
    CEBike2Dlg dlg;
    m_pMainWnd   = &dlg;
    INT_PTR nRes = dlg.DoModal();

    if (nRes == -1)
    {
        TRACE(traceAppMsg, 0,
              "Warnung: Dialogerstellung fehlgeschlagen.\n");
    }

    delete pShellManager;
    return FALSE; // Anwendung beenden (kein Nachrichtenpumpenbetrieb)
}

// -----------------------------------------------------------
int CEBike2App::ExitInstance()
{
    return CWinApp::ExitInstance();
}
