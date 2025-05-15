#pragma once
#include "DecisionComponent.h"
#include <string>
#include <vector>

class FastLoopMode : public DecisionComponent {
private:
    float score;
    std::vector<std::string> stepsTaken;

    void runEssentialChecks(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return score; }
};

class TriageMode : public DecisionComponent {
private:
    std::string primaryConcern;
    bool safeguardsApplied;

    void applyEthicalGuardrails(HWND hWnd);
    void assessIrreversibleHarm(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return safeguardsApplied ? 8.0f : 4.0f; }
};

class GroupMode : public DecisionComponent {
private:
    float consensusScore;
    std::vector<std::string> stakeholderViews;
    std::vector<std::string> dissentingOpinions;

    void mapDiversePerspectives(HWND hWnd);
    void buildConsensus(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return consensusScore; }
};