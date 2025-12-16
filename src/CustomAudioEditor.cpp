#include "CustomAudioEditor.h"
#include "BinaryData.h"
#include "utilities/MidiNoteNumberFromName.h"

using namespace juce;

CustomAudioEditor::CustomAudioEditor (CustomAudioProcessor* const p, RNBO::CoreObject& rnboObject)
    : AudioProcessorEditor (p)
    , _audioProcessor (p)
    , rnboProcessor (p)
    , _rnboObject (rnboObject)
    , presetBar ({}, p->presetTree)
    , _parameterInterface (_rnboObject.createParameterInterface (RNBO::ParameterEventInterface::SingleProducer, this))
{
    addAndMakeVisible (editorBG);

    auto parameters = rnboProcessor->getParameters();
    for (auto& param: rnboProcessor->getParameters())
    {
        if (param->getName (999) == "Scale")
        {
            if (auto* parameter = dynamic_cast<RangedAudioParameter*> (param))
            {
                scaleBox = std::make_unique<ParamBox> (parameter);
                addAndMakeVisible (*scaleBox);
                scaleBoxLabel.setFont (27.0f);
                addAndMakeVisible (scaleBoxLabel);
            }
        }
        if (param->getName (999) == "Steps")
        {
            if (auto* parameter = dynamic_cast<RangedAudioParameter*> (param))
            {
                callbacks.add (*parameter,
                               nlt::APVTSCallbacks::gui,
                               [this] (float val)
                               {
                                   melodySequencer.setSequenceLength (static_cast<int> (val));
                                   bassSequencer.setSequenceLength (static_cast<int> (val));
                                   kickSequencer.setSequenceLength (static_cast<int> (val));
                                   snareSequencer.setSequenceLength (static_cast<int> (val));
                                   hatSequencer.setSequenceLength (static_cast<int> (val));
                                   seqStepIndicator.setSequenceLength (static_cast<int> (val));
                               });
            }
        }
    }

    File saveLocation = File::getSpecialLocation (File::userDocumentsDirectory);
    saveLocation      = saveLocation.getChildFile ("Chippo");
    saveLocation.createDirectory();
    saveLocation.setReadOnly (false);

    presetBar.setSaveLocation (saveLocation);
    presetBar.setFileExtension ("hop");

    presetBar.setSaveFn ([this] (MemoryBlock& destData) { _audioProcessor->getStateInformation (destData); });
    presetBar.setLoadFn ([this] (MemoryBlock& loadData)
                         { _audioProcessor->setStateInformation (loadData.getData(), static_cast<int> (loadData.getSize())); });

    addAndMakeVisible (presetBar);

    setupSequencers();

    addAndMakeVisible (melodySequencer);
    addAndMakeVisible (bassSequencer);
    addAndMakeVisible (kickSequencer);
    addAndMakeVisible (snareSequencer);
    addAndMakeVisible (hatSequencer);
    addAndMakeVisible (seqStepIndicator);
    setupSliders();
    setupToggles();
    setupButtons();
    setupTooltips();
    addChildComponent (aboutPanel);

    static RNBO::MessageTag retrieveSequences { RNBO::TAG ("retrieveSequences") };
    static RNBO::MessageTag bang { RNBO::TAG ("") };
    _rnboObject.sendMessage (retrieveSequences, bang);
    setRepaintsOnMouseActivity (false);

    setSize (1120, 1020);

    File file;
}

CustomAudioEditor::~CustomAudioEditor()
{
    _audioProcessor->AudioProcessor::removeListener (this);
    setLookAndFeel (nullptr);
}

