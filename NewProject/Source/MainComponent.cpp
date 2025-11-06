#include "MainComponent.h"

MainComponent::MainComponent()
    : audioPlayer(),
    guiComponent(audioPlayer)
{
    addAndMakeVisible(guiComponent);
    setSize(800, 600);
    startTimer(33);
}

MainComponent::~MainComponent()
{
    stopTimer();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    audioPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (bufferToFill.buffer != nullptr && bufferToFill.numSamples > 0)
    {
        audioPlayer.getNextAudioBlock(bufferToFill);
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void MainComponent::releaseResources()
{
    audioPlayer.releaseResources();
}

void MainComponent::timerCallback()
{
    static int counter = 0;

    if (counter++ % 100 == 0)
    {
        if (audioPlayer.isFileLoaded())
        {
            if (audioPlayer.isPlaying())
            {
                DBG("=== AUDIO PLAYER STATUS ===");
                DBG("State: PLAYING");
                DBG("Position: " + juce::String(audioPlayer.getCurrentPosition(), 2) + "s");
                DBG("Total Length: " + juce::String(audioPlayer.getTotalLength(), 2) + "s");
                DBG("Speed: " + juce::String(audioPlayer.getPlaybackSpeed()) + "x");
                DBG("Muted: " + juce::String(audioPlayer.isMuted() ? "YES" : "NO"));
                DBG("Volume: " + juce::String(audioPlayer.getGain(), 2));

                double progress = (audioPlayer.getTotalLength() > 0.0) ?
                    (audioPlayer.getCurrentPosition() / audioPlayer.getTotalLength()) * 100.0 : 0.0;
                DBG("Progress: " + juce::String(progress, 1) + "%");
                DBG("=============================");
            }
            else if (!audioPlayer.isPlaying() && audioPlayer.getCurrentPosition() > 0.0)
            {
                DBG("=== AUDIO PLAYER STATUS ===");
                DBG("State: PAUSED");
                DBG("Position: " + juce::String(audioPlayer.getCurrentPosition(), 2) + "s");
                DBG("File: " + audioPlayer.getCurrentFileName());
                DBG("=============================");
            }
        }
        else
        {
            DBG("=== AUDIO PLAYER STATUS ===");
            DBG("State: NO FILE LOADED");
            DBG("Ready for file loading...");
            DBG("=============================");
        }

        if (counter % 300 == 0)
        {
            DBG("*** MEMORY CHECK ***");
            DBG("Reader Source: " + juce::String(audioPlayer.isFileLoaded() ? "VALID" : "NULL"));
            DBG("Transport Source Active: " + juce::String(audioPlayer.isTransportSourceActive() ? "YES" : "NO"));
            DBG("********************");
        }
    }
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    guiComponent.setBounds(getLocalBounds());
}