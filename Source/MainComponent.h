

#pragma once

#include <JuceHeader.h>
#include <string.h>
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
  
    void openButtonClicked();

    void updateLoopSize();
    
    //==========================================================================
    juce::TextButton openButton;
    juce::ToggleButton loopToggle { "Loop" };

    juce::Label loopStartTE;
    juce::Label loopStopTE;
    juce::Label loopStartLabel { "Loop Start" };
    juce::Label loopStopLabel  { "Loop Stop" };

    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    juce::AudioSampleBuffer fileBuffer;
    int position;

    bool isLooping;
    int loopStart;
    int loopStop;
    int loopSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
