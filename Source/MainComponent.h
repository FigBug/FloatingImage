#pragma once

#include <JuceHeader.h>

//==============================================================================
class MainComponent : public juce::Component, public juce::FileDragAndDropTarget
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

	bool isInterestedInFileDrag (const juce::StringArray& files) override;
	void fileDragEnter (const juce::StringArray& files, int x, int y) override;
	void fileDragExit (const juce::StringArray& files) override;
	void filesDropped (const juce::StringArray& files, int x, int y) override;

private:
    //==============================================================================
    std::unique_ptr<juce::PropertiesFile> properties;

    juce::ComponentDragger dragger;

    juce::Image image;
    float alpha = 0.5f;
    float scale = 1.0f;
	bool dragOver = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
