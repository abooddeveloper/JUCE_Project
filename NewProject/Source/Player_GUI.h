#pragma once
#include <JuceHeader.h>
#include "Player_Audio.h"

// ==============================================================================
// واجهة المستخدم للتحكم في مشغل الصوت
// ==============================================================================
class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ListBoxModel
{
public:
    PlayerGUI(PlayerAudio& audioProcessor);
    ~PlayerGUI() override = default;

    // ==========================================================================
    // دوال Component للرسم والتخطيط
    // ==========================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    // ==========================================================================
    // دالة Timer للتحديثات الدورية
    // ==========================================================================
    void timerCallback() override;

    // ==========================================================================
    // معالجة الأحداث
    // ==========================================================================
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // ==========================================================================
    // دوال تحويل الوقت
    // ==========================================================================
    juce::String time_in_minutes(double time);
    juce::String time_in_seconds(int time);

    // ==========================================================================
    // دوال تحديث الواجهة
    // ==========================================================================
    void updatePlayButton(); // تحديث حالة زر التشغيل
    
    void updateMuteButton(); // تحديث حالة زر الكتم
    void updateMetadataDisplay(); // تحديث عرض البيانات الوصفية (الميزة 5)

    // ==========================================================================
    // دوال ListBoxModel المطلوبة للميزة 8: قائمة التشغيل
    // ==========================================================================
    int getNumRows() override; // الحصول على عدد العناصر في القائمة
    void paintListBoxItem(int rowNumber, juce::Graphics& g,
        int width, int height, bool rowIsSelected) override; // رسم عنصر القائمة
    void selectedRowsChanged(int lastRowSelected) override; // معالجة تغيير العنصر المحدد
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent& event) override; // معالجة النقر المزدوج
    void backgroundClicked(const juce::MouseEvent& event) override; // معالجة النقر على الخلفية
    void deleteKeyPressed(int lastRowSelected) override; // معالجة ضغط مفتاح الحذف
    void returnKeyPressed(int lastRowSelected) override; // معالجة ضغط مفتاح الإدخال

private:
    PlayerAudio& audioPlayer; // المرجع إلى معالج الصوت

    // ==========================================================================
    // متغيرات الحالة
    // ==========================================================================
    bool will_looping = false;
    bool isPositionSliderDragging = false;
    double total_time ;
    double current_time ;
    juce::String time_text;
    double loop_start_point;
    double loop_end_point ;

    // ==========================================================================
    // عناصر واجهة المستخدم الأساسية
    // ==========================================================================
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playPauseButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loop_button{ "loop" };
    juce::Slider volumeSlider; // منزلق التحكم في الصوت
    juce::Slider speedSlider; // منزلق التحكم في السرعة
    juce::Label speedLabel;   // تسمية منزلق السرعة

    // ==========================================================================
    // عناصر التحكم في الموضع والتكرار
    // ==========================================================================
    juce::Slider position_slider; // منزلق الموضع
    juce::Label positionLabel; // تسمية منزلق الموضع
    juce::Label label_time;    // تسمية عرض الوقت
    juce::Slider loop_slider;
    juce::TextButton range_loop_button{ "range_loop" };

    // ==========================================================================
    // عناصر جديدة للميزة 5: عرض البيانات الوصفية
    // ==========================================================================
    juce::Label metadataLabel; // لعرض البيانات الوصفية
    juce::TextEditor metadataDisplay; // لعرض البيانات الوصفية بشكل منسق

    // ==========================================================================
    // عناصر جديدة للميزة 8: قائمة التشغيل
    // ==========================================================================
    juce::TextButton addToPlaylistButton{ "Add to Playlist" }; // زر إضافة للقائمة
    juce::TextButton clearPlaylistButton{ "Clear Playlist" }; // زر مسح القائمة
    juce::ListBox playlistBox; // صندوق القائمة
    juce::StringArray playlistFiles; // مصفوفة أسماء الملفات في القائمة
    std::vector<juce::File> playlistFileObjects; // متجه لحفظ كائنات الملفات

    std::unique_ptr<juce::FileChooser> fileChooser; // منتقي الملفات

    void loadFile(); // دالة تحميل الملف
};