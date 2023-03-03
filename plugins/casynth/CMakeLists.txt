set (PLUGIN casynth_dpf)

dpf_add_plugin (${PLUGIN}
    TARGETS vst2 vst3 lv2 clap
    FILES_DSP
        casynth.cpp
        engine.cpp
        note.cpp
        waves.cpp
    FILES_COMMON
        params.cpp
)

target_include_directories (${PLUGIN} PUBLIC
    .
)
