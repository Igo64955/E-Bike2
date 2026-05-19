// EBike2Dlg.cpp : Implementierung des Hauptdialogs
//
// Berechnet das Übersetzungsverhältnis eines E-Bikes:
//   i  = Z_Kettenrad / Z_Ritzel
//   Entfaltung (m/U) = i × π × Raddurchmesser
//
#include "stdafx.h"
#include "EBike2.h"
#include "EBike2Dlg.h"
#include "Resource.h"

// GradientFill benötigt msimg32.lib
#pragma comment(lib, "msimg32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ===========================================================
// Hilfsfunktion: lineare Interpolation zwischen zwei Farben
// ===========================================================
static COLORREF LerpColor(COLORREF c1, COLORREF c2, double t)
{
    int r = static_cast<int>(GetRValue(c1) + t * (GetRValue(c2) - GetRValue(c1)));
    int g = static_cast<int>(GetGValue(c1) + t * (GetGValue(c2) - GetGValue(c1)));
    int b = static_cast<int>(GetBValue(c1) + t * (GetBValue(c2) - GetBValue(c1)));
    return RGB(
        (std::min)(r, 255),
        (std::min)(g, 255),
        (std::min)(b, 255));
}

// ===========================================================
// CChartCtrl
// ===========================================================
CChartCtrl::CChartCtrl()
    : m_pGearData(nullptr)
    , m_nChainringTeeth(38)
    , m_dWheelDiameter(0.584)
{
}

void CChartCtrl::SetGearData(const std::vector<GearData>* pData,
                              int nChainring, double dWheelDiameter)
{
    m_pGearData        = pData;
    m_nChainringTeeth  = nChainring;
    m_dWheelDiameter   = dWheelDiameter;
    Invalidate();
}

BEGIN_MESSAGE_MAP(CChartCtrl, CStatic)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CChartCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
    return TRUE; // Vermeidet Flimmern (wir zeichnen alles in OnPaint)
}

void CChartCtrl::OnPaint()
{
    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

    // --- Doppelpufferung für flimmerfreie Darstellung ---
    CDC     memDC;
    CBitmap memBmp;
    memDC.CreateCompatibleDC(&dc);
    memBmp.CreateCompatibleBitmap(&dc, clientRect.Width(), clientRect.Height());
    CBitmap* pOldBmp = memDC.SelectObject(&memBmp);

    DrawBarChart(&memDC, clientRect);

    dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(),
              &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBmp);
}

void CChartCtrl::DrawGradientRect(CDC* pDC, const CRect& rc,
                                   COLORREF clrTop, COLORREF clrBottom)
{
    if (rc.IsRectEmpty()) return;

    // Verwende GradientFill (msimg32.dll) für schnelle Farbverläufe
    TRIVERTEX verts[2];
    verts[0].x     = rc.left;
    verts[0].y     = rc.top;
    verts[0].Red   = static_cast<COLOR16>(GetRValue(clrTop)    * 256);
    verts[0].Green = static_cast<COLOR16>(GetGValue(clrTop)    * 256);
    verts[0].Blue  = static_cast<COLOR16>(GetBValue(clrTop)    * 256);
    verts[0].Alpha = 0xFF00;

    verts[1].x     = rc.right;
    verts[1].y     = rc.bottom;
    verts[1].Red   = static_cast<COLOR16>(GetRValue(clrBottom) * 256);
    verts[1].Green = static_cast<COLOR16>(GetGValue(clrBottom) * 256);
    verts[1].Blue  = static_cast<COLOR16>(GetBValue(clrBottom) * 256);
    verts[1].Alpha = 0xFF00;

    GRADIENT_RECT gRect = { 0, 1 };
    ::GradientFill(pDC->GetSafeHdc(), verts, 2, &gRect, 1,
                   GRADIENT_FILL_RECT_V);
}