void CustomAudioEditor::resized()
{
    auto bounds = getLocalBounds();
    editorBG.setBounds (bounds);
    aboutPanelButton.setBounds (40, 10, 450, 100);
    aboutPanel.setBounds (bounds);
    presetBar.setBounds (570, 24, 474, 70);

    {
        auto topRowControlsY   = 140;
        auto topRowLabelOffset = 7;
        auto controlOffset     = 130;
        runToggle->setBounds (75, topRowControlsY, 35, 35);
        runLabel.setBounds (runToggle->getRight() + topRowLabelOffset, topRowControlsY, 100, 35);
        sliders[Sliders::rootNote]->setBounds (runToggle->getRight() + 90, topRowControlsY, 105, 35); // 185
        //        sliderLabels[Sliders::rootNote]->setBounds (sliders[Sliders::rootNote]->getRight() + topRowLabelOffset,
        //                                                    topRowControlsY,
        //                                                    100,
        //                                                    35);
        scaleBox->setBounds (sliders[Sliders::rootNote]->getRight() + controlOffset, topRowControlsY, 131, 35); // 393
        //        scaleBoxLabel.setBounds (scaleBox->getRight() + topRowLabelOffset, topRowControlsY, 150, 35);
        sliders[Sliders::stepLength]->setBounds (scaleBox->getRight() + controlOffset, topRowControlsY, 55, 35); // 630
        sliders[Sliders::density]->setBounds (sliders[Sliders::stepLength]->getRight() + controlOffset,
                                              topRowControlsY,
                                              65,
                                              35); // 790
    }

    auto toggleWidth = 30;
    int  knobW       = 136;
    {
        auto placeSynthControls = [this, knobW] (auto& params, int yPos)
        {
            size_t i = 0;
            sliders[params[i]]->setBounds (74, yPos + 15, 65, 115);    // level
            sliders[params[++i]]->setBounds (148, yPos + 19, 38, 110); // octave
            auto mmPos = sliders[params[i]]->getRight();
            sliders[params[++i]]->setBounds (mmPos, yPos, knobW, knobW + 10); // waveshape
            int xPos = 348;
            sliders[params[++i]]->setBounds (xPos, yPos, knobW, knobW + 10);            // A
            sliders[params[++i]]->setBounds (xPos += (knobW), yPos, knobW, knobW + 10); // D
            sliders[params[++i]]->setBounds (xPos += (knobW), yPos, knobW, knobW + 10); // S
            sliders[params[++i]]->setBounds (xPos += (knobW), yPos, knobW, knobW + 10); // R
        };
        placeSynthControls (Sliders::melodyIdts, 205);
        placeSynthControls (Sliders::bassIdts, 408);
    }

    {
        auto placeDrumControls = [this, toggleWidth] (const std::vector<Identifier>& params, int xPos)
        {
            sliders[params[0]]->setBounds (xPos, 622, 65, 115); // level
            auto buttonX = sliders[params[0]]->getRight() + 7;
            seqGenButtons[params[1]]->setBounds (buttonX, 622, toggleWidth, toggleWidth); // gen seq
            seqGenButtons[params[2]]->setBounds (buttonX,                                 // clear seq
                                                 seqGenButtons[params[1]]->getBottom() + 10,
                                                 toggleWidth,
                                                 toggleWidth);
        };
        placeDrumControls ({ Sliders::kickLevel, SeqButtons::kick, SeqButtons::clearKick }, 74);
        placeDrumControls ({ Sliders::snareLevel, SeqButtons::snare, SeqButtons::clearSnare }, 320);
        placeDrumControls ({ Sliders::hatLevel, SeqButtons::hat, SeqButtons::clearHat }, 570);
    }

    auto rightControlColumnX = 920;
    {
        auto placeSeqGenButtons = [toggleWidth, rightControlColumnX] (auto& button, int yPos)
        {
            auto buttonPaddingY = 7;
            button->setBounds (rightControlColumnX, yPos, toggleWidth, toggleWidth);
            return button->getBottom() + buttonPaddingY;
        };
        auto nextBtnY = placeSeqGenButtons (seqGenButtons[SeqButtons::melody], 233);
        nextBtnY      = placeSeqGenButtons (infinityToggle, nextBtnY);
        placeSeqGenButtons (seqGenButtons[SeqButtons::clearMelody], nextBtnY);

        nextBtnY = placeSeqGenButtons (seqGenButtons[SeqButtons::bass], 436);
        placeSeqGenButtons (seqGenButtons[SeqButtons::clearBass], nextBtnY);

        for (auto& b: SeqButtons::allIdts)
        {
            auto btnBnds = seqGenButtons[b]->getBounds();
            seqGenLabels[b]->setBounds (btnBnds.getRight() + 3, btnBnds.getY(), 150, btnBnds.getHeight());
        }
    }

    sliders[Sliders::reverbLevel]->setBounds (rightControlColumnX, 610, knobW, knobW);

    auto sequencerBounds = bounds.removeFromBottom (170);
    sequencerBounds.removeFromLeft (35); // label space
    auto indicatorBounds = sequencerBounds;
    sequencerBounds.removeFromTop (35); // for indicator
    sequencerBounds.removeFromRight (3);
    melodySequencer.setBounds (sequencerBounds.removeFromTop (25));
    bassSequencer.setBounds (sequencerBounds.removeFromTop (25));
    hatSequencer.setBounds (sequencerBounds.removeFromTop (25));
    snareSequencer.setBounds (sequencerBounds.removeFromTop (25));
    kickSequencer.setBounds (sequencerBounds.removeFromTop (25));
    seqStepIndicator.setBounds (indicatorBounds.withBottom (kickSequencer.getBottom()));
}

