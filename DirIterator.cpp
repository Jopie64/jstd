#include "StdAfx.h"
#include "DirIterator.h"
#include "JStd.h"
#include <windows.h>


namespace JStd
{

CDirIterator::CDirIterator(const wchar_t* dir)
:	m_Handle(NULL),
	m_bEnd(true)
{
	m_Handle = _wfindfirst64(dir, &m_FindData);
	if(m_Handle == -1)
	{
//		if(errno == ENOENT)
			return; //No entries. At end. Dont throw.
		throw std::runtime_error(JStd::String::Format("Cannot scan directory '%s': %d", JStd::String::ToMult(dir, CP_UTF8).c_str(), errno));
	}
	m_bEnd = false;
}

CDirIterator& CDirIterator::operator++()
{
	if(_wfindnext64(m_Handle, &m_FindData) == -1)
		m_bEnd = true;
	return *this;
}

CDirIterator::~CDirIterator(void)
{
	if(m_Handle != -1)
		_findclose(m_Handle);
}

bool CDirIterator::IsDirectory() const
{
	return File().attrib & _A_SUBDIR;
}



}