#ifndef CASYNTH_UI_HPP
#define CASYNTH_UI_HPP

#include "DistrhoUI.hpp"
#include "FL/Fl_Widget.H"

#ifdef _WIN32
#include <windows.h>
#else
#include <X11/X.h>
#endif
#include <memory>

// Forward decls.
class CaSynthUI;

class CaSynthEditor : public DISTRHO::UI {
    std::unique_ptr<CaSynthUI> fCaSynthUI;
    uintptr_t                  fParentWindow;

    // TODO: Cross-platform window type
#if _WIN32
    HWND fWidget;
#else
    Window fWidget;
#endif

public:
    CaSynthEditor();
    ~CaSynthEditor();

    // ----------------------------------------------------------------------------------------------------------------
    // FLTK Callbacks

    static void windowCloseCallback(Fl_Widget*, void*);

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

    void parameterChanged(uint32_t index, float value) override;
    void stateChanged(const char* key, const char* value) override;
    // void programLoaded(uint32_t index) override;

    // ----------------------------------------------------------------------------------------------------------------
    // External window overrides

    // uintptr_t getNativeWindowHandle() const noexcept override;
    void sizeChanged(uint width, uint height) override;
    void visibilityChanged(const bool visible) override;
    void uiIdle() override;

private:
    // ----------------------------------------------------------------------------------------------------------------
    // Embedding window helpers

    void _reparentWindow();
#if defined(_WIN32) || defined(__APPLE__)
    void _disableFltkScreenScaling();
#endif

    static uint32_t fEditorRefCount;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CaSynthEditor)
};

#endif
