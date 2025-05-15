#define NOMINMAX
#include <windows.h>

#include "SpecializedModes.h"
#include "CognitiveEmotionalLayer.h"
#include "ValueEthicalLayer.h"
#include "SystemsContextLayer.h"
#include "UserInput.h"
#include <sstream>

void FastLoopMode::runEssentialChecks(HWND hWnd) {
    EmotionalCalibration emotionalCheck;
    CoreMetaValueSpecification valueCheck;
    EthicalCoherence ethicalCheck;
    BiasAssumptionTesting biasCheck;
    
    emotionalCheck.execute(hWnd);
    valueCheck.execute(hWnd);
    ethicalCheck.execute(hWnd);
    biasCheck.execute(hWnd);
    
    std::stringstream ss;
    ss << "Emotional check: " << emotionalCheck.getScore() << "/10";
    stepsTaken.push_back(ss.str());
    ss.str("");
    ss << "Core values: " << valueCheck.getScore() << "/10";
    stepsTaken.push_back(ss.str());
    ss.str("");
    ss << "Ethical lens: " << ethicalCheck.getScore() << "/10";
    stepsTaken.push_back(ss.str());
    ss.str("");
    ss << "Bias check: " << biasCheck.getScore() << "/10";
    stepsTaken.push_back(ss.str());
    
    score = (emotionalCheck.getScore() + valueCheck.getScore() +
             ethicalCheck.getScore() + biasCheck.getScore()) / 4.0f;
}

void FastLoopMode::execute(HWND hWnd) {
    runEssentialChecks(hWnd);
}

std::string FastLoopMode::getReport() const {
    std::stringstream report;
    report << "Fast Loop Mode Report:\n";
    report << "  - Decision Quality Score: " << score << "/10\n";
    report << "  - Steps Taken:\n";
    for (const auto& step : stepsTaken) {
        report << "    - " << step << "\n";
    }
    if (score < 6.0f) {
        report << "  - Recommendation: Consider full loop for complex decisions.\n";
    }
    return report.str();
}

void TriageMode::applyEthicalGuardrails(HWND hWnd) {
    primaryConcern = UserInput::getStringInput(hWnd, "Enter primary concern for this urgent decision");
    
    std::vector<std::string> guardrails = {
        "Do no irreversible harm",
        "Preserve dignity",
        "Honor core values"
    };
    
    safeguardsApplied = UserInput::getScoreInput(hWnd, "Rate adherence to guardrails (1-10)", 1.0f, 10.0f) >= 6.0f;
}

void TriageMode::assessIrreversibleHarm(HWND hWnd) {
    std::string harmAssessment = UserInput::getStringInput(hWnd, "Describe potential irreversible harms");
    if (harmAssessment.empty()) {
        primaryConcern += "\nNo irreversible harm detected";
    } else {
        primaryConcern += "\nPotential irreversible impacts: " + harmAssessment;
    }
}

void TriageMode::execute(HWND hWnd) {
    applyEthicalGuardrails(hWnd);
    assessIrreversibleHarm(hWnd);
}

std::string TriageMode::getReport() const {
    std::stringstream report;
    report << "Triage Mode Report:\n";
    report << "  - Safeguards Applied: " << (safeguardsApplied ? "Yes" : "No") << "\n";
    report << "  - Primary Concern: " << primaryConcern << "\n";
    report << "  - Post-Action Review Required: Yes\n";
    if (!safeguardsApplied) {
        report << "  - Recommendation: Conduct immediate review to ensure ethical alignment.\n";
    }
    return report.str();
}

void GroupMode::mapDiversePerspectives(HWND hWnd) {
    for (int i = 0; i < 4; ++i) {
        std::stringstream prompt;
        prompt << "Enter stakeholder perspective " << (i + 1);
        std::string view = UserInput::getStringInput(hWnd, prompt.str());
        if (!view.empty()) {
            stakeholderViews.push_back(view);
        }
    }
    
    std::string dissent = UserInput::getStringInput(hWnd, "Enter any dissenting opinions (or leave blank)");
    if (!dissent.empty()) {
        dissentingOpinions.push_back(dissent);
    }
}

void GroupMode::buildConsensus(HWND hWnd) {
    consensusScore = UserInput::getScoreInput(hWnd, "Rate consensus among stakeholders (1-10)");
    if (!dissentingOpinions.empty()) {
        consensusScore = std::max(0.0f, consensusScore - 1.5f);
    }
}

void GroupMode::execute(HWND hWnd) {
    mapDiversePerspectives(hWnd);
    buildConsensus(hWnd);
}

std::string GroupMode::getReport() const {
    std::stringstream report;
    report << "Group Decision Report:\n";
    report << "  - Consensus Score: " << consensusScore << "/10\n";
    report << "  - Stakeholder Views:\n";
    for (const auto& view : stakeholderViews) {
        report << "    - " << view << "\n";
    }
    
    if (!dissentingOpinions.empty()) {
        report << "  - Dissenting Opinions:\n";
        for (const auto& dissent : dissentingOpinions) {
            report << "    - " << dissent << "\n";
        }
    }
    
    if (consensusScore < 6.0f) {
        report << "  - Recommendation: Facilitate further discussion to address dissent.\n";
    }
    
    return report.str();
}