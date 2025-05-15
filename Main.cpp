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

        hEditReport = GetDlgItem(hDlg, IDC_EDIT_REPORT);

        // Create color brushes
        hBrushCognitive = CreateSolidBrush(COLOR_COGNITIVE);
        hBrushValue = CreateSolidBrush(COLOR_VALUE);
        hBrushSystems = CreateSolidBrush(COLOR_SYSTEMS);
        hBrushImplementation = CreateSolidBrush(COLOR_IMPLEMENTATION);

        // Map layer buttons to their module panels (group boxes)
        layerPanels[IDC_BUTTON_COGNITIVE] = GetDlgItem(hDlg, IDC_BUTTON_COGNITIVE_READINESS);
        layerPanels[IDC_BUTTON_VALUE] = GetDlgItem(hDlg, IDC_BUTTON_CORE_VALUES);
        layerPanels[IDC_BUTTON_SYSTEMS] = GetDlgItem(hDlg, IDC_BUTTON_BIAS_TESTING);
        layerPanels[IDC_BUTTON_IMPLEMENTATION] = GetDlgItem(hDlg, IDC_BUTTON_FEEDBACK_LOOPS);

        // Hide all module panels initially
        for (const auto& pair : layerPanels)
        {
            ShowWindow(pair.second, SW_HIDE);
            EnableWindow(pair.second, FALSE);
            // Hide all child controls (module buttons and progress bars)
            HWND hChild = GetNextDlgGroupItem(hDlg, pair.second, FALSE);
            while (hChild)
            {
                ShowWindow(hChild, SW_HIDE);
                EnableWindow(hChild, FALSE);
                hChild = GetNextDlgGroupItem(hDlg, hChild, TRUE);
            }
        }

        // Set tooltips
        HWND hToolTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hDlg, NULL, GetModuleHandle(NULL), NULL);
        TOOLINFO toolInfo = { sizeof(toolInfo), TTF_IDISHWND | TTF_SUBCLASS, hDlg, 0, {0, 0, 0, 0}, NULL, NULL };

        // Store tooltip strings to ensure lifetime
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

        // Set dialog icon
        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_DECISIONLOOP));
        SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        DestroyIcon(hIcon);

        return (INT_PTR)TRUE;
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

    case WM_DESTROY:
    {
        DeleteObject(hBrushCognitive);
        DeleteObject(hBrushValue);
        DeleteObject(hBrushSystems);
        DeleteObject(hBrushImplementation);
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
