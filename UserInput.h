#pragma once
#include <windows.h>
#include <string>
#include <vector>

class UserInput {
public:
    // Use HWND directly
    static float getScoreInput(HWND hWndParent, const std::string& prompt, float min = 1.0f, float max = 10.0f);
    static std::string getStringInput(HWND hWndParent, const std::string& prompt);
    static std::string selectFromList(HWND hWndParent, const std::string& prompt, const std::vector<std::string>& options);

private:
    static INT_PTR CALLBACK InputDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK ListDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    
    struct InputDlgData {
        std::string prompt;
        std::string* result;
        float min, max;
        bool isScore;
    };
    
    struct ListDlgData {
        std::string prompt;
        std::vector<std::string> options;
        std::string* result;
    };
};