/*
This file is part of AWIN class library
Copyright (c) 2001-2010 Andy Yamov

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You can receive a copy of the GNU Lesser General Public License from 
http://www.gnu.org/
*/

#ifndef AWIN_WINDOW_H_INCLUDED
#define AWIN_WINDOW_H_INCLUDED

#include "awin_module.h"

inline 
HWND GetTopLevelParent( HWND hwnd )
{
	HWND	hwnd_p = GetParent( hwnd );
	while( hwnd_p )
	{
		hwnd = hwnd_p;
		hwnd_p = GetParent( hwnd );
	}
	return hwnd;
}


#define WM_GETINTERFACE		(WM_USER+779)
#define WM_NOTYFYREFLECT	(WM_USER+1002)
#define WM_WINDOWFOCUSED	(WM_USER+1003)


NAMESPACE_AWIN

class message_filter 
{
public:
    message_filter() {
    }
	virtual ~message_filter() {
    }

    virtual __lret handle_message( __uint32 m, WPARAM w, LPARAM l ) {
        return 0;
    }
};

__lret __stdcall subclass_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l );
__lret __stdcall subclassed_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l );
__lret __stdcall subclass_mdi_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l );

class win_class
{
public:
	static const _char	*get_class_default()
	{
		static bool registred = 0;
		static _char szDefClass[] = 
#if defined (_X86_)
			_T("defwindowclass_x86");
#elif defined (_AMD64_)
			_T("defwindowclass_amd64");
#endif //

		if( !registred )
		{
			WNDCLASS	wndclass;
			ZeroMemory( &wndclass, sizeof( wndclass ) );
			wndclass.lpszClassName = szDefClass;
			wndclass.hInstance = module::hinst();
			wndclass.lpfnWndProc = subclass_proc;
			wndclass.style	= CS_DBLCLKS;
			wndclass.hbrBackground = ::GetSysColorBrush( COLOR_WINDOW );
			wndclass.hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));

			ATOM a = ::RegisterClass( &wndclass );	

			if( !a )	{
				assert( false );
				DWORD	dw = ::GetLastError();
				return 0;
			}
			registred = true;
		}
		return szDefClass;
	}
	static const _char	*get_class_mdi()
	{
		static bool registred = 0;
		static _char szDefClass[] = _T("defmdiclass");

		if( !registred )
		{
			WNDCLASS	wndclass;
			ZeroMemory( &wndclass, sizeof( wndclass ) );
			wndclass.lpszClassName = szDefClass;
			wndclass.hInstance = module::hinst();
			wndclass.lpfnWndProc = subclass_mdi_proc;
			wndclass.hbrBackground = (HBRUSH)::GetStockObject( WHITE_BRUSH );
			wndclass.hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));

			::RegisterClass( &wndclass );		
			registred = true;
		}
		return szDefClass;
	}
};


//__declspec(novtable)
class win_impl : public message_filter
{
public:
    typedef std::map<HWND, win_impl*>	by_handle;
	static by_handle &get_route_map()	{return * _singelton_t<by_handle>::get_instance();}
protected:
	static void register_window( win_impl *pwin );
	static void unregister_window( win_impl *pwin );
public:
	win_impl();
	virtual ~win_impl();
public:
	virtual unsigned get_proc_code()	{return GWLP_WNDPROC;}
	virtual bool subclass( HWND hwnd, bool f_nccreate = false );
	virtual __lret detach( bool call_old_proc = true );

	virtual bool create_ex_hmenu( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0, DWORD ex_style = 0 );

