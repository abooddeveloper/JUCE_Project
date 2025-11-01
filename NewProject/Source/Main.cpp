/*
  ==============================================================================

    Main.cpp
    Created: 23 Oct 2025 11:05:57am
    Author:  abdoy

  ==============================================================================
*/

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
    const juce::String getApplicationVersion() override { return "1.0"; }

    // التهيئة - تنفيذ عند بدء التطبيق
    void initialise(const juce::String&) override
    {
        // إنشاء النافذة الرئيسية عند بدء التطبيق
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    // الإغلاق - تنظيف الذاكرة عند إنهاء التطبيق
    void shutdown() override
    {
        mainWindow = nullptr; // تنظيف الذاكرة
    }

private:
    // ==========================================================================
    // النافذة الرئيسية للتطبيق
    // ==========================================================================
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::lightgrey,
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true); // استخدام شريط العنوان الافتراضي للنظام
            setContentOwned(new MainComponent(), true); // إضافة المكون الرئيسي
            centreWithSize(600, 200); // توسيط النافذة
            setVisible(true); // جعل النافذة مرئية
        }

        // معالجة ضغط زر الإغلاق
        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    std::unique_ptr<MainWindow> mainWindow; // مؤشر للنافذة الرئيسية
};

// ==============================================================================
// بدء تشغيل تطبيق JUCE - الماكرو المطلوب لتشغيل التطبيق
// ==============================================================================
START_JUCE_APPLICATION(SimpleAudioPlayer)