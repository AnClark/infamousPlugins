

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/x.H>

#include "stuck_ui.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

#define STUCKUI_URI "http://infamousplugins.sourceforge.net/plugs.html#stuck_ui"

static LV2UI_Handle init_stuckUI(const struct _LV2UI_Descriptor * descriptor,
		const char * plugin_uri,
		const char * bundle_path,
		LV2UI_Write_Function write_function,
		LV2UI_Controller controller,
		LV2UI_Widget * widget,
		const LV2_Feature * const * features) 
{
    if(strcmp(plugin_uri, STUCK_URI) != 0)
    {
        return 0;
    }

    StuckUI* self = new StuckUI();
    if(!self) return 0;
    LV2UI_Resize* resize = NULL;

    self->controller = controller;
    self->write_function = write_function;

    void* parentXwindow = 0;
    for (int i = 0; features[i]; ++i)
    {
        if (!strcmp(features[i]->URI, LV2_UI__parent)) 
	{
           parentXwindow = features[i]->data;
        }
	else if (!strcmp(features[i]->URI, LV2_UI__resize)) 
	{
           resize = (LV2UI_Resize*)features[i]->data;
        }

    }

    self->ui = self->show();
    fl_open_display();
    // set host to change size of the window
    if (resize)
    {
      //resize->ui_resize(resize->handle, self->ui->w(), self->ui->h());
    }
    fl_embed( self->ui,(Window)parentXwindow);

    return (LV2UI_Handle)self;
}

void cleanup_stuckUI(LV2UI_Handle ui)
{
    StuckUI *self = (StuckUI*)ui;

    delete self;
}

void stuckUI_port_event(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void * buffer)
{
    StuckUI *self = (StuckUI*)ui;
    if(!format)
    {
      float val = *(float*)buffer;
      switch(port_index)
      {
        case STICKIT:
          self->stickit->value((int)val);
	  self->led->value((int)val);
	  break;
        case DRONEGAIN:
          self->volume->value(val);
	  break;
        case RELEASE:
          self->time->value(val);
	  break;
      }
    }
}

static int
idle(LV2UI_Handle handle)
{
  StuckUI* self = (StuckUI*)handle;
  
  self->idle();
  
  return 0;
}

static const LV2UI_Idle_Interface idle_iface = { idle };

static const void*
extension_data(const char* uri)
{
  if (!strcmp(uri, LV2_UI__idleInterface))
  {
    return &idle_iface;
  }
  return NULL;
}

static const LV2UI_Descriptor stuckUI_descriptor = {
    STUCKUI_URI,
    init_stuckUI,
    cleanup_stuckUI,
    stuckUI_port_event,
    extension_data
};

LV2_SYMBOL_EXPORT 
const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index) 
{
    switch (index) {
    case 0:
        return &stuckUI_descriptor;
    default:
        return NULL;
    }
}
