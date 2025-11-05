#include <JuceHeader.h>
#include "MainComponent.h"

// ==============================================================================
// تطبيق JUCE الرئيسي - نقطة الدخول للتطبيق
// ==============================================================================
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    // الحصول على اسم التطبيق
    const juce::String getApplicationName() override { return "Simple Audio Player"; }

    // الحصول على إصدار التطبيق
    const juce::String getApplicationVersion() override { return "1.0.0"; }

    // التهيئة - تنفيذ عند بدء التطبيق
    void initialise(const juce::String& commandLineParameters) override
    {
        juce::ignoreUnused(commandLineParameters);

        // إنشاء النافذة الرئيسية عند بدء التطبيق
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    // الإغلاق - تنظيف الذاكرة عند إنهاء التطبيق
    void shutdown() override
    {
        mainWindow = nullptr; // تنظيف الذاكرة
    }

    // معالجة طلب إعادة التشغيل من النظام
    void systemRequestedQuit() override
    {
        quit();
    }

    // معالجة فتح ملف من النظام (اختياري)
    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);
    }

private:
    // ==========================================================================
    // النافذة الرئيسية للتطبيق
    // ==========================================================================
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(const juce::String& name)
            : DocumentWindow(name,
                juce::Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true); // استخدام شريط العنوان الافتراضي للنظام
            setContentOwned(new MainComponent(), true); // إضافة المكون الرئيسي
            centreWithSize(800, 600); // توسيط النافذة بحجم مناسب
            setVisible(true); // جعل النافذة مرئية
            setResizable(true, true); // السماح بتغيير حجم النافذة
        }

        // معالجة ضغط زر الإغلاق
        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow; // مؤشر للنافذة الرئيسية
};

// ==============================================================================
// بدء تشغيل تطبيق JUCE - الماكرو المطلوب لتشغيل التطبيق
// ==============================================================================
START_JUCE_APPLICATION(SimpleAudioPlayer)