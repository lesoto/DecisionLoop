#include "framework.h"
#include "DecisionLoop.h"
#include "Resource.h"
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <sstream>

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
std::wstring StringToWString(const std::string& str);

DecisionLoop* g_decisionLoop = nullptr;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    g_decisionLoop = new DecisionLoop();

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), nullptr, MainDlgProc);

    delete g_decisionLoop;
    g_decisionLoop = nullptr;

    return 0;
}

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND hComboMode;
    static HWND hEditReport;

    switch (message)
    {
    case WM_INITDIALOG:
        {
            hComboMode = GetDlgItem(hDlg, IDC_COMBO_MODE);
            hEditReport = GetDlgItem(hDlg, IDC_EDIT_REPORT);

            SendMessage(hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Full Loop");
            SendMessage(hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Fast Loop");
            SendMessage(hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Triage Mode");
            SendMessage(hComboMode, CB_ADDSTRING, 0, (LPARAM)L"Group Mode");
            SendMessage(hComboMode, CB_SETCURSEL, 0, 0);

            HICON hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_DECISIONLOOP));
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            DestroyIcon(hIcon);
        }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_RUN:
            {
                int sel = (int)SendMessage(hComboMode, CB_GETCURSEL, 0, 0);
                if (sel == CB_ERR) {
                    MessageBox(hDlg, L"Please select a decision mode.", L"Error", MB_OK | MB_ICONERROR);
                    break;
                }

                // Pass hDlg (HWND) directly
                if (sel == 0) g_decisionLoop->runFullLoop(hDlg);
                else if (sel == 1) g_decisionLoop->runFastLoop(hDlg);
                else if (sel == 2) g_decisionLoop->runTriageMode(hDlg);
                else g_decisionLoop->runGroupMode(hDlg);

                std::stringstream report;
                report << "Overall Decision Quality Score: " << g_decisionLoop->getOverallScore() << "/10\n\n";
                report << g_decisionLoop->generateFullReport();
                std::wstring wReport = StringToWString(report.str());
                SetWindowText(hEditReport, wReport.c_str());
            }
            break;

        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

std::wstring StringToWString(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}