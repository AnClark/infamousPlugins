//Spencer Jackson
//note.h
#include<constants.h>


//need to sort out which ones will be controllable by ports :\
typedef struct _NOTE
{
    //midi stuff
    unsigned char value;
    float velocity;
    double pitchbend;

    //harmonics stuff
    unsigned char nharmonics;
    unsigned short harmonics; //list of currently alive harmonics
    uint32_t nframes_since_harm_change;
    float harm_gain[MAX_N_HARMONICS+1];

    double step[MAX_N_HARMONICS+1];//step size between frames
    double phase[MAX_N_HARMONICS+1];//phase of all waves + fundamental
    
    void (*base_func)(double); //sin, tri, sqr, other?
    unsigned char base_wave;
    double base_func_min;//domain of function i.e. [-pi,pi]
    double base_func_max;


    //the envelope transitions are handled through recursing, midi events through multiple calls of play
    float env_gain;
    unsigned char env_state;//0a, 1d, 2b, 3s, 4s, 5r
    float envelope[6];//slope/slope/val/slope/val/slope of envelope ADBSSR
    bool note_dead;

    void (*amod_func)(double);
    unsigned char amod_wave;
    float amod_gain;//port
    double amod_step;//we'll just link these to the synth
    double amod_phase;
    double amod_func_min;//domain of function i.e. [-pi,pi]
    double amod_func_max;

    void (*fmod_func)(double);
    unsigned char fmod_wave;
    float fmod_gain;//port //gain 1 mods \pm 1 semitone
    double fmod_step;
    double fmod_phase;
    double fmod_func_min;//domain of function i.e. [-pi,pi]
    double fmod_func_max;
}NOTE;

void start_note(NOTE *self, unsigned char velocity, float harmonic_gain[], unsigned short harmonics, float envelope[], unsigned char base_wave, unsigned char amod_wave, unsigned char fmod_wave);
void play_note(NOTE *self, uint32_t nframes, uint32_t start_frame, float buffer[] );

NOTE* init_note(NOTE *self, unsigned char value, unsigned char velocity, short pitchbend, unsigned char nharmonics, bool *harmonics);

//these must be called before play is called
void set_envelope(NOTE *self, float slope);
void set_harmonics(NOTE *self, unsigned short harmonics);
void set_pitchbend(NOTE *self, short pitchbend);
void end_note(NOTE *self);
