#ifndef DISTRHO_PLUGIN_INFO_H
#define DISTRHO_PLUGIN_INFO_H

#define DISTRHO_PLUGIN_BRAND "Spencer Jackson, AnClark Liu"
#define DISTRHO_PLUGIN_NAME "the infamous cellular automaton synth"
#define DISTRHO_PLUGIN_URI "https://github.com/AnClark/infamousPlugins.git"
#define DISTRHO_PLUGIN_CLAP_ID "infamousPlugins.casynth"

#ifdef ENABLE_UI
#define DISTRHO_PLUGIN_HAS_UI 1
#define DISTRHO_PLUGIN_HAS_EMBED_UI 1
#define DISTRHO_PLUGIN_HAS_EXTERNAL_UI 1
#endif
#define DISTRHO_PLUGIN_IS_RT_SAFE 1
#define DISTRHO_PLUGIN_IS_SYNTH 1
#define DISTRHO_PLUGIN_NUM_INPUTS 1
#define DISTRHO_PLUGIN_NUM_OUTPUTS 1
#define DISTRHO_UI_USER_RESIZABLE 1

#define DISTRHO_PLUGIN_WANT_STATE 1

#endif
