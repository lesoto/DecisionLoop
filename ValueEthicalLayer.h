#pragma once
#include "DecisionComponent.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm> 

class CoreMetaValueSpecification : public DecisionComponent {
private:
    std::vector<std::string> fundamentalValues;
    std::map<std::string, float> valueWeights;
    float consistencyScore = 0.0f;  // Initialize to 0

    void identifyFundamentalValues(HWND hWnd);
    void differentiateValueTypes(HWND hWnd);
    void assignValueWeights(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return consistencyScore; }
};

class EthicalCoherence : public DecisionComponent {
private:
    struct Perspective {
        float weight;
        std::string analysis;
        float score;
    };
    std::map<std::string, Perspective> perspectives;
    float coherenceScore = 0.0f;    // Initialize to 0

    void applyEthicalFrameworks(HWND hWnd);
    void computeCoherenceScore();

public:
    EthicalCoherence() {
        perspectives = {
            {"Utilitarianism", {0.33f, "", 0.0f}},
            {"Deontology", {0.33f, "", 0.0f}},
            {"Virtue Ethics", {0.34f, "", 0.0f}}
        };
    }
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return coherenceScore; }
};

class TimeHorizonIntegration : public DecisionComponent {
private:
    struct Horizon {
        std::string impact;
        float importance;
    };
    std::map<std::string, Horizon> horizons;
    float balanceScore = 0.0f;      // Initialize to 0

    void mapImpacts(HWND hWnd);
    void applyTemporalDiscounting();
    void simulateFutureSelf(HWND hWnd);

public:
    TimeHorizonIntegration() {
        horizons = {
            {"Immediate", {"Neutral", 0.25f}},
            {"1-6 months", {"Neutral", 0.25f}},
            {"1-5 years", {"Neutral", 0.25f}},
            {"5+ years", {"Neutral", 0.25f}}
        };
    }
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return balanceScore; }
};