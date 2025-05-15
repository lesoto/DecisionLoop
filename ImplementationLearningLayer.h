#pragma once
#include "DecisionComponent.h"
#include <string>
#include <vector>
#include <map>
#include "UserInput.h"
#include "json.hpp"
#include <fstream>
#include <ctime>
#include <sstream>
#include <numeric>  
#include <algorithm> 

using json = nlohmann::json;

class FeedbackLearningIntegration : public DecisionComponent {
public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return (learningScore + pdmScore) / 2.0f; }

private:
    struct LearningEntry {
        std::string context;
        std::vector<std::string> steps;
        float confidence;
        std::string lessons;
    };

    LearningEntry currentEntry;
    float learningScore = 0.0f;
    float pdmScore = 0.0f;
    std::map<std::string, float> decisionPatterns;

    void createDecisionJournal(HWND hWnd);
    void conductDecisionAutopsy(HWND hWnd);
    void updatePersonalDecisionModel(HWND hWnd);
    void processLearningEntry(HWND hWnd);
};

inline void FeedbackLearningIntegration::createDecisionJournal(HWND hWnd) {  
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

    json journal;  
    std::ifstream in("decision_journal.json");  
    if (in.is_open()) {  
        in >> journal;  
        in.close();  
    }  
    journal["entries"].push_back(entry);  

    std::ofstream out("decision_journal.json");  
    out << journal.dump(4);  
    out.close();  
}