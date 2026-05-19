// EBike2Dlg.h : Headerdatei für das Hauptdialogfenster
//              Enthält Tabelle und Grafik für das Übersetzungsverhältnis
//              eines E-Bikes mit einem Kettenrad und 10 Ritzeln
//
#pragma once

#include <vector>
#include <afxcmn.h>

// ============================================================
// Datenstruktur für einen Gang (Ritzel-Kombination)
// ============================================================
struct GearData
{
    int    nSprocket;       // Ritzel-Nummer (1 = kleinste Übersetzung, 10 = größte)
    int    nTeeth;          // Anzahl Zähne am Ritzel
    double dRatio;          // Übersetzungsverhältnis i = Z_Kettenrad / Z_Ritzel
    double dDevelopment;    // Entfaltung in m/Umdrehung = i × Radumfang
};

// ============================================================
// CChartCtrl – Balkendiagramm-Steuerelement (CStatic-Subclass)
// Zeichnet das Übersetzungs-Balkendiagramm mit GDI
// ============================================================
class CChartCtrl : public CStatic
{
public:
    CChartCtrl();

    // Zeigerverweis auf die berechneten Gangdaten setzen
    void SetGearData(const std::vector<GearData>* pData,
                     int nChainring, double dWheelDiameter);

protected:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    DECLARE_MESSAGE_MAP()

private:
    const std::vector<GearData>* m_pGearData;
    int    m_nChainringTeeth;
    double m_dWheelDiameter; // in Metern

    void DrawBarChart(CDC* pDC, const CRect& rc);
    void DrawGradientRect(CDC* pDC, const CRect& rc,
                          COLORREF clrTop, COLORREF clrBottom);
};

// ============================================================
// CEBike2Dlg – Hauptdialog der Anwendung
// ============================================================
class CEBike2Dlg : public CDialogEx
{
// Konstruktion
public:
    explicit CEBike2Dlg(CWnd* pParent = nullptr);

// Dialogdaten
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_EBIKE2_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

// Implementierung
protected:
    HICON          m_hIcon;
    CListCtrl      m_listGearTable;    // Tabelle mit Gangdaten
    CComboBox      m_comboWheelSize;   // Radgrößen-Auswahl
    CChartCtrl     m_chartCtrl;        // Balkendiagramm-Steuerelement

    int    m_nChainringTeeth;          // Zähne am Kettenrad
    double m_dWheelDiameter;           // Raddurchmesser in Metern

    std::vector<GearData> m_gearData;  // Berechnete Gangdaten
    std::vector<int>      m_sprocketTeeth; // Ritzelzähne der 10-fach-Kassette

    // Hilfsmethoden
    void InitializeControls();
    void CalculateGearRatios();
    void UpdateTable();
    CString BuildCassetteString() const;

    // Nachrichtenhandler
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedButtonCalculate();

    DECLARE_MESSAGE_MAP()
};
