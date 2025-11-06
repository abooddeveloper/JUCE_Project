#pragma once
#include <JuceHeader.h>
#include "Player_Audio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ListBoxModel
{
public:
    PlayerGUI(PlayerAudio& audioProcessor);
    ~PlayerGUI() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    juce::String time_in_minutes(double time);
    juce::String time_in_seconds(double time);

    void updatePlayButton();
    void updateMuteButton();
    void updateMetadataDisplay();
    void updateSpeedButtons();

    // دوال جديدة للرسم البياني الحقيقي
    void paintRealWaveform(juce::Graphics& g, juce::Rectangle<int> area);
    void updateWaveform();

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g,
        int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent& event) override;
    void backgroundClicked(const juce::MouseEvent& event) override;
    void deleteKeyPressed(int lastRowSelected) override;
    void returnKeyPressed(int lastRowSelected) override;

private:
    PlayerAudio& audioPlayer;

    bool will_looping ;
   
    double total_time ;
    double current_time ;
    juce::String time_text;
    double loop_start_point=0.0 ;
    double loop_end_point=1.0 ;

    // الأزرار الأساسية
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playPauseButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loop_button{ "Loop" };

    // أزرار التحكم في السرعة الجديدة
    juce::TextButton normalSpeedButton{ "Normal Speed" };
    juce::TextButton increaseSpeedButton{ "Increase Speed" };
    juce::TextButton decreaseSpeedButton{ "Decrease Speed" };

    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Label speedLabel;

    juce::Slider position_slider;
    juce::Label label_time;
    juce::Slider loop_slider;
    juce::TextButton range_loop_button{ "Range Loop" };

    juce::Label metadataLabel;
    juce::TextEditor metadataDisplay;

    juce::TextButton addToPlaylistButton{ "Add to Playlist" };
    juce::TextButton clearPlaylistButton{ "Clear Playlist" };
    juce::ListBox playlistBox;
    juce::StringArray playlistFiles;
    std::vector<juce::File> playlistFileObjects;

    // متغيرات جديدة للرسم البياني الحقيقي
    juce::Rectangle<int> waveformArea;
    bool hasRealWaveform = false;

    std::unique_ptr<juce::FileChooser> fileChooser;
};