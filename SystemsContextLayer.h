#pragma once
#include "DecisionComponent.h"
#include <string>
#include <vector>
#include <map>

class BiasAssumptionTesting : public DecisionComponent {
private:
    std::vector<std::pair<std::string, std::string>> biases;
    float detectionScore = 0.0f;    // Initialize to 0
    float flexibilityScore = 0.0f;  // Initialize to 0

    void applyDebiasingProtocols(HWND hWnd);
    void conductPremortem(HWND hWnd);
    void applyBayesianUpdating(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return (detectionScore + flexibilityScore) / 2.0f; }
};

class UncertaintyComplexityManagement : public DecisionComponent {
private:
    float adaptationScore;
    float navigationScore;
    std::string cynefinCategory;
    std::vector<std::string> scenarios;

    void assessRiskVsUncertainty(HWND hWnd);
    void applyScenarioPlanning(HWND hWnd);
    void determineCynefinCategory(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return (adaptationScore + navigationScore) / 2.0f; }
};

class InternalRoleDialogue : public DecisionComponent {
private:
    struct RoleData {
        std::string perspective;
        float influence;
    };
    std::map<std::string, RoleData> roles;
    float coherenceScore;
    float resolutionScore;

    void identifyInternalRoles(HWND hWnd);
    void conductDialogue(HWND hWnd);
    void resolveConflicts(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return (coherenceScore + resolutionScore) / 2.0f; }
};

class SocialEcologicalAnalysis : public DecisionComponent {
private:
    std::vector<std::string> affectedRelationships;
    std::vector<std::string> powerDynamics;
    std::vector<std::string> environmentalFactors;
    float fitScore;
    float impactScore;

    void assessRelationships(HWND hWnd);
    void analyzePowerDynamics(HWND hWnd);
    void evaluateEnvironmentalImpact(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return (fitScore + impactScore) / 2.0f; }
};