void CChartCtrl::DrawBarChart(CDC* pDC, const CRect& rc)
{
    // --- Hintergrund ---
    CBrush bgBrush(RGB(245, 248, 252));
    pDC->FillRect(rc, &bgBrush);

    if (!m_pGearData || m_pGearData->empty())
    {
        pDC->SetTextColor(RGB(150, 150, 150));
        pDC->TextOut(rc.left + 10, rc.top + 10,
                     _T("Bitte 'Berechnen' drücken..."));
        return;
    }

    const std::vector<GearData>& data = *m_pGearData;
    const int nBars = static_cast<int>(data.size());

    // --- Diagramm-Ränder (DLU-unabhängig, in Pixeln) ---
    const int LEFT   = 62;
    const int RIGHT  = 18;
    const int TOP    = 38;
    const int BOTTOM = 44;

    int plotLeft   = rc.left   + LEFT;
    int plotRight  = rc.right  - RIGHT;
    int plotTop    = rc.top    + TOP;
    int plotBottom = rc.bottom - BOTTOM;
    int plotW      = plotRight  - plotLeft;
    int plotH      = plotBottom - plotTop;

    if (plotW <= 0 || plotH <= 0) return;

    // --- Skalierung: maximale Entfaltung ---
    double dMaxDev = 0.0;
    for (const auto& gd : data)
        dMaxDev = (std::max)(dMaxDev, gd.dDevelopment);

    // Y-Achse auf nächste volle 0.5 m aufrunden
    double yMax = (std::ceil(dMaxDev / 0.5)) * 0.5;
    if (yMax <= 0.0) yMax = 1.0;

    // --- Rasterlinien und Y-Achsenbeschriftung ---
    const int nGridLines = 5;
    CFont fntSmall;
    fntSmall.CreateFont(11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                        DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));
    CFont* pOldFont = pDC->SelectObject(&fntSmall);
    pDC->SetTextColor(RGB(100, 100, 100));
    pDC->SetBkMode(TRANSPARENT);

    CPen gridPen(PS_DOT, 1, RGB(200, 210, 220));
    CPen axisPen(PS_SOLID, 2, RGB(60,  80, 110));

    for (int g = 0; g <= nGridLines; ++g)
    {
        double val = yMax * g / nGridLines;
        int y = plotBottom - static_cast<int>(val / yMax * plotH);

        // Raster
        CPen* pOld = pDC->SelectObject(&gridPen);
        pDC->MoveTo(plotLeft,  y);
        pDC->LineTo(plotRight, y);
        pDC->SelectObject(pOld);

        // Y-Beschriftung
        CString sVal;
        sVal.Format(_T("%.1f"), val);
        CSize sz = pDC->GetTextExtent(sVal);
        pDC->TextOut(plotLeft - sz.cx - 4, y - sz.cy / 2, sVal);
    }

    // --- Achsen ---
    {
        CPen* pOld = pDC->SelectObject(&axisPen);
        pDC->MoveTo(plotLeft, plotTop);
        pDC->LineTo(plotLeft, plotBottom);
        pDC->MoveTo(plotLeft,  plotBottom);
        pDC->LineTo(plotRight, plotBottom);
        pDC->SelectObject(pOld);
    }

    // --- Y-Achsentitel (gedreht) ---
    {
        LOGFONT lf = {};
        fntSmall.GetLogFont(&lf);
        lf.lfEscapement  = 900;
        lf.lfOrientation = 900;
        lf.lfWeight      = FW_BOLD;
        CFont rotFont;
        rotFont.CreateFontIndirect(&lf);
        CFont* pOldRot = pDC->SelectObject(&rotFont);
        pDC->SetTextColor(RGB(50, 80, 120));
        CString yTitle = _T("Entfaltung (m/U)");
        CSize   sz = pDC->GetTextExtent(yTitle);
        pDC->TextOut(rc.left + 14,
                     plotTop + (plotH + sz.cx) / 2, yTitle);
        pDC->SelectObject(pOldRot);
    }

    // --- Balken zeichnen ---
    int barW  = (plotW / nBars) * 3 / 4;  // 75 % der verfügbaren Breite pro Slot
    int slotW = plotW / nBars;

    // Farbpalette (blau → grün → gelb → rot)
    COLORREF clrPalette[10] = {
        RGB( 30,  90, 200),
        RGB( 20, 140, 210),
        RGB( 10, 190, 180),
        RGB( 30, 185,  90),
        RGB(100, 200,  30),
        RGB(180, 205,  20),
        RGB(220, 170,  10),
        RGB(230, 110,  10),
        RGB(220,  50,  10),
        RGB(200,  20,  20),
    };

    for (int i = 0; i < nBars; ++i)
    {
        const GearData& gd = data[i];

        int xSlot = plotLeft + i * slotW;
        int xBar  = xSlot + (slotW - barW) / 2;
        int barH  = static_cast<int>(gd.dDevelopment / yMax * plotH);
        if (barH < 1) barH = 1;

        CRect barRect(xBar, plotBottom - barH, xBar + barW, plotBottom);

        COLORREF clrTop    = LerpColor(RGB(255,255,255),
                                       clrPalette[i % 10], 0.6);
        COLORREF clrBottom = clrPalette[i % 10];

        // Farbverlauf-Balken
        DrawGradientRect(pDC, barRect, clrTop, clrBottom);

        // Rahmen (nur Umriss, keine Füllung)
        CBrush borderBrush(RGB(50, 70, 100));
        pDC->FrameRect(&barRect, &borderBrush);

        // Entfaltungswert im/über dem Balken
        CString sVal;
        sVal.Format(_T("%.2f"), gd.dDevelopment);
        pDC->SetTextColor(RGB(30, 30, 30));
        CSize szVal = pDC->GetTextExtent(sVal);
        int xText = xBar + (barW - szVal.cx) / 2;
        if (barH >= szVal.cy + 4)
        {
            pDC->SetTextColor(RGB(255, 255, 255));
            pDC->TextOut(xText, barRect.top + 3, sVal);
            pDC->SetTextColor(RGB(30, 30, 30));
        }
        else
        {
            pDC->TextOut(xText, barRect.top - szVal.cy - 2, sVal);
        }

        // X-Beschriftung: Ritzel-Nr.
        CString sNum;
        sNum.Format(_T("%d"), gd.nSprocket);
        CSize szNum = pDC->GetTextExtent(sNum);
        pDC->TextOut(xBar + (barW - szNum.cx) / 2,
                     plotBottom + 4, sNum);

        // Zähne-Beschriftung unter Ritzel-Nr.
        CString sTeeth;
        sTeeth.Format(_T("(%dZ)"), gd.nTeeth);
        CSize szTeeth = pDC->GetTextExtent(sTeeth);
        pDC->TextOut(xBar + (barW - szTeeth.cx) / 2,
                     plotBottom + 4 + szNum.cy, sTeeth);
    }

    // --- X-Achsentitel ---
    pDC->SetTextColor(RGB(60, 80, 110));
    CString xTitle = _T("Ritzel Nr. (Zähne)");
    CSize   szX    = pDC->GetTextExtent(xTitle);
    pDC->TextOut(plotLeft + (plotW - szX.cx) / 2,
                 rc.bottom - 16, xTitle);

    // --- Diagramm-Titel ---
    {
        LOGFONT lf = {};
        fntSmall.GetLogFont(&lf);
        lf.lfHeight = 13;
        lf.lfWeight = FW_BOLD;
        CFont titleFont;
        titleFont.CreateFontIndirect(&lf);
        CFont* pOldTitle = pDC->SelectObject(&titleFont);
        pDC->SetTextColor(RGB(20, 50, 100));

        CString sTitle;
        sTitle.Format(
            _T("Entfaltung  –  Kettenrad: %d Zähne  |  Rad: %.0f mm"),
            m_nChainringTeeth, m_dWheelDiameter * 1000.0);
        CSize szTitle = pDC->GetTextExtent(sTitle);
        pDC->TextOut(rc.left + (rc.Width() - szTitle.cx) / 2,
                     rc.top + 8, sTitle);
        pDC->SelectObject(pOldTitle);
    }

    pDC->SelectObject(pOldFont);
}

