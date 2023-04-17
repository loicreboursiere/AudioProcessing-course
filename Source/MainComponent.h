

#pragma once

#include <JuceHeader.h>
//==============================================================================
class MainContentComponent : public juce::AudioAppComponent
{
public:
    MainContentComponent();
    
    ~MainContentComponent() override;
    
    void prepareToPlay(int, double) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void resized() override;


private:
  
    struct GrainsConfig {
        int duration;
        int start;
        int tempo;
    };

    struct Grain {
        int start_ms;
        int stop_ms;
        int duration_ms;
        int start_sample;
        int stop_sample;
        int duration_sample;
    };
    
    
    void openButtonClicked();

    /*
        Init all fields of currentGrain with grainConfig values.
    */
    void makeGrain();


    /*
    *   Init currentGrain fields with loaded soudfiles values.
    */
    void initGrainWithSF( int soundFileSampleSize );

    /*
    * Convert time in ms to sample number.
    */
    int ms2Samples(int ms, int sampleRate);

    /*
    *   Convert sample number to ms.
    */
    int samples2Ms(int nbOfSamples, int sampleRate);

    //==========================================================================
    juce::TextButton openButton;
    juce::ToggleButton loopToggle { "Loop on/off" };

    juce::TextButton makeGrainButton{ "New loop" };

    juce::Label startPointLabel;
    juce::Label durationLabel;
    juce::Label startPointTE;
    juce::Label durationTE;

    juce::Label loopStartFeedbackLabel;
    juce::Label loopStopFeedbackLabel;
    juce::Label loopStartFeedbackTE;
    juce::Label loopStopFeedbackTE;
    
    juce::ToggleButton randomToggle{ "Automate" };
    juce::Label metroLabel;
    juce::Label metroTE;

    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    juce::AudioSampleBuffer fileBuffer;
    juce::AudioSampleBuffer grainBuffer;
    
    int position = 0;
    int startPoint;
    int duration;
    int metro;
    int fileLengthMs;
    int sampleRate;

    bool isLooping = false;

    GrainsConfig ranges;
    Grain currentGrain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
