#include "envfollower.hpp"

#include "params.hpp"

EnvFollowerPlugin::EnvFollowerPlugin()
    : DISTRHO::Plugin(envfollower_param_id::PARAM_COUNT, 0, 1) // 21 params, 0 program, 1 state
{
    fSampleRate  = getSampleRate();
    fProcessor = std::make_unique<envfollower_engine>(double(fSampleRate), this);
}

EnvFollowerPlugin::~EnvFollowerPlugin()
{
}

void EnvFollowerPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    parameter.hints = kParameterIsAutomatable;

    parameter.name       = envfollower_param::paramName(envfollower_param_id(index));
    parameter.shortName  = envfollower_param::paramSymbol(envfollower_param_id(index));
    parameter.symbol     = envfollower_param::paramSymbol(envfollower_param_id(index));
    parameter.ranges.def = envfollower_param::paramDefaultValue(envfollower_param_id(index));
    parameter.ranges.min = envfollower_param::paramMinValue(envfollower_param_id(index));
    parameter.ranges.max = envfollower_param::paramMaxValue(envfollower_param_id(index));

    if (envfollower_param::paramInt(envfollower_param_id(index)) || envfollower_param::paramEnum(envfollower_param_id(index)))
        parameter.hints |= kParameterIsInteger;

    // Process output params
    if (envfollower_param::paramOutput(envfollower_param_id(index)))
        parameter.hints |= kParameterIsOutput;

    // Process enum params
    switch (index) {

    }

    // Apply default values
    setParameterValue(index, parameter.ranges.def);
}

void EnvFollowerPlugin::initState(uint32_t index, State& state)
{
    // Assign a dummy state.
    state.key          = "Default";
    state.defaultValue = "";
}

float EnvFollowerPlugin::getParameterValue(uint32_t index) const
{
    if (fProcessor == nullptr)
        return 0.0f;

    return fProcessor->params[index];
}

void EnvFollowerPlugin::setParameterValue(uint32_t index, float value)
{
    if (fProcessor == nullptr)
        return;

    fProcessor->params[index] = value;
}

void EnvFollowerPlugin::setState(const char* key, const char* value)
{
}

void EnvFollowerPlugin::activate() { }

void EnvFollowerPlugin::run(const float** inputs, float** outputs, uint32_t frames)
//void EnvFollowerPlugin::run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{
    if (fProcessor == nullptr)
        return;

    fProcessor->process(inputs, outputs, frames);
}

void EnvFollowerPlugin::sampleRateChanged(double newSampleRate)
{
    this->fSampleRate = newSampleRate;

    // Re-init synth engine to match new sample rate
    fProcessor.reset();
    fProcessor = std::make_unique<envfollower_engine>(double(fSampleRate), this);
}

START_NAMESPACE_DISTRHO

Plugin* createPlugin()
{
    return new EnvFollowerPlugin();
}

END_NAMESPACE_DISTRHO