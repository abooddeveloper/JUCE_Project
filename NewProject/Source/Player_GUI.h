#pragma once
#include <JuceHeader.h>
#include "Player_Audio.h"

// واجهة المستخدم للتحكم في مشغل الصوت
class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI(PlayerAudio& audioProcessor);
    ~PlayerGUI() override = default;

    // دوال Component
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // معالجة الأحداث
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void updatePlayButton(); // تحديث حالة زر التشغيل
    void updateLoopButton(); 
private:
    PlayerAudio& audioPlayer; // المرجع إلى معالج الصوت

    // أزرار التحكم
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playPauseButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton restartButton{ "Restart" };
    juce::Slider volumeSlider; // منزلق التحكم في الصوت
      juce::ToggleButton loopButton{ "Loop" };
    void loadFile(); // دالة تحميل الملف
    std::unique_ptr<juce::FileChooser> fileChooser; // منتقي الملفات
};