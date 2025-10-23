#include "Player_Audio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats(); // تسجيل التنسيقات الأساسية (WAV, AIFF, etc.)
    setAudioChannels(0, 2); // إعداد قنوات الصوت (0 مدخلات، 2 مخرجات)
}

PlayerAudio::~PlayerAudio()
{
    shutdownAudio(); // إيقاف نظام الصوت
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // تحضير مصدر النقل للتشغيل
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource != nullptr && transportSource.isPlaying())
    {
        // الحصول على البيانات الصوتية من المصدر
        transportSource.getNextAudioBlock(bufferToFill);
        if (looping && !transportSource.isPlaying())
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
    else
    {
        // ملء المخزن بالصمت إذا لم يكن هناك تشغيل
        bufferToFill.clearActiveBufferRegion();
    }
}

void PlayerAudio::releaseResources()
{
    // تحرير موارد مصدر النقل
    transportSource.releaseResources();
}
void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
    
    if (readerSource != nullptr)
    {
        readerSource->setLooping(shouldLoop);
    }
}

bool PlayerAudio::isLooping() const
{
    return looping;
}

void PlayerAudio::toggleLoop()
{
    setLooping(!looping);
}


void PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        // إيقاف الصوت أولاً
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        // محاولة فتح الملف
        auto* reader = formatManager.createReaderFor(file);

        if (reader != nullptr)
        {
            // إنشاء مصدر الصوت من القارئ
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
             readerSource->setLooping(looping);
            // توصيل المصدر بمصدر النقل
            transportSource.setSource(readerSource.get(),
                0, // timeout
                nullptr, // thread
                reader->sampleRate);

            // إعادة التعيين وبدء التشغيل
            transportSource.setPosition(0.0);
            currentFileName = file.getFileName();
            playing = false;

            // تشغيل تلقائي بعد التحميل
            play();
        }
        else
        {
            currentFileName = "";
        }
    }
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
void PlayerAudio::togglemute() const
{
    if (ismuted)
    {
        setGain(currentVolume);
        ismuted = false;
    }
    else
    {
        currentVolume = transportSource.getGain();
        setGain(0.0f);
        ismuted = true;
    }
}

void PlayerAudio::setGain(float gain)
{
    currentVolume = gain;
    transportSource.setGain(gain); // ضبط مستوى الصوت
}

void PlayerAudio::setPosition(double position)
{
    transportSource.setPosition(position); // ضبط موضع التشغيل
}

bool PlayerAudio::isPlaying() const
{
    return playing;
}

bool PlayerAudio::isPaused() const
{
    return !playing && transportSource.getCurrentPosition() > 0.0;
}
bool PlayerAudio::togglemuted() const
{
    return ismuted;
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


juce::String PlayerAudio::getDebugInfo() const
{
    juce::String info;
    info += "File: " + currentFileName + "\n";
    info += "Loaded: " + juce::String(isFileLoaded() ? "Yes" : "No") + "\n";
    info += "Playing: " + juce::String(isPlaying() ? "Yes" : "No") + "\n";
    info += "Position: " + juce::String(getCurrentPosition()) + "\n";
	info += "Muted: " + juce::String(togglemute() ? "Yes" : "No") + "\n";

    return info;
}