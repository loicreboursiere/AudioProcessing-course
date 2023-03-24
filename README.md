# AudioProcessing Courses with JUCE

Here is the code material on which is based a 12h Audio Processing course based on [JUCE](http://juce.com).
The different steps of this course can be accessed through the following tags :

1. step-0 : base code for our course. Basically it is a file player. It has been mainly based on [Looping audio using the AudioSampleBuffer class](https://docs.juce.com/master/tutorial_looping_audio_sample_buffer.html) JUCE turorial, with few adjustments.


## Code

The base code is basically a simple file player. It embeds a simple GUI as an "Open" button which opens a file browser to select the soundfile to play.
As soon as the sound fil eis selected and the filebrowser is closed the sound starts to play until it reaches the end.

### GUI
The Open button "comes alive" through different steps.

1. In the private section of the MainComponent.h file, the button is declared :
```
juce::TextButton openButton;
```

2. In the constructor function of the MainComponent.cpp file the button is created, made visible and its properties are set : 
```
addAndMakeVisible(openButton);
openButton.setButtonText("Open...");
openButton.onClick = [this] { openButtonClicked(); };
```
The `.onClick` corresponds to a lambda which can be seen as a function declared, defined, and direcly assigned on one instruction.

3. In the resized function, the button is placed on the frame and its height and width are defined through the `setBounds` function :
```
openButton.setBounds(10, 10, getWidth() - 20, 20);
```


## Credits

The [sound file](https://freesound.org/people/ValentinSosnitskiy/sounds/495482/0) present in ressources folder is coming from [freesound.org](freesound.org) website and has been trimed from the original.





