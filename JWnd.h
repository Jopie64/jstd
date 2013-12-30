#pragma once
#include <memory>
#include <string>
#include "JGraphics.h"

namespace JStd { namespace Wnd {

template < typename T, BOOL(WINAPI* FcDestroy)(T)>
class ResourceWrapper
{
	ResourceWrapper(const ResourceWrapper&);
	ResourceWrapper& operator=(const ResourceWrapper&);
public:
	ResourceWrapper() : m_handle(NULL) {}
	ResourceWrapper(T handle) : m_handle(handle) {}
	virtual ~ResourceWrapper() { Destroy(); }
	ResourceWrapper(ResourceWrapper && that) : m_handle(NULL) { Swap(that); }
	ResourceWrapper& operator=(ResourceWrapper && that) { Swap(that); return *this; }


	void Attach(T handle) { Destroy(); m_handle = handle; }
	T Detach() { T returnMe = m_handle; m_handle = NULL; return returnMe; }
	T H() const { return m_handle; }
	bool IsValid() { return m_handle != NULL; }
	virtual void DestroyHandle(T handle) { FcDestroy(handle); }
	void Destroy() { if (!IsValid()) return; DestroyHandle(m_handle); m_handle = NULL; }
	void Swap(ResourceWrapper& that) { std::swap(m_handle, that.m_handle); }

private:
	T m_handle;
};

typedef Graphics::Point2d<int> Point;
typedef Graphics::Point2d<int> Size;
typedef Graphics::Rect<int> Rect;

template<typename T>
POINT ToPOINT(const Graphics::Point2d<T>& pt) { POINT ret = { pt.x, pt.y }; return ret; }
inline Point ToPoint(const POINT& pt) { return Point(pt.x, pt.y); }
template<typename T>
RECT ToRECT(const Graphics::Rect<T>& rect) { RECT ret = { rect.tl.x, rect.tl.y, rect.br.x, rect.br.y }; return ret; }
inline Rect ToRect(const RECT& rect) { return Rect(rect.left, rect.top, rect.right, rect.bottom); }





template<typename T>
BOOL WINAPI DeleteObjectWrapper(T handle)
{
	return DeleteObject(handle);
}

template<typename T>
class GdiResource : public ResourceWrapper<T, &DeleteObjectWrapper<T> >
{
public:
	GdiResource(){}
	GdiResource(GdiResource && that) { Swap(that); }
	GdiResource& operator=(GdiResource && that) { Swap(that); return *this; }

	HGDIOBJ HGdi() const { return H(); }
};

class Bitmap : public GdiResource<HBITMAP>
{
public:
	Bitmap(){}
	Bitmap(Bitmap && that) { Swap(that); }
	Bitmap& operator=(Bitmap && that) { Swap(that); return *this; }
};

typedef GdiResource<HBRUSH> Brush;

Brush CreateSolidBrush(COLORREF color);

class DC;

class DCSelect
{
public:
	DCSelect(DC* pDc) : m_pDc(pDc), m_OldObj(NULL){}
	DCSelect(DCSelect && that) : m_pDc(NULL), m_OldObj(NULL) { Swap(that); }
	DCSelect& operator=(DCSelect && that) { Swap(that); }

	~DCSelect();

	void Unselect();
	void Swap(DCSelect& that) { std::swap(m_pDc, that.m_pDc); std::swap(m_OldObj, that.m_OldObj); }

	DC* m_pDc;
	HGDIOBJ m_OldObj;
};

class DC : public ResourceWrapper < HDC, nullptr>
{
public:
	DC() : m_hWnd(NULL) {}
	DC(DC && that):m_hWnd(NULL) { Swap(that); }
	virtual ~DC() { Destroy(); }
	DC& operator=(DC && that) { Swap(that); }
	void Attach(HWND hWnd, HDC hDC) { ResourceWrapper::Attach(hDC); m_hWnd = hWnd; }
	virtual void DestroyHandle(HDC handle) override { ReleaseDC(m_hWnd, handle); }
	void Swap(DC& that) { ResourceWrapper::Swap(that); std::swap(m_hWnd, that.m_hWnd); }

	Bitmap	CreateCompatibleBitmap(const Size& sz);
	DC		CreateCompatibleDC();

	DCSelect Select(HGDIOBJ hGdi);

	template<class T>
	DCSelect Select(GdiResource<T>& res) { return Select(res.HGdi()); }

	void FillRect(const Rect& rect, const Brush& brush);
	void FillRect(const Rect& rect, COLORREF color);

	void BitBlt(const Point& destLoc, DC& dcSource, const Point& sourceLoc, const Size& size, DWORD rop);

	HWND m_hWnd;
};

class PaintDC : public DC
{
public:
	PaintDC(){ memset(&m_ps, 0, sizeof(m_ps)); }
	PaintDC(PaintDC && that){ memset(&m_ps, 0, sizeof(m_ps)); Swap(that); }
	virtual ~PaintDC() { Destroy(); }
	PaintDC& operator=(PaintDC && that) { Swap(that); }
	void Swap(PaintDC& that) { DC::Swap(that); std::swap(m_ps, that.m_ps); }
	virtual void DestroyHandle(HDC handle) override;

	PAINTSTRUCT m_ps;
};

class IWindow
{
public:
	virtual HWND H() const = 0;
	Rect GetClientRect() const;
	Rect GetWindowRect() const;
	void ScreenToClient(Rect& r);
	void ClientToScreen(Rect& r);
	void ScreenToClient(Point& p);
	void ClientToScreen(Point& p);
	PaintDC	Paint();
	DC	 GetDC();
	void Invalidate(bool erase = true);
};

typedef std::shared_ptr<IWindow> PWindow;

struct WndInit
{
	WndInit();
	DWORD	dwExStyle;
	std::wstring ClassName;
	std::wstring WindowName;
	DWORD	dwStyle;
	Rect	rect;
	HWND	hWndParent;
	HMENU	hMenu;
	HINSTANCE hInstance;
	LPVOID	lpParam;
};

class Wnd : public ResourceWrapper<HWND, &DestroyWindow>, public IWindow
{
public:
	virtual HWND H() const override { return ResourceWrapper::H(); }
	static Rect DefaultRect();

	static PWindow Create(const WndInit& init);
};

class WndSubclass : public IWindow
{
public:
	WndSubclass();
	virtual ~WndSubclass();

	void			Detach();
	void			Attach(PWindow pWnd);
	virtual HWND	H() const { return m_pWnd ? m_pWnd->H() : NULL; }
	virtual void	WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

private:
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	static UINT_PTR Sm_iIdGen;

	PWindow		m_pWnd;
	UINT_PTR	m_iIdSubclass;
};

}} //namespace JStd { namespace Wnd {
