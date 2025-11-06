#include "Player_GUI.h"

// ==============================================================================
// البناء - تهيئة واجهة المستخدم
// ==============================================================================
PlayerGUI::PlayerGUI(PlayerAudio& audioProcessor)
    : audioPlayer(audioProcessor),
    metadataLabel("metadataLabel", "Metadata:"),
    playlistBox("playlistBox", this)
{
    // ==========================================================================
    // إعداد الأزرار وإضافة المستمعين للأحداث
    // ==========================================================================
    for (auto* btn : { &loadButton, &playPauseButton, &stopButton, &restartButton, &muteButton,&loop_button,&range_loop_button })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // إعداد زر التكرار
    loop_button.setClickingTogglesState(true);
    addAndMakeVisible(loop_button);

    // إعداد أزرار التكرار الإضافية
    range_loop_button.setClickingTogglesState(true);
    range_loop_button.addListener(this);
    addAndMakeVisible(range_loop_button);

    // ==========================================================================
    // إعداد منزلق الصوت
    // ==========================================================================
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // إعداد منزلق السرعة
    // ==========================================================================
    speedSlider.setRange(0.25, 4.0, 0.05); // من 0.25x إلى 4x السرعة
    speedSlider.setValue(1.0); // سرعة عادية
    speedSlider.setSkewFactor(0.5); // جعل المنزلق لوغاريتمي
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    speedLabel.setText("Speed:", juce::dontSendNotification);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    speedLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(speedLabel);

    // إعداد منزلق الموضع
    // ==========================================================================
    position_slider.setRange(0.0, 1.0, 0.001);
    position_slider.setValue(0.0);
    position_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    position_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    position_slider.addListener(this);
    addAndMakeVisible(position_slider);

    /*positionLabel.setText("Position:", juce::dontSendNotification);
    positionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    positionLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(positionLabel);*/
    addAndMakeVisible(label_time);
    label_time.setText("0:00 / 0:00", juce::dontSendNotification);
    label_time.setColour(juce::Label::textColourId, juce::Colours::white);
    label_time.setJustificationType(juce::Justification::centredRight);
    label_time.setVisible(audioPlayer.label_time_visibility());

    // إعداد منزلق التكرار
    loop_slider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    loop_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    loop_slider.setRange(0.0, 1.0, 0.01);
    loop_slider.setMinAndMaxValues(0.1, 0.6);
    loop_slider.addListener(this);
    addAndMakeVisible(loop_slider);
    loop_slider.setVisible(false);
   

    // ==========================================================================
    // إعداد عناصر البيانات الوصفية
    // ==========================================================================
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(metadataLabel);

    metadataDisplay.setMultiLine(true);
    metadataDisplay.setReadOnly(true);
    metadataDisplay.setCaretVisible(false);
    metadataDisplay.setScrollbarsShown(true);
    metadataDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey);
    metadataDisplay.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(metadataDisplay);

    // ==========================================================================
    // إعداد عناصر قائمة التشغيل
    // ==========================================================================
    addToPlaylistButton.addListener(this);
    addAndMakeVisible(addToPlaylistButton);

    clearPlaylistButton.addListener(this);
    addAndMakeVisible(clearPlaylistButton);

    playlistBox.setMultipleSelectionEnabled(false);
    playlistBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::lightgrey);
    addAndMakeVisible(playlistBox);

    // بدء المؤقت للتحديثات الدورية
    startTimerHz(30); // 30 مرة في الثانية

    // ==========================================================================
    // تحديث الحالات الأولية للأزرار
    // ==========================================================================
    updatePlayButton();
    updateMuteButton();
    updateMetadataDisplay();
}

