#include "Player_GUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audioProcessor)
    : audioPlayer(audioProcessor),
    metadataLabel("metadataLabel", "Metadata:"),
    playlistBox("playlistBox", this)
{
    // قائمة جميع الأزرار بما في ذلك الجديدة
    juce::Button* buttons[] = {
        &loadButton, &playPauseButton, &stopButton, &restartButton,
        &muteButton, &loop_button, &range_loop_button,
        &normalSpeedButton, &increaseSpeedButton, &decreaseSpeedButton
    };
    const int numButtons = sizeof(buttons) / sizeof(buttons[0]);

    for (int i = 0; i < numButtons; ++i)
    {
        buttons[i]->addListener(this);
        addAndMakeVisible(buttons[i]);
    }

    loop_button.setClickingTogglesState(true);
    range_loop_button.setClickingTogglesState(true);

    // إعداد منزلق الصوت
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // إعداد منزلق السرعة
    speedSlider.setRange(0.25f, 4.0f, 0.05f);
    speedSlider.setValue(1.0f);
    speedSlider.setSkewFactor(0.5f);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    speedLabel.setText("Speed:", juce::dontSendNotification);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    speedLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(speedLabel);

    // إعداد منزلق الموضع
    position_slider.setRange(0.0, 1.0, 0.001);
    position_slider.setValue(0.0);
    position_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    position_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    position_slider.addListener(this);
    addAndMakeVisible(position_slider);

    // إعداد تسمية الوقت
    addAndMakeVisible(label_time);
    label_time.setText("0:00 / 0:00", juce::dontSendNotification);
    label_time.setColour(juce::Label::textColourId, juce::Colours::white);
    label_time.setJustificationType(juce::Justification::centredRight);

    // إعداد منزلق التكرار
    loop_slider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    loop_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    loop_slider.setRange(0.0, 1.0, 0.01);
    loop_slider.setMinAndMaxValues(0.1, 0.6);
    loop_slider.addListener(this);
    addAndMakeVisible(loop_slider);
    loop_slider.setVisible(false);

    // إعداد عرض البيانات الوصفية
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(metadataLabel);

    metadataDisplay.setMultiLine(true);
    metadataDisplay.setReadOnly(true);
    metadataDisplay.setCaretVisible(false);
    metadataDisplay.setScrollbarsShown(true);
    metadataDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey);
    metadataDisplay.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(metadataDisplay);

    // إعداد قائمة التشغيل
    addToPlaylistButton.addListener(this);
    addAndMakeVisible(addToPlaylistButton);

    clearPlaylistButton.addListener(this);
    addAndMakeVisible(clearPlaylistButton);

    playlistBox.setMultipleSelectionEnabled(false);
    playlistBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::lightgrey);
    addAndMakeVisible(playlistBox);

    // بدء المؤقت
    startTimerHz(30);

    // تحديث الحالات الأولية
    updatePlayButton();
    updateMuteButton();
    updateMetadataDisplay();
    updateSpeedButtons();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // رسم الرسم البياني الحقيقي إذا كان هناك ملف محمل
    if (hasRealWaveform && audioPlayer.isFileLoaded())
    {
        paintRealWaveform(g, waveformArea);
    }

    // رسم شريط التقدم البسيط في الأعلى
    if (audioPlayer.isFileLoaded())
    {
        auto area = getLocalBounds();
        auto progressArea = area.removeFromTop(3);

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

// دالة الرسم البياني الحقيقي تعتمد على بيانات الصوت الفعلية
void PlayerGUI::paintRealWaveform(juce::Graphics& g, juce::Rectangle<int> area)
{
    // رسم خلفية منطقة الموجة
    g.setColour(juce::Colours::black);
    g.fillRect(area);

    if (audioPlayer.isFileLoaded() && audioPlayer.getWaveformSize() > 0)
    {
        const auto& waveformData = audioPlayer.getWaveformData();
        int numSamples = audioPlayer.getWaveformSize();

        if (numSamples > 0)
        {
            // رسم المحور الأفقي (خط الصفر) في المنتصف
            int centerY = area.getCentreY();
            g.setColour(juce::Colours::grey.withAlpha(0.3f));
            g.drawLine(static_cast<float>(area.getX()), static_cast<float>(centerY),
                static_cast<float>(area.getRight()), static_cast<float>(centerY), 1.0f);

            // رسم الموجة الحقيقية - تستخدم المساحة الكاملة
            juce::Path waveformPath;
            bool pathStarted = false;

            // استخدام المساحة الكاملة للارتفاع
            int amplitude = area.getHeight() / 2;

            for (int i = 0; i < numSamples; ++i)
            {
                float x = static_cast<float>(area.getX()) +
                    (static_cast<float>(i) / static_cast<float>(numSamples - 1)) * static_cast<float>(area.getWidth());

                // استخدام البيانات الحقيقية من الصوت
                float sampleValue = waveformData[i];
                // استخدام المساحة الكاملة للارتفاع
                float y = static_cast<float>(centerY) - sampleValue * static_cast<float>(amplitude);

                if (!pathStarted)
                {
                    waveformPath.startNewSubPath(x, y);
                    pathStarted = true;
                }
                else
                {
                    waveformPath.lineTo(x, y);
                }
            }

            // تلوين الموجة بناءً على شدة الصوت
            g.setColour(juce::Colours::cyan);
            g.strokePath(waveformPath, juce::PathStrokeType(2.0f));

            // رسم منطقة مملوءة تحت الموجة للتأثير البصري - تستخدم المساحة الكاملة
            juce::Path filledPath = waveformPath;
            filledPath.lineTo(static_cast<float>(area.getRight()), static_cast<float>(area.getBottom()));
            filledPath.lineTo(static_cast<float>(area.getX()), static_cast<float>(area.getBottom()));
            filledPath.closeSubPath();

            juce::ColourGradient gradient(
                juce::Colours::cyan.withAlpha(0.4f),
                0, static_cast<float>(area.getY()),
                juce::Colours::darkblue.withAlpha(0.2f),
                0, static_cast<float>(area.getBottom()),
                false
            );

            g.setGradientFill(gradient);
            g.fillPath(filledPath);

            // رسم منطقة مملوءة فوق الموجة أيضاً
            juce::Path filledPathTop = waveformPath;
            filledPathTop.lineTo(static_cast<float>(area.getRight()), static_cast<float>(area.getY()));
            filledPathTop.lineTo(static_cast<float>(area.getX()), static_cast<float>(area.getY()));
            filledPathTop.closeSubPath();

            juce::ColourGradient gradientTop(
                juce::Colours::cyan.withAlpha(0.3f),
                0, static_cast<float>(area.getY()),
                juce::Colours::darkblue.withAlpha(0.1f),
                0, static_cast<float>(area.getCentreY()),
                false
            );

            g.setGradientFill(gradientTop);
            g.fillPath(filledPathTop);

            // رسم مؤشر الموضع الحالي
            if (audioPlayer.getTotalLength() > 0.0)
            {
                double progress = audioPlayer.getCurrentPosition() / audioPlayer.getTotalLength();
                int playheadX = area.getX() + static_cast<int>(progress * area.getWidth());

                // رسم خط المؤشر من الأعلى للأسفل
                g.setColour(juce::Colours::red);
                g.drawLine(static_cast<float>(playheadX), static_cast<float>(area.getY()),
                    static_cast<float>(playheadX), static_cast<float>(area.getBottom()), 2.0f);

                // رسم دائرة متحركة على المؤشر
                g.setColour(juce::Colours::yellow);
                float circleY = static_cast<float>(centerY);

                // حساب قيمة الصوت عند الموضع الحالي للمؤشر
                int sampleIndex = static_cast<int>(progress * (numSamples - 1));
                if (sampleIndex >= 0 && sampleIndex < numSamples)
                {
                    float currentSample = waveformData[sampleIndex];
                    circleY = static_cast<float>(centerY) - currentSample * static_cast<float>(amplitude);

                    // التأكد أن الدائرة تبقى داخل المنطقة
                    circleY = juce::jlimit(static_cast<float>(area.getY() + 10),
                        static_cast<float>(area.getBottom() - 10),
                        circleY);
                }

                g.fillEllipse(static_cast<float>(playheadX - 4), circleY - 4, 8.0f, 8.0f);

                // رسم ظل للمؤشر
                g.setColour(juce::Colours::yellow.withAlpha(0.5f));
                g.drawEllipse(static_cast<float>(playheadX - 6), circleY - 6, 12.0f, 12.0f, 1.0f);
            }
        }
    }
    else
    {
        // رسالة عندما لا توجد بيانات - في وسط المنطقة
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawText("Load an audio file to see the waveform", area, juce::Justification::centred);

        // رسم مثال توضيحي
        g.setColour(juce::Colours::grey.withAlpha(0.5f));
        juce::Path demoPath;
        int centerY = area.getCentreY();
        int amplitude = area.getHeight() / 2 - 20;

        demoPath.startNewSubPath(static_cast<float>(area.getX()), static_cast<float>(centerY));

        for (int x = area.getX(); x < area.getRight(); x += 3)
        {
            double phase = (static_cast<double>(x - area.getX()) / area.getWidth()) * 15.0;
            double y = centerY + amplitude * std::sin(phase);
            demoPath.lineTo(static_cast<float>(x), static_cast<float>(y));
        }

        g.strokePath(demoPath, juce::PathStrokeType(1.5f));
    }

    // رسم إطار حول منطقة الموجة
    g.setColour(juce::Colours::white);
    g.drawRect(area, 2);

    // رسم عنوان في الأعلى
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    juce::String title;
    if (audioPlayer.isFileLoaded())
    {
        title = "Waveform - " + audioPlayer.getCurrentFileName();
    }
    else
    {
        title = "Audio Waveform Display";
    }

    // خلفية شبه شفافة للنص
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.fillRect(area.getX(), area.getY(), area.getWidth(), 25);

    g.setColour(juce::Colours::white);
    g.drawText(title, area.getX(), area.getY(), area.getWidth(), 25, juce::Justification::centred);
}



void PlayerGUI::resized()
{
    auto area = getLocalBounds();

    // الصف الأول للأزرار الأساسية
    auto buttonRow1 = area.removeFromTop(50);
    loadButton.setBounds(buttonRow1.removeFromLeft(100).reduced(2));
    playPauseButton.setBounds(buttonRow1.removeFromLeft(80).reduced(2));
    stopButton.setBounds(buttonRow1.removeFromLeft(80).reduced(2));
    restartButton.setBounds(buttonRow1.removeFromLeft(80).reduced(2));
    loop_button.setBounds(buttonRow1.removeFromLeft(80).reduced(2));
    range_loop_button.setBounds(buttonRow1.removeFromLeft(100).reduced(2));
    muteButton.setBounds(buttonRow1.removeFromLeft(80).reduced(2));

    // الصف الثاني لأزرار السرعة الجديدة
    auto buttonRow2 = area.removeFromTop(40);
    normalSpeedButton.setBounds(buttonRow2.removeFromLeft(120).reduced(2));
    increaseSpeedButton.setBounds(buttonRow2.removeFromLeft(120).reduced(2));
    decreaseSpeedButton.setBounds(buttonRow2.removeFromLeft(120).reduced(2));

    // منطقة الرسم البياني للموجة (180 بكسل ارتفاع - أكبر قليلاً)
    waveformArea = area.removeFromTop(180).reduced(10, 5);

    // منطقة أزرار قائمة التشغيل
    auto playlistButtonRow = area.removeFromTop(30);
    addToPlaylistButton.setBounds(playlistButtonRow.removeFromLeft(120).reduced(2));
    clearPlaylistButton.setBounds(playlistButtonRow.removeFromLeft(120).reduced(2));

    // منطقة السرعة
    auto speedArea = area.removeFromTop(30).reduced(20, 5);
    speedLabel.setBounds(speedArea.removeFromLeft(60));
    speedSlider.setBounds(speedArea);

    // منطقة المنزلقات
    auto slider_area = area.removeFromTop(30).reduced(20, 5);
    loop_slider.setBounds(slider_area);
    position_slider.setBounds(slider_area);
    label_time.setBounds(area.removeFromTop(25).reduced(20, 0));

    // منطقة الصوت
    volumeSlider.setBounds(area.removeFromTop(30).reduced(20, 5));

    // منطقة البيانات الوصفية
    auto metadataArea = area.removeFromBottom(100);
    metadataLabel.setBounds(metadataArea.removeFromTop(20).reduced(5, 0));
    metadataDisplay.setBounds(metadataArea.reduced(5));

    // منطقة قائمة التشغيل
    playlistBox.setBounds(area.reduced(5));
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
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
                    updateSpeedButtons();
                    updateWaveform();
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
        }
        else {
            loop_button.setButtonText("Loop:off");
        }
    }
    else if (button == &range_loop_button)
    {
        if (range_loop_button.getToggleState()) {
            range_loop_button.setButtonText("Range Loop:on");
            loop_slider.setVisible(true);
            audioPlayer.set_loop_by_buttons(loop_start_point, loop_end_point);
        }
        else {
            range_loop_button.setButtonText("Range Loop:off");
            loop_slider.setVisible(false);
        }
    }
    else if (button == &muteButton)
    {
        audioPlayer.toggleMute();
        updateMuteButton();
    }
    // معالجة أزرار السرعة الجديدة
    else if (button == &normalSpeedButton)
    {
        audioPlayer.setPlaybackSpeed(1.0f);
        speedSlider.setValue(1.0f);
        updateSpeedButtons();
    }
    else if (button == &increaseSpeedButton)
    {
        float currentSpeed = audioPlayer.getPlaybackSpeed();
        float newSpeed = juce::jmin(4.0f, currentSpeed + 0.25f);
        audioPlayer.setPlaybackSpeed(newSpeed);
        speedSlider.setValue(newSpeed);
        updateSpeedButtons();
    }
    else if (button == &decreaseSpeedButton)
    {
        float currentSpeed = audioPlayer.getPlaybackSpeed();
        float newSpeed = juce::jmax(0.25f, currentSpeed - 0.25f);
        audioPlayer.setPlaybackSpeed(newSpeed);
        speedSlider.setValue(newSpeed);
        updateSpeedButtons();
    }
    else if (button == &addToPlaylistButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select audio files to add to playlist...",
            juce::File::getSpecialLocation(juce::File::userMusicDirectory),
            "*.wav;*.mp3;*.aiff;*.flac");

        auto fileBrowserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems;

        fileChooser->launchAsync(
            fileBrowserFlags,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                for (int i = 0; i < results.size(); ++i)
                {
                    auto& file = results.getReference(i);
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

void PlayerGUI::timerCallback()
{
    if (audioPlayer.isFileLoaded())
    {
        current_time = audioPlayer.get_current_time();
        total_time = audioPlayer.get_total_time();

        int current_minutes = static_cast<int>(current_time) / 60;
        int current_seconds = static_cast<int>(current_time) % 60;
        int total_minutes = static_cast<int>(total_time) / 60;
        int total_seconds = static_cast<int>(total_time) % 60;

        time_text = juce::String(current_minutes) + ":" +
            (current_seconds < 10 ? "0" : "") + juce::String(current_seconds) +
            " / " +
            juce::String(total_minutes) + ":" +
            (total_seconds < 10 ? "0" : "") + juce::String(total_seconds);

        label_time.setText(time_text, juce::dontSendNotification);

        if (!isPositionSliderDragging)
        {
            position_slider.setValue(current_time / total_time, juce::dontSendNotification);
        }

        if (range_loop_button.getToggleState() && audioPlayer.loop_position_state()) {
            audioPlayer.set_slider_looping();
        }

        if (will_looping && audioPlayer.is_transportSource_playing()) {
            audioPlayer.loop_on();
        }
    }

    // إعادة الرسم لتحديث المؤشر على الموجة
    repaint();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audioPlayer.setGain(static_cast<float>(slider->getValue()));
    }
    else if (slider == &speedSlider)
    {
        float newSpeed = static_cast<float>(slider->getValue());
        audioPlayer.setPlaybackSpeed(newSpeed);
        updateSpeedButtons();
    }
    else if (slider == &position_slider)
    {
        if (audioPlayer.isFileLoaded()) {
            isPositionSliderDragging = true;
            audioPlayer.position_slider_value(position_slider.getValue());
            isPositionSliderDragging = false;
        }
    }
    else if (slider == &loop_slider)
    {
        loop_start_point = slider->getMinValue();
        loop_end_point = slider->getMaxValue();
        audioPlayer.set_loop_by_buttons(loop_start_point, loop_end_point);
    }
}

// دالة جديدة لتحديث الرسم البياني للموجة
void PlayerGUI::updateWaveform()
{
    if (audioPlayer.isFileLoaded() && audioPlayer.getWaveformSize() > 0)
    {
        hasRealWaveform = true;
        repaint();
    }
    else
    {
        hasRealWaveform = false;
    }
}

void PlayerGUI::updateSpeedButtons()
{
    float currentSpeed = audioPlayer.getPlaybackSpeed();

    // تحديث ألوان الأزرار بناءً على السرعة الحالية
    if (currentSpeed == 1.0f) {
        normalSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        increaseSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        decreaseSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (currentSpeed > 1.0f) {
        normalSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        increaseSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
        decreaseSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else {
        normalSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        increaseSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        decreaseSpeedButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightblue);
    }

    // تحديث النص لعرض السرعة الحالية
    normalSpeedButton.setButtonText("Normal (" + juce::String(currentSpeed, 2) + "x)");
    increaseSpeedButton.setButtonText("Increase Speed");
    decreaseSpeedButton.setButtonText("Decrease Speed");
}

juce::String PlayerGUI::time_in_minutes(double time)
{
    int minutes = static_cast<int>(time) / 60;
    return juce::String(minutes);
}

juce::String PlayerGUI::time_in_seconds(double time)
{
    int seconds = static_cast<int>(time) % 60;
    return (seconds < 10 ? "0" + juce::String(seconds) : juce::String(seconds));
}

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

void PlayerGUI::updateMetadataDisplay()
{
    if (audioPlayer.isFileLoaded()) {
        auto metadata = audioPlayer.getMetadata();
        juce::String displayText;

        for (int i = 0; i < metadata.size(); ++i) {
            displayText += metadata[i] + "\n";
        }

        metadataDisplay.setText(displayText);
    }
    else {
        metadataDisplay.setText("No file loaded");
    }
}

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
    if (lastRowSelected >= 0 && lastRowSelected < (int)playlistFileObjects.size()) {
        audioPlayer.loadFile(playlistFileObjects[lastRowSelected]);
        updatePlayButton();
        updateMetadataDisplay();
        updateSpeedButtons();
        updateWaveform();
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
        if (lastRowSelected < (int)playlistFileObjects.size()) {
            playlistFileObjects.erase(playlistFileObjects.begin() + lastRowSelected);
        }
        playlistBox.updateContent();
    }
}

void PlayerGUI::returnKeyPressed(int lastRowSelected)
{
    selectedRowsChanged(lastRowSelected);
}