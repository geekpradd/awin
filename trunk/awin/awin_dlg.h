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

#ifndef AWIN_DLG_H_INCLUDED
#define AWIN_DLG_H_INCLUDED

#include "awin_window.h"
#include "awin_module.h"

NAMESPACE_AWIN

#define RT_DLGINIT  MAKEINTRESOURCE(240)

inline 
__lret __stdcall subclass_dlg_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
{
	if( m == WM_INITDIALOG )
	{
		win_impl	*pwi = (win_impl*)l;
		pwi->subclass( hwnd, true );

		return pwi->handle_message( m, w, l );
	}
	return 0;//DefDlgProc( hwnd, m, w, l );
}

template<class _win_impl>
class _dlg_impl_t : public _win_impl
{
public:
	typedef _dlg_impl_t<_win_impl>	dlg_impl;
public:
	_dlg_impl_t( unsigned id );

	unsigned	get_id() const	{return m_idTemplate;}
public:
	__lret do_modal( HWND hwndParent );
	int do_modeless( HWND hwndParent );
public:
	virtual void on_ok()				{end_dialog( IDOK );}
	virtual void on_cancel()			{end_dialog( IDCANCEL );}
	virtual void end_dialog( int idc )	{m_result = idc; EndDialog( handle(), idc );}
protected:
	virtual __lret on_initdialog();
	virtual __lret translate( MSG *pmsg )	{return ::IsDialogMessage( handle(), pmsg );}
	virtual __lret on_paint()		{return call_default();};
	virtual __lret on_command( __uint32 cmd );
	virtual __lret handle_translate( MSG *pmsg )	{return IsDialogMessage( handle(), pmsg );};
//	virtual __lret on_helpinfo( HELPINFO *phi ) {
//		return display_help_popup( module::hinst(), handle(), phi->dwContextId, phi->MousePos ); }
protected:
	__uint32	m_idTemplate;
	long	m_result;
};

template<class _win_impl> inline
_dlg_impl_t<_win_impl>::_dlg_impl_t( __uint32 id )
{		m_result = -2; m_idTemplate = id;		m_proc_def = (WNDPROC)DefDlgProc;	}

template<class _win_impl> inline
__lret _dlg_impl_t<_win_impl>::do_modal( HWND hwndParent )
{
/*	HRSRC	hrsrc = ::FindResource( module::hrc(), MAKEINTRESOURCE(m_idTemplate), RT_DIALOG );
	if( !hrsrc )return -1;
	DLGTEMPLATE	*ptempl = (DLGTEMPLATE*)LoadResource( module::hrc(), hrsrc );

	return ::DialogBoxIndirectParam( module::hinst(), ptempl, hwndParent, subclass_dlg_proc, (long)this );*/
	INT_PTR ret_code = ::DialogBoxParam( module::hrc(), MAKEINTRESOURCE(m_idTemplate), hwndParent, (DLGPROC)subclass_dlg_proc, (__lret)this );

	DWORD	dw = 0;
	if( ret_code <= 0 ) {
		dw = ::GetLastError();
	}
	return ret_code;

}

template<class _win_impl> inline
int _dlg_impl_t<_win_impl>::do_modeless( HWND hwndParent )
{
	HRSRC	hrsrc = ::FindResource( module::hrc(), MAKEINTRESOURCE(m_idTemplate), RT_DIALOG );
	if( !hrsrc )return -1;
	DLGTEMPLATE	*ptempl = (DLGTEMPLATE*)LoadResource( module::hrc(), hrsrc );

	return ::CreateDialogIndirectParam( module::hinst(), ptempl, hwndParent, (DLGPROC)subclass_dlg_proc, (__lret)this )!=0;
}

template<class _win_impl> inline
__lret _dlg_impl_t<_win_impl>::on_command( __uint32 cmd )
{
	switch( cmd )
	{
	case IDOK:on_ok();return true;
	case IDCANCEL:on_cancel();return true;
	};
	return false;
}

template<class _win_impl> inline
__lret _dlg_impl_t<_win_impl>::on_initdialog()
{
	__lret lres = _win_impl::on_initdialog();

	//load from RT_INITDLG resource
	HRSRC hrc = ::FindResource( module::hrc(), MAKEINTRESOURCE(m_idTemplate), RT_DLGINIT );
	if( !hrc )return lres;

	HGLOBAL	hres = ::LoadResource( module::hrc(), hrc );
	if( !hres )return lres;

	WORD *pres = (WORD*)::LockResource( hres );

	while( *pres )
	{
		unsigned	idc = *pres++;
		unsigned	m = *pres++;

		unsigned	size = *(unsigned*)pres;
		pres+=2;
//translate old messages
#define WIN16_LB_ADDSTRING  0x0401
#define WIN16_CB_ADDSTRING  0x0403
#define AFX_CB_ADDSTRING    0x1234

		if (m == WIN16_LB_ADDSTRING)
			m = LB_ADDSTRING;
		else if (m == WIN16_CB_ADDSTRING)
			m = CB_ADDSTRING;
#ifdef _WINDOWS_
		else if (m == AFX_CB_ADDSTRING)
			m = CBEM_INSERTITEM;
#endif //_WINDOWS_
//combo_ex
		if (m == LB_ADDSTRING || m == CB_ADDSTRING)
		{
			::SendDlgItemMessage( handle(), idc, m, 0, (LPARAM)pres );
		}
#ifdef _WINDOWS_
		else if (m == CBEM_INSERTITEM)
		{
			COMBOBOXEXITEM item;
			item.mask = CBEIF_TEXT;
			item.iItem = -1;
			item.pszText = (_char*)pres;

			::SendDlgItemMessage( handle(), idc, m, 0, (LPARAM)&item );
		}
#endif //_WINDOWS_
		
		pres = (WORD*)((byte*)pres+size);
	}


#ifdef _WINDOWS_
	//we don't need to free locked resources under CE
	::FreeResource( hres );
#endif //_WINDOWS_

#ifdef _NO_DLG_DEBUG_SYMBOLS
	add_debug_symbols( handle() );
#endif //_NO_DLG_DEBUG_SYMBOLS

	return lres;
}

typedef _dlg_impl_t<win_impl>	dlg_impl;

END_NAMESPACE_AWIN

#endif //AWIN_DLG_H_INCLUDED