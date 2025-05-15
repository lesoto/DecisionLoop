#include "DecisionLoop.h"
#include "CognitiveEmotionalLayer.h"
#include "ValueEthicalLayer.h"
#include "SystemsContextLayer.h"
#include "ImplementationLearningLayer.h"
#include "SpecializedModes.h"
#include <numeric>
#include <sstream>
#include <windows.h>
#include "Resource.h"

DecisionLoop::DecisionLoop() : overallScore(0.0f) {
    cognitiveEmotionalComponents.push_back(std::make_unique<CognitiveReadiness>());
    cognitiveEmotionalComponents.push_back(std::make_unique<EmotionalCalibration>());
    cognitiveEmotionalComponents.push_back(std::make_unique<ImpulseAlignment>());
    
    valueEthicalComponents.push_back(std::make_unique<CoreMetaValueSpecification>());
    valueEthicalComponents.push_back(std::make_unique<EthicalCoherence>());
    valueEthicalComponents.push_back(std::make_unique<TimeHorizonIntegration>());
    
    systemsContextComponents.push_back(std::make_unique<BiasAssumptionTesting>());
    systemsContextComponents.push_back(std::make_unique<UncertaintyComplexityManagement>());
    systemsContextComponents.push_back(std::make_unique<InternalRoleDialogue>());
    systemsContextComponents.push_back(std::make_unique<SocialEcologicalAnalysis>());
    
    implementationLearningComponents.push_back(std::make_unique<FeedbackLearningIntegration>());
    
    specializedModeComponents.push_back(std::make_unique<FastLoopMode>());
    specializedModeComponents.push_back(std::make_unique<TriageMode>());
    specializedModeComponents.push_back(std::make_unique<GroupMode>());
}

DecisionLoop::~DecisionLoop() {}

void DecisionLoop::runFullLoop(HWND hDlg) {
    for (auto& component : cognitiveEmotionalComponents) component->execute(hDlg);
    for (auto& component : valueEthicalComponents) component->execute(hDlg);
    for (auto& component : systemsContextComponents) component->execute(hDlg);
    for (auto& component : implementationLearningComponents) component->execute(hDlg);
    updateOverallScore();
}

void DecisionLoop::runFastLoop(HWND hDlg) {
    specializedModeComponents[0]->execute(hDlg);
    overallScore = specializedModeComponents[0]->getScore();
}

void DecisionLoop::runTriageMode(HWND hDlg) {
    specializedModeComponents[1]->execute(hDlg);
    overallScore = specializedModeComponents[1]->getScore();
}

void DecisionLoop::runGroupMode(HWND hDlg) {
    specializedModeComponents[2]->execute(hDlg);
    overallScore = specializedModeComponents[2]->getScore();
}

void DecisionLoop::updateOverallScore() {
    std::vector<float> scores;
    for (const auto& component : cognitiveEmotionalComponents) scores.push_back(component->getScore());
    for (const auto& component : valueEthicalComponents) scores.push_back(component->getScore());
    for (const auto& component : systemsContextComponents) scores.push_back(component->getScore());
    for (const auto& component : implementationLearningComponents) scores.push_back(component->getScore());
    overallScore = scores.empty() ? 0.0f : std::accumulate(scores.begin(), scores.end(), 0.0f) / scores.size();
}

float DecisionLoop::getOverallScore() {
    return overallScore;
}

std::string DecisionLoop::generateFullReport() {
    std::stringstream report;
    report << "Decision Loop Full Report:\n\n";
    report << "Overall Decision Quality Score: " << overallScore << "/10\n\n";

    report << "Cognitive-Emotional Layer:\n";
    for (const auto& component : cognitiveEmotionalComponents) {
        report << component->getReport() << "\n";
    }

    report << "Value-Ethical Layer:\n";
    for (const auto& component : valueEthicalComponents) {
        report << component->getReport() << "\n";
    }

    report << "Systems/Context Layer:\n";
    for (const auto& component : systemsContextComponents) {
        report << component->getReport() << "\n";
    }

    report << "Implementation/Learning Layer:\n";
    for (const auto& component : implementationLearningComponents) {
        report << component->getReport() << "\n";
    }

    return report.str();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Create the main dialog
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, [](HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        static DecisionLoop decisionLoop;

        switch (message)
        {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
        }
        return FALSE;
    });

    return 0;
}