	virtual bool create( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, unsigned ctrl_id = 0, const _char *pszClass = 0 ) {
		return create_ex_hmenu( style, rect, pszTitle, parent, (HMENU)(size_t)ctrl_id, pszClass, 0 );
	}
	virtual bool create_ex( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, unsigned ctrl_id, const _char *pszClass = 0, DWORD ex_style = 0 ){
		return create_ex_hmenu( style, rect, pszTitle, parent, (HMENU)(size_t)ctrl_id, pszClass, ex_style );
	}
public:
	HWND	handle() const	{return m_hwnd;}
public:
	virtual void	handle_init(){};
	virtual __lret	handle_message( UINT m, WPARAM w, LPARAM l );
protected:
	virtual __lret on_create( CREATESTRUCT *pcs )			{handle_init();return call_default();};
	virtual __lret on_close()								{return call_default();};
	virtual __lret on_paint();
	virtual __lret on_destroy();
	virtual __lret on_size( short cx, short cy, __ulong32 type )	{return call_default();}
	virtual __lret on_show( bool bShow, long status )		{return call_default();}
	virtual __lret on_initdialog()						{return 0;};
	virtual __lret on_timer( __ulong32 lEvent )				{return call_default();};
	virtual __lret on_command( __uint32 cmd )					{return call_default();};
	virtual __lret on_syscommand( __uint32 cmd )				{return call_default();};
	virtual __lret on_mousemove( const _point &point )	{return call_default();};
	virtual __lret on_lbuttondown( const _point &point )	{return call_default();};
	virtual __lret on_lbuttonup( const _point &point )	{return call_default();};
	virtual __lret on_rbuttondown( const _point &point )	{return call_default();};
	virtual __lret on_rbuttonup( const _point &point )	{return call_default();};
	virtual __lret on_keydown( long nVirtKey )			{return call_default();}
	virtual __lret on_keyup( long nVirtKey )				{return call_default();}
	virtual __lret on_char( long nVirtKey )				{return call_default();}
	virtual __lret on_getdlgcode( MSG* p )				{return call_default();}
	virtual __lret on_notify( __uint32 idc, NMHDR *pnmhdr )						{return notify_reflect();}
	virtual __lret on_measureitem( int idc, MEASUREITEMSTRUCT *pmsr )			{return notify_reflect();}
	virtual __lret on_drawitem( int idc, DRAWITEMSTRUCT *pdrw )				{return notify_reflect();}
	virtual __lret on_compareitem( int idc, COMPAREITEMSTRUCT *pdrw )			{return notify_reflect();}
	virtual __lret on_setfocus( HWND hwndOld )			{return call_default();};
	virtual __lret on_killfocus( HWND hwndOld )			{return call_default();};
	virtual __lret on_erasebkgnd( HDC )					{return call_default();}
	virtual __lret on_hscroll( unsigned code, unsigned pos, HWND hwndScroll )	{return call_default();}
	virtual __lret on_vscroll( unsigned code, unsigned pos, HWND hwndScroll )	{return call_default();}
	virtual __lret on_setcursor( unsigned code, unsigned hit )				{return call_default();}
	virtual __lret on_activate( unsigned code, HWND hwnd )					{return call_default();}
	virtual __lret on_activate_app( BOOL activate )					{return call_default();}

	virtual __lret call_default();
	virtual __lret notify_reflect();

