/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Api_connectionAudioProcessorEditor::Api_connectionAudioProcessorEditor (Api_connectionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(button);
    button.addListener(this);

    String respuesta = loginComponent.readJSON();
    var parsedJSON = JSON::parse(respuesta);
    
    int actualTimeInSec = getMyTime();
    int iat = int(parsedJSON["iat"]);
    int exp = int(parsedJSON["exp"]);
    
    //OPC 1
    if(!parsedJSON["vst_access"])
        addAndMakeVisible(loginComponent);
    else {
        if(!(actualTimeInSec >= iat && actualTimeInSec <= exp))
            addAndMakeVisible(loginComponent);
    }
    
    //OPC 2
    /*if(respuesta == "")
        addAndMakeVisible(loginComponent);*/
    
    setSize (500, 400);
}

Api_connectionAudioProcessorEditor::~Api_connectionAudioProcessorEditor()
{
}

//==============================================================================
void Api_connectionAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::goldenrod);
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    loginComponent.setAlpha(0.9f);
}

void Api_connectionAudioProcessorEditor::resized()
{
    loginComponent.setSize(375, 300);
    loginComponent.setCentreRelative(0.5f, 0.5);
     
    button.centreWithSize(125, 100);
    button.setColour(TextButton::buttonColourId, Colours::red);
}

int Api_connectionAudioProcessorEditor::getMyTime()
{
    return (int)(Time::currentTimeMillis() / 1000);
}

void Api_connectionAudioProcessorEditor::buttonClicked(Button* buttonClicked) {
    if(buttonClicked == &button) {
        //For Juce NetworkingDemo.h example
//        URL url = URL("https://api.cryptonator.com/api/ticker/btc-usd");
//        request.getResultText(url);
        
        request.header("Content-Type", "application/json");
        
        response = request.post("https://samplehouse.herokuapp.com/api/user/login").field("email", "jackbarry@testing.com").field("password", "password").execute();
//        DBG(response.bodyAsString);
        
        if(juce::JSON::parse(response.bodyAsString, parsedJson).wasOk()) {
            jwt = parsedJson["token"];
            
            int charIndex1 = jwt.indexOfAnyOf(".");
            
            String tempJwt = jwt.substring(charIndex1+1);
            int charIndex2 = tempJwt.indexOfAnyOf(".");
            
            String tokenPayload = jwt.substring(charIndex1+1, (charIndex1+1)+charIndex2);
            
            tokenPayload += String (std::string (tokenPayload.length() % 4, '='));
            
            MemoryOutputStream decodedStream;
            if(Base64::convertFromBase64(decodedStream, tokenPayload)) {
                DBG("Token:" << decodedStream.toString());
                
                if(juce::JSON::parse(decodedStream.toString(), parsedJson).wasOk()){
                    access = parsedJson["vst_access"];
                }
            } else {
                DBG("Decoded JWT: " << decodedStream.toString());
            }
        } else {
            DBG("FAIL");
        }
        
    }
}
