#include "params.hpp"

static struct envfollower_param_def envfollower_params[envfollower_param_id::PARAM_COUNT] = {
    { "CTL_IN", "Input Level", 0.0, 0.0, 1.0, PARAM_OUTPUT },
    { "CTL_OUT", "Output Level", 0.0, 0.0, 1.0, PARAM_OUTPUT },
    { "PEAKRMS", "Peak/RMS", 0.0, 0.0, 1.0, PARAM_FLOAT },
    { "THRESHOLD", "Threshold", 0.0, 0.0, 1.0, PARAM_FLOAT },
    { "SATURATION", "Saturation", 1.0, 0.0, 2.0, PARAM_FLOAT },
    { "ATIME", "Attack Time", 0.01, 0.001, 15.0, PARAM_FLOAT },
    { "DTIME", "Decay Time", 1.0, 0.001, 30.0, PARAM_FLOAT },
    { "CHANNEL", "MIDI Channel", 1, 1, 16, PARAM_INT },
    { "CONTROL_NO", "MIDI Control Number", 0, 0, 127, PARAM_INT },
    { "MMINV", "MIDI Minimum Value", 0, 0, 126, PARAM_INT },
    { "MMAXV", "MIDI Maximum Value", 127, 1, 127, PARAM_INT },
    { "MDIRECTION", "MIDI Direction", 0, 0, 1, PARAM_ENUM }, // Originally use lv2:scalePoint, here I use enum instead
    { "CMINV", "CV Port Minimum Value", 0.0, 0.0, 1.0, PARAM_FLOAT },
    { "CMAXV", "CV Port Maximum Value", 1.0, 0.0, 1.0, PARAM_FLOAT }
};

const char* envfollower_param::paramName(envfollower_param_id index)
{
    return envfollower_params[index].name;
}

const char* envfollower_param::paramSymbol(envfollower_param_id index)
{
    return envfollower_params[index].symbol;
}

float envfollower_param::paramDefaultValue(envfollower_param_id index)
{
    return envfollower_params[index].def;
}

float envfollower_param::paramMinValue(envfollower_param_id index)
{
    return envfollower_params[index].min;
}

float envfollower_param::paramMaxValue(envfollower_param_id index)
{
    return envfollower_params[index].max;
}

bool envfollower_param::paramFloat(envfollower_param_id index)
{
    return (envfollower_params[index].type == PARAM_FLOAT);
}

bool envfollower_param::paramEnum(envfollower_param_id index)
{
    return (envfollower_params[index].type == PARAM_ENUM);
}

bool envfollower_param::paramInt(envfollower_param_id index)
{
    return (envfollower_params[index].type == PARAM_INT);
}

bool envfollower_param::paramOutput(envfollower_param_id index)
{
    return (envfollower_params[index].type == PARAM_OUTPUT);
}
