#include "engine.hpp"

#include "DistrhoPlugin.hpp"

casynth_engine::casynth_engine(double sample_rate)
{
    uint8_t i;

    this->sample_rate = sample_rate; // Watch out here, simply using `sample_rate = sample_rate` is in vain!

    init_waves(&(waves));

    nactive    = 0;
    nsustained = 0;
    pitchbend  = 1;
    ibpm       = .5; // 60/120
    for (i = 0; i < 127; i++) {
        init_note(&(note[i]),
            &(waves),
            sample_rate,
            i,
            &(ncells),
            &(cell_lifetime),
            &(amod_g),
            &(fmod_g));
        active[i]    = 0;
        sustained[i] = 0;
    }

    harmonic_mode = HARMONIC_MODE_SINC;
    for (i = 0; i < MAX_N_HARMONICS; i++) {
        harm_gain_sinc[i] = 1 / (float)(MAX_N_HARMONICS + 1); //(nharmonics+1);
        harm_gain_saw[i]  = .29 / (float)(i + 2); //.29 makes it so gain=1 if all harmonics play
        harm_gain_sqr[i]  = (i % 2 != 0) * .48 / (float)(i + 2); // odd harmonics
        harm_gain_tri[i]  = (i % 2 != 0) * .83 / (float)((i + 2) * (i + 2));
    }
    // fundamental
    harm_gain_sinc[i] = 1 / (float)(MAX_N_HARMONICS + 1); //(nharmonics+1);
    harm_gain_saw[i]  = .29; //.29 makes it so gain=1 if all harmonics play
    harm_gain_sqr[i]  = .48;
    harm_gain_tri[i]  = .83;

    harm_gains = harm_gain_sinc;
}

casynth_engine::~casynth_engine()
{
}

