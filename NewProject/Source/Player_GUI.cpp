#include "Player_GUI.h"

// ==============================================================================
// البناء - تهيئة واجهة المستخدم
// ==============================================================================
PlayerGUI::PlayerGUI(PlayerAudio& audioProcessor)
    : audioPlayer(audioProcessor)
{
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

    // ==========================================================================
    // تحديث الحالات الأولية للأزرار
    // ==========================================================================
    updatePlayButton();
    updateLoopButton();
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
    loopButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
    muteButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));

    // وضع منزلق الصوت في المنطقة المتبقية
    volumeSlider.setBounds(area.removeFromTop(30).reduced(20, 5));
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
                    updateLoopButton();
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
    else if (button == &loopButton)
    {
        audioPlayer.toggleLoop();
        updateLoopButton();
        DBG("Looping: " + juce::String(audioPlayer.isLooping() ? "ON" : "OFF"));
    }
    else if (button == &muteButton)
    {
        audioPlayer.toggleMute();
        updateMuteButton();
    }
}

// ==============================================================================
// معالجة تغيير قيمة المنزلق
// ==============================================================================
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audioPlayer.setGain((float)slider->getValue());
    }
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
// تحديث حالة زر التكرار
// ==============================================================================
void PlayerGUI::updateLoopButton()
{
    if (audioPlayer.isLooping()) {
        loopButton.setButtonText("Loop: ON");
        loopButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    }
    else {
        loopButton.setButtonText("Loop: OFF");
        loopButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    }
    loopButton.setEnabled(audioPlayer.isFileLoaded());
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