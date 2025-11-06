#pragma once
#include <JuceHeader.h>

class PlayerAudio : public juce::AudioAppComponent
{
public:
    PlayerAudio();
    ~PlayerAudio() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadFile(const juce::File& file);
    void play();
    void pause();
    void stop();
    void restart();
    void togglePlayPause();
    void setGain(float gain);
    void setPosition(double position);
    void toggleMute();

    void setPlaybackSpeed(float speed);
    float getPlaybackSpeed() const;

    void loop_on();
    void position_slider_value(double slider_value);
    void set_loop_by_buttons(double start_point, double end_point);
    void set_slider_looping();
    bool is_transportSource_playing();

    bool isPlaying() const;
    bool isPaused() const;
    bool isFileLoaded() const;
    bool isMuted() const;
    double getCurrentPosition() const;
    double getTotalLength() const;
    bool label_time_visibility();
    bool loop_position_state();
    double get_total_time();
    double get_current_time();

    juce::String getDebugInfo() const;

    float getGain() const { return currentVolume; }
    bool isTransportSourceActive() const { return transportSource.isPlaying() || isPaused(); }

    juce::String getCurrentFileName() const;
    juce::StringArray getMetadata() const;
    juce::String getFormattedDuration() const;

    // دوال جديدة للرسم البياني
    juce::AudioFormatManager& getFormatManager() { return formatManager; }
    void generateWaveformData(const juce::File& file);
    const std::vector<float>& getWaveformData() const { return waveformData; }
    int getWaveformSize() const { return static_cast<int>(waveformData.size()); }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource;

    double currentSampleRate = 44100.0;
    float playbackSpeed = 1.0f;
    bool playing = false;
    float currentVolume = 0.5f;
    juce::String currentFileName;
    bool isMutedFlag = false;
    float volumeBeforeMute = 0.5f;

    double current_time = 0.0;
    double total_time = 0.0;
    double start_position_time = 0.0;
    double end_position_time = 1.0;

    juce::StringPairArray metadataArray;

    // بيانات الرسم البياني
    std::vector<float> waveformData;
    int waveformSamples = 1000;
};