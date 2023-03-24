

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
  
    void openButtonClicked();
    
    //==========================================================================
    juce::TextButton openButton;
    
    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    juce::AudioSampleBuffer fileBuffer;
    int position;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
