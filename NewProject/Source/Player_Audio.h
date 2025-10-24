#pragma once
#include <JuceHeader.h>

// ==============================================================================
// معالج الصوت المسؤول عن تحميل وتشغيل الملفات الصوتية
// ==============================================================================
class PlayerAudio : public juce::AudioAppComponent
{
public:
    PlayerAudio();
    ~PlayerAudio() override;

    // ==========================================================================
    // دوال AudioAppComponent المطلوبة
    // ==========================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // ==========================================================================
    // دوال التحكم في التشغيل
    // ==========================================================================
    void loadFile(const juce::File& file); // تحميل ملف صوتي
    void play();     // تشغيل
    void pause();    // إيقاف مؤقت
    void stop();     // إيقاف
    void restart();  // إعادة التشغيل
    void togglePlayPause(); // تبديل بين التشغيل والإيقاف المؤقت
    void setGain(float gain); // ضبط مستوى الصوت
    void setPosition(double position); // ضبط موضع التشغيل
    void toggleMute(); // كتم/إلغاء كتم الصوت
    void setLooping(bool shouldLoop); // ضبط وضع التكرار
    void toggleLoop(); // تبديل وضع التكرار

    // ==========================================================================
    // دوال الاستعلام عن الحالة
    // ==========================================================================
    bool isPlaying() const;     // هل يتم التشغيل حالياً؟
    bool isPaused() const;      // هل متوقف مؤقتاً؟
    bool isFileLoaded() const;  // هل هناك ملف محمل؟
    bool isLooping() const;     // هل التكرار مفعل؟
    bool isMuted() const;       // هل الصوت مكتوم؟
    double getCurrentPosition() const; // الموضع الحالي
    double getTotalLength() const;// الطول الكلي للملف

    juce::String getDebugInfo() const; // معلومات التصحيح

private:
    // ==========================================================================
    // المتغيرات الخاصة
    // ==========================================================================
    juce::AudioFormatManager formatManager;        // مدير تنسيقات الصوت
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource; // مصدر القارئ
    juce::AudioTransportSource transportSource;    // مصدر النقل للتحكم في التشغيل
    bool playing = false;          // حالة التشغيل
    float currentVolume = 0.5f;    // مستوى الصوت الحالي
    juce::String currentFileName;  // اسم الملف الحالي
    bool looping = false;          // حالة التكرار
    bool isMutedFlag = false;      // حالة الكتم
    float volumeBeforeMute = 0.5f; // مستوى الصوت قبل الكتم
};