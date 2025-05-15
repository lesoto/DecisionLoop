#define NOMINMAX
#include <windows.h>

#include "CognitiveEmotionalLayer.h"
#include "UserInput.h"
#include <fstream>
#include <vector>
#include <numeric>
#include <ctime>
#include <sstream>

void CognitiveReadiness::conductSelfAssessment(HWND hWnd) {
    float fatigue = UserInput::getScoreInput(hWnd, "Rate your fatigue level (1 = none, 10 = exhausted)", 1.0f, 10.0f);
    float distractions = UserInput::getScoreInput(hWnd, "Rate your distraction level (1 = none, 10 = high)", 1.0f, 10.0f);
    float focus = UserInput::getScoreInput(hWnd, "Rate your focus level (1 = low, 10 = high)", 1.0f, 10.0f);
    
    readinessScore = (focus * 0.5f + (10 - fatigue) * 0.3f + (10 - distractions) * 0.2f);
    
    std::ofstream out("cognitive_trends.txt", std::ios::app);
    out << time(nullptr) << "," << readinessScore << "\n";
    out.close();
}

void CognitiveReadiness::determineCognitiveMode() {
    cognitiveMode = readinessScore > 7.0f ? "Analytical" :
                    readinessScore > 4.0f ? "Balanced" : "Intuitive";
}

void CognitiveReadiness::applyReadinessTechniques(HWND hWnd) {
    if (readinessScore < 5.0f) {
        std::vector<std::string> techniques = {"Mindfulness exercise", "Physical movement", "Grounding exercise"};
        std::string technique = UserInput::selectFromList(hWnd, "Select a readiness technique", techniques);
        readinessTechniques.push_back(technique);
        readinessScore = std::min(10.0f, readinessScore + 2.0f);
    }
}

void CognitiveReadiness::execute(HWND hWnd) {
    conductSelfAssessment(hWnd);
    determineCognitiveMode();
    applyReadinessTechniques(hWnd);
    
    std::ifstream in("cognitive_trends.txt");
    std::vector<float> scores;
    std::string line;
    while (std::getline(in, line)) {
        size_t pos = line.find(",");
        if (pos != std::string::npos) {
            scores.push_back(std::stof(line.substr(pos + 1)));
        }
    }
    in.close();
    
    if (scores.size() >= 5) {
        float avg = std::accumulate(scores.begin(), scores.end(), 0.0f) / scores.size();
        if (avg < 4.0f) {
            MessageBoxA(hWnd, "Trend indicates potential burnout risk.", "Warning", MB_OK | MB_ICONWARNING);
        }
    }
}

std::string CognitiveReadiness::getReport() const {
    std::stringstream report;
    report << "Cognitive Readiness Report:\n";
    report << "  - Readiness Score: " << readinessScore << "/10\n";
    report << "  - Dominant Cognitive Mode: " << cognitiveMode << "\n";
    report << "  - Applied Techniques: ";
    for (const auto& tech : readinessTechniques) {
        report << tech << ", ";
    }
    if (readinessScore < 5.0f) {
        report << "\n  - Recommendation: Practice regular mindfulness to improve readiness.";
    }
    return report.str();
}

void EmotionalCalibration::identifyEmotions(HWND hWnd) {
    const std::vector<std::string> emotionList = {
        "Anxiety", "Excitement", "Frustration", "Contentment",
        "Overwhelm", "Confidence", "Anger", "Sadness", "Joy"
    };
    
    for (int i = 0; i < 3; ++i) {
        std::stringstream prompt;
        prompt << "Select emotion " << (i + 1);
        std::string emotion = UserInput::selectFromList(hWnd, prompt.str(), emotionList);
        prompt.str("");
        prompt << "Rate intensity of " << emotion;
        float intensity = UserInput::getScoreInput(hWnd, prompt.str(), 1.0f, 10.0f);
        emotions.emplace_back(emotion, intensity);
    }
}

void EmotionalCalibration::evaluateProportionality() {
    float totalIntensity = 0.0f;
    for (const auto& emotionPair : emotions) {
        totalIntensity += emotionPair.second;
    }
    
    float avgIntensity = emotions.empty() ? 0.0f : totalIntensity / emotions.size();
    regulationScore = 10.0f - std::min(10.0f, avgIntensity * 0.75f);
}

void EmotionalCalibration::applyRegulationStrategy(HWND hWnd) {
    std::vector<std::string> strategies = {"4-7-8 breathing", "Cognitive reappraisal", "Physical movement"};
    if (regulationScore < 6.0f) {
        regulationStrategy = UserInput::selectFromList(hWnd, "Select an emotional regulation strategy", strategies);
        regulationScore = std::min(10.0f, regulationScore + 3.0f);
    } else {
        regulationStrategy = "Maintain current state";
    }
}

void EmotionalCalibration::execute(HWND hWnd) {
    identifyEmotions(hWnd);
    evaluateProportionality();
    applyRegulationStrategy(hWnd);
}

std::string EmotionalCalibration::getReport() const {
    std::stringstream report;
    report << "Emotional Calibration Report:\n";
    report << "  - Regulation Score: " << regulationScore << "/10\n";
    report << "  - Detected Emotions:\n";
    for (const auto& emotionPair : emotions) {
        report << "    - " << emotionPair.first << ": " << emotionPair.second << "/10\n";
    }
    report << "  - Applied Regulation: " << regulationStrategy << "\n";
    if (regulationScore < 5.0f) {
        report << "  - Recommendation: Practice daily emotional check-ins.\n";
    }
    return report.str();
}

void ImpulseAlignment::simulateImpulses(HWND hWnd) {
    impulses = { UserInput::getStringInput(hWnd, "Enter primary impulse influencing your decision") };
}

void ImpulseAlignment::evaluateAlignment(HWND hWnd) {
    alignmentScore = UserInput::getScoreInput(hWnd, "Rate how well impulses align with your goals (1-10)", 1.0f, 10.0f);
}

void ImpulseAlignment::applyCBT_ACT_Tools(HWND hWnd) {
    if (alignmentScore < 6.0f) {
        std::vector<std::string> tools = {"Pause and reflect", "Reframe impulse", "Choose alternative action"};
        std::string tool = UserInput::selectFromList(hWnd, "Select a CBT/ACT tool to align impulses", tools);
        alignmentScore = std::min(10.0f, alignmentScore + 2.0f);
        impulses.push_back("Applied: " + tool);
    }
}

void ImpulseAlignment::execute(HWND hWnd) {
    simulateImpulses(hWnd);
    evaluateAlignment(hWnd);
    applyCBT_ACT_Tools(hWnd);
}

std::string ImpulseAlignment::getReport() const {
    std::stringstream report;
    report << "Impulse Alignment Report:\n";
    report << "  - Alignment Score: " << alignmentScore << "/10\n";
    report << "  - Impulses:\n";
    for (const auto& impulse : impulses) {
        report << "    - " << impulse << "\n";
    }
    if (alignmentScore < 6.0f) {
        report << "  - Recommendation: Use CBT/ACT tools regularly to align impulses.\n";
    }
    return report.str();
}