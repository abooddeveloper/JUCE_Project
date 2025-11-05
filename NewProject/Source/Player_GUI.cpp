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
    for (auto* btn : { &loadButton, &playPauseButton, &stopButton, &restartButton, &muteButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // إعداد زر التكرار
    loopButton.setClickingTogglesState(true);
    loopButton.addListener(this);
    addAndMakeVisible(loopButton);

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
    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.setValue(0.0);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);

    positionSlider.onDragStart = [this]() {
        isPositionSliderDragging = true; // بدأ المستخدم في السحب
        };

    positionSlider.onDragEnd = [this]() {
        isPositionSliderDragging = false; // انتهى السحب
        };
    addAndMakeVisible(positionSlider);

    positionLabel.setText("Position:", juce::dontSendNotification);
    positionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    positionLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(positionLabel);

    timeLabel.setText("0:00 / 0:00", juce::dontSendNotification);
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    timeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(timeLabel);

    // إعداد منزلق التكرار
    loop_slider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    loop_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    loop_slider.setRange(0.0, 1.0, 0.01);
    loop_slider.setMinAndMaxValues(0.1, 0.6);
    loop_slider.addListener(this);
    loop_slider.setVisible(false);
    addAndMakeVisible(loop_slider);

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
    updateLoopButton();
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
    loopButton.setBounds(buttonRow.removeFromLeft(80).reduced(2));
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

    // منطقة الموضع والوقت
    auto positionTimeArea = area.removeFromTop(40).reduced(20, 5);
    positionLabel.setBounds(positionTimeArea.removeFromLeft(60));
    timeLabel.setBounds(positionTimeArea.removeFromRight(100));
    positionSlider.setBounds(positionTimeArea);

    // منطقة منزلق التكرار
    auto loopSliderArea = area.removeFromTop(30).reduced(20, 5);
    loop_slider.setBounds(loopSliderArea);

    // وضع منزلق الصوت في المنطقة المتبقية
    volumeSlider.setBounds(area.removeFromTop(30).reduced(20, 5));

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
                    updateLoopButton();
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
    else if (button == &loopButton)
    {
        audioPlayer.toggleLoop();
        updateLoopButton();
        DBG("Looping: " + juce::String(audioPlayer.isLooping() ? "ON" : "OFF"));
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
    // تحديث تسمية الوقت
    if (audioPlayer.isFileLoaded())
    {
        double currentTime = audioPlayer.getCurrentPosition();
        double totalTime = audioPlayer.getTotalLength();

        // تحويل الوقت إلى دقائق وثواني
        auto formatTime = [](double time) -> juce::String {
            int minutes = static_cast<int>(time) / 60;
            int seconds = static_cast<int>(time) % 60;
            return juce::String(minutes) + ":" + (seconds < 10 ? "0" : "") + juce::String(seconds);
            };

        timeLabel.setText(formatTime(currentTime) + " / " + formatTime(totalTime),
            juce::dontSendNotification);

        // تحديث منزلق الموضع
        if (totalTime > 0.0 && !isPositionSliderDragging)
        {
            double position = currentTime / totalTime;
            positionSlider.setValue(position, juce::dontSendNotification);
        }

        // تحديث التكرار المدى
        if (range_loop_button.getToggleState() && audioPlayer.loop_position_state()) {
            audioPlayer.set_slider_looping();
        }
    }
    else
    {
        timeLabel.setText("0:00 / 0:00", juce::dontSendNotification);
        if (!isPositionSliderDragging) // لا نحدث المنزلق إذا كان المستخدم يسحبه
        {
            positionSlider.setValue(0.0, juce::dontSendNotification);
        }
    }

    // إعادة الرسم لتحديث شريط التقدم
    repaint();
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
    else if (slider == &positionSlider)
    {
        if (audioPlayer.isFileLoaded() && isPositionSliderDragging)
        {
            double newPosition = slider->getValue() * audioPlayer.getTotalLength();
            audioPlayer.setPosition(newPosition);
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
        updateLoopButton();
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