#pragma once

enum casynth_param_id {
    PARAM_CHANNEL,
    PARAM_MASTER_GAIN,
    PARAM_RULE,
    PARAM_CELL_LIFE,
    PARAM_INIT_CELLS,
    PARAM_NHARMONICS,
    PARAM_HARM_MODE,
    PARAM_HARM_WIDTH,
    PARAM_WAVE,
    PARAM_ENV_A,
    PARAM_ENV_D,
    PARAM_ENV_B,
    PARAM_ENV_SWL,
    PARAM_ENV_SUS,
    PARAM_ENV_R,
    PARAM_AMOD_WAV,
    PARAM_AMOD_FREQ,
    PARAM_AMOD_GAIN,
    PARAM_FMOD_WAV,
    PARAM_FMOD_FREQ,
    PARAM_FMOD_GAIN,
    PARAM_COUNT
};

enum casynth_id_for_ui {
    CHANNEL,
    MASTER_GAIN,
    RULE,
    CELL_LIFE,
    INIT_CELLS,
    NHARMONICS,
    HARM_MODE,
    HARM_WIDTH,
    WAVE,
    ENV_A,
    ENV_D,
    ENV_B,
    ENV_SWL,
    ENV_SUS,
    ENV_R,
    AMOD_WAV,
    AMOD_FREQ,
    AMOD_GAIN,
    FMOD_WAV,
    FMOD_FREQ,
    FMOD_GAIN
};

enum casynth_param_type {
    PARAM_FLOAT,
    PARAM_INT,
    PARAM_ENUM
};

struct casynth_param_def {
    const char*        symbol;
    const char*        name;
    float              def;
    float              min;
    float              max;
    casynth_param_type type;
};

namespace casynth_param {

const char* paramName(casynth_param_id index);
const char* paramSymbol(casynth_param_id index);

float paramDefaultValue(casynth_param_id index);
float paramMinValue(casynth_param_id index);
float paramMaxValue(casynth_param_id index);

bool paramFloat(casynth_param_id index);
bool paramEnum(casynth_param_id index);
bool paramInt(casynth_param_id index);
}
