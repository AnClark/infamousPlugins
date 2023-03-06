#include "engine.hpp"
#include "constants.h"
#include "envfollower.hpp"

envfollower_engine::envfollower_engine(double sample_rate, EnvFollowerPlugin* plugin_instance)
{
    envfollower_engine* plug = this;
    plug->plugin_instance    = plugin_instance;

    plug->sample_time = 1 / sample_rate;

    plug->current = 0;
    plug->prev    = 0;
    plug->mout    = 0;
    plug->mprev   = 0;
    plug->out     = 0;

    plug->atime = .1;
    float tmp   = 2.2 * plug->sample_time;
    float den   = 2 * plug->atime + tmp;
    plug->up[0] = (2 - plug->sample_time) * plug->atime / den;
    plug->up[1] = tmp / den;
    plug->up[2] = plug->sample_time * plug->atime / den;

    plug->dtime = .1;
    den         = 2 * plug->dtime + tmp;
    plug->dn[0] = (2 - plug->sample_time) * plug->dtime / den;
    plug->dn[1] = tmp / den;
    plug->dn[2] = plug->sample_time * plug->dtime / den;

    rms_init(&plug->rms_calc, 64);
}

envfollower_engine::~envfollower_engine()
{
}

void envfollower_engine::process(const float** inputs, float** outputs, uint32_t nframes)
{
    envfollower_engine* plug = this;
    // LV2_Atom midiatom;
    uint32_t     i;
    const float* buf = inputs[0]; // DPF audio input is read-only
    MidiEvent    ev_out;
    float        peak;
    float        rms;
    float        sat = params[PARAM_SATURATION];
    float        max = params[PARAM_MMAXV];
    if (sat <= params[PARAM_THRESHOLD]) {
        max = params[PARAM_MMINV];
    }
    if (max < params[PARAM_MMINV]) {
        max = params[PARAM_MMINV];
    }
    float mapping = (max - params[PARAM_MMINV]) / (sat - params[PARAM_THRESHOLD]);

    params[PARAM_CTL_IN]  = 0;
    params[PARAM_CTL_OUT] = 0;

    // prepare midi output
    ev_out.size = ev_out.kDataSize;
    ev_out.data[0] = 0;
    ev_out.data[1] = 0;
    ev_out.data[2] = 0;

    // recalculate filter coefficients as necessary
    if (plug->atime != params[PARAM_ATIME]) {
        plug->atime = params[PARAM_ATIME];
        float tmp   = 2.2 * plug->sample_time;
        float den   = 2.0 * plug->atime + tmp;
        plug->up[0] = (2.0 - plug->sample_time) * plug->atime / den;
        plug->up[1] = tmp / den;
        plug->up[2] = plug->sample_time * plug->atime / den;
    }
    if (plug->dtime != params[PARAM_DTIME]) {
        plug->dtime = params[PARAM_DTIME];
        float tmp   = 2.2 * plug->sample_time;
        float den   = 2.0 * plug->dtime + tmp;
        plug->dn[0] = (2.0 - plug->sample_time) * plug->dtime / den;
        plug->dn[1] = tmp / den;
        plug->dn[2] = plug->sample_time * plug->dtime / den;
    }

    // process data
    for (i = 0; i < nframes; i++) {
        // get values
        peak = buf[i] > 0 ? buf[i] : -buf[i];
        rms  = rms_shift(&plug->rms_calc, buf[i]);

        plug->prev    = plug->current;
        plug->current = (1.0 - params[PARAM_PEAKRMS]) * peak + params[PARAM_PEAKRMS] * rms;

        if (plug->current >= plug->out) {
            plug->out = plug->up[0] * plug->out + plug->up[1] * plug->current + plug->up[2] * plug->prev;
        } else {
            plug->out = plug->dn[0] * plug->out + plug->dn[1] * plug->current + plug->dn[2] * plug->prev;
        }
        params[PARAM_CTL_IN] += plug->current;

        // now map to the midi values
        if (plug->out <= params[PARAM_THRESHOLD]) {
            plug->mout = params[PARAM_MMINV];
        } else if (plug->out >= params[PARAM_SATURATION]) {
            plug->mout = params[PARAM_MMAXV];
        } else {
            plug->mout = mapping * (plug->out - params[PARAM_THRESHOLD]) + params[PARAM_MMINV];
        }

        if (params[PARAM_MDIRECTION]) { // aka. *plug->rev_p (REVERSE)
            plug->mout = params[PARAM_MMAXV] - plug->mout + params[PARAM_MMINV];
        }

        if (plug->mout != plug->mprev) {
            // make event
            ev_out.data[0] = MIDI_CONTROL_CHANGE + (uint8_t)(params[PARAM_CHANNEL] - 1);
            ev_out.data[1] = MIDI_DATA_MASK & (uint8_t)params[PARAM_CONTROL_NO];
            ev_out.data[2] = MIDI_DATA_MASK & plug->mout;

            plugin_instance->writeMidiEvent(ev_out);
        }
        plug->mprev = plug->mout;
        params[PARAM_CTL_OUT] += plug->mout / 127.0;

        // finally copy the intput to the output
        outputs[0][i] = buf[i];
    }

    // do average for output controls
    params[PARAM_CTL_IN] /= nframes;
    params[PARAM_CTL_OUT] /= nframes;
}
