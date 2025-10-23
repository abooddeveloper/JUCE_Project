#include "MainComponent.h"

MainComponent::MainComponent()
    : audioPlayer(),
    guiComponent(audioPlayer)
{
    addAndMakeVisible(guiComponent); // إضافة واجهة المستخدم وجعلها مرئية
    setSize(600, 200); // تحديد حجم المكون
    
    // بدء المؤقت للتحديثات الدورية (حوالي 30 إطار في الثانية)
    startTimer(33);
}

MainComponent::~MainComponent()
{
    stopTimer(); // إيقاف المؤقت
    shutdownAudio(); // إيقاف نظام الصوت
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // تحضير معالج الصوت للتشغيل
    audioPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // الحصول على كتلة الصوت التالية من المعالج
    audioPlayer.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // تحرير موارد الصوت
    audioPlayer.releaseResources();
}

void MainComponent::timerCallback()
{
    // تحديث دوري للتحقق من حالة التشغيل
    static int counter = 0;
    if (counter++ % 100 == 0) // كل 3 ثوان تقريباً (100 * 33ms ≈ 3.3s)
    {
        if (audioPlayer.isPlaying())
        {
            // طباعة معلومات التصحيح أثناء التشغيل
            DBG("🎵 Audio is PLAYING - Position: " +
                juce::String(audioPlayer.getCurrentPosition()));
        }
    }
}

void MainComponent::paint(juce::Graphics& g)
{
    // رسم خلفية المكون
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    // ضبط حدود واجهة المستخدم لتملأ المكون بالكامل
    guiComponent.setBounds(getLocalBounds());
}