#include "Player_Audio.h"

// ==============================================================================
// البناء - تهيئة معالج الصوت
// ==============================================================================
PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats(); // تسجيل التنسيقات الأساسية (WAV, AIFF, etc.)
    setAudioChannels(0, 2); // إعداد قنوات الصوت (0 مدخلات، 2 مخرجات)
}

// ==============================================================================
// التدمير - تنظيف الموارد
// ==============================================================================
PlayerAudio::~PlayerAudio()
{
    shutdownAudio(); // إيقاف نظام الصوت
}

// ==============================================================================
// تحضير الصوت للتشغيل
// ==============================================================================
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// ==============================================================================
// الحصول على كتلة الصوت التالية
// ==============================================================================
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource != nullptr && transportSource.isPlaying())
    {
        transportSource.getNextAudioBlock(bufferToFill);
        // معالجة التكرار التلقائي
        if (looping && !transportSource.isPlaying())
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
    else
    {
        bufferToFill.clearActiveBufferRegion(); // ملء المخزن بالصمت
    }
}

// ==============================================================================
// تحرير موارد الصوت
// ==============================================================================
void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

// ==============================================================================
// ضبط وضع التكرار
// ==============================================================================
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

// ==============================================================================
// كتم/إلغاء كتم الصوت
// ==============================================================================
void PlayerAudio::toggleMute()
{
    if (isMutedFlag)
    {
        // إلغاء الكتم - استعادة مستوى الصوت السابق
        setGain(volumeBeforeMute);
        isMutedFlag = false;
    }
    else
    {
        // كتم الصوت - حفظ المستوى الحالي وتعيينه إلى صفر
        volumeBeforeMute = transportSource.getGain();
        setGain(0.0f);
        isMutedFlag = true;
    }
}

bool PlayerAudio::isMuted() const
{
    return isMutedFlag;
}

// ==============================================================================
// تحميل ملف صوتي
// ==============================================================================
void PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        // إيقاف الصوت أولاً وتنظيف المصدر الحالي
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        // محاولة فتح الملف وإنشاء قارئ
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
            currentFileName = ""; // فشل التحميل
        }
    }
}

// ==============================================================================
// دوال التحكم في التشغيل
// ==============================================================================
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

// ==============================================================================
// ضبط مستوى الصوت
// ==============================================================================
void PlayerAudio::setGain(float gain)
{
    currentVolume = gain;
    transportSource.setGain(gain);
}

// ==============================================================================
// ضبط موضع التشغيل
// ==============================================================================
void PlayerAudio::setPosition(double position)
{
    transportSource.setPosition(position);
}

// ==============================================================================
// دوال الاستعلام عن الحالة
// ==============================================================================
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

// ==============================================================================
// الحصول على معلومات التصحيح
// ==============================================================================
juce::String PlayerAudio::getDebugInfo() const
{
    juce::String info;
    info += "File: " + currentFileName + "\n";
    info += "Loaded: " + juce::String(isFileLoaded() ? "Yes" : "No") + "\n";
    info += "Playing: " + juce::String(isPlaying() ? "Yes" : "No") + "\n";
    info += "Position: " + juce::String(getCurrentPosition()) + "\n";
    info += "Muted: " + juce::String(isMuted() ? "Yes" : "No") + "\n";
    info += "Looping: " + juce::String(isLooping() ? "ON" : "OFF") + "\n";

    return info;
}