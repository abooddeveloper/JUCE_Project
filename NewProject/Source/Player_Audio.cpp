#include "Player_Audio.h"

// ==============================================================================
// البناء - تهيئة معالج الصوت
// ==============================================================================
PlayerAudio::PlayerAudio()
    : thumbnailCache(5), //  مخزن لـ5 مصغرات
      audioThumbnail(512, formatManager, thumbnailCache), // مصغرة صوتية
      resampleSource(&transportSource, false) //  مصدر إعادة العينات
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
    currentSampleRate = sampleRate; // حفظ معدل العينات
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate); // [NEW] تحضير مصدر إعادة العينات
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// ==============================================================================
// الحصول على كتلة الصوت التالية
// ==============================================================================
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource != nullptr && transportSource.isPlaying())
    {
        //  استخدام مصدر إعادة العينات بدلاً من مصدر النقل مباشرة
        resampleSource.getNextAudioBlock(bufferToFill);
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


        // محاولة فتح الملف
        reader = formatManager.createReaderFor(file);
        total_time = (reader->lengthInSamples / reader->sampleRate);

        // محاولة فتح الملف وإنشاء قارئ
        auto* reader = formatManager.createReaderFor(file);


        if (reader != nullptr)
        {
            // إنشاء مصدر الصوت من القارئ
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            readerSource->setLooping(looping);
            //  توصيل مصدر القارئ بمصدر النقل
            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

            //  إنشاء المصغرة الصوتية للموجة
            createAudioThumbnail();

            transportSource.setPosition(0.0);
            currentFileName = file.getFileName();
            playing = false;

            // إعادة تعيين السرعة إلى الوضع الطبيعي عند تحميل ملف جديد
            setPlaybackSpeed(1.0f);

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

bool PlayerAudio::label_time_visibility() {
    if (isFileLoaded()) {
        return true;
    }
    else { return false; }
}
void PlayerAudio::set_loop_by_buttons(double start_point, double end_point) {
    start_position_time = start_point * total_time;
    end_position_time = end_point * total_time;
}
bool PlayerAudio::loop_position_state() {
    if (transportSource.getCurrentPosition() >= end_position_time) {
        return true;
    }
    else { return false; }
}
void  PlayerAudio::set_slider_looping() {
    transportSource.setPosition(start_position_time);
}
void PlayerAudio::setPlaybackSpeed(float speed)
{
    playbackSpeed = juce::jlimit(0.25f, 4.0f, speed); // تحديد السرعة بين 0.25x و4x

    // ضبط نسبة إعادة العينة بناءً على السرعة
    resampleSource.setResamplingRatio(playbackSpeed);
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
bool PlayerAudio::is_transportSource_playing() {
    return(!transportSource.isPlaying());
}
void PlayerAudio::loop_on() {
    /*readerSource->setLooping(true);*/
    if (!transportSource.isPlaying()) {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}
void PlayerAudio::position_slider_value(double slider_value) {
    double new_position = slider_value * total_time;
    transportSource.setPosition(new_position);
}
double PlayerAudio::get_total_time() {
    return total_time;
}
double PlayerAudio::get_current_time() {
    current_time = transportSource.getCurrentPosition();
    return current_time;
}
void PlayerAudio::createAudioThumbnail()
{
    // سيتم استدعاء هذه الدالة عند تحميل ملف جديد
    if (reader != nullptr)
    {
        audioThumbnail.clear();
        //  تحميل المصغرة من القارئ
        audioThumbnail.setSource(new juce::FileInputSource(juce::File(currentFileName)));
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
float PlayerAudio::getPlaybackSpeed() const
{
    return playbackSpeed;
}
juce::AudioThumbnail& PlayerAudio::getAudioThumbnail()
{
    return audioThumbnail;
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