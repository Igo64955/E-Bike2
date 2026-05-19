// EBike2.h : Hauptheaderdatei für die EBike2-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
    #error "Schließen Sie 'stdafx.h' vor dieser Datei für PCH ein."
#endif

#include "Resource.h"

// ============================================================
// CEBike2App
// Anwendungsklasse für den E-Bike Übersetzungsrechner
// ============================================================
class CEBike2App : public CWinApp
{
public:
    CEBike2App();

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()

private:
    ULONG_PTR m_gdiplusToken; // Für GDI+ (falls benötigt)
};

extern CEBike2App theApp;
