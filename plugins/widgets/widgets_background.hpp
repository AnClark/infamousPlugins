#pragma once

#include "Cairo.hpp"

typedef void (*DrawingFunction)(cairo_t*);

// avtk drawing method (adapted)
static void default_bg_drawing(cairo_t* cr)
{
    cairo_set_line_width(cr, 1.5);

    // fill background
    cairo_rectangle(cr, 0, 0, 100, 100);
    cairo_set_source_rgba(cr, 66 / 255.f, 66 / 255.f, 66 / 255.f, 1);
    cairo_fill(cr);

    // set up dashed lines, 1 px off, 1 px on
    double dashes[1];
    dashes[0] = 2.0;

    cairo_set_dash(cr, dashes, 1, 0.0);
    cairo_set_line_width(cr, 1.0);

    // loop over each 2nd line, drawing dots
    for (int i = 0; i < 100; i += 4) {
        cairo_move_to(cr, i, 0);
        cairo_line_to(cr, i, 100);
    }

    cairo_set_source_rgba(cr, 28 / 255.f, 28 / 255.f, 28 / 255.f, 0.5);
    cairo_stroke(cr);
    cairo_set_dash(cr, dashes, 0, 0.0);

    // draw header
    // backing
    cairo_rectangle(cr, 0, 0, 100, 20);
    cairo_set_source_rgb(cr, 28 / 255.f, 28 / 255.f, 28 / 255.f);
    cairo_fill(cr);

    // text NOT YET SUPPORTED
    // cairo_move_to( cr, 10, 14 );
    // cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
    // cairo_set_font_size( cr, 10 );
    // cairo_show_text( cr, label );

    // lower stripe
    cairo_move_to(cr, 0, 20);
    cairo_line_to(cr, 100, 20);
    cairo_stroke(cr);

    // stroke rim
    cairo_rectangle(cr, 0, 0, 100, 100);
    cairo_set_source_rgba(cr, 0 / 255.f, 153 / 255.f, 255 / 255.f, 1);
    cairo_stroke(cr);
}

START_NAMESPACE_DGL

class WidgetBackground : public CairoSubWidget {
private:
    int             drawing_w;
    int             drawing_h;
    DrawingFunction drawing_f; // pointer to draw function
    bool            stretch;

public:
#if 0
    WidgetBackground(Widget* const parent, int _drawing_w, int _drawing_h, DrawingFunction _drawing_f)
        : CairoSubWidget(parent)
        , stretch(true)
    {
        setDrawingSize(drawing_w, drawing_h);
        setDrawingFunction(_drawing_f);
    }
#else
    explicit WidgetBackground(SubWidget* const parent)
        : CairoSubWidget(parent)
        , stretch(true)
    {
    }

    explicit WidgetBackground(TopLevelWidget* const parent)
        : CairoSubWidget(parent)
        , stretch(true)
    {
    }
#endif

    void setDrawingSize(int _drawing_w, int _drawing_h)
    {
        drawing_w = _drawing_w;
        drawing_h = _drawing_h;
    }

    void setDrawingFunction(DrawingFunction _drawing_f)
    {
        drawing_f = _drawing_f;
    }

protected:
    void onCairoDisplay(const CairoGraphicsContext& context) override
    {
        cairo_t* const cr = context.handle;

        const Size<uint> sz = getSize();
        const int        w  = sz.getWidth();
        const int        h  = sz.getHeight();
        const Point<int> pt = getAbsolutePos();
        const int        x  = pt.getX();
        const int        y  = pt.getY();

        cairo_save(cr);

        // calcluate scale and centering
        double scalex,
            scaley,
            shiftx = 0,
            shifty = 0;
        scalex     = w / (double)drawing_w;
        scaley     = h / (double)drawing_h;
        if (!stretch) {
            if (scalex > scaley) {
                scalex = scaley;
                shiftx = (w - scalex * drawing_w) / 2.f;
            } else {
                scaley = scalex;
                shifty = h - scaley * drawing_h;
            }
        }
        // label behind value
        // draw_label();
        // move to position in the window
        cairo_translate(cr, x + shiftx, y + shifty);
        // scale the drawing
        cairo_scale(cr, scalex, scaley);
        // call the draw function
        if (drawing_f)
            drawing_f(cr);
        else
            default_bg_drawing(cr);

        cairo_restore(cr);
    }
};

END_NAMESPACE_DGL
