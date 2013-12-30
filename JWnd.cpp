#include "stdafx.h"
#include "JWnd.h"
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


Bitmap DC::CreateCompatibleBitmap(const Size& sz)
{
	Bitmap ret;
	ret.Attach(::CreateCompatibleBitmap(H(), sz.x, sz.y));
	return move(ret);
}

DC DC::CreateCompatibleDC()
{
	DC ret;
	ret.Attach(m_hWnd, ::CreateCompatibleDC(H()));
	return move(ret);
}

DCSelect DC::Select(HGDIOBJ hGdi)
{
	DCSelect ret(this);
	ret.m_OldObj = ::SelectObject(H(),hGdi);
	return move(ret);
}

void DC::FillRect(const Rect& rect, const Brush& brush)
{
	::FillRect(H(), &ToRECT(rect), brush.H());
}

void DC::FillRect(const Rect& rect, COLORREF color)
{
	FillRect(rect, CreateSolidBrush(color));
}

void DC::BitBlt(const Point& destLoc, DC& dcSource, const Point& sourceLoc, const Size& size, DWORD rop)
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

void IWindow::Invalidate(bool erase)
{
	RECT rect;
	::GetClientRect(H(), &rect);
	::InvalidateRect(H(), &rect, erase);
}

Rect Wnd::DefaultRect()
{
	return Rect(-1, -1, -1, -1);
}

PWindow Wnd::Create(
	DWORD dwExStyle,
	LPCWSTR lpClassName,
	LPCWSTR lpWindowName,
	DWORD dwStyle,
	const Rect& rect,
	HWND hWndParent,
	HMENU hMenu,
	HINSTANCE hInstance,
	LPVOID lpParam)
{
	bool isDefault = rect.tl.x == -1;
	Size sz = rect.Size();
	HWND hWnd = CreateWindowEx(dwExStyle,
		lpClassName,
		lpWindowName,
		dwStyle,
		isDefault ? CW_USEDEFAULT : rect.tl.x,
		isDefault ? 0             : rect.tl.y,
		isDefault ? CW_USEDEFAULT : sz.x,
		isDefault ? 0             : sz.y,
		hWndParent,
		hMenu,
		hInstance,
		lpParam);
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

}}