#include "Player_GUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioProcessor)
    : audioPlayer(audioProcessor)
{
    // إعداد الأزرار وإضافة المستمعين
    for (auto* btn : { &loadButton, &playPauseButton, &stopButton, &restartButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // إعداد منزلق الصوت
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

     loopButton.setClickingTogglesState(true);

    updateLoopButton();  // Set initial state  

    updatePlayButton(); // تحديث حالة زر التشغيل الأولي
}

void PlayerGUI::paint(juce::Graphics& g)
{
    // رسم خلفية الواجهة
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    auto buttonRow = area.removeFromTop(50); // صف للأزرار

    // توزيع الأزرار في الصف
    loadButton.setBounds(buttonRow.removeFromLeft(100).reduced(2));
    playPauseButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
    stopButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
    restartButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
      loopButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
    // وضع منزلق الصوت في المنطقة المتبقية
    volumeSlider.setBounds(area.removeFromTop(30).reduced(20, 5));
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        // فتح منتقي الملفات لتحميل ملف صوتي
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3;*.aiff;*.flac");

        auto folder = juce::File::getSpecialLocation(juce::File::userMusicDirectory);
        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this, folder](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    // تحميل الملف المحدد
                    audioPlayer.loadFile(file);
                    updatePlayButton(); // تحديث واجهة المستخدم
                    updateLoopButton();            
                 }
            });
    }
    else if (button == &playPauseButton)
    {
        // تبديل بين التشغيل والإيقاف المؤقت
        audioPlayer.togglePlayPause();
        updatePlayButton();
    }
    else if (button == &stopButton)
    {
        // إيقاف التشغيل
        audioPlayer.stop();
        updatePlayButton();
    }
    else if (button == &restartButton)
    {
        // إعادة التشغيل من البداية
        audioPlayer.restart();
        updatePlayButton();
    }
     else if (button == &loopButton) 
    {
        // Toggle looping state
        audioPlayer.toggleLoop();
        updateLoopButton();  // Update button appearance
        
        // Debug output
        DBG("Looping: " + juce::String(audioPlayer.isLooping() ? "ON" : "OFF"));
    }
}


void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        // ضبط مستوى الصوت عند تغيير المنزلق
        audioPlayer.setGain((float)slider->getValue());
    }
}

void PlayerGUI::updatePlayButton()
{
    if (audioPlayer.isFileLoaded()) {
        // تحديث نص زر التشغيل/الإيقاف بناءً على الحالة
        if (audioPlayer.isPlaying()) {
            playPauseButton.setButtonText("Pause");
        }
        else {
            playPauseButton.setButtonText("Play");
        }
        playPauseButton.setEnabled(true); // تمكين الزر إذا كان هناك ملف محمل
    }
    else {
        playPauseButton.setButtonText("Play");
        playPauseButton.setEnabled(false); // تعطيل الزر إذا لم يكن هناك ملف محمل
    }
}
void PlayerGUI::updateLoopButton()
{
    // Update loop button appearance based on state
    if (audioPlayer.isLooping()) {
        loopButton.setButtonText("Loop: ON");
        loopButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    }
    else {
        loopButton.setButtonText("Loop: OFF");
        loopButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    }
    
    // Enable/disable based on whether a file is loaded
    loopButton.setEnabled(audioPlayer.isFileLoaded());
}
void PlayerGUI::loadFile()
{
    // تم التنفيذ في buttonClicked
}