#pragma once

#include <io.h>

namespace JStd
{

class CDirIterator
{
public:
	CDirIterator(const wchar_t* dir);
	virtual ~CDirIterator(void);

	operator bool()const{return !m_bEnd;}

	CDirIterator& operator++();

	const _wfinddata64_t&	File() const {return m_FindData;}

	bool					IsDirectory() const;


private:

	intptr_t		m_Handle;
	_wfinddata64_t	m_FindData;
	bool			m_bEnd;
};


}