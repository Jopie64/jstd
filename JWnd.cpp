#include "JWnd.h"
#include "JStd.h"
#include <CommCtrl.h>
#include <exception>

#pragma comment(lib, "Comctl32.lib")

namespace JStd { namespace Wnd {

using namespace std;

DCSelect::~DCSelect()
{
	Unselect();
}

void DCSelect::Unselect()
{
	if (!m_pDc)
		return;
	SelectObject(m_pDc->H(), m_OldObj);
}


Bitmap DC::CreateCompatibleBitmap(const Size& sz) const
{
	Bitmap ret;
	ret.Attach(::CreateCompatibleBitmap(H(), sz.x, sz.y));
	return move(ret);
}

DC DC::CreateCompatibleDC() const
{
	DC ret;
	ret.Attach(m_hWnd, ::CreateCompatibleDC(H()));
	return move(ret);
}

DCSelect DC::Select(HGDIOBJ hGdi) const
{
	DCSelect ret(this);
	ret.m_OldObj = ::SelectObject(H(),hGdi);
	return move(ret);
}

void DC::FillRect(const Rect& rect, const Brush& brush) const
{
	::FillRect(H(), &ToRECT(rect), brush.H());
}

void DC::FillRect(const Rect& rect, COLORREF color) const
{
	FillRect(rect, CreateSolidBrush(color));
}

void DC::BitBlt(const Point& destLoc, DC& dcSource, const Point& sourceLoc, const Size& size, DWORD rop) const
{
	::BitBlt(H(), destLoc.x, destLoc.y, size.x, size.y, dcSource.H(), sourceLoc.x, sourceLoc.y, rop);
}

void PaintDC::DestroyHandle(HDC handle)
{
	EndPaint(m_hWnd, &m_ps);
}

Brush CreateSolidBrush(COLORREF color)
{
	Brush ret;
	ret.Attach(::CreateSolidBrush(color));
	return move(ret);
}

Rect IWindow::GetClientRect() const
{
	RECT rect;
	::GetClientRect(H(), &rect);
	return ToRect(rect);
}

Rect IWindow::GetWindowRect() const
{
	RECT rect;
	::GetWindowRect(H(), &rect);
	return ToRect(rect);
}

void IWindow::ScreenToClient(Point& p)
{
	POINT P = ToPOINT(p);
	::ScreenToClient(H(), &P);
	p = ToPoint(P);
}

void IWindow::ClientToScreen(Point& p)
{
	POINT P = ToPOINT(p);
	::ClientToScreen(H(), &P);
	p = ToPoint(P);
}

void IWindow::ScreenToClient(Rect& r)
{
	ScreenToClient(r.tl);
	ScreenToClient(r.br);
}

void IWindow::ClientToScreen(Rect& r)
{
	ClientToScreen(r.tl);
	ClientToScreen(r.br);
}

PaintDC IWindow::Paint()
{
	PaintDC ret;
	ret.Attach(H(), BeginPaint(H(), &ret.m_ps));
	return move(ret);
}

DC IWindow::GetDC()
{
	DC ret;
	ret.Attach(H(), ::GetDC(H()));
	return move(ret);
}


void IWindow::Invalidate(bool erase)
{
	RECT rect;
	::GetClientRect(H(), &rect);
	::InvalidateRect(H(), &rect, erase);
}

WndInit::WndInit()
:	dwExStyle(0),
	dwStyle(0),
	rect(Wnd::DefaultRect()),
	hWndParent(NULL),
	hMenu(NULL),
	hInstance(GetModuleHandle(NULL)),
	lpParam(NULL)
{
}

Rect Wnd::DefaultRect()
{
	return Rect(-1, -1, -1, -1);
}

PWindow Wnd::Create(const WndInit& init)
{
	bool isDefault = init.rect.tl.x == -1;
	Size sz = init.rect.Size();
	HWND hWnd = CreateWindowEx(
		init.dwExStyle,
		JStd::String::ToAW(init.ClassName, CP_ACP).c_str(),
		JStd::String::ToAW(init.WindowName, CP_ACP).c_str(),
		init.dwStyle,
		isDefault ? CW_USEDEFAULT : init.rect.tl.x,
		isDefault ? 0             : init.rect.tl.y,
		isDefault ? CW_USEDEFAULT : sz.x,
		isDefault ? 0             : sz.y,
		init.hWndParent,
		init.hMenu,
		init.hInstance,
		init.lpParam);
	if (!hWnd)
		throw std::exception("Could not create window");
	std::shared_ptr<Wnd> ret = make_shared<Wnd>();
	ret->Attach(hWnd);
	return ret;
}


WndSubclass::WndSubclass() : m_iIdSubclass(0)
{
}

WndSubclass::~WndSubclass()
{
	Detach();
}

void WndSubclass::Detach()
{
	if (!m_pWnd)
		return;
	RemoveWindowSubclass(H(), &StaticWndProc, m_iIdSubclass);
	m_pWnd = nullptr;
	m_iIdSubclass = 0;
}

void WndSubclass::Attach(PWindow pWnd)
{
	Detach();
	m_pWnd = pWnd;
	m_iIdSubclass = Sm_iIdGen++;
	if (!SetWindowSubclass(H(), &StaticWndProc, m_iIdSubclass, (DWORD_PTR)this))
	{
		m_pWnd = nullptr;
		throw std::exception("SetWindowSubclass()");
	}
}

UINT_PTR WndSubclass::Sm_iIdGen = 1;

LRESULT WndSubclass::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	((WndSubclass*) dwRefData)->WndProc(uMsg, wParam, lParam);
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}


int RunLoop(HACCEL hAccelTable, const std::function<void()>& FcRunFrame)
{
	MSG msg;
	if (FcRunFrame)
	{
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{

				// handle or dispatch messages
				if (msg.message == WM_QUIT)
					return msg.wParam;
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
				FcRunFrame();
		}
	}
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}

}}