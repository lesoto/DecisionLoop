#pragma once
#include "CognitiveEmotionalLayer.h"
#include "ValueEthicalLayer.h"
#include "SystemsContextLayer.h"
#include "ImplementationLearningLayer.h"
#include "SpecializedModes.h"
#include <vector>
#include <memory>
#include <windows.h>
#include <string>

class DecisionLoop {
public:
    DecisionLoop();
    ~DecisionLoop();
    void runFullLoop(HWND hDlg);
    void runFastLoop(HWND hDlg);
    void runTriageMode(HWND hDlg);
    void runGroupMode(HWND hDlg);
    float getOverallScore();
    std::string generateFullReport();

private:
    std::vector<std::unique_ptr<DecisionComponent>> cognitiveEmotionalComponents;
    std::vector<std::unique_ptr<DecisionComponent>> valueEthicalComponents;
    std::vector<std::unique_ptr<DecisionComponent>> systemsContextComponents;
    std::vector<std::unique_ptr<DecisionComponent>> implementationLearningComponents;
    std::vector<std::unique_ptr<DecisionComponent>> specializedModeComponents;
    float overallScore;
};