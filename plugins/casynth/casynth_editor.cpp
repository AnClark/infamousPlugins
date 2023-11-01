#include "casynth_editor.hpp"
#include "DistrhoUI.hpp"
#include "params.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/x.H>
#include <time.h>

#include "casynth_ui.h"

// Initialize reference count
uint32_t CaSynthEditor::fEditorRefCount = 0;

CaSynthEditor::CaSynthEditor()
    : UI(500, 500)
    , fCaSynthUI(nullptr)
    , fParentWindow(0)
#if _WIN32
    , fWidget(nullptr)
#else
    , fWidget(0)
#endif
{
    // Use unique pointer to prevent UI crash.
    //
    // DPF loads editor instance twice. This will load NTK UI twice as well,
    // resulting in malloc() conflict ("unaligned tcache chunk detected").
    // Crashes occurs during set_initial_condition(), and occationally.
    //
    // By using unique pointer, only one UI instance is allowed, so it's much
    // safer and reliable.
    fCaSynthUI = std::make_unique<CaSynthUI>();
    fCaSynthUI->set_editor_instance(this);

    // Remember to clean up existed FLTK window as well
    if (fCaSynthUI->ui) {
        delete fCaSynthUI->ui;
        fCaSynthUI = nullptr;
    }

    fParentWindow = getParentWindowHandle();

#if _WIN32
    /**
     * Set FLTK global UI behaviors.
     *
     * All instances shares one config, so only set them once is enough.
     * Here I use a global static reference count to make sure that those APIs will be only called once
     * during the lifetime of all plugin instances.
     * Many DAWs runs multiply plugin instances in one process, so they share the same static variable.
     *
     * NOTICE: _disableFltkScreenScaling() MUST be invoked only once, otherwise UI may not be shown. See its document.
     */
    if (fEditorRefCount++ == 0) { // Check and update reference count. 0 means no other instance exists
        // Sync cairo contexts with Fl_Window
        // See: vendor/fltk/README.Cairo.txt:64
        Fl::cairo_autolink_context(true);

        // Disable FLTK Hi-DPI scaling
        _disableFltkScreenScaling();
    }
#endif

    fCaSynthUI->ui = fCaSynthUI->show();
    srand((unsigned int)time(NULL));
    fl_open_display();

    // Set FLTK window close callback. This callback gracefully handles the closing of UI.
    // NOTICE: FL_Window's callback is called when you close the window.
    fCaSynthUI->ui->callback(CaSynthEditor::windowCloseCallback, this);

    // set host to change size of the window
    setSize(fCaSynthUI->ui->w(), fCaSynthUI->ui->h());

    // get widget window ID
    fWidget = fl_xid(fCaSynthUI->ui);

    // Now let's reparent plugin window
    _reparentWindow();
}

CaSynthEditor::~CaSynthEditor()
{
    if (fCaSynthUI->ui) {
        delete fCaSynthUI->ui;
        fCaSynthUI->ui = nullptr;
    }

    // Check and update reference count
    if (fEditorRefCount > 0)
        fEditorRefCount--;
}

void CaSynthEditor::parameterChanged(uint32_t index, float value)
{
    if (!fCaSynthUI || !fCaSynthUI->ui) {
        d_stderr2(">> parameterChanged: No UI available.");
        return;
    }

    switch (index) {
        case PARAM_CHANNEL:
            fCaSynthUI->channel->value(value);
            break;
        case PARAM_MASTER_GAIN:
            fCaSynthUI->mastergain->value(value);
            break;
        case PARAM_WAVE:
            fCaSynthUI->cellwaveform->value(value);
            break;
        case PARAM_CELL_LIFE:
            fCaSynthUI->lifetime->value(value);
            break;
        case PARAM_HARM_MODE:
            fCaSynthUI->harmgain->value(value);
            break;
        case PARAM_NHARMONICS:
            fCaSynthUI->nharmonics->value(value);
            break;
        case PARAM_HARM_WIDTH:
            fCaSynthUI->harmwidth->value(value);
            break;
        case PARAM_ENV_A:
            fCaSynthUI->a->value(value);
            break;
        case PARAM_ENV_D:
            fCaSynthUI->d->value(value);
            break;
        case PARAM_ENV_B:
            fCaSynthUI->b->value(value);
            break;
        case PARAM_ENV_SWL:
            fCaSynthUI->sw->value(value);
            break;
        case PARAM_ENV_SUS:
            fCaSynthUI->su->value(value);
            break;
        case PARAM_ENV_R:
            fCaSynthUI->r->value(value);
            break;
        case PARAM_AMOD_WAV:
            fCaSynthUI->amwave->value(value);
            break;
        case PARAM_AMOD_FREQ:
            fCaSynthUI->amfreq->value(value);
            break;
        case PARAM_AMOD_GAIN:
            fCaSynthUI->amgain->value(value);
            break;
        case PARAM_FMOD_WAV:
            fCaSynthUI->fmwave->value(value);
            break;
        case PARAM_FMOD_FREQ:
            fCaSynthUI->fmfreq->value(value);
            break;
        case PARAM_FMOD_GAIN:
            fCaSynthUI->fmgain->value(value);
            break;
        case PARAM_RULE:
            fCaSynthUI->set_rule(value);
            break;
        case PARAM_INIT_CELLS:
            fCaSynthUI->set_initial_condition(value);
            break;
    } // switch
}

