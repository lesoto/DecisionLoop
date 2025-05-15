#pragma once
#include "DecisionComponent.h"
#include <string>
#include <vector>

class CognitiveReadiness : public DecisionComponent {
private:
    float readinessScore;
    std::string cognitiveMode;
    std::vector<std::string> readinessTechniques;

    void conductSelfAssessment(HWND hWnd);
    void determineCognitiveMode();
    void applyReadinessTechniques(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return readinessScore; }
};

class EmotionalCalibration : public DecisionComponent {
private:
    std::vector<std::pair<std::string, float>> emotions;
    float regulationScore;
    std::string regulationStrategy;

    void identifyEmotions(HWND hWnd);
    void evaluateProportionality();
    void applyRegulationStrategy(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return regulationScore; }
};

class ImpulseAlignment : public DecisionComponent {
private:
    float alignmentScore;
    std::vector<std::string> impulses;

    void simulateImpulses(HWND hWnd);
    void evaluateAlignment(HWND hWnd);
    void applyCBT_ACT_Tools(HWND hWnd);

public:
    void execute(HWND hWnd) override;
    std::string getReport() const override;
    float getScore() const override { return alignmentScore; }
};