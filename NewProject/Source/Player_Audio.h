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

    void position_slider_value(double slider_value);


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
    void setPlaybackSpeed(float speed); // ضبط سرعة التشغيل
    float getPlaybackSpeed() const; // الحصول على سرعة التشغيل الحالية
    void createAudioThumbnail(); // إنشاء مصغرة صوتية للموجة
    juce::AudioThumbnail& getAudioThumbnail(); // الحصول على المصغرة الصوتية
    void loop_on();
   /* void loop_off();*/

    void toggleMute(); // كتم/إلغاء كتم الصوت
    void setLooping(bool shouldLoop); // ضبط وضع التكرار
    

    // ==========================================================================

    // دوال الاستعلام عن الحالة
    // ==========================================================================
    bool isPlaying() const;     // هل يتم التشغيل حالياً؟
    bool isPaused() const;      // هل متوقف مؤقتاً؟
    bool isFileLoaded() const;  // هل هناك ملف محمل؟
    bool isLooping() const;     // هل التكرار مفعل؟
    bool isMuted() const;       // هل الصوت مكتوم؟
    double getCurrentPosition() const; // الموضع الحالي

    double getTotalLength() const;     // الطول الكلي للملف
    

  


    juce::String getDebugInfo() const; // معلومات التصحيح
    bool label_time_visibility();
    bool loop_position_state();
    void set_slider_looping();
    void set_loop_by_buttons(double start_point, double end_point);
    double get_total_time();
    double get_current_time();
    bool is_transportSource_playing();
private:
    // ==========================================================================
    // المتغيرات الخاصة
    // ==========================================================================
    juce::AudioFormatManager formatManager;        // مدير تنسيقات الصوت
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource; // مصدر القارئ
    juce::AudioTransportSource transportSource;    // مصدر النقل للتحكم في التشغيل
    juce::ResamplingAudioSource resampleSource; // مصدر إعادة العينات للتحكم في السرعة
    juce::AudioThumbnailCache thumbnailCache; // مخزن المصغرات
    juce::AudioThumbnail audioThumbnail; // المصغرة الصوتية
    bool playing = false;          // حالة التشغيل
    float currentVolume = 0.5f;    // مستوى الصوت الحالي

    juce::String currentFileName;   // اسم الملف الحالي
    double current_time;
    double total_time;
    juce::String time_text;
    juce::AudioFormatReader* reader;
    double start_position_time;
    double end_position_time;
    bool if_looping;

    
    bool looping = false;          // حالة التكرار
    bool isMutedFlag = false;      // حالة الكتم
    float volumeBeforeMute = 0.5f; // مستوى الصوت قبل الكتم
    double currentSampleRate = 44100.0; // معدل العينات الحالي
    float playbackSpeed = 1.0f; // سرعة التشغيل الحالية 1.0 = سرعة عادي
};