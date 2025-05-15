#define NOMINMAX
#include <windows.h>
#include "DecisionComponent.h"
#include <string>
#include "ImplementationLearningLayer.h"
#include "UserInput.h"
#include "json.hpp"
#include <fstream>
#include <ctime>
#include <sstream>
#include <numeric>
#include <algorithm> 

using json = nlohmann::json;

// Remove createDecisionJournal since it's already defined inline in the header

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
    
    float sum = std::accumulate(decisionPatterns.begin(), decisionPatterns.end(), 0.0f,
        [](float acc, const std::pair<const std::string, float>& pair) {
            return acc + pair.second;
        });
    pdmScore = (sum / decisionPatterns.size()) * 10.0f;
}

void FeedbackLearningIntegration::processLearningEntry(HWND hWnd) {
    // Implementation if needed
}

void FeedbackLearningIntegration::execute(HWND hWnd) {
    createDecisionJournal(hWnd);
    conductDecisionAutopsy(hWnd);
    updatePersonalDecisionModel(hWnd);
}

std::string FeedbackLearningIntegration::getReport() const {
    std::stringstream rep;
    rep << "Feedback & Learning Report:\n";
    rep << "  - Learning Integration Score: " << learningScore << "/10\n";
    rep << "  - PDM Evolution Score: " << pdmScore << "/10\n";
    
    rep << "  - Decision Journal Entry:\n";
    rep << "    - Context: " << currentEntry.context << "\n";
    rep << "    - Steps:\n";
    for (const auto& step : currentEntry.steps) {
        rep << "      * " << step << "\n";
    }
    rep << "    - Confidence: " << (currentEntry.confidence * 100) << "%\n";
    rep << "    - Lessons: " << currentEntry.lessons << "\n";
    
    rep << "  - Identified Decision Patterns:\n";
    for (const auto& pair : decisionPatterns) {
        rep << "    - " << pair.first << ": " << (pair.second * 100) << "%\n";
    }
    
    if (learningScore < 6.0f) {
        rep << "  - Recommendation: Review past journal entries to identify patterns.\n";
    }
    
    return rep.str();
}