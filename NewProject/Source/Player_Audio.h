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
    

    // دوال التحكم في السرعة
    void setPlaybackSpeed(float speed); // ضبط سرعة التشغيل
    float getPlaybackSpeed() const; // الحصول على سرعة التشغيل الحالية

    // دوال التحكم في التكرار المتقدم
    void loop_on();
    void position_slider_value(double slider_value);
    void set_loop_by_buttons(double start_point, double end_point);
    void set_slider_looping();
    bool is_transportSource_playing();

    // ==========================================================================
    // دوال الاستعلام عن الحالة
    // ==========================================================================
    bool isPlaying() const;     // هل يتم التشغيل حالياً؟
    bool isPaused() const;      // هل متوقف مؤقتاً؟
    bool isFileLoaded() const;  // هل هناك ملف محمل؟
    
    bool isMuted() const;       // هل الصوت مكتوم؟
    double getCurrentPosition() const; // الموضع الحالي
    double getTotalLength() const;// الطول الكلي للملف
    bool label_time_visibility();
    bool loop_position_state();
    double get_total_time();
    double get_current_time();

    juce::String getDebugInfo() const; // معلومات التصحيح

    // في PlayerAudio.h - إضافة هذه الدوال العامة
    float getGain() const { return currentVolume; }
    bool isTransportSourceActive() const { return transportSource.isPlaying() || isPaused(); }


    // ==========================================================================
    // دوال جديدة للميزة 5: البيانات الوصفية
    // ==========================================================================
    juce::String getCurrentFileName() const; // الحصول على اسم الملف الحالي
    juce::StringArray getMetadata() const;   // الحصول على جميع البيانات الوصفية
    juce::String getFormattedDuration() const; // الحصول على المدة بصيغة مقروءة

private:
    // ==========================================================================
    // المتغيرات الخاصة
    // ==========================================================================
    juce::AudioFormatManager formatManager;        // مدير تنسيقات الصوت
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource; // مصدر القارئ
    juce::AudioTransportSource transportSource;    // مصدر النقل للتحكم في التشغيل
    juce::ResamplingAudioSource resampleSource{ &transportSource, false }; // مصدر إعادة العينات للتحكم في السرعة
    juce::AudioFormatReader* reader;
    // متغيرات حالة التشغيل
    double currentSampleRate = 44100.0; // معدل العينات الحالي
    float playbackSpeed = 1.0f; // سرعة التشغيل الحالية (1.0 = سرعة عادية)
    bool playing = false;          // حالة التشغيل
    float currentVolume = 0.5f;    // مستوى الصوت الحالي
    juce::String currentFileName;  // اسم الملف الحالي
    bool looping = false;          // حالة التكرار
    bool isMutedFlag = false;      // حالة الكتم
    float volumeBeforeMute = 0.5f; // مستوى الصوت قبل الكتم

    // متغيرات التكرار المتقدم
   
    double current_time;
    double total_time;
    juce::String time_text;
    double start_position_time;
    double end_position_time;
    bool if_looping;

    // ==========================================================================
    // متغيرات جديدة للميزة 5: البيانات الوصفية
    // ==========================================================================
    
    juce::StringPairArray metadataArray; // مصفوفة أزواج نصية لتخزين البيانات الوصفية
};