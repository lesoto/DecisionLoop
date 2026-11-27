#include "framework.h"
#include "DecisionLoop.h"
#include "Resource.h"
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <map>
#include "CognitiveEmotionalLayer.h"
#include "ValueEthicalLayer.h"
#include "SystemsContextLayer.h"
#include "ImplementationLearningLayer.h"
#include "SpecializedModes.h"

#pragma comment(lib, "Comctl32.lib")

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

// Forward declarations
INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
std::wstring StringToWString(const std::string& str);

// Global variables
DecisionLoop* g_decisionLoop = nullptr;
static std::map<int, HWND> layerPanels;
static int currentLayer = 0;
static HBRUSH hBrushCognitive = nullptr;
static HBRUSH hBrushValue = nullptr;
static HBRUSH hBrushSystems = nullptr;
static HBRUSH hBrushImplementation = nullptr;
static HPEN hPenBorder = nullptr;
static HPEN hPenHighlight = nullptr;

static void DrawModernButton(LPDRAWITEMSTRUCT pdis)
{
    RECT rcItem = pdis->rcItem;
    HDC hdc = pdis->hDC;
    bool isPressed = (pdis->itemState & ODS_SELECTED) != 0;
    bool isFocused = (pdis->itemState & ODS_FOCUS) != 0;
    bool isDisabled = (pdis->itemState & ODS_DISABLED) != 0;
    bool isHot = (pdis->itemState & ODS_HOTLIGHT) != 0;

    COLORREF baseColor = RGB(240, 240, 240); 
    switch (pdis->CtlID)
    {
    case IDC_BUTTON_COGNITIVE:
        baseColor = COLOR_COGNITIVE;
        break;
    case IDC_BUTTON_VALUE:
        baseColor = COLOR_VALUE;
        break;
    case IDC_BUTTON_SYSTEMS:
        baseColor = COLOR_SYSTEMS;
        break;
    case IDC_BUTTON_IMPLEMENTATION:
        baseColor = COLOR_IMPLEMENTATION;
        break;
    default:
        baseColor = RGB(240, 240, 240); // Default color
    }

    // Adjust colors based on button state
    COLORREF fillColor = baseColor;
    if (isPressed)
    {
        // Darken color when pressed
        fillColor = RGB(
            GetRValue(baseColor) * 0.8,
            GetGValue(baseColor) * 0.8,
            GetBValue(baseColor) * 0.8
        );
    }
    else if (isHot)
    {
        // Lighten color when hovered
        fillColor = RGB(
            min(255, GetRValue(baseColor) * 1.2),
            min(255, GetGValue(baseColor) * 1.2),
            min(255, GetBValue(baseColor) * 1.2)
        );
    }

    // Fill background
    HBRUSH hBrush = CreateSolidBrush(fillColor);
    FillRect(hdc, &rcItem, hBrush);
    DeleteObject(hBrush);

    // Draw border
    if (!isPressed)
    {
        // Draw highlight on top and left edges
        HPEN hHighlight = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hHighlight);
        MoveToEx(hdc, rcItem.left, rcItem.bottom - 1, NULL);
        LineTo(hdc, rcItem.left, rcItem.top);
        LineTo(hdc, rcItem.right - 1, rcItem.top);
        SelectObject(hdc, hOldPen);
        DeleteObject(hHighlight);
    }

    // Draw shadow on bottom and right edges
    HPEN hShadow = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hShadow);
    MoveToEx(hdc, rcItem.right - 1, rcItem.top, NULL);
    LineTo(hdc, rcItem.right - 1, rcItem.bottom - 1);
    LineTo(hdc, rcItem.left, rcItem.bottom - 1);
    SelectObject(hdc, hOldPen);
    DeleteObject(hShadow);

    // Draw text
    SetBkMode(hdc, TRANSPARENT);
    COLORREF textColor = isDisabled ? RGB(160, 160, 160) : RGB(0, 0, 0);
    SetTextColor(hdc, textColor);

    // Get button text
    WCHAR szText[256];
    GetWindowText(pdis->hwndItem, szText, 256);

    // Adjust text position if button is pressed
    RECT rcText = rcItem;
    if (isPressed)
    {
        OffsetRect(&rcText, 1, 1);
    }

    // Draw text with antialiasing
    HFONT hFont = (HFONT)SendMessage(pdis->hwndItem, WM_GETFONT, 0, 0);
    if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    DrawText(hdc, szText, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, hOldFont);

    // Draw focus rect if needed
    if (isFocused && !isPressed)
    {
        RECT rcFocus = rcItem;
        InflateRect(&rcFocus, -3, -3);
        DrawFocusRect(hdc, &rcFocus);
    }
}

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditReport;
    static std::map<int, float> moduleScores; // Store scores for progress bars
    static std::vector<std::wstring> tooltipStrings; // Store tooltip strings

    switch (message)
    {
    

    case WM_INITDIALOG:
    {
        if (!g_decisionLoop)
        {
            MessageBox(hDlg, L"Failed to initialize DecisionLoop.", L"Error", MB_ICONERROR);
            EndDialog(hDlg, -1);
            return (INT_PTR)FALSE;
        }

        INITCOMMONCONTROLSEX iccex = { 
            sizeof(INITCOMMONCONTROLSEX),
            ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES 
        };
        InitCommonControlsEx(&iccex);

        HINSTANCE hInstance = GetModuleHandle(NULL);

        // Set application icons
        HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DECISIONLOOP));
        SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL)));

        // Create toolbar
        HWND hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
            WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
            0, 0, 0, 0, hDlg, (HMENU)IDR_TOOLBAR, hInstance, NULL);

        SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

        // Set button icons
        SendDlgItemMessage(hDlg, IDC_BUTTON_COGNITIVE_READINESS, BM_SETIMAGE, IMAGE_ICON,
            (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BRAIN)));
        SendDlgItemMessage(hDlg, IDC_BUTTON_EMOTIONAL_CALIB, BM_SETIMAGE, IMAGE_ICON,
            (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HEART)));
        SendDlgItemMessage(hDlg, IDC_BUTTON_COGNITIVE, BM_SETIMAGE, IMAGE_ICON,
            (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BRAIN)));

        hEditReport = GetDlgItem(hDlg, IDC_EDIT_REPORT);

        // Create color brushes
        hBrushCognitive = CreateSolidBrush(COLOR_COGNITIVE);
        hBrushValue = CreateSolidBrush(COLOR_VALUE);
        hBrushSystems = CreateSolidBrush(COLOR_SYSTEMS);
        hBrushImplementation = CreateSolidBrush(COLOR_IMPLEMENTATION);

        layerPanels[IDC_BUTTON_COGNITIVE] = GetDlgItem(hDlg, IDC_BUTTON_COGNITIVE_READINESS);
        layerPanels[IDC_BUTTON_VALUE] = GetDlgItem(hDlg, IDC_BUTTON_CORE_VALUES);
        layerPanels[IDC_BUTTON_SYSTEMS] = GetDlgItem(hDlg, IDC_BUTTON_BIAS_TESTING);
        layerPanels[IDC_BUTTON_IMPLEMENTATION] = GetDlgItem(hDlg, IDC_BUTTON_FEEDBACK_LOOPS);

        // Enumerate siblings once. Dialog navigation wraps and skips hidden controls.
        for (HWND child = GetWindow(hDlg, GW_CHILD); child;
             child = GetWindow(child, GW_HWNDNEXT))
        {
            const int controlId = GetDlgCtrlID(child);
            if (controlId >= IDC_BUTTON_COGNITIVE_READINESS &&
                controlId <= IDC_STATIC_FEEDBACK_SCORE)
            {
                ShowWindow(child, SW_HIDE);
                EnableWindow(child, FALSE);
            }
        }
        HWND hToolTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            hDlg, NULL, hInstance, NULL);

        TOOLINFO toolInfo = { sizeof(toolInfo), TTF_IDISHWND | TTF_SUBCLASS, hDlg, 0, {0, 0, 0, 0}, NULL, NULL };

        tooltipStrings = {
            L"Assess mental readiness and emotional alignment",
            L"Evaluate core values and ethical alignment",
            L"Analyze biases and contextual complexity",
            L"Implement decisions and learn from outcomes",
            L"Quick decisions with limited time",
            L"Urgent, high-stakes decisions",
            L"Collaborative team decisions"
        };

        std::vector<int> buttonIds = {
            IDC_BUTTON_COGNITIVE,
            IDC_BUTTON_VALUE,
            IDC_BUTTON_SYSTEMS,
            IDC_BUTTON_IMPLEMENTATION,
            IDC_BUTTON_FAST_LOOP,
            IDC_BUTTON_TRIAGE_MODE,
            IDC_BUTTON_GROUP_MODE
        };

        for (size_t i = 0; i < buttonIds.size(); ++i)
        {
            toolInfo.uId = (UINT_PTR)GetDlgItem(hDlg, buttonIds[i]);
            toolInfo.lpszText = const_cast<LPWSTR>(tooltipStrings[i].c_str());
            SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
        }

        DWORD buttonStyle = GetWindowLong(GetDlgItem(hDlg, IDC_BUTTON_COGNITIVE), GWL_STYLE);
        buttonStyle |= BS_OWNERDRAW;
        SetWindowLong(GetDlgItem(hDlg, IDC_BUTTON_COGNITIVE), GWL_STYLE, buttonStyle);
        SetWindowLong(GetDlgItem(hDlg, IDC_BUTTON_VALUE), GWL_STYLE, buttonStyle);
        SetWindowLong(GetDlgItem(hDlg, IDC_BUTTON_SYSTEMS), GWL_STYLE, buttonStyle);
        SetWindowLong(GetDlgItem(hDlg, IDC_BUTTON_IMPLEMENTATION), GWL_STYLE, buttonStyle);

        return (INT_PTR)TRUE;
    }

    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hDlg, &rc);

        HWND hToolBar = GetDlgItem(hDlg, IDR_TOOLBAR);
        if (hToolBar)
        {
            SendMessage(hToolBar, TB_AUTOSIZE, 0, 0);
        }

        InvalidateRect(hDlg, NULL, TRUE);
        return TRUE;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
        break;
    }

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
        switch (pdis->CtlID)
        {
        case IDC_BUTTON_COGNITIVE:
        case IDC_BUTTON_VALUE:
        case IDC_BUTTON_SYSTEMS:
        case IDC_BUTTON_IMPLEMENTATION:
            DrawModernButton(pdis);
            return TRUE;
        }
        break;
    }

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return (INT_PTR)TRUE;

    case WM_CTLCOLORBTN:
    {
        HDC hdcButton = (HDC)wParam;
        HWND hwndButton = (HWND)lParam;
        int buttonId = GetDlgCtrlID(hwndButton);
        
        switch (buttonId)
        {
        case IDC_BUTTON_COGNITIVE:
            return (INT_PTR)hBrushCognitive;
        case IDC_BUTTON_VALUE:
            return (INT_PTR)hBrushValue;
        case IDC_BUTTON_SYSTEMS:
            return (INT_PTR)hBrushSystems;
        case IDC_BUTTON_IMPLEMENTATION:
            return (INT_PTR)hBrushImplementation;
        }
        break;
    }

    case WM_DESTROY:
    {
        if (g_decisionLoop)
        {
            delete g_decisionLoop;
            g_decisionLoop = nullptr;
        }
        
        if (hBrushCognitive) DeleteObject(hBrushCognitive);
        if (hBrushValue) DeleteObject(hBrushValue);
        if (hBrushSystems) DeleteObject(hBrushSystems);
        if (hBrushImplementation) DeleteObject(hBrushImplementation);
        if (hPenBorder) DeleteObject(hPenBorder);
        if (hPenHighlight) DeleteObject(hPenHighlight);
        
        return (INT_PTR)TRUE;
    }

    } 
    return (INT_PTR)FALSE;
} 

