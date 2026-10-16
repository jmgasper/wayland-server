#pragma once
#include "Wayland.h"
#include "WlGlobal.h"


class HaikuOutputGlobal: public WlGlocal {
public:
	static HaikuOutputGlobal *Create(struct wl_display *display);
	virtual ~HaikuOutputGlobal() = default;
	void Bind(struct wl_client *wl_client, uint32_t version, uint32_t id) override;
};

class HaikuSurface;

int32_t HaikuOutputScale();
// Sends the outputs' size and scale to their clients again where the
// desktop changed.
void HaikuOutputsUpdate();
// Tells a surface that it is shown on its client's outputs, so that a client
// such as GTK follows their scale when it changes.
void HaikuOutputsEnter(HaikuSurface *surface);
