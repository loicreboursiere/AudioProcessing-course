#include "MainComponent.h"

//==============================================================================
MainContentComponent::MainContentComponent()
{
    addAndMakeVisible(openButton);
    addAndMakeVisible(loopToggle);
    addAndMakeVisible(loopStartTE);
    addAndMakeVisible(loopStopTE);
    addAndMakeVisible(loopStartLabel);
    addAndMakeVisible(loopStopLabel);

    openButton.setButtonText("Open...");
    loopStartLabel.setText("Loop start : ", juce::dontSendNotification);
    loopStopLabel.setText("Loop stop : ", juce::dontSendNotification);

    loopStartTE.setEditable(true); 
    loopStopTE.setEditable(true);
    //durationMinTE.setText("10", juce::dontSendNotification);

    loopStartTE.setJustificationType(juce::Justification::left);
    loopStopTE.setJustificationType(juce::Justification::left);

    loopStartTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    loopStopTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);

    openButton.onClick = [this] { openButtonClicked(); };
    loopToggle.onClick = [this] { loopToggle.getToggleState() ? isLooping = true : isLooping = false; };
    loopStartTE.onTextChange = [this] { loopStart = loopStartTE.getText().getIntValue(); position = loopStart; };
    loopStopTE.onTextChange = [this] { loopStop = loopStopTE.getText().getIntValue(); };
    setSize(300, 200);

    formatManager.registerBasicFormats();

}

MainContentComponent::~MainContentComponent() 
{
    shutdownAudio();
}

//==============================================================================
void MainContentComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainContentComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    auto numInputChannels = fileBuffer.getNumChannels();
    auto numOutputChannels = bufferToFill.buffer->getNumChannels();

    auto outputSamplesRemaining = bufferToFill.numSamples;                                  // [8]
    auto outputSamplesOffset = bufferToFill.startSample;                                    // [9]

    auto loopSize = loopStop - loopStart;

    while (outputSamplesRemaining > 0 && position <= loopSize )
    {
        //auto bufferSamplesRemaining = fileBuffer.getNumSamples() - position;                // [10]
        
        auto bufferSamplesRemaining = loopSize - position;
        auto samplesThisTime = juce::jmin(outputSamplesRemaining, bufferSamplesRemaining); // [11]

        for (auto channel = 0; channel < numOutputChannels; ++channel)
        {
            bufferToFill.buffer->copyFrom(channel,                                         // [12]
                outputSamplesOffset,                             //  [12.1]
                fileBuffer,                                      //  [12.2]
                channel % numInputChannels,                      //  [12.3]
                position,                                        //  [12.4]
                samplesThisTime);                                //  [12.5]
        }

        outputSamplesRemaining -= samplesThisTime;                                          // [13]
        outputSamplesOffset += samplesThisTime;                                             // [14]
        position += samplesThisTime; 
                                                       
        if (isLooping)
        {
            if (position == fileBuffer.getNumSamples())
                position = loopStart;
        }
        
    }

    
}


//==============================================================================
void MainContentComponent::releaseResources()
{
    fileBuffer.setSize(0, 0);
}

void MainContentComponent::resized()
{
    openButton.setBounds(10, 10, getWidth() - 80, 20);
    loopToggle.setBounds(getWidth() - 60, 10, 50, 20);
    loopStartLabel.setBounds(10, 40, 100, 20);
    loopStartTE.setBounds(110, 40, 100, 20);
    loopStopLabel.setBounds(10, 70, 100, 20);
    loopStopTE.setBounds(110, 70, 100, 20);
}

void MainContentComponent::openButtonClicked()
{
    shutdownAudio();                                                                            // [1]

    chooser = std::make_unique<juce::FileChooser>("Select a sound file",
        juce::File{},
        "*.wav, *.mp3");
    auto chooserFlags = juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file == juce::File{})
                return;

            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file)); // [2]

            if (reader.get() != nullptr)
            {
                //auto duration = (float)reader->lengthInSamples / reader->sampleRate;               // [3]

                fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);  // [4]
                reader->read(&fileBuffer,                                                      // [5]
                    0,                                                                //  [5.1]
                    (int)reader->lengthInSamples,                                    //  [5.2]
                    0,                                                                //  [5.3]
                    true,                                                             //  [5.4]
                    true);                                                            //  [5.5]
                position = 0;                                                                   // [6]
                setAudioChannels(0, (int)reader->numChannels);                                // [7]
                
                loopStart = 0;
                loopStop = reader->lengthInSamples;
                loopStartTE.setText( juce::String(loopStart), juce::dontSendNotification);
                loopStopTE .setText( juce::String(loopStop), juce::dontSendNotification );
            }
        });
}