void CustomAudioEditor::audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float value)
{
    // Handle parameter changes here
}

void CustomAudioEditor::handleMessageEvent (const RNBO::MessageEvent& event)
{
    static RNBO::MessageTag melodyTag { RNBO::TAG ("melodySequenceOut") };
    static RNBO::MessageTag bassTag { RNBO::TAG ("bassSequenceOut") };
    static RNBO::MessageTag kickTag { RNBO::TAG ("kickSequenceOut") };
    static RNBO::MessageTag snareTag { RNBO::TAG ("snareSequenceOut") };
    static RNBO::MessageTag hatTag { RNBO::TAG ("hatSequenceOut") };
    static RNBO::MessageTag stepPosition { RNBO::TAG ("stepPosition") };
    static RNBO::MessageTag transportBPM { RNBO::TAG ("presetMessage") };

    if (event.getTag() == stepPosition)
    {
        currentStep = static_cast<int> (event.getNumValue());
    }
    else if (event.getTag() == melodyTag)
    {
        melodySequencer.setSequenceWithEvent (event);
    }
    else if (event.getTag() == hatTag)
    {
        hatSequencer.setSequenceWithEvent (event);
    }
    else if (event.getTag() == bassTag)
    {
        bassSequencer.setSequenceWithEvent (event);
    }
    else if (event.getTag() == kickTag)
    {
        kickSequencer.setSequenceWithEvent (event);
    }
    else if (event.getTag() == snareTag)
    {
        snareSequencer.setSequenceWithEvent (event);
    }
}

void CustomAudioEditor::sendSequencerValues (SequencerComponent& seq, StringRef inputTag)
{
    auto newValues = seq.getCurrentSequence();
    auto newList   = RNBO::make_unique<RNBO::list>();
    newList->reserve (newValues.size());
    for (auto v: newValues)
        newList->push (static_cast<int> (v));
    _parameterInterface->sendMessage (RNBO::TAG (inputTag), std::move (newList));
}

static auto checkSliderType = [] (auto& sliderGroup, auto& p) -> Identifier
{
    for (auto& paramIdt: sliderGroup)
        if (paramIdt.toString() == p->getParameterID())
            return paramIdt;
    return {};
};

