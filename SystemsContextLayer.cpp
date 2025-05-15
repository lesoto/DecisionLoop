#define NOMINMAX
#include <windows.h>


#include "SystemsContextLayer.h"
#include "UserInput.h"
#include <algorithm>
#include <sstream>

void BiasAssumptionTesting::applyDebiasingProtocols(HWND hWnd) {
    const std::vector<std::string> biasTypes = {
        "Confirmation", "Sunk Cost", "Overconfidence",
        "Anchoring", "Availability", "Blind Spot"
    };
    
    for (int i = 0; i < 3; ++i) {
        std::stringstream prompt;
        prompt << "Select bias " << (i + 1);
        std::string bias = UserInput::selectFromList(hWnd, prompt.str(), biasTypes);
        prompt.str("");
        prompt << "Enter evidence for " << bias << " bias";
        std::string evidence = UserInput::getStringInput(hWnd, prompt.str());
        biases.emplace_back(bias, evidence);
    }
    
    detectionScore = UserInput::getScoreInput(hWnd, "Rate effectiveness of bias detection (1-10)");
}

void BiasAssumptionTesting::conductPremortem(HWND hWnd) {
    std::string failureScenario = UserInput::getStringInput(hWnd, "Imagine the decision failed. Describe why");
    flexibilityScore = UserInput::getScoreInput(hWnd, "Rate your openness to changing your mind (1-10)");
}

void BiasAssumptionTesting::applyBayesianUpdating(HWND hWnd) {
    std::string newEvidence = UserInput::getStringInput(hWnd, "What new evidence would change your decision?");
    if (!newEvidence.empty()) {
        detectionScore = std::min(10.0f, detectionScore + 2.0f);
    }
}

void BiasAssumptionTesting::execute(HWND hWnd) {
    applyDebiasingProtocols(hWnd);
    conductPremortem(hWnd);
    applyBayesianUpdating(hWnd);
}

std::string BiasAssumptionTesting::getReport() const {
    std::stringstream report;
    report << "Bias and Assumption Testing Report:\n";
    report << "  - Bias Detection Score: " << detectionScore << "/10\n";
    report << "  - Belief Flexibility Score: " << flexibilityScore << "/10\n";
    report << "  - Identified Biases:\n";
    for (const auto& biasPair : biases) {
        report << "    - " << biasPair.first << " Bias: " << biasPair.second << "\n";
    }
    if (detectionScore < 6.0f) {
        report << "  - Recommendation: Conduct regular bias checks to improve accuracy.\n";
    }
    return report.str();
}

void UncertaintyComplexityManagement::assessRiskVsUncertainty(HWND hWnd) {
    float riskScore = UserInput::getScoreInput(hWnd, "Rate quantifiable risks (1-10)");
    float uncertaintyScore = UserInput::getScoreInput(hWnd, "Rate non-quantifiable uncertainties (1-10)");
    adaptationScore = (riskScore + uncertaintyScore) / 2.0f;
}

void UncertaintyComplexityManagement::applyScenarioPlanning(HWND hWnd) {
    for (int i = 0; i < 3; ++i) {
        std::stringstream prompt;
        prompt << "Enter scenario " << (i + 1);
        scenarios.push_back(UserInput::getStringInput(hWnd, prompt.str()));
    }
    navigationScore = UserInput::getScoreInput(hWnd, "Rate confidence in navigating these scenarios (1-10)");
}

void UncertaintyComplexityManagement::determineCynefinCategory(HWND hWnd) {
    std::vector<std::string> categories = {"Simple", "Complicated", "Complex", "Chaotic"};
    cynefinCategory = UserInput::selectFromList(hWnd, "Select Cynefin category for this decision", categories);
}

void UncertaintyComplexityManagement::execute(HWND hWnd) {
    assessRiskVsUncertainty(hWnd);
    applyScenarioPlanning(hWnd);
    determineCynefinCategory(hWnd);
}

std::string UncertaintyComplexityManagement::getReport() const {
    std::stringstream report;
    report << "Uncertainty Management Report:\n";
    report << "  - Uncertainty Adaptation Score: " << adaptationScore << "/10\n";
    report << "  - Complexity Navigation Score: " << navigationScore << "/10\n";
    report << "  - Cynefin Framework Category: " << cynefinCategory << "\n";
    report << "  - Scenarios Developed:\n";
    for (const auto& scenario : scenarios) {
        report << "    - " << scenario << "\n";
    }
    if (navigationScore < 6.0f) {
        report << "  - Recommendation: Develop more detailed scenarios for complex decisions.\n";
    }
    return report.str();
}