void CaSynthEditor::sizeChanged(uint width, uint height)
{
    if (fCaSynthUI != nullptr && fCaSynthUI->ui != nullptr)
        if (width > 0 && height > 0)
            fCaSynthUI->ui->size(width, height);
}

void CaSynthEditor::stateChanged(const char* key, const char* value)
{
}

void CaSynthEditor::visibilityChanged(const bool visible)
{
}

void CaSynthEditor::uiIdle()
{
    if (fCaSynthUI != nullptr && fCaSynthUI->ui != nullptr)
        fCaSynthUI->idle();
}

void CaSynthEditor::_reparentWindow()
{
#if defined(_WIN32)
    // Reparent window
    SetParent((HWND)fWidget, (HWND)fParentWindow);
    // Reset window style (no border)
    SetWindowLongPtrA((HWND)fWidget, GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_SYSMENU);
    // Reset window position and size
    fCaSynthUI->ui->position(0, 0);
#elif defined(__APPLE__)
#warning "Apple embed window feature is not yet implemented"
    return;
#else
    // On Linux we prefer NTK. Invoke NTK embed window API
    fl_embed(fCaSynthUI->ui, (Window)fParentWindow);
#endif
}

#if defined(_WIN32) || defined(__APPLE__) // NTK does not support Fl::screen_scale()
void CaSynthEditor::_disableFltkScreenScaling()
{
    /**
     * Disable FLTK Hi-DPI auto scaling.
     * This will set all screen's scale factor to 1.0f.
     *
     * NOTICE: This method should only be called ONCE. Otherwise UI rendering issues will occur:
     *         1. If you open more than 2 UIs in REAPER by using floating UI (or other DAWs with similar feature)
     *            the UI may fail to load.
     *         2. Even though you loaded more than 1 UI in REAPER, the second UI may have wrong position.
     *            Such a UI misbehavior cannot be corrected by Fl_Window::position() or SetWindowPos().
     */

    DISTRHO_SAFE_ASSERT(fEditorRefCount == 0)

    for (int scr = 0; scr < Fl::screen_count(); scr++) {
        Fl::screen_scale(scr, 1.0f);
    }
}
#endif

void CaSynthEditor::windowCloseCallback(Fl_Widget* widget, void* v)
{
    /**
     * FLTK window close callback function.
     * Gracefully handles the exit of plugin UI.
     *
     * On DPF, it's up to developers to handle the process of closing external window.
     *
     * In many 3rd-party toolkits (e.g. Qt, FLTK, NTK), simply closing the window does not mean to close
     * the DISTRHO::UI instance. Instead, the instance is still active on background, even though the host
     * is already closed.
     * To entirely close the instance, you need to call hide(). This asks DPF to close and clean up
     * explicitly.
     *
     * In FLTK, we add a callback for plugin's main window. When the window is about to close, the callback
     * will invoke hide(), to make sure everything cleans up well.
     * This callback is a static member of CaSynthEditor.
     */

    // 1st argument is the caller instance. Here it's the main window instance.
    Fl_Double_Window* window = (Fl_Double_Window*)widget;

    // 2nd argument is for userdata. We pass the editor instance here.
    CaSynthEditor* editor = (CaSynthEditor*)v;

    d_stderr("[CaSynth] Attempt to close FLTK window");

    // Close FLTK window
    // NOTICE: On window manager, `hide` means "close". Sometimes such a naming may be confusive.
    window->hide();

    // Ask DPF to close UI instance.
    // If you don't call this, plugin will remain alive even though host is closed.
    editor->hide();
}

START_NAMESPACE_DISTRHO

UI* createUI()
{
    return new CaSynthEditor();
}

END_NAMESPACE_DISTRHO