void CustomAudioEditor::setupSliders()
{
    auto parameters = rnboProcessor->getParameters();

    int milkAlternator = 0;
    // clip off the top of the "fill" image so the slider better resembles a glass
    auto milkFillOne = ImageCache::getFromMemory (BinaryData::Milk1_Full_png, BinaryData::Milk1_Full_pngSize);
    milkFillOne.clear (milkFillOne.getBounds().removeFromTop (milkFillOne.getBounds().proportionOfHeight (0.07f)),
                       Colours::transparentBlack);
    auto milkFillTwo = ImageCache::getFromMemory (BinaryData::Milk2_Full_png, BinaryData::Milk2_Full_pngSize);
    milkFillTwo.clear (milkFillTwo.getBounds().removeFromTop (milkFillTwo.getBounds().proportionOfHeight (0.07f)),
                       Colours::transparentBlack);

    int cookieAlternator = 0;
    for (auto& parameter: parameters)
    {
        if (auto* param = dynamic_cast<RangedAudioParameter*> (parameter))
        {
            auto paramIdt = checkSliderType (Sliders::allIdts, param);
            if (paramIdt.isValid())
            {
                bool isLinearBarVerticalSlider { false };
                if (Sliders::reverbLevel.toString() == param->getParameterID())
                {
                    auto slider = std::make_unique<ParamSliderRotary> (param);
                    slider->addImage (ImageCache::getFromMemory (BinaryData::CookieSugar1_png, BinaryData::CookieSugar1_pngSize));
                    sliders[paramIdt] = std::move (slider);
                }
                // gain sliders use milk glasses
                else if (checkSliderType (Sliders::trackGainIdts, param).isValid())
                {
                    auto slider = std::make_unique<ParamSliderLinearVertical> (param);
                    if (milkAlternator++ % 2)
                    {
                        slider->setImageBg (ImageCache::getFromMemory (BinaryData::Milk1_png, BinaryData::Milk1_pngSize));
                        slider->setImageFill (milkFillOne);
                    }
                    else
                    {
                        slider->setImageBg (ImageCache::getFromMemory (BinaryData::Milk2_png, BinaryData::Milk2_pngSize));
                        slider->setImageFill (milkFillTwo);
                    }
                    sliders[paramIdt] = std::move (slider);
                }
                // octave sliders are spoons
                else if (checkSliderType (Sliders::octaveIdts, param).isValid())
                {
                    auto slider = std::make_unique<ParamSliderLinearVertical> (param);
                    slider->setImageBg (
                        ImageCache::getFromMemory (BinaryData::SpoonOutline_png, BinaryData::SpoonOutline_pngSize));
                    slider->setImageFill (ImageCache::getFromMemory (BinaryData::spoon_png, BinaryData::spoon_pngSize));
                    sliders[paramIdt] = std::move (slider);
                }
                // melodyWaveshape and glide get candy coated chips
                else if (Sliders::melodyWaveshape.toString() == param->getParameterID()
                         || Sliders::bassSlide.toString() == param->getParameterID())
                {
                    auto slider = std::make_unique<ParamSliderRotary> (param);
                    if (paramIdt == Sliders::melodyWaveshape)
                        slider->addImage (ImageCache::getFromMemory (BinaryData::CookieMM1_png, BinaryData::CookieMM1_pngSize));
                    else
                        slider->addImage (ImageCache::getFromMemory (BinaryData::CookieMM2_png, BinaryData::CookieMM2_pngSize));
                    sliders[paramIdt] = std::move (slider);
                }
                // density, step length, and root note are linearbarvertical
                else if (Sliders::stepLength.toString() == param->getParameterID()
                         || Sliders::rootNote.toString() == param->getParameterID()
                         || Sliders::density.toString() == param->getParameterID())
                {
                    isLinearBarVerticalSlider = true;
                    auto slider               = std::make_unique<nlt::APVTSControl<nlt::Slider>> (param);
                    slider->setSliderStyle (Slider::SliderStyle::LinearBarVertical);
                    slider->setVelocityBasedMode (true);
                    slider->setMouseCursor (MouseCursor::UpDownResizeCursor);
                    if (paramIdt == Sliders::rootNote)
                    {
                        slider->textFromValueFunction = [] (double value) -> String
                        { return String (value) + " - " + MidiMessage::getMidiNoteName ((int) value, true, true, 4); };
                        slider->valueFromTextFunction = [] (const String& text) -> double
                        {
                            auto s          = text.trim();
                            bool isMidiNote = s.containsOnly ("0123456789");
                            if (!isMidiNote)
                                return nlt::midiNoteNumberFromName (text);
                            return text.getDoubleValue();
                        };
                    }
                    else if (paramIdt == Sliders::density)
                    {
                        slider->textFromValueFunction = [] (double value) -> String { return String (value) + "%"; };
                    }
                    else
                    {
                        slider->textFromValueFunction = [] (double value) -> String { return String (value); };
                    }
                    sliders[paramIdt] = std::move (slider);
                }
                // ADSR get a chocolate chip
                else
                {
                    auto slider = std::make_unique<ParamSliderRotary> (param);
                    switch (cookieAlternator++ % 4)
                    {
                        case 0:
                            slider->addImage (ImageCache::getFromMemory (BinaryData::Cookie1_png, BinaryData::Cookie1_pngSize));
                            break;
                        case 1:
                            slider->addImage (ImageCache::getFromMemory (BinaryData::Cookie2_png, BinaryData::Cookie2_pngSize));
                            break;
                        case 2:
                            slider->addImage (ImageCache::getFromMemory (BinaryData::Cookie3_png, BinaryData::Cookie3_pngSize));
                            break;
                        case 3:
                            slider->addImage (ImageCache::getFromMemory (BinaryData::Cookie4_png, BinaryData::Cookie4_pngSize));
                            break;
                    }
                    // mix it up for 2nd row
                    if (cookieAlternator == 4)
                        cookieAlternator += 3;
                    sliders[paramIdt] = std::move (slider);
                }

                if (!isLinearBarVerticalSlider)
                    sliders[paramIdt]->setPopupDisplayEnabled (true, true, nullptr, 2000);
                addAndMakeVisible (*sliders[paramIdt]);

                sliderLabels[paramIdt] = std::make_unique<Label> ("slider label", param->getName (999));
                sliderLabels[paramIdt]->setJustificationType (Justification::centred);
                addAndMakeVisible (*sliderLabels[paramIdt]);
            }
        }
    }
}