std::wstring StringToWString(const std::string& str)
{
    // First call to get required size (this is correct)
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);

    // Fix the second call - it needs 6 parameters, not 5
    MultiByteToWideChar(
        CP_UTF8,                    // CodePage
        0,                          // dwFlags
        str.c_str(),               // lpMultiByteStr
        (int)str.size(),           // cbMultiByte
        &wstrTo[0],                // lpWideCharStr
        size_needed                 // cchWideChar
    );
    return wstrTo;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,      // Note: Changed to LPSTR for ANSI
    int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    INITCOMMONCONTROLSEX iccex = { 
        sizeof(INITCOMMONCONTROLSEX),
        ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES
    };
    if (!InitCommonControlsEx(&iccex))
    {
        MessageBox(NULL, L"Failed to initialize common controls.", L"Error", MB_ICONERROR);
        return 1;
    }

    g_decisionLoop = new DecisionLoop();
    if (!g_decisionLoop)
    {
        MessageBox(NULL, L"Failed to initialize DecisionLoop.", L"Error", MB_ICONERROR);
        return 1;
    }

    INT_PTR result = DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, MainDlgProc);
    
    if (result == -1)
    {
        DWORD error = GetLastError();
        WCHAR errorMsg[256];
        swprintf_s(errorMsg, L"Dialog creation failed. Error code: %d", error);
        MessageBox(NULL, errorMsg, L"Error", MB_ICONERROR);
    }

    delete g_decisionLoop;
    g_decisionLoop = nullptr;

    return (int)result;
}