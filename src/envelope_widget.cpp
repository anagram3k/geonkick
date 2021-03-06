/**
 * File name: envelope_widget.cpp
 * Project: Geonkick (A kick synthesizer)
 *
 * Copyright (C) 2017 Iurie Nistor <http://geontime.com>
 *
 * This file is part of Geonkick.
 *
 * GeonKick is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "envelope_widget.h"
#include "general_envelope.h"
#include "oscillator_envelope.h"
#include "envelope_draw_area.h"
#include "geonkick_button.h"

#include <RkContainer.h>

RK_DECLARE_IMAGE_RC(show_ampl_env_active);
RK_DECLARE_IMAGE_RC(show_ampl_env);
RK_DECLARE_IMAGE_RC(show_freq_env_active);
RK_DECLARE_IMAGE_RC(show_freq_env);
RK_DECLARE_IMAGE_RC(show_filter_env);
RK_DECLARE_IMAGE_RC(show_filter_env_active);
RK_DECLARE_IMAGE_RC(layer1);
RK_DECLARE_IMAGE_RC(layer1_disabled);
RK_DECLARE_IMAGE_RC(layer2);
RK_DECLARE_IMAGE_RC(layer2_disabled);
RK_DECLARE_IMAGE_RC(layer3);
RK_DECLARE_IMAGE_RC(layer3_disabled);
RK_DECLARE_IMAGE_RC(show_osc1_envelopes_button_active);
RK_DECLARE_IMAGE_RC(show_osc1_envelopes_button);
RK_DECLARE_IMAGE_RC(show_osc2_envelopes_button_active);
RK_DECLARE_IMAGE_RC(show_osc2_envelopes_button);
RK_DECLARE_IMAGE_RC(show_noise_envelopes_button_active);
RK_DECLARE_IMAGE_RC(show_noise_envelopes_button);
RK_DECLARE_IMAGE_RC(show_general_envelopes_button_active);
RK_DECLARE_IMAGE_RC(show_general_envelopes_button);

EnvelopeWidget::EnvelopeWidget(GeonkickWidget *parent,
                               GeonkickApi *api,
                               const std::vector<std::unique_ptr<Oscillator>> &oscillators)
          : GeonkickWidget(parent)
          , drawArea{nullptr}
          , showAmplitudeEnvButton{nullptr}
          , showFrequencyEnvButton{nullptr}
	  , showFilterEnvButton{nullptr}
          , osccillator1EvelopesButton{nullptr}
          , osccillator2EvelopesButton{nullptr}
          , noiseEvelopesButton{nullptr}
          , generalEvelopesButton{nullptr}
          , layer1Button{nullptr}
          , layer2Button{nullptr}
          , layer3Button{nullptr}
          , geonkickApi{api}
{
        // Create drawing area.
        drawArea = new EnvelopeWidgetDrawingArea(this, geonkickApi);
        drawArea->show();

        auto rect = drawArea->getDrawingArea();

        // Oscillator1 envelope
        auto oscillator = oscillators[static_cast<int>(Oscillator::Type::Oscillator1)].get();
        auto envelope = std::dynamic_pointer_cast<Envelope>(std::make_shared<OscillatorEnvelope>(oscillator, rect));
        envelopes.insert({static_cast<int>(Envelope::Category::Oscillator1), envelope});
        envelope->setCategory(Envelope::Category::Oscillator1);

        // Oscillator2 envelope
        oscillator = oscillators[static_cast<int>(Oscillator::Type::Oscillator2)].get();
        envelope = std::dynamic_pointer_cast<Envelope>(std::make_shared<OscillatorEnvelope>(oscillator, rect));
        envelopes.insert({static_cast<int>(Envelope::Category::Oscillator2), envelope});
        envelope->setCategory(Envelope::Category::Oscillator2);

        // Noise envelope
        oscillator = oscillators[static_cast<int>(Oscillator::Type::Noise)].get();
        envelope = std::dynamic_pointer_cast<Envelope>(std::make_shared<OscillatorEnvelope>(oscillator, rect));
        envelopes.insert({static_cast<int>(Envelope::Category::Noise), envelope});
        envelope->setCategory(Envelope::Category::Noise);

        // General nevelope
        envelope = std::dynamic_pointer_cast<Envelope>(std::make_shared<GeneralEnvelope>(geonkickApi, rect));
        envelopes.insert({static_cast<int>(Envelope::Category::General), envelope});
        envelope->setCategory(Envelope::Category::General);
        createButtomMenu();
        showEnvelope(Envelope::Category::General);
}

void EnvelopeWidget::createButtomMenu()
{
        auto buttomAreaWidget = new GeonkickWidget(this);
        buttomAreaWidget->setPosition(drawArea->x(), drawArea->y() + drawArea->height());
        buttomAreaWidget->setFixedSize(drawArea->width(), 34);
        buttomAreaWidget->show();

        // Amplitude Envelope button
        showAmplitudeEnvButton = new GeonkickButton(buttomAreaWidget);
        RK_ACT_BIND(showAmplitudeEnvButton,
                    toggled,
                    RK_ACT_ARGS(bool pressed),
                    this,
                    showEnvelopeType(Envelope::Type::Amplitude));
        showAmplitudeEnvButton->setPressedImage(RkImage(77, 30, RK_IMAGE_RC(show_ampl_env_active)));
        showAmplitudeEnvButton->setUnpressedImage(RkImage(77, 30, RK_IMAGE_RC(show_ampl_env)));
        showAmplitudeEnvButton->show();
        showAmplitudeEnvButton->setPressed(true);

        // Frequency Envelope button
        showFrequencyEnvButton = new GeonkickButton(buttomAreaWidget);
        RK_ACT_BIND(showFrequencyEnvButton,
                    toggled,
                    RK_ACT_ARGS(bool pressed),
                    this,
                    showEnvelopeType(Envelope::Type::Frequency));
        showFrequencyEnvButton->setPressedImage(RkImage(77, 30, RK_IMAGE_RC(show_freq_env_active)));
        showFrequencyEnvButton->setUnpressedImage(RkImage(77, 30, RK_IMAGE_RC(show_freq_env)));
        showFrequencyEnvButton->show();

        // Filter Envelope.
        showFilterEnvButton = new GeonkickButton(buttomAreaWidget);
        RK_ACT_BIND(showFilterEnvButton,
                    toggled,
                    RK_ACT_ARGS(bool pressed),
                    this,
                    showEnvelopeType(Envelope::Type::FilterCutOff));
        showFilterEnvButton->setPressedImage(RkImage(77, 30, RK_IMAGE_RC(show_filter_env_active)));
        showFilterEnvButton->setUnpressedImage(RkImage(77, 30, RK_IMAGE_RC(show_filter_env)));
        showFilterEnvButton->show();

        // General envelope button
        generalEvelopesButton = new GeonkickButton(buttomAreaWidget);
        generalEvelopesButton->setPressed(true);
        RK_ACT_BIND(generalEvelopesButton,
                    toggled,
                    RK_ACT_ARGS(bool pressed),
                    this,
                    showEnvelope(Envelope::Category::General));
        generalEvelopesButton->setPressedImage(RkImage(90, 30, RK_IMAGE_RC(show_general_envelopes_button_active)));
        generalEvelopesButton->setUnpressedImage(RkImage(90, 30, RK_IMAGE_RC(show_general_envelopes_button)));
        generalEvelopesButton->show();

        // Noise envelope button
        noiseEvelopesButton = new GeonkickButton(buttomAreaWidget);
        RK_ACT_BIND(noiseEvelopesButton,
                    toggled,
                    RK_ACT_ARGS(bool pressed),
                    this,
                    showEnvelope(Envelope::Category::Noise));
        noiseEvelopesButton->setPressedImage(RkImage(90, 30, RK_IMAGE_RC(show_noise_envelopes_button_active)));
        noiseEvelopesButton->setUnpressedImage(RkImage(90, 30, RK_IMAGE_RC(show_noise_envelopes_button)));
        noiseEvelopesButton->show();

        // Oscillator2 envelopes button
        osccillator2EvelopesButton = new GeonkickButton(buttomAreaWidget);
        RK_ACT_BIND(osccillator2EvelopesButton,
                    toggled,
                    RK_ACT_ARGS(bool pressed),
                    this,
                    showEnvelope(Envelope::Category::Oscillator2));
        osccillator2EvelopesButton->setPressedImage(RkImage(90, 30, RK_IMAGE_RC(show_osc2_envelopes_button_active)));
        osccillator2EvelopesButton->setUnpressedImage(RkImage(90, 30, RK_IMAGE_RC(show_osc2_envelopes_button)));
        osccillator2EvelopesButton->show();

        // Oscillator1 envelopes button
        osccillator1EvelopesButton = new GeonkickButton(buttomAreaWidget);
        RK_ACT_BIND(osccillator1EvelopesButton,
                    toggled,
                    RK_ACT_ARGS(bool pressed),
                    this,
                    showEnvelope(Envelope::Category::Oscillator1));
        osccillator1EvelopesButton->setPressedImage(RkImage(90, 30, RK_IMAGE_RC(show_osc1_envelopes_button_active)));
        osccillator1EvelopesButton->setUnpressedImage(RkImage(90, 30, RK_IMAGE_RC(show_osc1_envelopes_button)));
        osccillator1EvelopesButton->show();
        createLayersButtons(buttomAreaWidget);

	menuContainer = new RkContainer(buttomAreaWidget);
	menuContainer->addWidget(showAmplitudeEnvButton);
	menuContainer->addWidget(showFrequencyEnvButton);
	menuContainer->addWidget(showFilterEnvButton);
	menuContainer->addWidget(generalEvelopesButton, Rk::Alignment::AlignRight);
	menuContainer->addWidget(noiseEvelopesButton, Rk::Alignment::AlignRight);
	menuContainer->addWidget(osccillator2EvelopesButton, Rk::Alignment::AlignRight);
	menuContainer->addWidget(osccillator1EvelopesButton, Rk::Alignment::AlignRight);
	menuContainer->addSpace(20, Rk::Alignment::AlignRight);
	menuContainer->addWidget(layer3Button, Rk::Alignment::AlignRight);
	menuContainer->addSpace(5, Rk::Alignment::AlignRight);
        menuContainer->addWidget(layer2Button, Rk::Alignment::AlignRight);
	menuContainer->addSpace(5, Rk::Alignment::AlignRight);
        menuContainer->addWidget(layer1Button, Rk::Alignment::AlignRight);
}

void EnvelopeWidget::updateKickGraph(std::shared_ptr<RkImage> graphImage)
{
        if (graphImage && !graphImage->isNull())
                drawArea->updateKickGraph(graphImage);
}

Envelope* EnvelopeWidget::getEnvelope(Envelope::Category category)
{
        auto res = envelopes.find(static_cast<int>(category));
        if (res != envelopes.end())
                return res->second.get();
        return nullptr;
}

void EnvelopeWidget::showEnvelope(Envelope::Category category)
{
        generalEvelopesButton->setPressed(category == Envelope::Category::General);
        osccillator1EvelopesButton->setPressed(category == Envelope::Category::Oscillator1);
        osccillator2EvelopesButton->setPressed(category == Envelope::Category::Oscillator2);
        noiseEvelopesButton->setPressed(category == Envelope::Category::Noise);
        auto envelope = getEnvelope(category);
        if (envelope) {
                drawArea->setEnvelope(envelope);
                showEnvelopeType(envelope->type());
        }

}

void EnvelopeWidget::showEnvelopeType(Envelope::Type type)
{
        if (type == Envelope::Type::DistortionVolume
            || type == Envelope::Type::DistortionDrive) {
                auto envelope = getEnvelope(Envelope::Category::General);
                generalEvelopesButton->setPressed(true);
                if (envelope)
                        drawArea->setEnvelope(envelope);
        }

        showAmplitudeEnvButton->setPressed(type == Envelope::Type::Amplitude);
        showFilterEnvButton->setPressed(type == Envelope::Type::FilterCutOff);
        showFrequencyEnvButton->setPressed(type == Envelope::Type::Frequency);
        auto envelope = drawArea->getEnvelope();
	showFrequencyEnvButton->show(envelope->isSupportedType(Envelope::Type::Frequency));
	menuContainer->update();
        if (envelope)
                envelope->setType(type);
        drawArea->update();
        action envelopeTypeSelected(type);
}

void EnvelopeWidget::hideEnvelope(bool b)
{
        drawArea->setHideEnvelope(b);
}

void EnvelopeWidget::createLayersButtons(GeonkickWidget *buttomAreaWidget)
{
        layer1Button = new GeonkickButton(buttomAreaWidget);
        layer1Button->setSize(24, 24);
        layer1Button->setBackgroundColor(buttomAreaWidget->background());
        layer1Button->setUnpressedImage(RkImage(layer1Button->size(), RK_IMAGE_RC(layer1_disabled)));
        layer1Button->setPressedImage(RkImage(layer1Button->size(), RK_IMAGE_RC(layer1)));
        layer1Button->setCheckable(true);
        layer1Button->setPressed(true);
        RK_ACT_BIND(layer1Button, toggled, RK_ACT_ARGS(bool b),
                    this, setLayer(GeonkickApi::Layer::Layer1));

        layer2Button = new GeonkickButton(buttomAreaWidget);
        layer2Button->setSize(24, 24);
        layer2Button->setBackgroundColor(buttomAreaWidget->background());
        layer2Button->setUnpressedImage(RkImage(layer2Button->size(), RK_IMAGE_RC(layer2_disabled)));
        layer2Button->setPressedImage(RkImage(layer2Button->size(), RK_IMAGE_RC(layer2)));
        layer2Button->setCheckable(true);
        RK_ACT_BIND(layer2Button, toggled, RK_ACT_ARGS(bool b),
                    this, setLayer(GeonkickApi::Layer::Layer2));

        layer3Button = new GeonkickButton(buttomAreaWidget);
        layer3Button->setBackgroundColor(buttomAreaWidget->background());
        layer3Button->setSize(24, 24);
        layer3Button->setUnpressedImage(RkImage(layer3Button->size(), RK_IMAGE_RC(layer3_disabled)));
        layer3Button->setPressedImage(RkImage(layer3Button->size(), RK_IMAGE_RC(layer3)));
        layer3Button->setCheckable(true);
        RK_ACT_BIND(layer3Button, toggled, RK_ACT_ARGS(bool b),
                    this, setLayer(GeonkickApi::Layer::Layer3));
}

void EnvelopeWidget::setLayer(GeonkickApi::Layer layer)
{
        layer1Button->setPressed(GeonkickApi::Layer::Layer1 == layer);
        layer2Button->setPressed(GeonkickApi::Layer::Layer2 == layer);
        layer3Button->setPressed(GeonkickApi::Layer::Layer3 == layer);
        geonkickApi->setLayer(layer);
        action requestUpdateGui();
}

void EnvelopeWidget::updateGui()
{
        for (const auto &envelope: envelopes) {
                if (envelope.second->isSupportedType(Envelope::Type::Amplitude))
                        envelope.second->updatePoints();
                if (envelope.second->isSupportedType(Envelope::Type::Frequency))
                        envelope.second->updatePoints();
		if (envelope.second->isSupportedType(Envelope::Type::FilterCutOff))
                        envelope.second->updatePoints();
        }
        drawArea->update();
}