// ===========================================================
// CEBike2Dlg
// ===========================================================

CEBike2Dlg::CEBike2Dlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_EBIKE2_DIALOG, pParent)
    , m_nChainringTeeth(38)
    , m_dWheelDiameter(0.584)   // 27.5" (584 mm)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // Standard 10-fach E-Bike Kassette (z.B. Shimano Deore 10-speed)
    // Ritzel: 11 – 13 – 15 – 17 – 19 – 21 – 24 – 28 – 34 – 42
    m_sprocketTeeth = { 11, 13, 15, 17, 19, 21, 24, 28, 34, 42 };
}

void CEBike2Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_GEAR_TABLE,  m_listGearTable);
    DDX_Control(pDX, IDC_COMBO_WHEEL_SIZE, m_comboWheelSize);
    DDX_Control(pDX, IDC_STATIC_CHART,     m_chartCtrl);
}

BEGIN_MESSAGE_MAP(CEBike2Dlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CEBike2Dlg::OnBnClickedButtonCalculate)
END_MESSAGE_MAP()

// -----------------------------------------------------------
BOOL CEBike2Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // Fenstertitel mit korrekten Umlauten setzen
    SetWindowText(_T("E-Bike \u00DCbersetzungsverh\u00E4ltnis-Rechner"));

    InitializeControls();
    CalculateGearRatios();
    UpdateTable();

    return TRUE;
}

