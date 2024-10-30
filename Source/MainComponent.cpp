#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
	setOpaque (false);
	setVisible (true);
	centreWithSize (640, 480);
	addToDesktop (juce::ComponentPeer::windowAppearsOnTaskbar);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
	if (image.isNull())
	{
		g.fillCheckerBoard (getLocalBounds().toFloat(), 50.0f, 50.0f, juce::Colours::black.withMultipliedAlpha (alpha), juce::Colours::white.withMultipliedAlpha (alpha));
		return;
	}

	g.setOpacity (alpha);
	g.drawImage (image, getLocalBounds().toFloat());
}

void MainComponent::resized()
{
}

void MainComponent::mouseDown (const juce::MouseEvent& e)
{
	dragger.startDraggingComponent (this, e);
}

void MainComponent::mouseDrag (const juce::MouseEvent& e)
{
	dragger.dragComponent (this, e, nullptr);
}

void MainComponent::mouseUp (const juce::MouseEvent& e)
{
	if (e.mouseWasClicked())
	{
		auto chooser = std::make_shared<juce::FileChooser> ("Open Image", juce::File(), "*.png");
		chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this, chooser] (const juce::FileChooser&)
		{
			if (auto f = chooser->getResult(); f.existsAsFile())
			{
				if (auto i = juce::ImageFileFormat::loadFrom (f); ! i.isNull())
				{
					image = i;
					repaint();
					setSize (image.getWidth() / 2, image.getHeight() / 2);
				}
			}
		});

		return;
	}
}
