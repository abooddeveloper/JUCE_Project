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


void PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        // إيقاف الصوت أولاً
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        // محاولة فتح الملف
        reader = formatManager.createReaderFor(file);
        total_time = (reader->lengthInSamples / reader->sampleRate);
        if (reader != nullptr)
        {
            // إنشاء مصدر الصوت من القارئ
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

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
    return info;
}