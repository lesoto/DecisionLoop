#include "UserInput.h"
#include "Resource.h"
#include <commctrl.h>
#include <sstream>
#include <windows.h>
#include <string>

// Helper functions for string conversions
std::wstring to_wstring(const std::string& str) {
    if (str.empty()) return L"";
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len - 1, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}

std::string to_string(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(len - 1, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);
    return str;
}

// Implementation of static methods
float UserInput::getScoreInput(HWND hWndParent, const std::string& prompt, float min, float max)
{
    InputDlgData data = { prompt, new std::string(), min, max, true };
    if (DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_INPUT_DIALOG), hWndParent, InputDlgProc, (LPARAM)&data) == IDOK)
    {
        try {
            float value = std::stof(*data.result);
            if (value >= min && value <= max) {
                delete data.result;
                return value;
            }
        }
        catch (...) {}
    }
    delete data.result;
    return min;
}

std::string UserInput::getStringInput(HWND hWndParent, const std::string& prompt)
{
    InputDlgData data = { prompt, new std::string(), 0.0f, 0.0f, false };
    if (DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_INPUT_DIALOG), hWndParent, InputDlgProc, (LPARAM)&data) == IDOK)
    {
        std::string result = *data.result;
        delete data.result;
        return result;
    }
    delete data.result;
    return "";
}

std::string UserInput::selectFromList(HWND hWndParent, const std::string& prompt, const std::vector<std::string>& options)
{
    ListDlgData data = { prompt, options, new std::string() };
    if (DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_LIST_DIALOG), hWndParent, ListDlgProc, (LPARAM)&data) == IDOK)
    {
        std::string result = *data.result;
        delete data.result;
        return result;
    }
    delete data.result;
    return options.empty() ? "" : options[0];
}

// Dialog procedures remain unchanged
INT_PTR CALLBACK UserInput::InputDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static InputDlgData* data;
    switch (message)
    {
    case WM_INITDIALOG:
        data = (InputDlgData*)lParam;
        SetWindowTextW(hDlg, L"Input");
        SetDlgItemTextW(hDlg, IDC_EDIT_PROMPT, to_wstring(data->prompt).c_str());
        if (data->isScore)
        {
            std::stringstream ss;
            ss << "Enter a value between " << data->min << " and " << data->max;
            SetDlgItemTextW(hDlg, IDC_STATIC_INSTRUCTION, to_wstring(ss.str()).c_str());
        }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            {
                const int BUFSIZE = 256;
                WCHAR wbuffer[BUFSIZE];
                int copied = GetDlgItemTextW(hDlg, IDC_EDIT_INPUT, wbuffer, BUFSIZE);
                if (copied > 0)
                {
                    *data->result = to_string(std::wstring(wbuffer, copied));
                }
                EndDialog(hDlg, IDOK);
            }
            break;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            break;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK UserInput::ListDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static ListDlgData* data;
    switch (message)
    {
    case WM_INITDIALOG:
        data = (ListDlgData*)lParam;
        SetWindowTextW(hDlg, L"Select Option");
        SetDlgItemTextW(hDlg, IDC_EDIT_PROMPT, to_wstring(data->prompt).c_str());
        {
            HWND hList = GetDlgItem(hDlg, IDC_LIST_OPTIONS);
            for (const auto& option : data->options)
            {
                SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)to_wstring(option).c_str());
            }
            SendMessageW(hList, LB_SETCURSEL, 0, 0);
        }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            {
                HWND hList = GetDlgItem(hDlg, IDC_LIST_OPTIONS);
                int sel = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
                if (sel != LB_ERR && sel < (int)data->options.size())
                {
                    *data->result = data->options[sel];
                }
                EndDialog(hDlg, IDOK);
            }
            break;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            break;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        break;
    }
    return (INT_PTR)FALSE;
}
