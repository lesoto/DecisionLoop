#pragma once
#include <string>
#include <windows.h>

class DecisionComponent {
public:
    virtual void execute(HWND hWnd) = 0;
    virtual std::string getReport() const = 0;
    virtual float getScore() const = 0;
    virtual ~DecisionComponent() = default;
};