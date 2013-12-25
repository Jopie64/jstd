#pragma once

namespace JStd
{
namespace Graphics
{
// Some graphics helpers

template<typename T>
struct Point2d
{
	Point2d() : x(0), y(0) {}
	Point2d(T x, T y) : x(x), y(y) {}
	
	template<typename T2>
	Point2d(const Point2d<T2>& that):x(that.x), y(that.y) {}

	Point2d& operator+= (const Point2d& that) { x += that.x; y += that.y; return *this; }
	Point2d& operator-= (const Point2d& that) { x -= that.x; y -= that.y; return *this; }

	bool operator==(const Point2d& that) const { return x == that.x && y == that.y; }
	bool operator!=(const Point2d& that) const { return x != that.x || y != that.y; }

	T x;
	T y;
};

template<typename T>
struct Rect
{
	typedef Point2d<T> point_type;
	typedef Point2d<decltype(((T) 0) - ((T) 0))>  size_type;
	Rect(){}
	Rect(T l, T t, T r, T b) : tl(l, t), br(r, b){}
	Rect(const Point2d<T>& tl, const Point2d<T>& br) : tl(tl), br(br){}

	bool operator == (const Rect& that) const { return tl == that.tl && br == that.br; }
	bool operator != (const Rect& that) const { return tl != that.tl || br != that.br; }

	point_type CenterPoint() { size_type sz = Size(); return point_type(tl.x + sz.x / 2, tl.y + sz.y / 2); }

	size_type Size() const { point_type ret(br); ret -= tl; return ret; }
	bool PtInRect(const point_type& pt) const
	{
		return
			pt.x >= tl.x &&
			pt.y >= tl.y &&
			pt.x < br.x &&
			pt.y < br.y;
	}

	static Rect FromPoints(const Point2d<T>& tl, const Point2d<T>& br) { return Rect(tl, br); }
	static Rect FromPointSize(const Point2d<T>& tl, Point2d<T> size) { size += tl;  return Rect(tl, size); }

	point_type tl;
	point_type br;
};

}
}

