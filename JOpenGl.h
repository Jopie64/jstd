#pragma once
#include "JWnd.h"

namespace JStd { namespace GL {
;
using Wnd::PWindow;

struct WndInit : Wnd::WndInit
{
	WndInit();

	BYTE colorBits;
};
PWindow CreateGlWindow(WndInit& init);

}}
