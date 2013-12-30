#include "stdafx.h"
#include "JOpenGl.h"
#include "JStd.h"
#include <gl/GL.h>

namespace JStd { namespace GL {
;

class GLWndClass : public CSingleton < GLWndClass>
{
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	GLWndClass()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc = (WNDPROC) GLWndClass::WndProc;					// WndProc Handles Messages
		wc.cbClsExtra = 0;									// No Extra Window Data
		wc.cbWndExtra = 0;									// No Extra Window Data
		wc.hInstance = GetModuleHandle(NULL);							// Set The Instance
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground = NULL;									// No Background Required For GL
		wc.lpszMenuName = NULL;									// We Don't Want A Menu
		wc.lpszClassName = L"OpenGL";								// Set The Class Name

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
			throw std::runtime_error("Unable to register OpenGL window class");
	}
};

WndInit::WndInit()
: colorBits(32)
{
	dwStyle |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	ClassName = L"OpenGL";
}


PWindow CreateOpenGlWindow(WndInit& init)
{
	PWindow pWnd = Wnd::Wnd::Create(init);
	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		init.colorBits,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	Wnd::DC dc = pWnd->GetDC();

	GLuint PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(dc.H(), &pfd)))	// Did Windows Find A Matching Pixel Format?
		throw std::exception("Can't Find A Suitable PixelFormat.");
	if (!SetPixelFormat(dc.H(), PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
		throw std::exception("Can't Set The PixelFormat.");

	return pWnd;
}


}}