// ==============================================================================
// الرسم - رسم خلفية الواجهة
// ==============================================================================
void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // رسم شريط تقدم بسيط
    if (audioPlayer.isFileLoaded())
    {
        auto area = getLocalBounds();
        auto progressArea = area.removeFromTop(3); // شريط تقدم رفيع أعلى الواجهة

        double progress = 0.0;
        if (audioPlayer.getTotalLength() > 0.0)
        {
            progress = audioPlayer.getCurrentPosition() / audioPlayer.getTotalLength();
        }

        int progressWidth = static_cast<int>(progress * progressArea.getWidth());

        g.setColour(juce::Colours::lightblue);
        g.fillRect(progressArea.getX(), progressArea.getY(), progressWidth, progressArea.getHeight());

        g.setColour(juce::Colours::white);
        g.drawRect(progressArea);
    }
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
    muteButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));

    // منطقة أزرار قائمة التشغيل
    auto playlistButtonRow = area.removeFromTop(30);
    addToPlaylistButton.setBounds(playlistButtonRow.removeFromLeft(120).reduced(2));
    clearPlaylistButton.setBounds(playlistButtonRow.removeFromLeft(120).reduced(2));

    // منطقة السرعة
    auto speedArea = area.removeFromTop(30).reduced(20, 5);
    speedLabel.setBounds(speedArea.removeFromLeft(60));
    speedSlider.setBounds(speedArea);

    
    
    // وضع منزلق الصوت في المنطقة المتبقية
    volumeSlider.setBounds(area.removeFromTop(30).reduced(20, 5));

    auto slider_area = area.removeFromTop(30).reduced(20, 5);
    loop_slider.setBounds(slider_area);
    position_slider.setBounds(slider_area);
    label_time.setBounds(area.removeFromTop(25).reduced(20, 0));

    // منطقة البيانات الوصفية
    auto metadataArea = area.removeFromBottom(100);
    metadataLabel.setBounds(metadataArea.removeFromTop(20).reduced(5, 0));
    metadataDisplay.setBounds(metadataArea.reduced(5));

    // منطقة قائمة التشغيل
    playlistBox.setBounds(area.reduced(5));
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
                    
                    updateMetadataDisplay();
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
    else if (button == &loop_button)
    {
        will_looping = loop_button.getToggleState();
        if (will_looping) {
            loop_button.setButtonText("Loop:on");
            audioPlayer.loop_on();
        }
        else {
            loop_button.setButtonText("Loop:off");
        }
    }
    else if (button == &range_loop_button)
    {
        if (range_loop_button.getToggleState()) {
            range_loop_button.setButtonText("range_Loop:on");
            loop_slider.setVisible(true);
            audioPlayer.set_loop_by_buttons(loop_start_point, loop_end_point);
            audioPlayer.set_slider_looping();
        }
        else {
            range_loop_button.setButtonText("range_Loop:off");
            loop_slider.setVisible(false);
        }
    }
    else if (button == &muteButton)
    {
        audioPlayer.toggleMute();
        updateMuteButton();
    }
    else if (button == &addToPlaylistButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select audio files to add to playlist...",
            juce::File::getSpecialLocation(juce::File::userMusicDirectory),
            "*.wav;*.mp3;*.aiff;*.flac");

        // استخدام اسم متغير مختلف بدلاً من 'flags'
        auto fileBrowserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems;

        fileChooser->launchAsync(
            fileBrowserFlags, // استخدام المتغير الجديد
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                for (auto& file : results)
                {
                    if (file.existsAsFile())
                    {
                        playlistFiles.add(file.getFileName());
                        playlistFileObjects.push_back(file);
                    }
                }
                playlistBox.updateContent();
            });
    }
    else if (button == &clearPlaylistButton)
    {
        playlistFiles.clear();
        playlistFileObjects.clear();
        playlistBox.updateContent();
    }
}

// ==============================================================================
// رد نداء المؤقت - للتحديثات الدورية
// ==============================================================================
void PlayerGUI::timerCallback()
{
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

// ==============================================================================
// معالجة تغيير قيمة المنزلق
// ==============================================================================
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audioPlayer.setGain((float)slider->getValue());
    }
    // معالجة تغيير سرعة التشغيل
    else if (slider == &speedSlider)
    {
        audioPlayer.setPlaybackSpeed((float)slider->getValue());
    }
    // معالجة تغيير الموضع
    else if (slider == &position_slider)
    {
        if (audioPlayer.isFileLoaded()) {
            audioPlayer.position_slider_value(position_slider.getValue());
        }
    }
    // معالجة تغيير نطاق التكرار
    else if (slider == &loop_slider)
    {
        loop_start_point = slider->getMinValue();
        loop_end_point = slider->getMaxValue();
        audioPlayer.set_loop_by_buttons(loop_start_point, loop_end_point);
        audioPlayer.set_slider_looping();
    }
}

// ==============================================================================
// دوال تحويل الوقت
// ==============================================================================
juce::String PlayerGUI::time_in_minutes(double time)
{
    int minutes = time / 60;
    return juce::String(minutes);
}

juce::String PlayerGUI::time_in_seconds(int time)
{
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
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
    }
}

// ==============================================================================
// تحديث عرض البيانات الوصفية
// ==============================================================================
void PlayerGUI::updateMetadataDisplay()
{
    if (audioPlayer.isFileLoaded()) {
        auto metadata = audioPlayer.getMetadata();
        juce::String displayText;

        for (auto& line : metadata) {
            displayText += line + "\n";
        }

        metadataDisplay.setText(displayText);
    }
    else {
        metadataDisplay.setText("No file loaded");
    }
}

// ==============================================================================
// دوال ListBoxModel المطلوبة للميزة 8: قائمة التشغيل
// ==============================================================================
int PlayerGUI::getNumRows()
{
    return playlistFiles.size();
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g,
    int width, int height, bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= playlistFiles.size())
        return;

    if (rowIsSelected) {
        g.fillAll(juce::Colours::lightblue);
    }
    else {
        if (rowNumber % 2 == 0) {
            g.fillAll(juce::Colours::white);
        }
        else {
            g.fillAll(juce::Colour(0xFFEEEEEE));
        }
    }

    g.setColour(juce::Colours::black);
    g.setFont(14.0f);
    g.drawText(playlistFiles[rowNumber],
        5, 0, width - 5, height,
        juce::Justification::centredLeft, true);
}

void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected >= 0 && lastRowSelected < playlistFileObjects.size()) {
        audioPlayer.loadFile(playlistFileObjects[lastRowSelected]);
        updatePlayButton();
        
        updateMetadataDisplay();
    }
}

void PlayerGUI::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
    selectedRowsChanged(row);
}

void PlayerGUI::backgroundClicked(const juce::MouseEvent&)
{
    playlistBox.deselectAllRows();
}

void PlayerGUI::deleteKeyPressed(int lastRowSelected)
{
    if (lastRowSelected >= 0 && lastRowSelected < playlistFiles.size()) {
        playlistFiles.remove(lastRowSelected);
        playlistFileObjects.erase(playlistFileObjects.begin() + lastRowSelected);
        playlistBox.updateContent();
    }
}

void PlayerGUI::returnKeyPressed(int lastRowSelected)
{
    selectedRowsChanged(lastRowSelected);
}