	virtual __lret handle_reflect_message( MSG *pmsg, long *plProcessed );
	virtual __lret on_notify_reflect( NMHDR *pnmhdr, long *plProcessed )		{return 0;}
	virtual __lret on_getinterface()							{return 0;}
	virtual __lret on_windowfocused( HWND hwnd )				{return 0;}
	virtual __lret on_lbuttondblclk( const _point &point )	{return call_default();};
	virtual __lret on_rbuttondblclk( const _point &point )	{return call_default();};
	virtual __lret on_contextmenu( const _point &point )		{return call_default();}
	virtual __lret on_cancelmode()							{return call_default();}
	virtual __lret on_initmenupopup( HMENU h, unsigned flags )	{return call_default();}
	virtual __lret on_enable( bool enable )						{return call_default();}
	virtual __lret on_wininichange( const char *psz_sect, unsigned index )	{return call_default();}
#ifdef _WINDOWS_
	virtual __lret on_mouseactivate( unsigned hit, unsigned message )			{return call_default();}
	virtual __lret on_mouseleave()								{return call_default();}
	virtual __lret on_nccalcsize( NCCALCSIZE_PARAMS *p, bool full_params )	{return call_default();}
	virtual __lret on_getminmaxinfo( MINMAXINFO *pmminfo ){return call_default();};
	virtual __lret on_dropfiles( HANDLE hdrop )			{return call_default();};
	virtual __lret on_nchittest( const _point &point )		{return call_default();};
	virtual __lret on_nclbuttondown( const _point &point )	{return call_default();};
	virtual __lret on_nclbuttonup( const _point &point )		{return call_default();};
	virtual __lret on_ncmousemove( const _point &point )						{return call_default();}
	virtual __lret on_ncpaint( HRGN h )										{return call_default();}
	virtual __lret on_ncdestroy();
	virtual __lret on_helpinfo( HELPINFO *phi )				{return call_default();}
	virtual __lret on_ctlcolor( unsigned msg, HWND hwnd )	{return notify_reflect();}
#endif //_WINDOWS_
public:	//wrappers
/*	HWND get_dlg_item( UINT idc )								{return ::GetDlgItem( handle(), idc );}
	long get_window_rect( RECT *pr )							{return ::GetWindowRect( handle(), pr ); }
	long get_client_rect( RECT *pr )							{return ::GetClientRect( handle(), pr ); }
	long move_window( const RECT &rect, long lRepaint = true )	{return ::MoveWindow( handle(), rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, lRepaint );}
	long send_message( UINT m, WPARAM w = 0, LPARAM l = 0)		{return ::SendMessage( handle(), m, w, l );}
	long get_dlg_item_text( UINT idc, _char *psz, int cb )		{return ::GetDlgItemText( handle(), idc, psz, cb );}
	long is_dlg_button_checked( UINT idc )						{return ::IsDlgButtonChecked( handle(), idc );}
	long check_dlg_button( UINT idc, int check )				{return ::CheckDlgButton( handle(), idc, check );}
	long set_dlg_item_text( UINT idc, const _char *psz )			{return ::SetDlgItemText( handle(), idc, psz );}*/
	

	void	redraw( RECT *prect, long erase ) const;

protected:
	HWND			m_hwnd;
	MSG				m_current_message;
public:
	long			m_lock_delete, 
					m_kill_on_destroy, 
					m_call_old;
protected:
	WNDPROC			m_proc_old, m_proc_def;

};


inline __lret win_impl::notify_reflect()
{
	HWND	hwnd = 0;

	unsigned	m = m_current_message.message;
	if( m == WM_NOTIFY )
		hwnd = ((NMHDR*)m_current_message.lParam)->hwndFrom;
	else if( m == WM_DRAWITEM || m == WM_MEASUREITEM || m == WM_COMPAREITEM ) {
		hwnd = ::GetDlgItem( handle(), (int)m_current_message.wParam );
	}
	else if( m == WM_CTLCOLORMSGBOX ||
		m == WM_CTLCOLOREDIT || m == WM_CTLCOLORLISTBOX || m == WM_CTLCOLORBTN ||
		m == WM_CTLCOLORDLG || m == WM_CTLCOLORSCROLLBAR || m == WM_CTLCOLORSTATIC ) {
		hwnd = (HWND)m_current_message.lParam;
	}
	else {
		return 0;
	}
	
	
	long	lProcessed = false;
	__lret	lret_reflect = 0;
	
	if( hwnd )
		lret_reflect = SendMessage( hwnd, WM_NOTYFYREFLECT, (WPARAM)&lProcessed, (LPARAM)&m_current_message );
	
	return lProcessed?lret_reflect:call_default();
};



inline __lret	win_impl::on_paint()
{
	if( m_proc_old )return call_default();;

	PAINTSTRUCT	paint;
	HDC	hdc = ::BeginPaint( m_hwnd, &paint );
	::EndPaint( m_hwnd, &paint );
	return 1;
}

#ifdef _WINDOWS_
inline __lret	win_impl::on_ncdestroy()
{
	return detach();
}
#endif //_WINDOWS_

