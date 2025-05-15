#define NOMINMAX
#include <windows.h>

#include "ValueEthicalLayer.h"
#include "UserInput.h"
#include <algorithm>
#include <numeric>
#include <sstream>

void CoreMetaValueSpecification::identifyFundamentalValues(HWND hWnd) {
    const std::vector<std::string> valueList = {
        "Integrity", "Growth", "Connection", "Security", "Creativity",
        "Service", "Autonomy", "Achievement", "Balance", "Justice"
    };
    
    for (int i = 0; i < 4; ++i) {
        std::stringstream prompt;
        prompt << "Select value " << (i + 1);
        std::string value = UserInput::selectFromList(hWnd, prompt.str(), valueList);
        if (std::find(fundamentalValues.begin(), fundamentalValues.end(), value) == fundamentalValues.end()) {
            fundamentalValues.push_back(value);
        }
    }
}

void CoreMetaValueSpecification::differentiateValueTypes(HWND hWnd) {
    std::vector<std::string> types = {"Inherited", "Current", "Aspirational"};
    for (const auto& value : fundamentalValues) {
        std::stringstream prompt;
        prompt << "Classify " << value;
        std::string type = UserInput::selectFromList(hWnd, prompt.str(), types);
    }
    consistencyScore = UserInput::getScoreInput(hWnd, "Rate how well these values align with your decision (1-10)", 1.0f, 10.0f);
}

void CoreMetaValueSpecification::assignValueWeights(HWND hWnd) {
    float totalWeight = 0.0f;
    for (const auto& value : fundamentalValues) {
        std::stringstream prompt;
        prompt << "Enter weight for " << value << " (1-100)";
        float weight = UserInput::getScoreInput(hWnd, prompt.str(), 1.0f, 100.0f);
        valueWeights[value] = weight / 100.0f;
        totalWeight += weight;
    }
    
    if (totalWeight > 0) {
        for (auto& valuePair : valueWeights) {
            valuePair.second /= (totalWeight / 100.0f);
        }
    }
    
    consistencyScore = std::min(10.0f, consistencyScore * (1.0f - std::abs(totalWeight - 100.0f) / 200.0f));
}

void CoreMetaValueSpecification::execute(HWND hWnd) {
    identifyFundamentalValues(hWnd);
    differentiateValueTypes(hWnd);
    assignValueWeights(hWnd);
}

std::string CoreMetaValueSpecification::getReport() const {
    std::stringstream report;
    report << "Core Value Specification Report:\n";
    report << "  - Value-Decision Consistency: " << consistencyScore << "/10\n";
    report << "  - Fundamental Values and Weights:\n";
    for (const auto& valuePair : valueWeights) {
        report << "    - " << valuePair.first << ": " << (valuePair.second * 100) << "%\n";
    }
    if (consistencyScore < 6.0f) {
        report << "  - Recommendation: Reassess value priorities for better alignment.\n";
    }
    return report.str();
}

void EthicalCoherence::applyEthicalFrameworks(HWND hWnd) {
    for (auto& perspectivePair : perspectives) {
        std::stringstream prompt;
        prompt << "Enter weight for " << perspectivePair.first << " (1-100)";
        perspectivePair.second.weight = UserInput::getScoreInput(hWnd, prompt.str(), 1.0f, 100.0f) / 100.0f;
        prompt.str("");
        prompt << "Enter analysis for " << perspectivePair.first;
        perspectivePair.second.analysis = UserInput::getStringInput(hWnd, prompt.str());
        prompt.str("");
        prompt << "Rate " << perspectivePair.first << " alignment (1-10)";
        perspectivePair.second.score = UserInput::getScoreInput(hWnd, prompt.str(), 1.0f, 10.0f);
    }
}

void EthicalCoherence::computeCoherenceScore() {
    float totalWeightedScore = 0.0f;
    float totalWeight = 0.0f;
    
    for (const auto& perspectivePair : perspectives) {
        totalWeightedScore += perspectivePair.second.score * perspectivePair.second.weight;
        totalWeight += perspectivePair.second.weight;
    }
    
    coherenceScore = totalWeight > 0 ? totalWeightedScore / totalWeight : 0.0f;
}

void EthicalCoherence::execute(HWND hWnd) {
    applyEthicalFrameworks(hWnd);
    computeCoherenceScore();
}

std::string EthicalCoherence::getReport() const {
    std::stringstream report;
    report << "Ethical Coherence Report:\n";
    report << "  - Overall Coherence Score: " << coherenceScore << "/10\n";
    
    for (const auto& perspectivePair : perspectives) {
        report << "  - " << perspectivePair.first << " (Weight: " << (perspectivePair.second.weight * 100) << "%)\n";
        report << "    - Score: " << perspectivePair.second.score << "/10\n";
        report << "    - Analysis: " << perspectivePair.second.analysis << "\n";
    }
    
    if (coherenceScore < 6.0f) {
        report << "  - Recommendation: Revisit ethical frameworks for stronger alignment.\n";
    }
    
    return report.str();
}

void TimeHorizonIntegration::mapImpacts(HWND hWnd) {
    std::vector<std::string> impactTypes = {"Positive", "Negative", "Neutral", "Mixed"};
    for (auto& horizonPair : horizons) {
        std::stringstream prompt;
        prompt << "Select impact for " << horizonPair.first;
        horizonPair.second.impact = UserInput::selectFromList(hWnd, prompt.str(), impactTypes);
        prompt.str("");
        prompt << "Rate importance of " << horizonPair.first << " (1-100)";
        horizonPair.second.importance = UserInput::getScoreInput(hWnd, prompt.str(), 1.0f, 100.0f) / 100.0f;
    }
}

void TimeHorizonIntegration::applyTemporalDiscounting() {
    horizons["Immediate"].importance *= 1.2f;
    horizons["1-6 months"].importance *= 1.0f;
    horizons["1-5 years"].importance *= 0.8f;
    horizons["5+ years"].importance *= 0.6f;
    
    float totalImportance = 0.0f;
    for (const auto& horizonPair : horizons) {
        totalImportance += horizonPair.second.importance;
    }
    if (totalImportance > 0) {
        for (auto& horizonPair : horizons) {
            horizonPair.second.importance /= totalImportance;
        }
    }
}

void TimeHorizonIntegration::simulateFutureSelf(HWND hWnd) {
    std::string narrative = UserInput::getStringInput(hWnd, "Describe how this decision aligns with your future self");
    float alignmentScore = UserInput::getScoreInput(hWnd, "Rate future-self alignment (1-10)", 1.0f, 10.0f);
    
    float immediateScore = horizons["Immediate"].impact == "Positive" ? 8.0f : 5.0f;
    balanceScore = (immediateScore * 0.4f + alignmentScore * 0.6f);
}

void TimeHorizonIntegration::execute(HWND hWnd) {
    mapImpacts(hWnd);
    applyTemporalDiscounting();
    simulateFutureSelf(hWnd);
}

std::string TimeHorizonIntegration::getReport() const {
    std::stringstream report;
    report << "Time Horizon Integration Report:\n";
    report << "  - Temporal Balance Score: " << balanceScore << "/10\n";
    
    for (const auto& horizonPair : horizons) {
        report << "  - " << horizonPair.first << ":\n";
        report << "    - Impact: " << horizonPair.second.impact << "\n";
        report << "    - Importance Weight: " << (horizonPair.second.importance * 100) << "%\n";
    }
    
    if (balanceScore < 6.0f) {
        report << "  - Recommendation: Focus on long-term impacts for better balance.\n";
    }
    
    return report.str();
}