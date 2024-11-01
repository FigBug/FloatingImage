#pragma once

#include <JuceHeader.h>

//==============================================================================
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    juce::PropertiesFile& getSettings();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void moved() override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;


private:
    //==============================================================================
    std::unique_ptr<juce::PropertiesFile> properties;

    juce::ComponentDragger dragger;

    juce::Image image;
    float alpha = 0.5f;
    float scale = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