void CustomAudioEditor::setupSequencers()
{
    sequenceEditActions.add (melodySequencer,
                             [this] (juce::ChangeBroadcaster* broadcaster)
                             {
                                 if (auto* sequencer = dynamic_cast<SequencerComponent*> (broadcaster))
                                 {
                                     sendSequencerValues (*sequencer, "melodySequenceIn");
                                 }
                             });
    sequenceEditActions.add (bassSequencer,
                             [this] (juce::ChangeBroadcaster* broadcaster)
                             {
                                 if (auto* sequencer = dynamic_cast<SequencerComponent*> (broadcaster))
                                 {
                                     sendSequencerValues (*sequencer, "bassSequenceIn");
                                 }
                             });
    sequenceEditActions.add (kickSequencer,
                             [this] (juce::ChangeBroadcaster* broadcaster)
                             {
                                 if (auto* sequencer = dynamic_cast<SequencerComponent*> (broadcaster))
                                 {
                                     sendSequencerValues (*sequencer, "kickSequenceIn");
                                 }
                             });
    sequenceEditActions.add (snareSequencer,
                             [this] (juce::ChangeBroadcaster* broadcaster)
                             {
                                 if (auto* sequencer = dynamic_cast<SequencerComponent*> (broadcaster))
                                     sendSequencerValues (*sequencer, "snareSequenceIn");
                             });
    sequenceEditActions.add (hatSequencer,
                             [this] (juce::ChangeBroadcaster* broadcaster)
                             {
                                 if (auto* sequencer = dynamic_cast<SequencerComponent*> (broadcaster))
                                     sendSequencerValues (*sequencer, "hatSequenceIn");
                             });

    currentStepAction.setAction (
        [this]()
        {
            seqStepIndicator.setCurrentStep (currentStep);
            //            melodySequencer.setCurrentStep (static_cast<int> (currentStep));
            //            hatSequencer.setCurrentStep (static_cast<int> (currentStep));
            //            bassSequencer.setCurrentStep (static_cast<int> (currentStep));
            //            kickSequencer.setCurrentStep (static_cast<int> (currentStep));
            //            snareSequencer.setCurrentStep (static_cast<int> (currentStep));
        },
        24.0f);
}

void CustomAudioEditor::setupToggles()
{
    RNBO::ParameterInfo parameterInfo;
    RNBO::CoreObject&   coreObject = rnboProcessor->getRnboObject();

    auto parameters = rnboProcessor->getParameters();
    for (auto& parameter: parameters)
    {
        if (auto* param = dynamic_cast<RangedAudioParameter*> (parameter))
        {
            for (auto& paramIdt: Toggles::allIdts)
            {
                if (paramIdt.toString() == param->getParameterID())
                {
                    auto set = [this, &paramIdt] (auto& toggle)
                    {
                        toggle->setClickingTogglesState (true);
                        toggle->setName (paramIdt.toString() + " toggle");
                        addAndMakeVisible (toggle.get());
                    };
                    if (paramIdt == Toggles::run)
                    {
                        runToggle = std::make_unique<ParamToggle> (param);
                        set (runToggle);
                    }
                    else if (paramIdt == Toggles::generateMelodyAlways)
                    {
                        infinityToggle = std::make_unique<ParamImageButton> (param);
                        set (infinityToggle);
                    }
                }
            }
        }
    }
    runLabel.setFont (27.0f);
    addAndMakeVisible (runLabel);

    auto imgOff = ImageCache::getFromMemory (BinaryData::MM1_Outline_png, BinaryData::MM1_Outline_pngSize);
    auto imgOn  = ImageCache::getFromMemory (BinaryData::MM1_png, BinaryData::MM1_pngSize);
    infinityToggle->setImages (false,
                               true,
                               true,
                               imgOff,
                               1.0f,
                               {},
                               imgOff,
                               1.0f,
                               Colours::transparentBlack,
                               imgOn,
                               1.0f,
                               Colours::transparentBlack);
}

