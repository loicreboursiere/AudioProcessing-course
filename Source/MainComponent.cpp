#include "MainComponent.h"

//==============================================================================
MainContentComponent::MainContentComponent()
{
    addAndMakeVisible(openButton);
    addAndMakeVisible(loopToggle);
    addAndMakeVisible(startPointLabel);
    addAndMakeVisible(durationLabel);
    addAndMakeVisible(startPointTE);
    addAndMakeVisible(durationTE);
    addAndMakeVisible(makeGrainButton);
    //addAndMakeVisible(metroLabel);
    //addAndMakeVisible(metroTE);
    //addAndMakeVisible(randomToggle);
    addAndMakeVisible(loopStartFeedbackLabel);
    addAndMakeVisible(loopStopFeedbackLabel);
    addAndMakeVisible(loopStartFeedbackTE);
    addAndMakeVisible(loopStopFeedbackTE);


    startPointTE.setEditable(true);
    durationTE.setEditable(true);
    metroTE.setEditable(true);

    openButton.setButtonText("Open...");
    startPointLabel.setText("Start point", juce::dontSendNotification);
    durationLabel.setText("Duration", juce::dontSendNotification);
    startPointTE.setText("10", juce::dontSendNotification);
    durationTE.setText("500", juce::dontSendNotification);
    metroLabel.setText("Tempo", juce::dontSendNotification);
    metroTE.setText("300", juce::dontSendNotification);
    loopStartFeedbackLabel.setText("Loop start", juce::dontSendNotification);
    loopStopFeedbackLabel.setText("Loop stop", juce::dontSendNotification);
    loopStartFeedbackTE.setText("0", juce::dontSendNotification);
    loopStopFeedbackTE.setText("0", juce::dontSendNotification);


    startPointTE       .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    durationTE         .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    metroTE            .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    loopStartFeedbackTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    loopStopFeedbackTE .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);


    ranges.start    = startPointTE.getText().getIntValue();
    ranges.duration = durationTE.getText().getIntValue();

    openButton.onClick        = [this] { openButtonClicked(); };
    loopToggle.onClick        = [this] { isLooping = loopToggle.getToggleState(); };
    startPointTE.onTextChange = [this] { ranges.start = startPointTE.getText().getIntValue(); };
    durationTE.onTextChange   = [this] { ranges.duration = durationTE.getText().getIntValue(); };
    
    makeGrainButton.onClick   = [this] { makeGrain(); };

    setSize(400, 200);

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
    this->sampleRate = sampleRate;
}

void MainContentComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    auto numInputChannels = fileBuffer.getNumChannels();
    auto numOutputChannels = bufferToFill.buffer->getNumChannels();

    auto outputSamplesRemaining = bufferToFill.numSamples;                                  // [8]
    auto outputSamplesOffset = bufferToFill.startSample;                                    // [9]

    auto numSamplesInBlock = bufferToFill.buffer->getNumSamples();

    //auto loopSize
    //juce::AudioBuffer grain
    
    while (outputSamplesRemaining > 0 )//&& position <= currentGrain.duration_sample )//fileBuffer.getNumSamples() )
    {
        //auto bufferSamplesRemaining = fileBuffer.getNumSamples() - position;                // [10]
        auto bufferSamplesRemaining = ( currentGrain.duration_sample + currentGrain.start_sample ) - position;
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
        position += samplesThisTime;                                                        // [15]

        if ( isLooping )
        {
            //if (position == fileBuffer.getNumSamples())
            //    position = 0;
            if (position == currentGrain.stop_sample)
                position = currentGrain.start_sample;
        }
    }
    
    
    /*
    for (int sample = 0; sample < numSamplesInBlock; ++sample)
    {
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            const float* fileData = fileBuffer.getReadPointer(channel % fileBuffer.getNumChannels());
            float* channelData = bufferToFill.buffer->getWritePointer(channel);
            if (position > fileBuffer.getNumSamples())
            {
                position = fileBuffer.getNumSamples();
                DBG("filePosition trimed back to numSamplesInFile");
            }
            channelData[sample] = fileData[position];
        }

        if (position < currentGrain.stop_sample)
        {
            ++position;
        }
        else if (isLooping)
        {
            position = currentGrain.start_sample;
        }
    }
    */
}


//==============================================================================
void MainContentComponent::releaseResources()
{
    fileBuffer.setSize(0, 0);
}

void MainContentComponent::resized()
{
    openButton.setBounds( 10, 10, getWidth() - 120, 20 );
    loopToggle.setBounds( getWidth() - 110, 10, 100, 20 );
    
    startPointLabel.setBounds(10, 50, 70, 20);
    startPointTE.setBounds(80, 50, 60, 20);
    durationLabel.setBounds(140, 50, 70, 20);
    durationTE.setBounds(210, 50, 60, 20);
    makeGrainButton.setBounds(280, 50, 50, 20);

    loopStartFeedbackLabel.setBounds(10, 85, 70, 20);
    loopStartFeedbackTE.setBounds(80, 85, 60, 20);
    loopStopFeedbackLabel.setBounds(140, 85, 70, 20);
    loopStopFeedbackTE.setBounds(210, 85, 60, 20);

    metroLabel.setBounds(10, 120, 50, 20);
    metroTE.setBounds( 70, 120, 60, 20 );
    randomToggle.setBounds(130, 120, 140, 20);

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
                initGrainWithSF(fileBuffer.getNumSamples());
            }
        });
    
}
/**
* Create random start and duration of the grain and feed currentGrain struct
*/
void MainContentComponent::makeGrain()
{
    // Feed currentGrain struct
    currentGrain.duration_ms     = juce::Random::getSystemRandom().nextInt( ranges.duration );
    //currentGrain.start_ms = juce::Random::getSystemRandom().nextInt( samples2Ms(fileBuffer.getNumSamples(), sampleRate) - ranges.duration );
    currentGrain.start_ms        = juce::jmin(
        juce::Random::getSystemRandom().nextInt(ranges.start),
        samples2Ms(fileBuffer.getNumSamples(), sampleRate) - ranges.duration
        );
    currentGrain.stop_ms         = currentGrain.start_ms + currentGrain.duration_ms;
    currentGrain.start_sample    = ms2Samples(currentGrain.start_ms, sampleRate );
    currentGrain.stop_sample     = ms2Samples(currentGrain.stop_ms, sampleRate);
    currentGrain.duration_sample = ms2Samples(currentGrain.duration_ms, sampleRate);

    // UpdateGUI
    loopStartFeedbackTE          .setText(juce::String(currentGrain.start_ms), juce::dontSendNotification);
    loopStopFeedbackTE           .setText(juce::String(currentGrain.stop_ms), juce::dontSendNotification);

    // Update position for processing 
    position                     = currentGrain.start_sample;

}

void MainContentComponent::initGrainWithSF(int soundFileSampleSize)
{
    currentGrain.start_ms = 0;
    currentGrain.start_sample = 0;
    currentGrain.duration_sample = soundFileSampleSize;
    currentGrain.duration_ms = samples2Ms(currentGrain.duration_sample, sampleRate);
    currentGrain.stop_ms = currentGrain.start_ms + currentGrain.duration_ms;
    currentGrain.stop_sample = currentGrain.start_sample + currentGrain.duration_sample;

}


int MainContentComponent::ms2Samples(int ms, int sampleRate)
{
    return (int) (sampleRate * ms / 1000);
}

int MainContentComponent::samples2Ms(int samples, int sampleRate)
{
    return (int)(samples / sampleRate * 1000);
}