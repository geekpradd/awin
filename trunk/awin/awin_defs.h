///////////////////////////////////////////////////////////////////////////////
// This file is part of AWIN class library
// Copyright (c) 2001-2010 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED_T("AS-IS") AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef AWIN_TYPES_H_INCLUDED
#define AWIN_TYPES_H_INCLUDED

//
// STD includes
//
#include "map"
#include "string"
#include "list"
#include "vector"

//
// windows includes
//
#include "tchar.h"
#include "commctrl.h"

//define own namespace
#define NAMESPACE_AWIN	namespace awin{
#define END_NAMESPACE_AWIN		}
#define NAMESPACE_AWIN_UTILS	namespace awin{ namespace utils {
#define END_NAMESPACE_AWIN_UTILS		} }

NAMESPACE_AWIN

typedef TCHAR   awin_char;
typedef std::basic_string<TCHAR>    awin_string;




//-------------------------------------------------------------------------------------------------
//тип символа _char и макрос для создания строк

#if _MSC_VER <= 1200
#define __w64
#endif //_MSC_VER

#ifdef UNICODE
//UNICODE
typedef wchar_t _char;
#define _text(quote) L##quote
#define _sscanf		swscanf
#define _sprintf	swprintf
#else
//ASCII
typedef char _char;
#define _text(quote) quote
#define _sscanf		sscanf
#define _sprintf	sprintf
#endif //_char

//тип позиции (указатель) определен как long для совместимости с позицией list
//естественно, это применимо только в 32битной Windows
typedef PVOID		u_ptr;

typedef size_t		__register;

typedef unsigned long		u_param;
typedef unsigned short		u_short;
typedef unsigned long		u_long;
typedef unsigned char		u_char;
typedef unsigned char		byte;

typedef unsigned __int8		__uint8;
typedef unsigned __int16	__uint16;
typedef unsigned __int32	__uint32;
typedef unsigned __int64	__uint64;

typedef	unsigned long		__ulong32;

//-------------------------------------------------------------------------------------------------
#ifdef	_WIN64

typedef __int64 __lret;
typedef __int64 int_t;
typedef __int64 long_t;
typedef __int64 long_ptr;
#define sizeof_u( _type )	((__uint32)sizeof( _type ))

//-------------------------------------------------------------------------------------------------
#else	//_WIN64

typedef long __w64 __lret;
typedef __int32	__w64 int_t;
typedef long __w64 long_t;
typedef long long_ptr;
#define sizeof_u( _type )	((__uint32)sizeof( _type ))

#endif	//_WIN64
//-------------------------------------------------------------------------------------------------




template <class _class>
class _static_ptr_t
{
	typedef	_class	*_class_ptr;
public:
    _static_ptr_t()
    {        
		if( !instance() ) {
			instance() = (_class_ptr)this;    
		}
	}
    ~_static_ptr_t()
    {        
		if( instance() == (_class_ptr)this ) {
			instance() = 0;    
		}
	}
public:
    static _class *&instance()
    {static _class  *pclass = 0;return pclass;}
};

template<class _class>
class _singelton_t : public _class
{
public:
	_singelton_t() {
		if( static_instance() == 0 ) {
			static_instance() = this;
		}
	}

	~_singelton_t() {
		if( static_instance() == this ) {
			static_instance() = 0;
		}
	}

	static _class *get_instance() {
		static _singelton_t<_class>	instance;
		return static_instance();
	}
private:
	static _class *&static_instance() {
		static _class *ptr;
		return ptr;
	}
};



//
// windows classes
//

class _rect : public RECT
{
public:
	_rect()
	{left = top = right = bottom = 0;}
	_rect( long l, long t, long r, long b )
	{left = l;top = t;right = r, bottom = b;}
	_rect( const RECT &r )
	{left = r.left, right = r.right, top = r.top, bottom = r.bottom;}

	_rect& operator=( const RECT &r )
	{left = r.left, right = r.right, top = r.top, bottom = r.bottom;return *this;}

	bool	pt_in_rect( const POINT &pt ) const
	{	return pt.x >= left && pt.x < right && pt.y >= top && pt.y < bottom;}
	void offset( int dx, int dy )
	{	left+=dx;right+=dx;top+=dy;bottom+=dy;}

	long	width()		const {return right-left;}
	long	height()	const {return bottom-top;}
	long	hcenter()	const {return (left+right)>>1;}
	long	vcenter()	const {return (top+bottom)>>1;}
	bool	is_empty()	const {return width() == 0 && height() == 0;}
	void	empty()			{top=bottom=left=right=0;}

	POINT	*as_points()	{return (POINT*)(RECT*)this;}
};

class _size : public SIZE
{
public:
	_size()	
	{cx = cy = 0;}
	_size( int dx, int dy )	
	{cx = dx; cy = dy;}
};

class _point : public POINT
{
public:
	_point()			{x = y = 0;}
	_point( __lret l )	{y = short(l>>16); x = short(l&0xFFFF);}
	_point( const POINT &p )	{x = p.x; y = p.y;}
	_point( const POINTL &p )	{x = p.x; y = p.y;}
	_point( int _x, int _y)		{x = _x; y = _y;}
};

#define CW_DEFAULT_RECT	_rect( CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT ) 


#define reference_counter_impl	reference_impl



END_NAMESPACE_AWIN

#endif //AWIN_TYPES_H_INCLUDED
