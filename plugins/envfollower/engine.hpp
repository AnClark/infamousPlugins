#pragma once

#include "params.hpp"

extern "C" {
#include "rms_calc.h"
}

// Forward decls.
namespace DISTRHO {
struct MidiEvent;
}
class EnvFollowerPlugin;

class envfollower_engine {
private:
    float sample_time;

    float current;
    float prev;
    uint8_t mout;
    uint8_t mprev;

    double sum;
    float out;

    float atime;
    float up[3];
    float dtime;
    float dn[3];

    RMS_CALC rms_calc;

    float params[PARAM_COUNT];

    friend class EnvFollowerPlugin;
    EnvFollowerPlugin *plugin_instance; // For writing MIDI output data

public:
    envfollower_engine(double sample_rate, EnvFollowerPlugin *plugin_instance);
    ~envfollower_engine();

    void process(const float** inputs, float** outputs, uint32_t nframes);
};
