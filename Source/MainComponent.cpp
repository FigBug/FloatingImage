#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setOpaque (false);
    setVisible (true);
    centreWithSize (640, 480);
    addToDesktop (juce::ComponentPeer::windowAppearsOnTaskbar);

    auto& props = getSettings();
    alpha = props.getDoubleValue ("alpha", alpha);
    scale = props.getDoubleValue ("scale", scale);

    if (auto f = juce::File (props.getValue ("file")); f.existsAsFile())
        if (auto i = juce::ImageFileFormat::loadFrom (f); ! i.isNull())
            image = i;

    if (auto pos = props.getValue ("pos"); pos.isNotEmpty())
        setBounds (juce::Rectangle<int>::fromString (pos));

    if (image.isValid())
        setSize (image.getWidth() / scale, image.getHeight() / scale);
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
    getProperties().set ("pos", getBounds().toString());
}

void MainComponent::moved()
{
    resized();
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
        juce::PopupMenu m;

        m.addItem ("Open...", [this]
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
                        setSize (image.getWidth() / scale, image.getHeight() / scale);

                        getSettings().setValue ("file", f.getFullPathName());
                    }
                }
            });
        });
        m.addSeparator();

        auto setScale = [this] (float s)
        {
            scale = s;
            if (image.isValid())
                setSize (image.getWidth() / scale, image.getHeight() / scale);
            getSettings().setValue ("scale", scale);
        };

        juce::PopupMenu sm;
        sm.addItem ("1x", true, juce::approximatelyEqual (1.0f, scale), [setScale] { setScale (1.0); });
        sm.addItem ("2x", true, juce::approximatelyEqual (2.0f, scale), [setScale] { setScale (2.0); });
        sm.addItem ("3x", true, juce::approximatelyEqual (3.0f, scale), [setScale] { setScale (3.0); });
        sm.addItem ("4x", true, juce::approximatelyEqual (4.0f, scale), [setScale] { setScale (4.0); });

        m.addSubMenu ("Scale Factor", sm);

        m.addSeparator();
        m.addItem ("Quit", [] { juce::JUCEApplication::quit(); });

        m.showMenuAsync (juce::PopupMenu::Options().withMousePosition());

        return;
    }
}

void MainComponent::mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails& w)
{
    auto dy = w.deltaY * 0.5f;

    if (w.isReversed)
        dy *= -1;

    alpha = std::clamp (alpha + dy, 0.0f, 1.0f);
    repaint ();

    getProperties().set ("alpha", alpha);
}

juce::PropertiesFile& MainComponent::getSettings()
{
    if (properties == nullptr)
    {
       #if JUCE_MAC
        auto dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory).getChildFile ("Application Support/RabienSoftware/FloatingImage");
       #elif JUCE_LINUX
        auto dir = juce::File ("~/.config/RabienSoftware/" + getName());
       #else
        auto dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory).getChildFile ("RabienSoftwareFloatingImage));
       #endif

        dir.createDirectory();

        juce::PropertiesFile::Options options;
        properties = std::make_unique<juce::PropertiesFile> (dir.getChildFile ("settings.xml"), options);
    }
    return *properties;
}