void casynth_engine::process(const float* input_mono, float* output_mono, uint32_t nframes, const DISTRHO::MidiEvent* midiEvents, uint32_t midiEventCount)
{
    casynth_engine* synth = this;
    uint8_t         i, j, k;
    float*          buf      = output_mono;
    uint32_t        frame_no = 0;
    uint32_t        t        = 0;
    uint8_t*        message;
    uint8_t         type;
    uint8_t         num, val;
    int16_t         bend;
    bool            firstnote = true;
    NOTE*           note;
    double          astep = synth->waves.func_domain * (synth->params[PARAM_AMOD_FREQ]) / synth->sample_rate;
    double          fstep = synth->waves.func_domain * (synth->params[PARAM_FMOD_FREQ]) / synth->sample_rate; // need to decide where to calculate this. Probably not here.

    synth->ncells        = synth->params[PARAM_NHARMONICS];
    synth->cell_lifetime = synth->sample_rate * (synth->params[PARAM_CELL_LIFE]) * synth->ibpm;
    synth->amod_g        = synth->params[PARAM_AMOD_GAIN];
    synth->fmod_g        = synth->params[PARAM_FMOD_GAIN];

    for (t = 0; t < nframes; t++) // start by filling buffer with 0s, we'll add to this
    {
        buf[t] = 0;
    }
    t = 0;

    for (uint32_t event_index = 0; event_index < midiEventCount; event_index++) {
        const MidiEvent& event = midiEvents[event_index];

        message    = (uint8_t*)calloc(sizeof(uint8_t), midiEvents->kDataSize);
        message[0] = event.data[0];
        message[1] = event.data[1];
        message[2] = event.data[2];
        message[3] = event.data[3];

        if (!(params[PARAM_CHANNEL]) || ((message[0] & MIDI_CHANNEL_MASK) == (params[PARAM_CHANNEL]) + 1)) {
            type = message[0] & MIDI_TYPE_MASK;
            t    = event.frame;

            if (type == MIDI_NOTE_ON) {
                num = message[1] & MIDI_DATA_MASK;
                val = message[2] & MIDI_DATA_MASK;
                if (firstnote) // only calculate these if there is a note in this period
                {
                    firstnote = false;
                    // condition envelope
                    synth->envelope[ENV_ATTACK]  = 1 / (float)(params[PARAM_ENV_A] * synth->sample_rate);
                    synth->envelope[ENV_DECAY]   = (params[PARAM_ENV_B] * .9999 - 1) / (float)(params[PARAM_ENV_D] * synth->sample_rate);
                    synth->envelope[ENV_BREAK]   = params[PARAM_ENV_B] * .9999; // scale a little so it will not immediately move to swell if b==1
                    synth->envelope[ENV_SWELL]   = (params[PARAM_ENV_SUS] - params[PARAM_ENV_B]) / (float)(params[PARAM_ENV_SWL] * synth->sample_rate);
                    synth->envelope[ENV_SUSTAIN] = params[PARAM_ENV_SUS];
                    if (params[PARAM_ENV_SUS])
                        synth->envelope[ENV_RELEASE] = -(params[PARAM_ENV_SUS]) / (float)(params[PARAM_ENV_R] * synth->sample_rate);
                    else if (synth->envelope[ENV_DECAY] < synth->envelope[ENV_SWELL])
                        synth->envelope[ENV_RELEASE] = synth->envelope[ENV_DECAY]; // release same rate as decay
                    else
                        synth->envelope[ENV_RELEASE] = synth->envelope[ENV_SWELL]; // release same rate as swell

                    // set harmonic gains
                    if (params[PARAM_HARM_MODE] != synth->harmonic_mode) {
                        type = synth->harmonic_mode = params[PARAM_HARM_MODE];
                        if (type == HARMONIC_MODE_SINC) {
                            synth->harm_gains = synth->harm_gain_sinc;
                        } else if (type == HARMONIC_MODE_SAW) {
                            synth->harm_gains = synth->harm_gain_saw;
                        } else if (type == HARMONIC_MODE_SQR) {
                            synth->harm_gains = synth->harm_gain_sqr;
                        } else if (type == HARMONIC_MODE_TRI) {
                            synth->harm_gains = synth->harm_gain_tri;
                        }
                    }
                }
                if (val) {
                    if (synth->note[num].note_dead == true) {
                        synth->active[synth->nactive++] = num; // push new note onto active stack
                    } else // note still playing, finish the old one
                    {
                        play_note(&synth->note[num],
                            &(synth->waves),
                            t - frame_no, // play to frame before event
                            &(buf[frame_no]),
                            synth->pitchbend,
                            params[PARAM_MASTER_GAIN],
                            (uint8_t)params[PARAM_RULE],
                            params[PARAM_WAVE],
                            params[PARAM_FMOD_WAV],
                            fstep,
                            params[PARAM_AMOD_WAV],
                            astep); // note can't be dead about to start again
                    }

                    start_note(&(synth->note[num]),
                        &(synth->waves),
                        val,
                        t,
                        synth->harm_gains,
                        params[PARAM_INIT_CELLS],
                        params[PARAM_HARM_WIDTH] / 1200,
                        synth->envelope);
                } else // velocity zero == note off
                {
                    for (i = 0; i < synth->nactive; i++) {
                        if (synth->active[i] == num) {
                            if (synth->sus) {
                                if (!synth->note[num].sus) {
                                    synth->note[num].sus                  = true;
                                    synth->sustained[synth->nsustained++] = num;
                                }
                            } else {
                                end_note(&(synth->note[num]), t);
                            }
                        }
                    }
                }
            } else if (type == MIDI_NOTE_OFF) {
                num = message[1] & MIDI_DATA_MASK;
                for (i = 0; i < synth->nactive; i++) {
                    if (synth->active[i] == num) {
                        if (synth->sus) {
                            if (!synth->note[num].sus) {
                                synth->note[num].sus                  = true;
                                synth->sustained[synth->nsustained++] = num;
                            }
                        } else {
                            end_note(&(synth->note[num]), t);
                        }
                    }
                }

            } else if (type == MIDI_CONTROL_CHANGE) {
                num = message[1] & MIDI_DATA_MASK;
                if (num == MIDI_SUSTAIN_PEDAL) {
                    val = message[2] & MIDI_DATA_MASK;
                    if (val < 64) {
                        synth->sus = false;
                        // end all sus. notes
                        for (i = 0; i < synth->nsustained; i++) {
                            if (synth->note[synth->sustained[i]].sus) {
                                end_note(&(synth->note[synth->sustained[i]]), t);
                            }
                        }
                        synth->nsustained = 0;
                    } else {
                        synth->sus = true;
                    }
                } else if (num == MIDI_ALL_NOTES_OFF || num == MIDI_ALL_SOUND_OFF) {
                    // if (event->time.frames == 0) {
                    //    event->time.frames++;
                    // }
                    for (i = 0; i < synth->nactive; i++) {
                        num = synth->active[i];
                        end_note(&(synth->note[num]), t);
                    }
                    synth->nactive    = 0;
                    synth->nsustained = 0;
                }

            } else if (type == MIDI_PITCHBEND) {
                bend = (message[1] & MIDI_DATA_MASK) + ((message[2] & MIDI_DATA_MASK) << 7) - MIDI_PITCH_CENTER;
                // run and update current position because this blocks (affects all notes)
                // run_active_notes
                for (j = 0; j < synth->nactive; j++) {
                    note = &(synth->note[synth->active[j]]);
                    play_note(note,
                        &(this->waves),
                        t - frame_no, // play to frame before event
                        &(buf[frame_no]),
                        this->pitchbend,
                        this->params[PARAM_MASTER_GAIN],
                        (uint8_t)this->params[PARAM_RULE],
                        this->params[PARAM_WAVE],
                        this->params[PARAM_FMOD_WAV],
                        fstep,
                        this->params[PARAM_AMOD_WAV],
                        astep);

                    // cleanup dead notes
                    if (note->note_dead) {
                        synth->nactive--;
                        for (k = j; k < synth->nactive; k++) {
                            synth->active[k] = synth->active[k + 1];
                        }
                    }
                }
                synth->pitchbend = myPow2(bend / 49152.0); // 49152 is 12*8192/2
                frame_no         = event.frame;
            } // message types
        } // correct channel

    } // for each event

    // finish off whatever frames are left
    if (frame_no != nframes - 1) {
        // run_active_notes
        for (j = 0; j < synth->nactive; j++) {
            note = &(synth->note[synth->active[j]]);
            play_note(note,
                &(synth->waves),
                nframes - frame_no,
                &(buf[frame_no]),
                synth->pitchbend,
                params[PARAM_MASTER_GAIN],
                (uint8_t)params[PARAM_RULE],
                params[PARAM_WAVE],
                params[PARAM_FMOD_WAV],
                fstep,
                params[PARAM_AMOD_WAV],
                astep);
            // cleanup dead notes
            if (note->note_dead) {
                synth->nactive--;
                for (k = j; k < synth->nactive; k++) {
                    synth->active[k] = synth->active[k + 1];
                }
                j--; // indexes changed
            }
        } // active notes
    } // leftovers
}

void casynth_engine::panic()
{
    casynth_engine* synth = this;
    uint8_t         i, num;

    // Perform ALL_NOTES_OFF and ALL_SOUNDS_OFF.
    // This routine is copied from process().
    for (i = 0; i < synth->nactive; i++) {
        num = synth->active[i];
        end_note(&(synth->note[num]), 0);
    }
    synth->nactive    = 0;
    synth->nsustained = 0;
}
