#include "Player_GUI.h"

// ==============================================================================
// البناء - تهيئة واجهة المستخدم
// ==============================================================================
PlayerGUI::PlayerGUI(PlayerAudio& audioProcessor)
    : audioPlayer(audioProcessor)
{

    // إعداد الأزرار وإضافة المستمعين
    for (auto* btn : { &loadButton, &playPauseButton, &stopButton, &restartButton,&loop_button,&range_loop_button })

    // ==========================================================================
    // إعداد الأزرار وإضافة المستمعين للأحداث
    // ==========================================================================
    for (auto* btn : { &loadButton, &playPauseButton, &stopButton, &restartButton, &muteButton })

    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // إعداد زر التكرار
    loopButton.setClickingTogglesState(true);
    loopButton.addListener(this);
    addAndMakeVisible(loopButton);

    // ==========================================================================
    // إعداد منزلق الصوت
    // ==========================================================================
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    position_slider.setRange(0.0, 1.0, 0.01);
    position_slider.setValue(0.0);
    position_slider.addListener(this);
    addAndMakeVisible(position_slider);
    position_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    position_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    startTimerHz(50);
    addAndMakeVisible(label_time);
    label_time.setText("0:00 / 0:00", juce::dontSendNotification);
    label_time.setColour(juce::Label::textColourId, juce::Colours::white);
    label_time.setJustificationType(juce::Justification::centredRight);
    label_time.setVisible(audioPlayer.label_time_visibility());
    loop_slider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    loop_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(loop_slider);
    loop_slider.setVisible(false);
    loop_slider.setMinAndMaxValues(0.1, 0.6);
    loop_slider.setRange(0.0, 1.0, 0.01);
    loop_slider.addListener(this);
    loop_button.setClickingTogglesState(true);
    range_loop_button.setClickingTogglesState(true);
    updatePlayButton(); // تحديث حالة زر التشغيل الأولي


    // ==========================================================================
    // تحديث الحالات الأولية للأزرار
    // ==========================================================================
    updatePlayButton();
    
    updateMuteButton();

}

// ==============================================================================
// الرسم - رسم خلفية الواجهة
// ==============================================================================
void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

// ==============================================================================
// إعادة التحجيم - توزيع عناصر الواجهة
// ==============================================================================
void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    auto buttonRow = area.removeFromTop(50); // صف للأزرار

    // توزيع الأزرار في الصف بشكل متساو
    loadButton.setBounds(buttonRow.removeFromLeft(100).reduced(2));
    playPauseButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
    stopButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
    restartButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));

    loop_button.setBounds(buttonRow.removeFromLeft(80).reduced(2));
    range_loop_button.setBounds(buttonRow.removeFromLeft(100).reduced(2));

    loopButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
    muteButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));


    // وضع منزلق الصوت في المنطقة المتبقية
    volumeSlider.setBounds(area.removeFromTop(30).reduced(20, 5));
    auto slider_area = area.removeFromTop(30).reduced(20, 5);
    loop_slider.setBounds(slider_area);
    position_slider.setBounds(slider_area);
    label_time.setBounds(area.removeFromTop(25).reduced(20, 0));
}

// ==============================================================================
// معالجة النقر على الأزرار
// ==============================================================================
void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        // فتح منتقي الملفات لتحميل ملف صوتي
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File::getSpecialLocation(juce::File::userMusicDirectory),
            "*.wav;*.mp3;*.aiff;*.flac");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    audioPlayer.loadFile(file);
                    updatePlayButton();
                    
                }
            });
    }
    else if (button == &playPauseButton)
    {
        audioPlayer.togglePlayPause();
        updatePlayButton();
    }
    else if (button == &stopButton)
    {
        audioPlayer.stop();
        updatePlayButton();
    }
    else if (button == &restartButton)
    {
        audioPlayer.restart();
        updatePlayButton();
    }

    else if (button == &loop_button) {
        will_looping = loop_button.getToggleState();
        if (will_looping) {
            loop_button.setButtonText("Loop:on");
            audioPlayer.loop_on();
        }
        else {
            loop_button.setButtonText("Loop:off");
        }
    }
    else if (button == &range_loop_button) {
        if (range_loop_button.getToggleState()) {
            range_loop_button.setButtonText("range_Loop:on");
            loop_slider.setVisible(true);
            audioPlayer.set_loop_by_buttons(loop_start_point, loop_end_point);
            audioPlayer.set_slider_looping();
        }
        else {
            range_loop_button.setButtonText("range_Loop:off");
            loop_slider.setVisible(false);
            audioPlayer.set_loop_by_buttons(loop_start_point, loop_end_point);
        }
    }
}
void PlayerGUI::timerCallback() {
    label_time.setVisible(audioPlayer.label_time_visibility());
    current_time = audioPlayer.get_current_time();
    total_time = audioPlayer.get_total_time();
    time_text = time_in_minutes(current_time) + ":" + time_in_seconds(current_time) + " / " + time_in_minutes(total_time) + ":" + time_in_seconds(total_time);
    label_time.setText(time_text, juce::dontSendNotification);
    position_slider.setValue(current_time / total_time, juce::dontSendNotification);
    if (range_loop_button.getToggleState() && audioPlayer.loop_position_state()) {
        audioPlayer.set_slider_looping();
    }
    if (will_looping && audioPlayer.is_transportSource_playing()) {
        audioPlayer.loop_on();
    }
    
}
//loadfile()
// ==============================================================================
// معالجة تغيير قيمة المنزلق
// ==============================================================================
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audioPlayer.setGain((float)slider->getValue());
    }
    else if (slider == &position_slider) {
        if (audioPlayer.isFileLoaded()) {
            audioPlayer.position_slider_value(position_slider.getValue());
        }
    }
    else if (slider == &loop_slider) {
        loop_start_point = slider->getMinValue();
        loop_end_point = slider->getMaxValue();
        audioPlayer.set_loop_by_buttons(loop_start_point, loop_end_point);
        audioPlayer.set_slider_looping();

    }
}
juce::String PlayerGUI::time_in_minutes(double time) {
    int minutes = time / 60;
    return juce::String(minutes);
}
juce::String PlayerGUI::time_in_seconds(int time) {
    int seconds = (time % 60);
    return (seconds < 10 ? "0" + juce::String(seconds) : juce::String(seconds));
}

// ==============================================================================
// تحديث حالة زر التشغيل/الإيقاف
// ==============================================================================
void PlayerGUI::updatePlayButton()
{
    if (audioPlayer.isFileLoaded()) {
        if (audioPlayer.isPlaying()) {
            playPauseButton.setButtonText("Pause");
        }
        else {
            playPauseButton.setButtonText("Play");
        }
        playPauseButton.setEnabled(true);
    }
    else {
        playPauseButton.setButtonText("Play");
        playPauseButton.setEnabled(false);
    }
}


// ==============================================================================
// تحديث حالة زر الكتم
// ==============================================================================
void PlayerGUI::updateMuteButton()
{
    if (audioPlayer.isMuted()) {
        muteButton.setButtonText("Unmute");
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    }
    else {
        muteButton.setButtonText("Mute");
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
}