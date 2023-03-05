#include "casynth.hpp"

casynthPlugin::casynthPlugin()
    : DISTRHO::Plugin(casynth_param_id::PARAM_COUNT, 0, 1) // 21 params, 0 program, 1 state
{
    fSampleRate  = getSampleRate();
    fSynthesizer = std::make_unique<casynth_engine>(double(fSampleRate));
}

casynthPlugin::~casynthPlugin()
{
}

void casynthPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    parameter.hints = kParameterIsAutomatable;

    parameter.name       = casynth_param::paramName(casynth_param_id(index));
    parameter.shortName  = casynth_param::paramSymbol(casynth_param_id(index));
    parameter.symbol     = casynth_param::paramSymbol(casynth_param_id(index));
    parameter.ranges.def = casynth_param::paramDefaultValue(casynth_param_id(index));
    parameter.ranges.min = casynth_param::paramMinValue(casynth_param_id(index));
    parameter.ranges.max = casynth_param::paramMaxValue(casynth_param_id(index));

    if (casynth_param::paramInt(casynth_param_id(index)) || casynth_param::paramEnum(casynth_param_id(index)))
        parameter.hints |= kParameterIsInteger;

    // Process enum params
    switch (index) {
        case PARAM_HARM_MODE: {
            parameter.enumValues.count          = 4;
            parameter.enumValues.restrictedMode = true;
            parameter.enumValues.values         = new ParameterEnumerationValue[4];
            {
                parameter.enumValues.values[0].label = "Sine";
                parameter.enumValues.values[0].value = 0;

                parameter.enumValues.values[1].label = "Saw";
                parameter.enumValues.values[1].value = 1;

                parameter.enumValues.values[2].label = "Square";
                parameter.enumValues.values[2].value = 2;

                parameter.enumValues.values[3].label = "Triangle";
                parameter.enumValues.values[3].value = 3;
            }
            break;
        }
        case PARAM_WAVE:
        case PARAM_AMOD_WAV:
        case PARAM_FMOD_WAV: {
            parameter.enumValues.count          = 6;
            parameter.enumValues.restrictedMode = true;
            parameter.enumValues.values         = new ParameterEnumerationValue[6];
            {
                parameter.enumValues.values[0].label = "Sine";
                parameter.enumValues.values[0].value = 0;

                parameter.enumValues.values[1].label = "Saw";
                parameter.enumValues.values[1].value = 1;

                parameter.enumValues.values[2].label = "Square";
                parameter.enumValues.values[2].value = 2;

                parameter.enumValues.values[3].label = "Triangle";
                parameter.enumValues.values[3].value = 3;

                parameter.enumValues.values[4].label = "White Noise";
                parameter.enumValues.values[4].value = 4;

                parameter.enumValues.values[5].label = "Random";
                parameter.enumValues.values[5].value = 5;
            }
            break;
        }
    }

    // Apply default values
    setParameterValue(index, parameter.ranges.def);
}

void casynthPlugin::initState(uint32_t index, State& state)
{
    // Assign a dummy state.
    state.key          = "Default";
    state.defaultValue = "";
}

float casynthPlugin::getParameterValue(uint32_t index) const
{
    if (fSynthesizer == nullptr)
        return 0.0f;

    return fSynthesizer->params[index];
}

void casynthPlugin::setParameterValue(uint32_t index, float value)
{
    if (fSynthesizer == nullptr)
        return;

    fSynthesizer->params[index] = value;
}

void casynthPlugin::setState(const char* key, const char* value)
{
    if (fSynthesizer == nullptr)
        return;

    // The only usage of state is: to switch off all sounds when loading a preset from DAW.
    fSynthesizer->panic();
}

void casynthPlugin::activate() { }

void casynthPlugin::run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{
    if (fSynthesizer == nullptr)
        return;

    fSynthesizer->process(inputs[0], outputs[0], frames, midiEvents, midiEventCount);
}

void casynthPlugin::sampleRateChanged(double newSampleRate)
{
    this->fSampleRate = newSampleRate;

    // Re-init synth engine to match new sample rate
    fSynthesizer.reset();
    fSynthesizer = std::make_unique<casynth_engine>(double(fSampleRate));
}

START_NAMESPACE_DISTRHO

Plugin* createPlugin()
{
    return new casynthPlugin();
}

END_NAMESPACE_DISTRHO