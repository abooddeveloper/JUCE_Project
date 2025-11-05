#pragma once
#include <JuceHeader.h>
#include "Player_GUI.h"
#include "Player_Audio.h"

// ==============================================================================
// المكون الرئيسي الذي يدير الصوت والواجهة
// ==============================================================================
class MainComponent : public juce::AudioAppComponent,
    public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    // ==========================================================================
    // دوال AudioAppComponent المطلوبة
    // ==========================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // ==========================================================================
    // دوال Timer للتحكم في التحديثات الدورية
    // ==========================================================================
    void timerCallback() override;

    // ==========================================================================
    // دوال Component للرسم والتخطيط
    // ==========================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    PlayerAudio audioPlayer;      // معالج الصوت
    PlayerGUI guiComponent;       // واجهة المستخدم
};