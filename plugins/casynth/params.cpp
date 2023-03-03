#include "params.hpp"

#include "DistrhoPluginInfo.h"

static struct casynth_param_def casynth_params[casynth_param_id::PARAM_COUNT] = {
    { "CHANNEL", "MIDI Channel", 0, 0, 16, PARAM_INT },
    { "MASTER_GAIN", "Master Gain", 1.0, 0.001, 2.0, PARAM_FLOAT },
    { "RULE", "Cell Automaton Rule", 30, 0, 255, PARAM_INT },
    { "CELL_LIFE", "Cell Lifetime", 0.25, 0.001, 10.0, PARAM_FLOAT },
    { "INIT_CELLS", "Initial Condition", 1, 0, 65535, PARAM_INT },
    { "NHARMONICS", "Harmonics", 12, 0, 16, PARAM_INT },
    { "HARM_MODE", "Harmonic Gain Mode", 1, 0, 3, PARAM_ENUM },
    { "HARM_WIDTH", "Harmonic Width", 0.0, 0.000, 50.0, PARAM_FLOAT },
    { "WAVE", "Waveform", 0, 0, 5, PARAM_ENUM },
    { "ENV_A", "Attack", 0.10, 0.001, 15.0, PARAM_FLOAT },
    { "ENV_D", "Decay", 0.10, 0.001, 15.0, PARAM_FLOAT },
    { "ENV_B", "Break Point", 0.5, 0.0, 1.0, PARAM_FLOAT },
    { "ENV_SWL", "Swell", 3.0, 0.001, 15.0, PARAM_FLOAT },
    { "ENV_SUS", "Sustain", 0.75, 0.0, 1.0, PARAM_FLOAT },
    { "ENV_R", "Release", 0.1, 0.001, 15.0, PARAM_FLOAT },
    { "AMOD_WAV", "AM Waveform", 0, 0, 5, PARAM_ENUM },
    { "AMOD_FREQ", "AM Frequency", 0.0, 0.0, 110.0, PARAM_FLOAT },
    { "AMOD_GAIN", "AM Gain", 0.10, -1.00, 1.00, PARAM_FLOAT },
    { "FMOD_WAV", "FM Waveform", 0, 0, 5, PARAM_ENUM },
    { "FMOD_FREQ", "FM Frequency", 0.0, 0.0, 110.0, PARAM_FLOAT },
    { "FMOD_GAIN", "FM Gain", 0.10, -4.00, 4.00, PARAM_FLOAT }
};

const char* casynth_param::paramName(casynth_param_id index)
{
    return casynth_params[index].name;
}

const char* casynth_param::paramSymbol(casynth_param_id index)
{
    return casynth_params[index].symbol;
}

float casynth_param::paramDefaultValue(casynth_param_id index)
{
    return casynth_params[index].def;
}

float casynth_param::paramMinValue(casynth_param_id index)
{
    return casynth_params[index].min;
}

float casynth_param::paramMaxValue(casynth_param_id index)
{
    return casynth_params[index].max;
}

bool casynth_param::paramFloat(casynth_param_id index)
{
    return (casynth_params[index].type == PARAM_FLOAT);
}

bool casynth_param::paramEnum(casynth_param_id index)
{
    return (casynth_params[index].type == PARAM_ENUM);
}

bool casynth_param::paramInt(casynth_param_id index)
{
    return (casynth_params[index].type == PARAM_INT);
}
