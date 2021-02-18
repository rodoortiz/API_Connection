/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RestRequest.h"
#include "LoginComponent.h"

//==============================================================================
/**
*/
class Api_connectionAudioProcessorEditor  : public juce::AudioProcessorEditor, public Button::Listener
{
public:
    Api_connectionAudioProcessorEditor (Api_connectionAudioProcessor&);
    ~Api_connectionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void buttonClicked(Button* buttonCliked) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Api_connectionAudioProcessor& audioProcessor;
    
    LoginComponent loginComponent;
    TextButton button{"Connect"};
    
    RestRequest request;
    RestRequest::Response response;
    URL url;

    var parsedJson;
    String jwt;
    bool access = true;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Api_connectionAudioProcessorEditor)
};
