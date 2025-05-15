#include "DecisionLoop.h"
#include <numeric>

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
}

void DecisionLoop::runFastLoop(HWND hDlg) {
    specializedModeComponents[0]->execute(hDlg);
}

void DecisionLoop::runTriageMode(HWND hDlg) {
    specializedModeComponents[1]->execute(hDlg);
}

void DecisionLoop::runGroupMode(HWND hDlg) {
    specializedModeComponents[2]->execute(hDlg);
}

float DecisionLoop::getOverallScore() {
    return overallScore;
}

std::string DecisionLoop::generateFullReport() {
    return "Full Report";
}