void InternalRoleDialogue::identifyInternalRoles(HWND hWnd) {
    std::vector<std::string> roleList = {
        "Protector", "Visionary", "Skeptic", "Future Self", "Inner Child", "Shadow"
    };
    
    for (int i = 0; i < 4; ++i) {
        std::stringstream prompt;
        prompt << "Select role " << (i + 1);
        std::string role = UserInput::selectFromList(hWnd, prompt.str(), roleList);
        prompt.str("");
        prompt << "Enter perspective for " << role;
        std::string perspective = UserInput::getStringInput(hWnd, prompt.str());
        prompt.str("");
        prompt << "Rate influence of " << role << " (1-10)";
        float influence = UserInput::getScoreInput(hWnd, prompt.str()) / 10.0f;
        roles[role] = {perspective, influence};
    }
}

void InternalRoleDialogue::conductDialogue(HWND hWnd) {
    std::string dialogue = UserInput::getStringInput(hWnd, "Summarize the dialogue between roles");
    coherenceScore = UserInput::getScoreInput(hWnd, "Rate coherence of internal dialogue (1-10)");
}

void InternalRoleDialogue::resolveConflicts(HWND hWnd) {
    std::string conflict = UserInput::getStringInput(hWnd, "Describe any conflicts between roles");
    if (conflict.empty()) {
        resolutionScore = coherenceScore;
    } else {
        resolutionScore = UserInput::getScoreInput(hWnd, "Rate conflict resolution effectiveness (1-10)");
    }
}

void InternalRoleDialogue::execute(HWND hWnd) {
    identifyInternalRoles(hWnd);
    conductDialogue(hWnd);
    resolveConflicts(hWnd);
}

std::string InternalRoleDialogue::getReport() const {
    std::stringstream report;
    report << "Internal Role Dialogue Report:\n";
    report << "  - Role Coherence Score: " << coherenceScore << "/10\n";
    report << "  - Conflict Resolution Score: " << resolutionScore << "/10\n";
    report << "  - Internal Roles and Perspectives:\n";
    for (const auto& rolePair : roles) {
        report << "    - " << rolePair.first << " (" << (rolePair.second.influence * 100) << "% influence)\n";
        report << "      Perspective: " << rolePair.second.perspective << "\n";
    }
    if (resolutionScore < 6.0f) {
        report << "  - Recommendation: Use IFS techniques to resolve internal conflicts.\n";
    }
    return report.str();
}

void SocialEcologicalAnalysis::assessRelationships(HWND hWnd) {
    for (int i = 0; i < 3; ++i) {
        std::stringstream prompt;
        prompt << "Enter relationship " << (i + 1);
        std::string rel = UserInput::getStringInput(hWnd, prompt.str());
        if (!rel.empty()) {
            affectedRelationships.push_back(rel);
        }
    }
    fitScore = UserInput::getScoreInput(hWnd, "Rate fit with relationships (1-10)");
}

void SocialEcologicalAnalysis::analyzePowerDynamics(HWND hWnd) {
    for (int i = 0; i < 3; ++i) {
        std::stringstream prompt;
        prompt << "Enter power dynamic " << (i + 1);
        std::string dyn = UserInput::getStringInput(hWnd, prompt.str());
        if (!dyn.empty()) {
            powerDynamics.push_back(dyn);
        }
    }
}

void SocialEcologicalAnalysis::evaluateEnvironmentalImpact(HWND hWnd) {
    for (int i = 0; i < 3; ++i) {
        std::stringstream prompt;
        prompt << "Enter environmental factor " << (i + 1);
        std::string env = UserInput::getStringInput(hWnd, prompt.str());
        if (!env.empty()) {
            environmentalFactors.push_back(env);
        }
    }
    impactScore = UserInput::getScoreInput(hWnd, "Rate environmental impact alignment (1-10)");
}

void SocialEcologicalAnalysis::execute(HWND hWnd) {
    assessRelationships(hWnd);
    analyzePowerDynamics(hWnd);
    evaluateEnvironmentalImpact(hWnd);
}

std::string SocialEcologicalAnalysis::getReport() const {
    std::stringstream report;
    report << "Social-Ecological Analysis Report:\n";
    report << "  - Contextual Fit Score: " << fitScore << "/10\n";
    report << "  - Stakeholder Impact Score: " << impactScore << "/10\n";
    
    report << "  - Affected Relationships:\n";
    for (const auto& rel : affectedRelationships) {
        report << "    - " << rel << "\n";
    }
    
    report << "  - Power Dynamics:\n";
    for (const auto& dyn : powerDynamics) {
        report << "    - " << dyn << "\n";
    }
    
    report << "  - Environmental Factors:\n";
    for (const auto& env : environmentalFactors) {
        report << "    - " << env << "\n";
    }
    
    if (fitScore < 6.0f || impactScore < 6.0f) {
        report << "  - Recommendation: Reassess social and ecological impacts.\n";
    }
    
    return report.str();
}