inline __lret	win_impl::on_destroy()
{
#ifdef _WINDOWS_
	return call_default();
#else
	return detach();
#endif //_WINDOWS_
}

inline 
win_impl::win_impl()
{
	m_call_old = false;
	m_proc_old = 0;
	m_proc_def = 0;

	m_kill_on_destroy = 0;
	m_lock_delete = 0;
	m_hwnd = 0;
	memset( &m_current_message, 0, sizeof( m_current_message ) );
	m_proc_def = DefWindowProc;
}

inline
win_impl::~win_impl()
{
}

inline bool win_impl::subclass( HWND hwnd, bool f_nccreate )
{
	if( !hwnd ) {
		return false;
	}

	m_call_old = !f_nccreate;

	m_proc_old = (WNDPROC)(LONG_PTR)::GetWindowLongPtr( hwnd, get_proc_code() );
	m_hwnd = hwnd;

	win_impl::register_window( this );
	::SetWindowLongPtr( m_hwnd, get_proc_code(), (__lret)subclassed_proc );

    if( !f_nccreate )
		handle_init();


#ifdef _DEBUG_WINDOW_PROC
	_trace_file( 0, "[~] win_impl::m_thunk:%08X, module %08X, hwnd=%08X", &m_thunk, module::hinst(), m_hwnd );
#endif //_DEBUG_WINDOW_PROC

	return true;
}

inline __lret win_impl::detach( bool call_old_proc )
{
	if( !m_hwnd || !m_proc_old )return false;

#ifdef _DEBUG_WINDOW_PROC
	_trace_file( 0, "[~] win_impl::detach hwnd=%08X", m_hwnd );
#endif //_DEBUG_WINDOW_PROC

#pragma warning(disable:4311)
	::SetWindowLongPtr( m_hwnd, get_proc_code(), (long_ptr)m_proc_old );
#pragma warning(default:4311)

	__lret	ret_code = 0;
	if( call_old_proc )
		ret_code = CallWindowProc( m_proc_old, m_hwnd, m_current_message.message,
			m_current_message.wParam, m_current_message.lParam );

	win_impl::unregister_window( this );

    m_hwnd  = 0;
	m_proc_old = 0;

	return ret_code;
}


inline
bool win_impl::create_ex_hmenu( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass, DWORD ex_style )
{
	m_hwnd = 0;

	int	cx = (rect.right == CW_USEDEFAULT)?CW_USEDEFAULT:rect.right-rect.left;
	int	cy = (rect.bottom == CW_USEDEFAULT)?CW_USEDEFAULT:rect.bottom-rect.top;
	HWND hwnd = CreateWindowEx( ex_style, pszClass?pszClass:win_class::get_class_default(), pszTitle, style, 
							rect.left, rect.top, cx, cy,
							parent, hmenu, module::hinst(), this );
	if( !hwnd )		{
		unsigned	dw = ::GetLastError();
		assert( false );
		return false;
	}

	if( !m_hwnd )
		return subclass( hwnd );
	return true;
}

