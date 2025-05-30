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

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
std::wstring StringToWString(const std::string& str);

DecisionLoop* g_decisionLoop = nullptr;

// Store module panel visibility
static std::map<int, HWND> layerPanels;
static int currentLayer = 0; // 0 = none, 1 = Cognitive, 2 = Value, 3 = Systems, 4 = Implementation

// Color brushes
static HBRUSH hBrushCognitive = nullptr;
static HBRUSH hBrushValue = nullptr;
static HBRUSH hBrushSystems = nullptr;
static HBRUSH hBrushImplementation = nullptr;

// Add these constants at the top of the file with other static variables
static HPEN hPenBorder = nullptr;
static HPEN hPenHighlight = nullptr;

// Add this function before MainDlgProc
void DrawModernButton(LPDRAWITEMSTRUCT pdis)
{
    RECT rcItem = pdis->rcItem;
    HDC hdc = pdis->hDC;
    bool isPressed = (pdis->itemState & ODS_SELECTED) != 0;
    bool isFocused = (pdis->itemState & ODS_FOCUS) != 0;
    bool isDisabled = (pdis->itemState & ODS_DISABLED) != 0;
    bool isHot = (pdis->itemState & ODS_HOTLIGHT) != 0;

    // Create gradient background
    COLORREF baseColor;
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
        // Initialize common controls
        INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES };
        InitCommonControlsEx(&icex);

        HINSTANCE hInstance = GetModuleHandle(NULL);
        
        // Set application icons
        HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DECISIONLOOP));
        SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL)));

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

        for (const auto& pair : layerPanels)
        {
            ShowWindow(pair.second, SW_HIDE);
            EnableWindow(pair.second, FALSE);
            HWND hChild = GetNextDlgGroupItem(hDlg, pair.second, FALSE);
            while (hChild)
            {
                ShowWindow(hChild, SW_HIDE);
                EnableWindow(hChild, FALSE);
                hChild = GetNextDlgGroupItem(hDlg, hChild, TRUE);
            }
        }

        HWND hToolTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
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

    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hDlg, &rc);
        
        // Create and use a solid brush instead of gradient
        HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);
        
        return TRUE;
    }

    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        HWND hWnd = (HWND)lParam;
        int id = GetDlgCtrlID(hWnd);
        if (id == IDC_BUTTON_COGNITIVE || id == IDC_BUTTON_COGNITIVE_READINESS ||
            id == IDC_BUTTON_EMOTIONAL_CALIB || id == IDC_BUTTON_IMPULSE_ALIGN)
        {
            SetBkColor(hdc, COLOR_COGNITIVE);
            return (INT_PTR)hBrushCognitive;
        }
        else if (id == IDC_BUTTON_VALUE || id == IDC_BUTTON_CORE_VALUES ||
                 id == IDC_BUTTON_ETHICAL_COHERENCE || id == IDC_BUTTON_TIME_HORIZONS)
        {
            SetBkColor(hdc, COLOR_VALUE);
            return (INT_PTR)hBrushValue;
        }
        else if (id == IDC_BUTTON_SYSTEMS || id == IDC_BUTTON_BIAS_TESTING ||
                 id == IDC_BUTTON_UNCERTAINTY_MGMT || id == IDC_BUTTON_INTERNAL_DIALOGUE ||
                 id == IDC_BUTTON_SOCIAL_ECO)
        {
            SetBkColor(hdc, COLOR_SYSTEMS);
            return (INT_PTR)hBrushSystems;
        }
        else if (id == IDC_BUTTON_IMPLEMENTATION || id == IDC_BUTTON_FEEDBACK_LOOPS)
        {
            SetBkColor(hdc, COLOR_IMPLEMENTATION);
            return (INT_PTR)hBrushImplementation;
        }
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_BUTTON_COGNITIVE)) {
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        return (INT_PTR)FALSE;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        int notifyCode = HIWORD(wParam);

        // Handle Layer Button Clicks (Toggle Panels)
        if (notifyCode == BN_CLICKED && (id == IDC_BUTTON_COGNITIVE || id == IDC_BUTTON_VALUE ||
            id == IDC_BUTTON_SYSTEMS || id == IDC_BUTTON_IMPLEMENTATION))
        {
            int newLayer = id;
            if (newLayer == currentLayer)
            {
                // Hide current panel
                ShowWindow(layerPanels[currentLayer], SW_HIDE);
                EnableWindow(layerPanels[currentLayer], FALSE);
                HWND hChild = GetNextDlgGroupItem(hDlg, layerPanels[currentLayer], FALSE);
                while (hChild)
                {
                    ShowWindow(hChild, SW_HIDE);
                    EnableWindow(hChild, FALSE);
                    hChild = GetNextDlgGroupItem(hDlg, hChild, TRUE);
                }
                currentLayer = 0;
            }
            else
            {
                // Hide current panel if any
                if (currentLayer != 0)
                {
                    ShowWindow(layerPanels[currentLayer], SW_HIDE);
                    EnableWindow(layerPanels[currentLayer], FALSE);
                    HWND hChild = GetNextDlgGroupItem(hDlg, layerPanels[currentLayer], FALSE);
                    while (hChild)
                    {
                        ShowWindow(hChild, SW_HIDE);
                        EnableWindow(hChild, FALSE);
                        hChild = GetNextDlgGroupItem(hDlg, hChild, TRUE);
                    }
                }
                // Show new panel
                ShowWindow(layerPanels[newLayer], SW_SHOW);
                EnableWindow(layerPanels[newLayer], TRUE);
                HWND hChild = GetNextDlgGroupItem(hDlg, layerPanels[newLayer], FALSE);
                while (hChild)
                {
                    ShowWindow(hChild, SW_SHOW);
                    EnableWindow(hChild, TRUE);
                    hChild = GetNextDlgGroupItem(hDlg, hChild, TRUE);
                }
                currentLayer = newLayer;
            }
            return (INT_PTR)TRUE;
        }

        // Handle Module Button Clicks
        if (notifyCode == BN_CLICKED)
        {
            std::unique_ptr<DecisionComponent> component;
            switch (id)
            {
            case IDC_BUTTON_COGNITIVE_READINESS:
                component = std::make_unique<CognitiveReadiness>();
                break;
            case IDC_BUTTON_EMOTIONAL_CALIB:
                component = std::make_unique<EmotionalCalibration>();
                break;
            case IDC_BUTTON_IMPULSE_ALIGN:
                component = std::make_unique<ImpulseAlignment>();
                break;
            case IDC_BUTTON_CORE_VALUES:
                component = std::make_unique<CoreMetaValueSpecification>();
                break;
            case IDC_BUTTON_ETHICAL_COHERENCE:
                component = std::make_unique<EthicalCoherence>();
                break;
            case IDC_BUTTON_TIME_HORIZONS:
                component = std::make_unique<TimeHorizonIntegration>();
                break;
            case IDC_BUTTON_BIAS_TESTING:
                component = std::make_unique<BiasAssumptionTesting>();
                break;
            case IDC_BUTTON_UNCERTAINTY_MGMT:
                component = std::make_unique<UncertaintyComplexityManagement>();
                break;
            case IDC_BUTTON_INTERNAL_DIALOGUE:
                component = std::make_unique<InternalRoleDialogue>();
                break;
            case IDC_BUTTON_SOCIAL_ECO:
                component = std::make_unique<SocialEcologicalAnalysis>();
                break;
            case IDC_BUTTON_FEEDBACK_LOOPS:
                component = std::make_unique<FeedbackLearningIntegration>();
                break;
            case IDC_BUTTON_FAST_LOOP:
                g_decisionLoop->runFastLoop(hDlg);
                break;
            case IDC_BUTTON_TRIAGE_MODE:
                g_decisionLoop->runTriageMode(hDlg);
                break;
            case IDC_BUTTON_GROUP_MODE:
                g_decisionLoop->runGroupMode(hDlg);
                break;
            case IDC_BUTTON_PROFILE:
                MessageBox(hDlg, L"User profile and settings (not implemented).", L"Profile", MB_OK);
                break;
            case IDCANCEL:
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }

            if (component)
            {
                component->execute(hDlg);
                float score = component->getScore();
                moduleScores[id] = score;

                // Update progress bar
                HWND hProgress = NULL;
                switch (id)
                {
                case IDC_BUTTON_COGNITIVE_READINESS: hProgress = GetDlgItem(hDlg, IDC_STATIC_COGNITIVE_SCORE); break;
                case IDC_BUTTON_EMOTIONAL_CALIB: hProgress = GetDlgItem(hDlg, IDC_STATIC_EMOTIONAL_SCORE); break;
                case IDC_BUTTON_IMPULSE_ALIGN: hProgress = GetDlgItem(hDlg, IDC_STATIC_IMPULSE_SCORE); break;
                case IDC_BUTTON_CORE_VALUES: hProgress = GetDlgItem(hDlg, IDC_STATIC_VALUES_SCORE); break;
                case IDC_BUTTON_ETHICAL_COHERENCE: hProgress = GetDlgItem(hDlg, IDC_STATIC_ETHICAL_SCORE); break;
                case IDC_BUTTON_TIME_HORIZONS: hProgress = GetDlgItem(hDlg, IDC_STATIC_HORIZONS_SCORE); break;
                case IDC_BUTTON_BIAS_TESTING: hProgress = GetDlgItem(hDlg, IDC_STATIC_BIAS_SCORE); break;
                case IDC_BUTTON_UNCERTAINTY_MGMT: hProgress = GetDlgItem(hDlg, IDC_STATIC_UNCERTAINTY_SCORE); break;
                case IDC_BUTTON_INTERNAL_DIALOGUE: hProgress = GetDlgItem(hDlg, IDC_STATIC_DIALOGUE_SCORE); break;
                case IDC_BUTTON_SOCIAL_ECO: hProgress = GetDlgItem(hDlg, IDC_STATIC_SOCIAL_SCORE); break;
                case IDC_BUTTON_FEEDBACK_LOOPS: hProgress = GetDlgItem(hDlg, IDC_STATIC_FEEDBACK_SCORE); break;
                }
                if (hProgress)
                {
                    SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 10));
                    SendMessage(hProgress, PBM_SETPOS, (WPARAM)(score), 0);
                }

                // Display report
                std::wstring wReport = StringToWString(component->getReport());
                SetWindowText(hEditReport, wReport.c_str());
            }
        }
        break;
    }

    case WM_DRAWITEM:
    {
        if (((LPDRAWITEMSTRUCT)lParam)->CtlID == IDC_BUTTON_COGNITIVE) {
            DrawModernButton((LPDRAWITEMSTRUCT)lParam);
            return TRUE;
        }

        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
        int id = pdis->CtlID;
        COLORREF btnColor = RGB(255, 255, 255); // Default
        if (id == IDC_BUTTON_COGNITIVE || id == IDC_BUTTON_COGNITIVE_READINESS ||
            id == IDC_BUTTON_EMOTIONAL_CALIB || id == IDC_BUTTON_IMPULSE_ALIGN)
            btnColor = COLOR_COGNITIVE;
        else if (id == IDC_BUTTON_VALUE || id == IDC_BUTTON_CORE_VALUES ||
            id == IDC_BUTTON_ETHICAL_COHERENCE || id == IDC_BUTTON_TIME_HORIZONS)
            btnColor = COLOR_VALUE;
        else if (id == IDC_BUTTON_SYSTEMS || id == IDC_BUTTON_BIAS_TESTING ||
            id == IDC_BUTTON_UNCERTAINTY_MGMT || id == IDC_BUTTON_INTERNAL_DIALOGUE ||
            id == IDC_BUTTON_SOCIAL_ECO)
            btnColor = COLOR_SYSTEMS;
        else if (id == IDC_BUTTON_IMPLEMENTATION || id == IDC_BUTTON_FEEDBACK_LOOPS)
            btnColor = COLOR_IMPLEMENTATION;

        HBRUSH hBrush = CreateSolidBrush(btnColor);
        FillRect(pdis->hDC, &pdis->rcItem, hBrush);
        DeleteObject(hBrush);

        HBRUSH hBorderBrush = CreateSolidBrush(RGB(150, 150, 150));
        FrameRect(pdis->hDC, &pdis->rcItem, hBorderBrush);
        DeleteObject(hBorderBrush);

        WCHAR szText[256];
        GetWindowText(pdis->hwndItem, szText, 256);
        SetBkMode(pdis->hDC, TRANSPARENT);
        SetTextColor(pdis->hDC, RGB(0, 0, 0));
        DrawText(pdis->hDC, szText, -1, &pdis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        return TRUE;
    }

    case WM_DESTROY:
    {
        DeleteObject(hBrushCognitive);
        DeleteObject(hBrushValue);
        DeleteObject(hBrushSystems);
        DeleteObject(hBrushImplementation);

        if (hPenBorder) DeleteObject(hPenBorder);
        if (hPenHighlight) DeleteObject(hPenHighlight);
        
        return (INT_PTR)TRUE;
    }

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

std::wstring StringToWString(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}


