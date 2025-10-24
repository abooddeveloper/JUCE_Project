#pragma once
#include <JuceHeader.h>
#include "Player_Audio.h"

// ==============================================================================
// واجهة المستخدم للتحكم في مشغل الصوت
// ==============================================================================
class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI(PlayerAudio& audioProcessor);
    ~PlayerGUI() override = default;

    // ==========================================================================
    // دوال Component للرسم والتخطيط
    // ==========================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    // ==========================================================================
    // معالجة الأحداث
    // ==========================================================================
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // ==========================================================================
    // دوال تحديث الواجهة
    // ==========================================================================
    void updatePlayButton(); // تحديث حالة زر التشغيل
    void updateLoopButton(); // تحديث حالة زر التكرار
    void updateMuteButton(); // تحديث حالة زر الكتم

private:
    PlayerAudio& audioPlayer; // المرجع إلى معالج الصوت

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

    std::unique_ptr<juce::FileChooser> fileChooser; // منتقي الملفات
};