inline 
__lret	win_impl::handle_message( UINT m, WPARAM w, LPARAM l )
{
	__lret	lres = 0;
//store old message
	m_lock_delete++;

	MSG	last_current;
	memcpy( &last_current, &m_current_message, sizeof(m_current_message) );
	m_current_message.message = m;
	m_current_message.lParam = l;
	m_current_message.wParam = w;

	if( m == WM_SETFOCUS )
	{
		HWND	hwnd_top = ::GetTopLevelParent( handle() );
		if( hwnd_top != handle() )
			::SendMessage( hwnd_top, WM_WINDOWFOCUSED, 0, (LPARAM)handle() );
	}

	struct internal_by16
	{
		__uint16	lo;
		__uint16	hi;
	};
	union internal_param
	{
		size_t		u;
		__uint32	u32;
		__uint16	u16;

		__int32		i32;
		__int16		i16;

		internal_by16	by16;

	};	
	
	internal_param	lp, wp;

	lp.u = l;
	wp.u = w;

//process message map
	switch( m )
	{
	case WM_CREATE:			lres = on_create( (CREATESTRUCT*)l );break;
	case WM_PAINT:			lres = on_paint();break;
	case WM_DESTROY:		lres = on_destroy();break;
	case WM_CLOSE:			lres = on_close();break;
	case WM_SIZE:			lres = on_size( lp.by16.lo, lp.by16.hi, wp.u32 );break;
	case WM_SHOWWINDOW:		lres = on_show( w != 0, lp.i32 );break;
	case WM_INITDIALOG:		lres = on_initdialog();break;
	case WM_COMMAND:		lres = on_command( wp.u32 );break;
	case WM_SYSCOMMAND:		lres = on_syscommand( wp.u16 );break;
	case WM_TIMER:			lres = on_timer( wp.u32 );break;
	case WM_LBUTTONDOWN:	lres = on_lbuttondown( _point( lp.i32 ) );break;
	case WM_LBUTTONUP:		lres = on_lbuttonup( _point( lp.i32 ) );break;
	case WM_RBUTTONDOWN:	lres = on_rbuttondown( _point( lp.i32 ) );break;
	case WM_RBUTTONUP:		lres = on_rbuttonup( _point( lp.i32 ) );break;
	case WM_MOUSEMOVE:		lres = on_mousemove( _point( lp.i32 ) );break;
	case WM_LBUTTONDBLCLK:	lres = on_lbuttondblclk( _point( lp.i32 ) );break;
	case WM_RBUTTONDBLCLK:	lres = on_rbuttondblclk( _point( lp.i32 ) );break;
	case WM_GETINTERFACE:	lres = on_getinterface();break;
	case WM_NOTIFY:			lres = on_notify( wp.u32, (NMHDR*)l );break;
	case WM_NOTYFYREFLECT:	lres = handle_reflect_message( (MSG*)l, (long*)w );break;
	case WM_SETFOCUS:		lres = on_setfocus( (HWND)w );break;
	case WM_KILLFOCUS:		lres = on_killfocus( (HWND)w );break;
	case WM_CHAR:			lres = on_char( wp.u32 );break;
	case WM_KEYDOWN:		lres = on_keydown( wp.u32 );break;
	case WM_KEYUP:			lres = on_keyup( wp.u32 );break;
	case WM_GETDLGCODE:		lres = on_getdlgcode( (MSG *)l );break;
	case WM_ERASEBKGND:		lres = on_erasebkgnd( (HDC)w );break; 
	case WM_HSCROLL:		lres = on_hscroll( wp.by16.lo, wp.by16.hi, (HWND)l );break;
	case WM_VSCROLL:		lres = on_vscroll( wp.by16.lo, wp.by16.hi, (HWND)l );break;
	case WM_DRAWITEM:		lres = on_drawitem( wp.u32, (DRAWITEMSTRUCT*)l );break;
	case WM_MEASUREITEM:	lres = on_measureitem( wp.u32, (MEASUREITEMSTRUCT*)l );break;
	case WM_COMPAREITEM:	lres = on_compareitem( wp.u32, (COMPAREITEMSTRUCT*)l );break;
	case WM_SETCURSOR:		lres = on_setcursor( HIWORD(l), LOWORD(l) );break;
	case WM_WINDOWFOCUSED:	lres = on_windowfocused( (HWND)l );break;
	case WM_ACTIVATE:		lres = on_activate( LOWORD(w), (HWND)l );break;
	case WM_ACTIVATEAPP:	lres = on_activate_app( w!=0 );break;
	case WM_CONTEXTMENU:	lres = on_contextmenu( _point( l ) );break;
	case WM_CANCELMODE:		lres = on_cancelmode();break;
	case WM_INITMENUPOPUP:  lres = on_initmenupopup( (HMENU)w, lp.u32 );break;
	case WM_ENABLE:			lres = on_enable( w!=0 );break;
	case WM_SETTINGCHANGE:	lres = on_wininichange( (const char*)l, wp.u32 );break;
#ifdef _WINDOWS_
	case WM_MOUSEACTIVATE:	lres = on_mouseactivate( LOWORD(l), HIWORD(l) );break;
	case WM_MOUSELEAVE:		lres = on_mouseleave();break;
	case WM_NCCALCSIZE:		lres = on_nccalcsize( (NCCALCSIZE_PARAMS *)l, w != 0 );break;
	case WM_GETMINMAXINFO:	lres = on_getminmaxinfo( (MINMAXINFO *)l );break;
	case WM_DROPFILES:		lres = on_dropfiles( (HANDLE)w );break;
	case WM_NCHITTEST:		lres = on_nchittest( _point( l ) );break;
	case WM_NCLBUTTONDOWN:	lres = on_nclbuttondown( _point( l ) );break;
	case WM_NCLBUTTONUP:	lres = on_nclbuttonup( _point( l ) );break;
	case WM_NCMOUSEMOVE:	lres = on_ncmousemove( _point( l ) );break;
	case WM_NCPAINT:		lres = on_ncpaint( (HRGN)w );break;
	case WM_NCDESTROY:		lres = on_ncdestroy();break;
	case WM_HELP:			lres = on_helpinfo( (HELPINFO*)l );break;
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:	lres = on_ctlcolor( m, (HWND)l );break;
#endif //_WINDOWS_
	default:lres = call_default();
	}
//restore original message	
	memcpy( &m_current_message, &last_current, sizeof(m_current_message) );
	m_lock_delete--;

	if( !m_hwnd && m_lock_delete == 0 && m_kill_on_destroy )
		delete this;

	return lres ;
}

