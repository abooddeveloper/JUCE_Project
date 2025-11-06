#include "Player_Audio.h"

PlayerAudio::PlayerAudio()
    : resampleSource(&transportSource, false)
{
    formatManager.registerBasicFormats();
    setAudioChannels(0, 2);
}

PlayerAudio::~PlayerAudio()
{
    shutdownAudio();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource != nullptr)
    {
        resampleSource.getNextAudioBlock(bufferToFill);
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void PlayerAudio::toggleMute()
{
    if (isMutedFlag)
    {
        setGain(volumeBeforeMute);
        isMutedFlag = false;
    }
    else
    {
        volumeBeforeMute = transportSource.getGain();
        setGain(0.0f);
        isMutedFlag = true;
    }
}

bool PlayerAudio::isMuted() const
{
    return isMutedFlag;
}

void PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();
        metadataArray.clear();
        waveformData.clear();

        auto* reader = formatManager.createReaderFor(file);

        if (reader != nullptr)
        {
            total_time = (reader->lengthInSamples / reader->sampleRate);

            metadataArray = reader->metadataValues;

            if (metadataArray.getValue("Title", "").isEmpty())
            {
                metadataArray.set("Title", file.getFileNameWithoutExtension());
            }

            if (metadataArray.getValue("Artist", "").isEmpty())
            {
                metadataArray.set("Artist", "Unknown Artist");
            }

            if (metadataArray.getValue("Album", "").isEmpty())
            {
                metadataArray.set("Album", "Unknown Album");
            }

            metadataArray.set("File Name", file.getFileName());
            metadataArray.set("File Size", juce::String(file.getSize() / 1024) + " KB");
            metadataArray.set("File Path", file.getFullPathName());
            metadataArray.set("Sample Rate", juce::String(reader->sampleRate) + " Hz");
            metadataArray.set("Bit Depth", juce::String(reader->bitsPerSample) + " bit");
            metadataArray.set("Channels", juce::String(reader->numChannels));
            metadataArray.set("Length (seconds)", juce::String(reader->lengthInSamples / reader->sampleRate, 2));

            // توليد بيانات الرسم البياني
            generateWaveformData(file);

            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);

            transportSource.setPosition(0.0);
            currentFileName = file.getFileName();
            playing = false;

            play();
        }
        else
        {
            currentFileName = "";
        }
    }
}

void PlayerAudio::generateWaveformData(const juce::File& file)
{
    waveformData.clear();

    if (!file.existsAsFile())
        return;

    auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file));
    if (reader == nullptr)
        return;

    const int numSamples = static_cast<int>(reader->lengthInSamples);
    const int numChannels = reader->numChannels;
    waveformSamples = 1000;

    if (numSamples == 0)
        return;

    waveformData.resize(waveformSamples, 0.0f);

    // قراءة البيانات الصوتية
    juce::AudioSampleBuffer buffer(numChannels, numSamples);
    reader->read(&buffer, 0, numSamples, 0, true, true);

    // حساب متوسط القيم للرسم البياني
    const int step = numSamples / waveformSamples;

    for (int i = 0; i < waveformSamples; ++i)
    {
        float maxValue = 0.0f;
        int startSample = i * step;
        int endSample = juce::jmin(startSample + step, numSamples);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float* channelData = buffer.getReadPointer(channel);

            for (int j = startSample; j < endSample; ++j)
            {
                float absValue = std::abs(channelData[j]);
                if (absValue > maxValue)
                    maxValue = absValue;
            }
        }

        waveformData[i] = maxValue;
    }
}

void PlayerAudio::setPlaybackSpeed(float speed)
{
    playbackSpeed = juce::jlimit(0.25f, 4.0f, speed);
    resampleSource.setResamplingRatio(playbackSpeed);
}

float PlayerAudio::getPlaybackSpeed() const
{
    return playbackSpeed;
}

void PlayerAudio::play()
{
    if (readerSource != nullptr)
    {
        transportSource.start();
        playing = true;
    }
}

