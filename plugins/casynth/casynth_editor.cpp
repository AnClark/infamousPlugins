#include "DistrhoUI.hpp"

// #include "Artwork.hpp"
// #include "DemoWidgetBanner.hpp"
// #include "DemoWidgetClickable.hpp"
#include "widgets_background.hpp"

#include "draw_casLabels.h"
#include "draw_casbg.h"

START_NAMESPACE_DISTRHO

/**
  We need a few classes from DGL.
 */
using DGL_NAMESPACE::CairoGraphicsContext;
using DGL_NAMESPACE::CairoImage;
using DGL_NAMESPACE::CairoImageButton;
using DGL_NAMESPACE::CairoImageKnob;

static const uint32_t BG_WIDTH = cairo_code_draw_casbg_get_width();
static const uint32_t BG_HEIGHT = cairo_code_draw_casbg_get_height();

class CairoExampleUI : public UI {
public:
    CairoExampleUI()
        : UI(BG_WIDTH, BG_HEIGHT)
    {
        fBackground = new WidgetBackground(this);
        fBackground->setSize(BG_WIDTH, BG_HEIGHT);
        fBackground->setAbsolutePos(0, 0);
        fBackground->setDrawingSize(BG_WIDTH, BG_HEIGHT);
        fBackground->setDrawingFunction(cairo_code_draw_casbg_render);

        fTitleLabel = new WidgetBackground(this);
        fTitleLabel->setSize(140, 95);
        fTitleLabel->setAbsolutePos(100, 10);
        fTitleLabel->setDrawingSize(140, 95);
        fTitleLabel->setDrawingFunction(cairo_code_draw_casTitleLabel_render);

        // Debug
        auto area = fTitleLabel->getConstrainedAbsoluteArea();
        d_stderr("fTitleLabel draw area: %dx%d", area.getWidth(), area.getHeight());

#if 0
        // we can use this if/when our resources are scalable, for now they are PNGs
        const double scaleFactor = getScaleFactor();
        if (scaleFactor != 1.0)
            setSize(200 * scaleFactor, 200 * scaleFactor);
#else
        // without scalable resources, let DPF handle the scaling internally
        setGeometryConstraints(BG_WIDTH, BG_HEIGHT, false, true);
#endif
    }

protected:
    void onCairoDisplay(const CairoGraphicsContext& context) override
    {
        cairo_t* const cr = context.handle;
        cairo_set_source_rgb(cr, 1.0, 0.8, 0.5);
        cairo_paint(cr);
    }

#if 0
    // we can use this if/when our resources are scalable, for now they are PNGs
    void onResize(const ResizeEvent& ev) override
    {
        UI::onResize(ev);

        const double scaleFactor = getScaleFactor();

        fWidgetClickable->setSize(50*scaleFactor, 50*scaleFactor);
        fWidgetClickable->setAbsolutePos(100*scaleFactor, 100*scaleFactor);

        fWidgetBanner->setSize(180*scaleFactor, 80*scaleFactor);
        fWidgetBanner->setAbsolutePos(10*scaleFactor, 10*scaleFactor);

        fKnob->setSize(80*scaleFactor, 80*scaleFactor);
        fKnob->setAbsolutePos(10*scaleFactor, 100*scaleFactor);

        fButton->setSize(60*scaleFactor, 35*scaleFactor);
        fButton->setAbsolutePos(100*scaleFactor, 160*scaleFactor);
    }
#endif

    void parameterChanged(uint32_t index, float value) override
    {
        // unused
        (void)index;
        (void)value;
    }

    void stateChanged(const char* key, const char* value) override
    {
    }

private:
    ScopedPointer<WidgetBackground> fTitleLabel;
    ScopedPointer<WidgetBackground> fBackground;
};

UI* createUI()
{
    return new CairoExampleUI;
}

END_NAMESPACE_DISTRHO
