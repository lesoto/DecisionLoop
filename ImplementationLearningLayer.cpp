#define NOMINMAX
#include <windows.h>

#include "ImplementationLearningLayer.h"
#include "UserInput.h"
#include "json.hpp"
#include <fstream>
#include <ctime>
#include <sstream>

using json = nlohmann::json;

void FeedbackLearningIntegration::createDecisionJournal(HWND hWnd) {
    currentEntry.context = UserInput::getStringInput(hWnd, "Enter decision context");
    currentEntry.steps = {
        UserInput::getStringInput(hWnd, "Step 1 taken"),
        UserInput::getStringInput(hWnd, "Step 2 taken"),
        UserInput::getStringInput(hWnd, "Step 3 taken")
    };
    currentEntry.confidence = UserInput::getScoreInput(hWnd, "Rate your confidence (1-10)", 1.0f, 10.0f) / 10.0f;
    currentEntry.lessons = UserInput::getStringInput(hWnd, "Enter lessons learned");
    
    learningScore = 8.0f + (currentEntry.confidence * 2.0f);
    
    json entry = {
        {"context", currentEntry.context},
        {"steps", currentEntry.steps},
        {"confidence", currentEntry.confidence},
        {"lessons", currentEntry.lessons},
        {"timestamp", time(nullptr)}
    };
    
    std::ifstream in("decision_journal.json");
    json journal;
    if (in.is_open()) {
        in >> journal;
        in.close();
    }
    journal["entries"].push_back(entry);
    
    std::ofstream out("decision_journal.json");
    out << journal.dump(4);
    out.close();
}

void FeedbackLearningIntegration::conductDecisionAutopsy(HWND hWnd) {
    float expectedOutcome = UserInput::getScoreInput(hWnd, "Rate expected outcome (1-10)", 1.0f, 10.0f);
    float actualOutcome = UserInput::getScoreInput(hWnd, "Rate actual outcome (1-10)", 1.0f, 10.0f);
    learningScore = std::min(10.0f, learningScore + (actualOutcome - expectedOutcome) / 2.0f);
}

void FeedbackLearningIntegration::updatePersonalDecisionModel(HWND hWnd) {
    std::vector<std::string> patterns = {
        "Overweight short-term", "Underestimate complexity",
        "Strong ethical coherence", "Good stakeholder consideration"
    };
    
    for (const auto& pattern : patterns) {
        std::stringstream prompt;
        prompt << "Rate pattern: " << pattern << " (1-10)";
        float strength = UserInput::getScoreInput(hWnd, prompt.str(), 1.0f, 10.0f) / 10.0f;
        decisionPatterns[pattern] = strength;
    }
    
    pdmScore = std::accumulate(decisionPatterns.begin(), decisionPatterns.end(), 0.0f,
        [](float sum, const auto& p) { return sum + p.second; }) / decisionPatterns.size() * 10.0f;
}

void FeedbackLearningIntegration::execute(HWND hWnd) {
    createDecisionJournal(hWnd);
    conductDecisionAutopsy(hWnd);
    updatePersonalDecisionModel(hWnd);
}

std::string FeedbackLearningIntegration::getReport() const {
    std::stringstream report;
    report << "Feedback & Learning Report:\n";
    report << "  - Learning Integration Score: " << learningScore << "/10\n";
    report << "  - PDM Evolution Score: " << pdmScore << "/10\n";
    
    report << "  - Decision Journal Entry:\n";
    report << "    - Context: " << currentEntry.context << "\n";
    report << "    - Steps: ";
    for (const auto& step : currentEntry.steps) {
        report << step << ", ";
    }
    report << "\n    - Confidence: " << (currentEntry.confidence * 100) << "%\n";
    report << "    - Lessons: " << currentEntry.lessons << "\n";
    
    report << "  - Identified Decision Patterns:\n";
    for (const auto& patternPair : decisionPatterns) {
        report << "    - " << patternPair.first << ": " << (patternPair.second * 100) << "%\n";
    }
    
    if (learningScore < 6.0f) {
        report << "  - Recommendation: Review past journal entries to identify patterns.\n";
    }
    
    return report.str();
}