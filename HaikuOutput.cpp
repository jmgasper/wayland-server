#include "HaikuOutput.h"
#include <Screen.h>
#include <AutoDeleter.h>
#include <dlfcn.h>
#include <math.h>

extern const struct wl_interface wl_output_interface;


enum {
	OUTPUT_VERSION = 3,
};


class HaikuOutput: public WlOutput {
protected:
	virtual ~HaikuOutput() = default;
};


// The buffer scale clients should draw at: on a scaled desktop (air/OS,
// BScreen::DeviceScale(), looked up at run time) its device scale rounded up.
// Surfaces drawn at a higher density than the desktop are filtered down.
int32_t HaikuOutputScale()
{
	typedef float (*DeviceScaleFunction)(BScreen *screen);
	static DeviceScaleFunction deviceScale
		= (DeviceScaleFunction)dlsym(RTLD_DEFAULT, "_ZN7BScreen11DeviceScaleEv");
	if (deviceScale == NULL) return 1;
	BScreen screen;
	float scale = deviceScale(&screen);
	if (!(scale > 1)) return 1;
	return (int32_t)ceilf(scale - 0.01f);
}


HaikuOutputGlobal *HaikuOutputGlobal::Create(struct wl_display *display)
{
	ObjectDeleter<HaikuOutputGlobal> global(new(std::nothrow) HaikuOutputGlobal());
	if (!global.IsSet()) return NULL;
	if (!global->Init(display, &wl_output_interface, OUTPUT_VERSION)) return NULL;
	return global.Detach();
}

void HaikuOutputGlobal::Bind(struct wl_client *wl_client, uint32_t version, uint32_t id)
{
	HaikuOutput *output = new(std::nothrow) HaikuOutput();
	if (output == NULL) {
		wl_client_post_no_memory(wl_client);
		return;
	}
	if (!output->Init(wl_client, version, id)) {
		return;
	}

	BScreen screen;
	int32_t width = (int32_t)screen.Frame().Width() + 1;
	int32_t height = (int32_t)screen.Frame().Height() + 1;
	int32_t scale = HaikuOutputScale();
	output->SendGeometry(0, 0, (float)width * 0.3528, (float)height * 0.3528, WlOutput::subpixelUnknown, "Unknown make", "Unknown model", WlOutput::transformNormal);
	// the mode is in the pixels of the scale's buffers
	output->SendMode(WlOutput::modeCurrent | WlOutput::modePreferred, width * scale, height * scale, 60000);
	output->SendScale(scale);
	output->SendDone();
}
