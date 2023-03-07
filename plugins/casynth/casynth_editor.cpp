#include "casynth_editor.hpp"
#include "DistrhoUI.hpp"
#include "params.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/x.H>
#include <time.h>

#include "casynth_ui.h"

CaSynthEditor::CaSynthEditor()
    : UI(500, 500)
    , fCaSynthUI(nullptr)
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
    // Sync cairo contexts with Fl_Window
    // See: vendor/fltk/README.Cairo.txt:64
    Fl::cairo_autolink_context(true);
#endif

    fCaSynthUI->ui = fCaSynthUI->init();
#if _WIN32
    // On Windows, use a Fl_Window to contain parent window info,
    // so we can directly make use of FLTK's built-in embed window strategy (even though it's for FLTK's objects originally).
    // Read the document of Fl_Window::override_winid() for more information. (This is AnClark's own extention, just in vendor/fltk/)
    fParentWindowObject = std::make_unique<Fl_Window>(800, 480, DISTRHO_PLUGIN_NAME);
    fParentWindowObject->override_winid(fParentWindow); // Override window ID with parent window's native ID
    fParentWindowObject->cursor(FL_CURSOR_DEFAULT); // Explicitly show the cursor
    fCaSynthUI->ui->parent(&(*fParentWindowObject));
#endif

    fCaSynthUI->ui->show();
    srand((unsigned int)time(NULL));
    fl_open_display();

    // set host to change size of the window
    setSize(fCaSynthUI->ui->w(), fCaSynthUI->ui->h());

#ifndef _WIN32
    // On Linux, we prefer NTK's embed window API. This is NTK's own extension.
    fl_embed(fCaSynthUI->ui, (Window)fParentWindow);
#endif

    fWidget = fl_xid(fCaSynthUI->ui);
}

CaSynthEditor::~CaSynthEditor()
{
    if (fCaSynthUI->ui) {
        delete fCaSynthUI->ui;
        fCaSynthUI->ui = nullptr;
    }
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

START_NAMESPACE_DISTRHO

UI* createUI()
{
    return new CaSynthEditor();
}

END_NAMESPACE_DISTRHO
