#ifndef CASYNTH_HPP
#define CASYNTH_HPP

#include "DistrhoPlugin.hpp"
#include "engine.hpp"

#include <memory>


class casynthPlugin : public DISTRHO::Plugin {
    std::unique_ptr<casynth_engine> fSynthesizer;
    float fSampleRate;

public:
    casynthPlugin();
    ~casynthPlugin();

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // Information

    /**
        Get the plugin label.@n
        This label is a short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    */
    const char* getLabel() const noexcept override
    {
        return DISTRHO_PLUGIN_NAME;
    }

    /**
           Get an extensive comment/description about the plugin.@n
           Optional, returns nothing by default.
         */
    const char* getDescription() const override
    {
        return "Additive synth controlled by an automaton";
    }

    /**
           Get the plugin author/maker.
         */
    const char* getMaker() const noexcept override
    {
        return DISTRHO_PLUGIN_BRAND;
    }

    /**
           Get the plugin license (a single line of text or a URL).@n
           For commercial plugins this should return some short copyright information.
         */
    const char* getLicense() const noexcept override
    {
        return "GPLv3";
    }

    /**
        Get the plugin version, in hexadecimal.
        @see d_version()
        */
    uint32_t getVersion() const noexcept override
    {
        return d_version(0, 1, 0);
    }

    /**
           Get the plugin unique Id.@n
           This value is used by LADSPA, DSSI and VST plugin formats.
           @see d_cconst()
         */
    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('i', 'f', 'c', 'a');
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;

    // ----------------------------------------------------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void  setParameterValue(uint32_t index, float value) override;

    // ----------------------------------------------------------------------------------------------------------------
    // Audio/MIDI Processing

    void activate() override;
    void run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount) override;

    // ----------------------------------------------------------------------------------------------------------------
    // Callbacks (optional)

    // void bufferSizeChanged(int newBufferSize);
    void sampleRateChanged(double newSampleRate) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(casynthPlugin)
};


#endif
