// stdafx.h : Vorkompilierte Header-Datei
// Beinhaltet Standard-System-Includes und projektspezifische Includes,
// die häufig, aber selten geändert werden.
//
#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Mindest-Zielplattform: Windows 7
#ifndef WINVER
#define WINVER 0x0601
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include <afxwin.h>          // MFC Kern- und Standardkomponenten
#include <afxext.h>          // MFC-Erweiterungen
#include <afxdisp.h>         // MFC Automation-Klassen
#include <afxdtctl.h>        // MFC-Unterstützung für Internet Explorer 4 Common Controls
#include <afxcmn.h>          // MFC-Unterstützung für Windows Common Controls
#include <afxcontrolbars.h>  // MFC-Unterstützung für Ribbons und Steuerleisten
#include <afxdialogex.h>     // CDialogEx

#define NOMINMAX
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
