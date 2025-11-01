#pragma once
#include <JuceHeader.h>
#include "Player_Audio.h"

// ==============================================================================
// واجهة المستخدم للتحكم في مشغل الصوت
// ==============================================================================
class PlayerGUI : public juce::Component,
                  public juce::Button::Listener,
                  public juce::Slider::Listener,
                  public juce::Timer
    
{
public:
    PlayerGUI(PlayerAudio& audioProcessor);
    ~PlayerGUI() override = default;

    void timerCallback() override;
    // دوال Component
    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::String time_in_minutes(double time);
    juce::String time_in_seconds(int time);


    // ==========================================================================
    // دوال Component للرسم والتخطيط
    // ==========================================================================
    

    // ==========================================================================

    // معالجة الأحداث
    // ==========================================================================
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // ==========================================================================
    // دوال تحديث الواجهة
    // ==========================================================================
    void updatePlayButton(); // تحديث حالة زر التشغيل
    void updateMuteButton(); // تحديث حالة زر الكتم

private:
    PlayerAudio& audioPlayer; // المرجع إلى معالج الصوت

     bool will_looping;
     
    // أزرار التحكم


    // ==========================================================================
    // عناصر واجهة المستخدم
    // ==========================================================================

    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playPauseButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton muteButton{ "Mute" };
    juce::ToggleButton loopButton{ "Loop" };
    juce::Slider volumeSlider; // منزلق التحكم في الصوت

    juce::TextButton loop_button{ "loop" };
    juce::Slider position_slider;
    juce::Slider loop_slider;
    juce::Label label_time;
    double total_time;
    double current_time;
    juce::String time_text;
    double loop_start_point;
    double loop_end_point;
    juce::TextButton range_loop_button{ "range_loop" };
    void loadFile(); // دالة تحميل الملف

    std::unique_ptr<juce::FileChooser> fileChooser; // منتقي الملفات
};