void PlayerAudio::pause()
{
    transportSource.stop();
    playing = false;
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
    playing = false;
}

void PlayerAudio::restart()
{
    if (readerSource != nullptr)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
        playing = true;
    }
}

void PlayerAudio::togglePlayPause()
{
    if (playing) {
        pause();
    }
    else {
        play();
    }
}

void PlayerAudio::setGain(float gain)
{
    currentVolume = gain;
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double position)
{
    transportSource.setPosition(position);
}

void PlayerAudio::loop_on()
{
    if (!transportSource.isPlaying()) {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void PlayerAudio::position_slider_value(double slider_value)
{
    double new_position = slider_value * total_time;
    transportSource.setPosition(new_position);
}

void PlayerAudio::set_loop_by_buttons(double start_point, double end_point)
{
    start_position_time = start_point * total_time;
    end_position_time = end_point * total_time;
}

bool PlayerAudio::loop_position_state()
{
    if (transportSource.getCurrentPosition() >= end_position_time) {
        return true;
    }
    else {
        return false;
    }
}

void PlayerAudio::set_slider_looping()
{
    transportSource.setPosition(start_position_time);
}

bool PlayerAudio::is_transportSource_playing()
{
    return (!transportSource.isPlaying());
}

bool PlayerAudio::isPlaying() const
{
    return playing;
}

bool PlayerAudio::isPaused() const
{
    return !playing && transportSource.getCurrentPosition() > 0.0;
}

bool PlayerAudio::isFileLoaded() const
{
    return readerSource != nullptr;
}

double PlayerAudio::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getTotalLength() const
{
    if (readerSource != nullptr && readerSource->getAudioFormatReader() != nullptr)
        return readerSource->getTotalLength() / readerSource->getAudioFormatReader()->sampleRate;
    return 0.0;
}

bool PlayerAudio::label_time_visibility()
{
    return isFileLoaded();
}

double PlayerAudio::get_total_time()
{
    return total_time;
}

double PlayerAudio::get_current_time()
{
    current_time = transportSource.getCurrentPosition();
    return current_time;
}

juce::String PlayerAudio::getCurrentFileName() const
{
    return currentFileName;
}

juce::StringArray PlayerAudio::getMetadata() const
{
    juce::StringArray metadataList;

    metadataList.add("File: " + currentFileName);

    if (isFileLoaded()) {
        metadataList.add("Duration: " + getFormattedDuration());
    }

    auto allKeys = metadataArray.getAllKeys();
    for (int i = 0; i < allKeys.size(); ++i) {
        auto& key = allKeys[i];
        juce::String value = metadataArray.getValue(key, "");
        if (value.isNotEmpty()) {
            metadataList.add(key + ": " + value);
        }
    }

    return metadataList;
}

juce::String PlayerAudio::getFormattedDuration() const
{
    if (!isFileLoaded())
        return "00:00";

    double totalSeconds = getTotalLength();
    int minutes = static_cast<int>(totalSeconds) / 60;
    int seconds = static_cast<int>(totalSeconds) % 60;

    return juce::String::formatted("%02d:%02d", minutes, seconds);
}

juce::String PlayerAudio::getDebugInfo() const
{
    juce::String info;
    info += "File: " + currentFileName + "\n";
    info += "Loaded: " + juce::String(isFileLoaded() ? "Yes" : "No") + "\n";
    info += "Playing: " + juce::String(isPlaying() ? "Yes" : "No") + "\n";
    info += "Position: " + juce::String(getCurrentPosition()) + "\n";
    info += "Muted: " + juce::String(isMuted() ? "Yes" : "No") + "\n";
    info += "Speed: " + juce::String(getPlaybackSpeed()) + "x\n";

    auto metadata = getMetadata();
    for (int i = 0; i < metadata.size(); ++i)
    {
        info += metadata[i] + "\n";
    }

    return info;
}