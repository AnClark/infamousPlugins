#pragma once

#include "note.hpp"
#include "params.hpp"
#include "waves.hpp"

// Forward decls.
namespace DISTRHO {
struct MidiEvent;
}
class casynthPlugin;

class casynth_engine {
private:
    double     sample_rate;
    WAVESOURCE waves;
    NOTE       note[127];
    uint8_t    active[127];
    uint8_t    nactive;
    bool       sus;
    uint8_t    sustained[127];
    uint8_t    nsustained;
    float      ibpm;

    double pitchbend;

    uint8_t harmonic_mode;
    uint8_t ncells;
    float   cell_lifetime;
    float   harm_gain_sinc[MAX_N_HARMONICS + 1];
    float   harm_gain_saw[MAX_N_HARMONICS + 1];
    float   harm_gain_sqr[MAX_N_HARMONICS + 1];
    float   harm_gain_tri[MAX_N_HARMONICS + 1];
    float*  harm_gains; // points to appropriate array

    float envelope[6];

    float amod_g;
    float fmod_g;

    float params[PARAM_COUNT];

    friend class casynthPlugin;

public:
    casynth_engine(double sample_rate);
    ~casynth_engine();

    void process(const float* input_mono, float* output_mono, uint32_t nframes, const DISTRHO::MidiEvent* midiEvents, uint32_t midiEventCount);
};
