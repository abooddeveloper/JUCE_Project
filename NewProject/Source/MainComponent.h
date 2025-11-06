#pragma once
#include <JuceHeader.h>
#include "Player_GUI.h"
#include "Player_Audio.h"

class MainComponent : public juce::AudioAppComponent,
    public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void timerCallback() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    PlayerAudio audioPlayer;
    PlayerGUI guiComponent;
};