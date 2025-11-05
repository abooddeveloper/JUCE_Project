#include "MainComponent.h"

// ==============================================================================
// البناء - تهيئة المكون الرئيسي
// ==============================================================================
MainComponent::MainComponent()
    : audioPlayer(),
    guiComponent(audioPlayer)
{
    addAndMakeVisible(guiComponent); // إضافة واجهة المستخدم وجعلها مرئية
    setSize(800, 600); // تحديد حجم المكون

    // بدء المؤقت للتحديثات الدورية (حوالي 30 إطار في الثانية)
    startTimer(33);
}

// ==============================================================================
// التدمير - تنظيف الموارد
// ==============================================================================
MainComponent::~MainComponent()
{
    stopTimer(); // إيقاف المؤقت
    shutdownAudio(); // إيقاف نظام الصوت
}

// ==============================================================================
// تحضير الصوت للتشغيل
// ==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // تحضير معالج الصوت للتشغيل
    audioPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// ==============================================================================
// الحصول على كتلة الصوت التالية
// ==============================================================================
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // تحقق من أن bufferToFill صالح
    if (bufferToFill.buffer != nullptr && bufferToFill.numSamples > 0)
    {
        audioPlayer.getNextAudioBlock(bufferToFill);
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}
// ==============================================================================
// تحرير موارد الصوت
// ==============================================================================
void MainComponent::releaseResources()
{
    // تحرير موارد الصوت
    audioPlayer.releaseResources();
}

// ==============================================================================
// رد نداء المؤقت - للتحديثات الدورية
// ==============================================================================
void MainComponent::timerCallback()
{
    static int counter = 0;

    // تحديث معلومات التصحيح كل 3 ثوان تقريباً (100 * 33ms ≈ 3.3s)
    if (counter++ % 100 == 0)
    {
        // معلومات حالة التشغيل الأساسية
        if (audioPlayer.isFileLoaded())
        {
            if (audioPlayer.isPlaying())
            {
                DBG("=== AUDIO PLAYER STATUS ===");
                DBG("State: PLAYING");
                DBG("Position: " + juce::String(audioPlayer.getCurrentPosition(), 2) + "s");
                DBG("Total Length: " + juce::String(audioPlayer.getTotalLength(), 2) + "s");
                DBG("Looping: " + juce::String(audioPlayer.isLooping() ? "ON" : "OFF"));
                DBG("Speed: " + juce::String(audioPlayer.getPlaybackSpeed()) + "x");
                DBG("Muted: " + juce::String(audioPlayer.isMuted() ? "YES" : "NO"));
                DBG("Volume: " + juce::String(audioPlayer.getGain(), 2));

                // معلومات التقدم
                double progress = (audioPlayer.getTotalLength() > 0.0) ?
                    (audioPlayer.getCurrentPosition() / audioPlayer.getTotalLength()) * 100.0 : 0.0;
                DBG("Progress: " + juce::String(progress, 1) + "%");
                DBG("=============================");
            }
            else if (audioPlayer.isPaused())
            {
                DBG("=== AUDIO PLAYER STATUS ===");
                DBG("State: PAUSED");
                DBG("Position: " + juce::String(audioPlayer.getCurrentPosition(), 2) + "s");
                DBG("File: " + audioPlayer.getCurrentFileName());
                DBG("=============================");
            }
        }
        else
        {
            DBG("=== AUDIO PLAYER STATUS ===");
            DBG("State: NO FILE LOADED");
            DBG("Ready for file loading...");
            DBG("=============================");
        }

        // تحقق من سلامة الذاكرة (للاكتشاف المبكر للمشاكل)
        if (counter % 300 == 0) // كل 10 ثوان تقريباً
        {
            DBG("*** MEMORY CHECK ***");
            DBG("Reader Source: " + juce::String(audioPlayer.isFileLoaded() ? "VALID" : "NULL"));
            DBG("Transport Source Active: " + juce::String(audioPlayer.isTransportSourceActive() ? "YES" : "NO"));
            DBG("********************");
        }
    }

   
   
}

//

// ==============================================================================
// الرسم - رسم خلفية المكون
// ==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // رسم خلفية المكون
    g.fillAll(juce::Colours::darkgrey);
}

// ==============================================================================
// إعادة التحجيم - ضبط حدود المكونات الداخلية
// ==============================================================================
void MainComponent::resized()
{
    // ضبط حدود واجهة المستخدم لتملأ المكون بالكامل
    guiComponent.setBounds(getLocalBounds());
}