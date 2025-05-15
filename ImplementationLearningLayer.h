#pragma once
#include "DecisionComponent.h"
#include <string>
#include <vector>
#include <map>

class FeedbackLearningIntegration : public DecisionComponent {
private:
    struct JournalEntry {
        std::string context;
        std::vector<std::string> steps;
        float confidence;
        std::string lessons;
    };
    JournalEntry currentEntry;
    float learningScore;
    float pdmScore;
    std::map<std::string, float> decisionPatterns;

    void createDecisionJournal(HWND hWnd);
    void conductDecisionAutopsy(HWND hWnd);
    void updatePersonalDecisionModel(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return (learningScore + pdmScore) / 2.0f; }
};