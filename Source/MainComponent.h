#pragma once

#include <JuceHeader.h>

//==============================================================================
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

	void mouseDown (const juce::MouseEvent& e) override;
	void mouseDrag (const juce::MouseEvent& e) override;
	void mouseUp (const juce::MouseEvent& e) override;

private:
    //==============================================================================
	juce::ComponentDragger dragger;

	juce::Image image;
	float alpha = 0.5f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