void CustomAudioEditor::setupButtons()
{
    size_t             chipAlternator = 0;
    std::vector<Image> chipImages { ImageCache::getFromMemory (BinaryData::Chip1_png, BinaryData::Chip1_pngSize),
                                    ImageCache::getFromMemory (BinaryData::Chip2_png, BinaryData::Chip2_pngSize),
                                    ImageCache::getFromMemory (BinaryData::Chip3_png, BinaryData::Chip3_pngSize) };
    for (auto& b: SeqButtons::genIdts)
    {
        auto& button = seqGenButtons[b] = std::make_unique<ImageButton>();
        button->setName (b.toString());
        auto& img = chipImages[chipAlternator++ % chipImages.size()];
        button->setImages (false,
                           true,
                           true,
                           img,
                           1.0f,
                           {},
                           img,
                           1.0f,
                           Colours::black.withAlpha (0.2f),
                           img,
                           1.0f,
                           Colours::black.withAlpha (0.4f));
        button->setClickingTogglesState (false);
        button->setMouseCursor (MouseCursor::PointingHandCursor);

        String           inportTag = "generate" + b.toString();
        RNBO::MessageTag generateInport { RNBO::TAG (inportTag.toRawUTF8()) };
        button->onClick = [this, generateInport]()
        {
            static RNBO::MessageTag bang { RNBO::TAG ("") };
            _rnboObject.sendMessage (generateInport, bang);
        };
        seqGenLabels[b] = std::make_unique<Label> ("seq button", "generate " + b.toString());
        seqGenLabels[b]->setFont (16.0f);
        addAndMakeVisible (*seqGenLabels[b]);
        addAndMakeVisible (*seqGenButtons[b]);
    }
    for (auto& b: SeqButtons::clearIdts)
    {
        auto& button = seqGenButtons[b] = std::make_unique<ImageButton>();
        button->setName (b.toString());
        auto& img = chipImages[chipAlternator++ % chipImages.size()];
        button->setImages (false,
                           true,
                           true,
                           img,
                           1.0f,
                           {},
                           img,
                           1.0f,
                           Colours::black.withAlpha (0.2f),
                           img,
                           1.0f,
                           Colours::black.withAlpha (0.4f));
        button->setClickingTogglesState (false);
        button->setMouseCursor (MouseCursor::PointingHandCursor);

        String           inportTag = b.toString() + "Seq";
        RNBO::MessageTag generateInport { RNBO::TAG (inportTag.toRawUTF8()) };
        button->onClick = [this, generateInport]()
        {
            static RNBO::MessageTag bang { RNBO::TAG ("") };
            _rnboObject.sendMessage (generateInport, bang);
        };

        seqGenLabels[b] =
            std::make_unique<Label> ("seq button", "clear " + b.toString().fromFirstOccurrenceOf ("clear", false, true));
        addAndMakeVisible (*seqGenLabels[b]);
        addAndMakeVisible (*seqGenButtons[b]);
    }

    aboutPanelButton.setMouseCursor (MouseCursor::PointingHandCursor);
    aboutPanelButton.onClick = [this]() { aboutPanel.setVisible (true); };
    addAndMakeVisible (aboutPanelButton);
}

void CustomAudioEditor::setupTooltips()
{
    tooltipWindow->setMillisecondsBeforeTipAppears (1200);
    scaleBox->setTooltip ("Select scale used for generated notes");
    runToggle->setTooltip ("Make Chippo Go");
    infinityToggle->setTooltip ("Generate synth melody every sequence loop");

    sliders[Sliders::stepLength]->setTooltip ("Total length of sequence");
    sliders[Sliders::rootNote]->setTooltip ("Root note of the generated sequence");
    sliders[Sliders::density]->setTooltip ("Higher, more notes; lower, more rests");
    sliders[Sliders::reverbLevel]->setTooltip ("Reverb send amount");
    sliders[Sliders::melodyWaveshape]->setTooltip ("Adjust the waveshape of the melody");
    sliders[Sliders::bassSlide]->setTooltip ("Portamento amount for bassline");

    seqGenButtons[SeqButtons::melody]->setTooltip ("Generate a new synth melody");
    seqGenButtons[SeqButtons::bass]->setTooltip ("Generate a new bassline");
    seqGenButtons[SeqButtons::kick]->setTooltip ("Generate a new kick sequence");
    seqGenButtons[SeqButtons::snare]->setTooltip ("Generate a new snare sequence");
    seqGenButtons[SeqButtons::hat]->setTooltip ("Generate a new hihat sequence");

    seqGenButtons[SeqButtons::clearMelody]->setTooltip ("Clear synth melody sequence");
    seqGenButtons[SeqButtons::clearBass]->setTooltip ("Clear bassline sequence");
    seqGenButtons[SeqButtons::clearKick]->setTooltip ("Clear kick sequence");
    seqGenButtons[SeqButtons::clearSnare]->setTooltip ("Clear snare sequence");
    seqGenButtons[SeqButtons::clearHat]->setTooltip ("Clear hihat sequence");
}