void CEBike2Dlg::InitializeControls()
{
    // Kettenrad-Zähne
    SetDlgItemInt(IDC_EDIT_CHAINRING, m_nChainringTeeth);

    // Kassetten-Info
    SetDlgItemText(IDC_STATIC_CASSETTE,
                   CString(_T("Kassette: ")) + BuildCassetteString());

    // Radgröße-Combobox befüllen
    // Index 0 = 26",  1 = 27.5",  2 = 28"/700c,  3 = 29"
    m_comboWheelSize.AddString(_T("26\"   (559 mm)"));
    m_comboWheelSize.AddString(_T("27.5\" (584 mm)"));
    m_comboWheelSize.AddString(_T("28\" / 700c (622 mm)"));
    m_comboWheelSize.AddString(_T("29\"   (622 mm)"));
    m_comboWheelSize.SetCurSel(1); // Standard: 27.5"

    // ListView-Spalten
    m_listGearTable.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    CRect rc;
    m_listGearTable.GetClientRect(&rc);
    int colW = rc.Width() / 4;

    m_listGearTable.InsertColumn(0, _T("Ritzel Nr."),              LVCFMT_CENTER, colW);
    m_listGearTable.InsertColumn(1, _T("Z\u00E4hne"),             LVCFMT_CENTER, colW);
    m_listGearTable.InsertColumn(2, _T("\u00DCbersetzung i"),      LVCFMT_CENTER, colW);
    m_listGearTable.InsertColumn(3, _T("Entfaltung m/U"),          LVCFMT_CENTER, colW);
}

// -----------------------------------------------------------
CString CEBike2Dlg::BuildCassetteString() const
{
    CString s;
    for (int i = 0; i < static_cast<int>(m_sprocketTeeth.size()); ++i)
    {
        CString t;
        t.Format(_T("%d"), m_sprocketTeeth[i]);
        if (i > 0) s += _T(" – ");
        s += t;
    }
    s += _T(" Zähne");
    return s;
}

// -----------------------------------------------------------
void CEBike2Dlg::CalculateGearRatios()
{
    // Kettenrad-Zähne auslesen
    BOOL bOK = FALSE;
    int  nTeeth = GetDlgItemInt(IDC_EDIT_CHAINRING, &bOK, FALSE);
    m_nChainringTeeth = (bOK && nTeeth >= 1 && nTeeth <= 99) ? nTeeth : 38;

    // Raddurchmesser aus Combobox
    const double diameters[] = { 0.559, 0.584, 0.622, 0.622 };
    int nSel = m_comboWheelSize.GetCurSel();
    m_dWheelDiameter = (nSel >= 0 && nSel < 4) ? diameters[nSel] : 0.584;

    double circumference = M_PI * m_dWheelDiameter; // Radumfang in Metern

    m_gearData.clear();
    for (int i = 0; i < static_cast<int>(m_sprocketTeeth.size()); ++i)
    {
        GearData gd;
        gd.nSprocket    = i + 1;
        gd.nTeeth       = m_sprocketTeeth[i];
        gd.dRatio       = static_cast<double>(m_nChainringTeeth) / gd.nTeeth;
        gd.dDevelopment = gd.dRatio * circumference;
        m_gearData.push_back(gd);
    }
}

// -----------------------------------------------------------
void CEBike2Dlg::UpdateTable()
{
    m_listGearTable.DeleteAllItems();

    for (int i = 0; i < static_cast<int>(m_gearData.size()); ++i)
    {
        const GearData& gd = m_gearData[i];
        CString s;

        // Spalte 0: Ritzel-Nummer
        s.Format(_T("%d"), gd.nSprocket);
        m_listGearTable.InsertItem(i, s);

        // Spalte 1: Zähne
        s.Format(_T("%d"), gd.nTeeth);
        m_listGearTable.SetItemText(i, 1, s);

        // Spalte 2: Übersetzungsverhältnis
        s.Format(_T("%.3f"), gd.dRatio);
        m_listGearTable.SetItemText(i, 2, s);

        // Spalte 3: Entfaltung in m/Umdrehung
        s.Format(_T("%.2f"), gd.dDevelopment);
        m_listGearTable.SetItemText(i, 3, s);
    }

    // Diagramm mit aktuellen Daten aktualisieren
    m_chartCtrl.SetGearData(&m_gearData,
                             m_nChainringTeeth,
                             m_dWheelDiameter);
}

// -----------------------------------------------------------
void CEBike2Dlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND,
                    reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        dc.DrawIcon((rect.Width()  - cxIcon + 1) / 2,
                    (rect.Height() - cyIcon + 1) / 2, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CEBike2Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CEBike2Dlg::OnBnClickedButtonCalculate()
{
    CalculateGearRatios();
    UpdateTable();
}
