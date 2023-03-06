#pragma once

enum envfollower_param_id {
    PARAM_CTL_IN,
    PARAM_CTL_OUT,
    PARAM_PEAKRMS,
    PARAM_THRESHOLD,
    PARAM_SATURATION,
    PARAM_ATIME,
    PARAM_DTIME,
    PARAM_CHANNEL,
    PARAM_CONTROL_NO,
    PARAM_MMINV,
    PARAM_MMAXV,
    PARAM_MDIRECTION,
    PARAM_CMINV,
    PARAM_CMAXV,
    PARAM_CDIRECTION,
    PARAM_COUNT
};

enum envfollower_id_for_ui {
    CTL_IN,
    CTL_OUT,
    PEAKRMS,
    THRESHOLD, // 6
    SATURATION,
    ATIME,
    DTIME,
    CHANNEL,
    CONTROL_NO, // 11
    MINV,
    MAXV,
    REVERSE,
    CMINV = 10,
    CMAXV, // 16
    CREVERSE,
    CV_OUT
};

enum envfollower_param_type {
    PARAM_FLOAT,
    PARAM_INT,
    PARAM_ENUM,
    PARAM_OUTPUT
};

struct envfollower_param_def {
    const char*            symbol;
    const char*            name;
    float                  def;
    float                  min;
    float                  max;
    envfollower_param_type type;
};

namespace envfollower_param {

const char* paramName(envfollower_param_id index);
const char* paramSymbol(envfollower_param_id index);

float paramDefaultValue(envfollower_param_id index);
float paramMinValue(envfollower_param_id index);
float paramMaxValue(envfollower_param_id index);

bool paramFloat(envfollower_param_id index);
bool paramEnum(envfollower_param_id index);
bool paramInt(envfollower_param_id index);
bool paramOutput(envfollower_param_id index);
}
