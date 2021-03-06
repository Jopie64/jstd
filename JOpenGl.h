#pragma once
#include "JWnd.h"
#include <gl/GL.h>

namespace JStd { namespace GL {
;
using Wnd::PWindow;

struct WndInit : Wnd::WndInit
{
	WndInit();

	BYTE colorBits;
};

class GlRenderCtxt : public Wnd::ResourceWrapper<HGLRC, nullptr>
{
public:
	virtual ~GlRenderCtxt();
	virtual void DestroyHandle(HGLRC handle) override;

};

class GlWnd : public Wnd::IWindow
{
public:
	virtual void	SwapBuffers() =0;
	virtual const Wnd::DC&	GetMainDC() = 0;
};

typedef std::shared_ptr<GlWnd> PGlWnd;

PGlWnd CreateGlWindow(const WndInit& init);

}}