inline __lret win_impl::handle_reflect_message( MSG *pmsg, long *plProcessed )
{
	if( pmsg->message == WM_NOTIFY )
		return on_notify_reflect( (NMHDR*)pmsg->lParam, plProcessed );
	return 0;
}

inline __lret win_impl::call_default()
{
	if( m_proc_old && m_call_old )
		return CallWindowProc( m_proc_old, handle(), m_current_message.message, m_current_message.wParam, m_current_message.lParam );
	if( m_proc_def )
		return CallWindowProc( m_proc_def, handle(), m_current_message.message, m_current_message.wParam, m_current_message.lParam );
	return 0;
}


inline void	win_impl::redraw( RECT *prect, long erase ) const
{
	unsigned	style = (unsigned)::GetWindowLong( handle(), GWL_STYLE );
	::InvalidateRect( handle(), prect, erase );

	if( style & WS_CHILD )
	{
		HWND	hwnd_p = ::GetParent( handle() );
		_rect	rect;
		if( !prect )
		{
			::GetClientRect( handle(), &rect );
			prect = &rect;
		}
		::MapWindowPoints( handle(), hwnd_p, (POINT*)prect, 2 );

		::InvalidateRect( hwnd_p, prect, erase );
	}
}

inline 
__lret __stdcall subclass_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
{
// message to subclass for windows / windows x64
#if defined( _WINDOWS_)
	if( m == WM_NCCREATE )
#else 
	if( m == WM_CREATE )
#endif //
	{
		CREATESTRUCT	*pcs = (CREATESTRUCT*) l;
		win_impl	*pwi = (win_impl*)pcs->lpCreateParams;
		pwi->subclass( hwnd, true );
		return pwi->handle_message( m, w, l );
	}
	return 0;
}



inline
void win_impl::register_window( win_impl *pwin )
{
	by_handle	&map = get_route_map();
	map[pwin->handle()] = pwin;
}

inline
void win_impl::unregister_window( win_impl *pwin )
{
	by_handle	&map = get_route_map();
	map.erase( pwin->handle() );
}


inline 
__lret __stdcall subclassed_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
{
	win_impl::by_handle	&map = win_impl::get_route_map();

    win_impl::by_handle::const_iterator   i = map.find( hwnd );
    if( i == map.end() ) {
        return 0;
    }

    return i->second->handle_message( m, w, l );
}

END_NAMESPACE_AWIN

#endif //AWIN_WINDOW_H_INCLUDED
