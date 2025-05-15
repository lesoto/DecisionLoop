#pragma once
#include "DecisionComponent.h"
#include <string>
#include <vector>
#include <windows.h>

class FastLoopMode : public DecisionComponent {
public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override;

protected:
    void runEssentialChecks(HWND hWnd);

private:
    float score = 0.0f;
    std::string report;
    std::vector<std::string> stepsTaken;
};

class TriageMode : public DecisionComponent {
private:
    std::string primaryConcern;
    bool safeguardsApplied = false;  // Boolean flag for safeguards
    
    void applyEthicalGuardrails(HWND hWnd);
    void assessIrreversibleHarm(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return safeguardsApplied ? 10.0f : 5.0f; }
};

class GroupMode : public DecisionComponent {
public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override;

protected:
    void mapDiversePerspectives(HWND hWnd);
    void buildConsensus(HWND hWnd);

private:
    float score = 0.0f;
    std::string report;
    std::vector<std::string> stakeholderViews;
    std::vector<std::string> dissentingOpinions;
    float consensusScore = 0.0f;
};