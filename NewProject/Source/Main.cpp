/*
  ==============================================================================

    Main.cpp
    Created: 23 Oct 2025 11:05:57am
    Author:  abdoy

  ==============================================================================
*/
#include <JuceHeader.h>
#include "MainComponent.h"

// تطبيق JUCE الرئيسي
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Simple Audio Player"; }
    const juce::String getApplicationVersion() override { return "1.0"; }

    void initialise(const juce::String&) override
    {
        // إنشاء النافذة الرئيسية عند بدء التطبيق
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        // تنظيف الذاكرة عند إغلاق التطبيق
        mainWindow = nullptr;
    }

private:
    // النافذة الرئيسية للتطبيق
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

        void closeButtonPressed() override
        {
            // طلب إغلاق التطبيق عند الضغط على زر الإغلاق
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    std::unique_ptr<MainWindow> mainWindow;
};

// بدء تشغيل تطبيق JUCE
START_JUCE_APPLICATION(SimpleAudioPlayer)