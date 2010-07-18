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

#ifndef AWIN_PAGE_H_INCLUDED
#define AWIN_PAGE_H_INCLUDED

#include "awin_dlg.h"
#include "awin_utils.h"

NAMESPACE_AWIN

#define		ID_WIZ_BTN_APPLY	0x3021
#define		ID_WIZ_BTN_BACK		0x3023
#define		ID_WIZ_BTN_NEXT		0x3024
#define		ID_WIZ_BTN_FINISH	0x3025
#define		ID_WIZ_BTN_CANCEL	IDCANCEL
#define		ID_WIZ_BTN_HELP		0x9

class page_impl : public dlg_impl
{
public:
	page_impl( unsigned id ) : dlg_impl( id )
	{
	}

	~page_impl() {
		//unregister_id_mapping( (long_t)m_idTemplate );
	}

	void set_modified( bool is_modified = true )
	{
		if( is_modified )
			PropSheet_Changed( GetParent( handle() ), handle() );
		else
			PropSheet_UnChanged( GetParent( handle() ), handle() );
	}


	virtual __lret on_wizard_next()			{return 0;}
	virtual __lret on_wizard_back()			{return 0;}
	virtual __lret on_wizard_finish()		{return 0;}
	virtual __lret on_setactive()			{return 0;}
	virtual __lret on_killactive()			{return 0;}
	virtual __lret on_query_cancel()		{return 0;}


	virtual __lret on_notify( __uint32 idc, NMHDR *pnmhdr )
	{
		if( idc == 0 && pnmhdr->code == PSN_APPLY )
			on_ok();

		__lret lres = 0;

		if( pnmhdr->code == PSN_WIZNEXT )
			lres = on_wizard_next();
		else if( pnmhdr->code == PSN_WIZBACK )
			lres = on_wizard_back();
		else if( pnmhdr->code == PSN_WIZFINISH )
			lres = on_wizard_finish();
		else if( pnmhdr->code == PSN_SETACTIVE )
			lres = on_setactive();
		else if( pnmhdr->code == PSN_KILLACTIVE )
			lres = on_killactive();
		else if( pnmhdr->code == PSN_QUERYCANCEL )
			lres = on_query_cancel();

		if( lres )return lres;

		return dlg_impl::on_notify( idc, pnmhdr );
	}

	virtual void on_ok()		
	{
		set_modified( false );
	}

	virtual bool subclass( HWND hwnd, bool f_nccreate = false )
	{
		if( !win_impl::subclass( hwnd, f_nccreate ) )return false;
//		SetWindowLong( hwnd, DWL_DLGPROC, (LONG)&m_thunk );
		return true;
	}

	virtual __lret handle_message( UINT m, WPARAM w, LPARAM l )
	{
		__lret lres = dlg_impl::handle_message( m, w, l );
		if( m == WM_NOTIFY )
		{
			if( lres )
			{
				::SetWindowLongPtr( handle(), DWLP_MSGRESULT, (LONG_PTR)lres );
				return lres;
			}
		}
		return lres;
	}


	static UINT CALLBACK __create_proc(
		HWND hwnd,
		UINT m,
		PROPSHEETPAGE *ppsp
	)
	{
		if( m == PSPCB_CREATE )
		{
			page_impl	*p = (page_impl*)ppsp->lParam;
			p->subclass( hwnd, false );
		}
		return 1;
	}

	static 
	__lret __stdcall subclass_page_proc(HWND hwnd, UINT m, WPARAM w, LPARAM l )
	{
		if( m == WM_INITDIALOG )
		{
			PROPSHEETPAGE	*p = (PROPSHEETPAGE*)l;
			win_impl	*pwi = (win_impl*)p->lParam;
			pwi->subclass( hwnd, true );
			return pwi->handle_message( m, w, l );
		}
		return 0;//DefDlgProc( hwnd, m, w, l );
	}


	long create_page( PROPSHEETPAGE *ppage )
	{
		HRSRC	hrsrc = ::FindResource( module::hrc(), MAKEINTRESOURCE( m_idTemplate ), RT_DIALOG );
		if( !hrsrc )return 0;

		ZeroMemory( ppage, sizeof( *ppage ) );
		ppage->dwSize = sizeof( *ppage );
		ppage->dwFlags = /*PSP_DEFAULT|PSP_USECALLBACK|*/PSP_DLGINDIRECT;
		ppage->hInstance = module::hrc();
		ppage->pResource = (DLGTEMPLATE*)::LoadResource( module::hrc(), hrsrc );
		ppage->pszTitle = 0;
		ppage->pfnDlgProc = (DLGPROC)subclass_page_proc;
		ppage->lParam = (LPARAM)this;
		ppage->pfnCallback = 0;

		//register_id_mapping( (long_t)m_idTemplate, (long_t)ppage->pResource );

		return true;
	}

	long create_page( PROPSHEETPAGE *ppage, const _char* psz_title )
	{
		HRSRC	hrsrc = ::FindResource( module::hrc(), MAKEINTRESOURCE( m_idTemplate ), RT_DIALOG );
		if( !hrsrc )return 0;

		ZeroMemory( ppage, sizeof( *ppage ) );
		ppage->dwSize = sizeof( *ppage );
		ppage->dwFlags = /*PSP_DEFAULT|PSP_USECALLBACK|*/PSP_DLGINDIRECT|(psz_title?PSP_USETITLE:0);
		ppage->hInstance = module::hrc();
		ppage->pResource = (DLGTEMPLATE*)::LoadResource( module::hrc(), hrsrc );
		ppage->pszTitle = psz_title;
		ppage->pfnDlgProc = (DLGPROC)subclass_page_proc;
		ppage->lParam = (LPARAM)this;
		ppage->pfnCallback = 0;

		//register_id_mapping( (long_t)m_idTemplate, (long_t)ppage->pResource );

		return true;
	}

	long create_page( PROPSHEETPAGE *ppage, DWORD flags, __uint32 ids_title, __uint32 ids_subtitle )
	{
		HRSRC	hrsrc = ::FindResource( module::hrc(), MAKEINTRESOURCE( m_idTemplate ), RT_DIALOG );
		if( !hrsrc )return 0;

		ZeroMemory( ppage, sizeof( *ppage ) );

		ppage->dwSize			= sizeof( PROPSHEETPAGE );
		ppage->dwFlags			= flags|PSP_DLGINDIRECT;
		ppage->hInstance		= module::hrc();
		//ppage->pszTemplate		= MAKEINTRESOURCE( m_idTemplate );
		ppage->pResource		= (DLGTEMPLATE*)::LoadResource( module::hrc(), hrsrc );
		ppage->pszHeaderTitle	= MAKEINTRESOURCE( ids_title );
		ppage->pszHeaderSubTitle= MAKEINTRESOURCE( ids_subtitle );
		ppage->pfnDlgProc		= (DLGPROC)subclass_page_proc;
		ppage->lParam			= (LPARAM)this;
		ppage->pfnCallback		= 0;


		//register_id_mapping( (long_t)m_idTemplate, (long_t)ppage->pResource );

		return true;
	}

/*private:
	// тип для карты
	typedef std::map<long_t, long_t>	map_id;

	// получить карту для трансляции идентификаторов на ресурсы
	static map_id	&get_map() {
		static map_id	m;
		return m;
	}

	static void unregister_id_mapping( long_t res_id )  {
		map_id	&m = get_map();
		map_id::iterator i = m.find( res_id );

		if( i != m.end() ) {
			m.erase( i );
		}
	}

	static void register_id_mapping( long_t res_id, long_t resource ) {
		map_id	&m = get_map();
		m[res_id] = resource;
	}
public:
	static long_t resolve_id( long_t res_id ) {
		map_id	&m = get_map();

		map_id::iterator i = m.find( res_id );
		if( i == m.end() ) {
			// нет такой страницы
			return -1;
		}

		return i->second;
	}*/


};

template<class _win_impl>
class _property_sheet_impl_t : public _win_impl
{
public:
	typedef _property_sheet_impl_t<_win_impl>	class_property_sheet;
public:
	_property_sheet_impl_t()
	{
		ZeroMemory( &psh, sizeof( psh ) );
		psh.dwSize = sizeof( psh );
		psh.dwFlags = PSH_DEFAULT|PSH_PROPSHEETPAGE;
		psh.hInstance = module::hrc();
		psh.pszCaption = 0;
		psh.nPages = 0;
		psh.ppsp = 0;
		is_centered = false;
		m_font = 0;

		id_back = id_next = id_finish = id_cancel = id_help = 0;
	}

	~_property_sheet_impl_t()
	{
		::free( (void*)psh.ppsp );
		::free( (void*)psh.pszCaption );
	}

	void set_caption( unsigned ids )
	{
		::free( (void*)psh.pszCaption );
        psh.pszCaption = _tcsdup( utils::load_string( ids, module::hrc() ) );
	}
	void set_caption( const _char* psz_caption )
	{
		::free( (void*)psh.pszCaption );		
		psh.pszCaption = _tcsdup( psz_caption );
	}

	void set_font(HFONT font) { m_font = font; }

	PROPSHEETPAGE *alloc_pages( int count )
	{
		::free( (void*)psh.ppsp );
		
		psh.ppsp = (PROPSHEETPAGE*)::malloc( count*sizeof( *psh.ppsp ) );
		psh.nPages = count;

		assert( psh.ppsp );

		return (PROPSHEETPAGE*)psh.ppsp;
	}


	// определить страницу по идентификатору
	__lret map_wizard_page( UINT idd_page ) const
	{
		if( idd_page == -1 )	return -1;

		for( unsigned n = 0; n < psh.nPages; n++ )
		{
			const PROPSHEETPAGE	*ppage = (const PROPSHEETPAGE*)psh.ppsp+n;

			page_impl* const pbase_page = (page_impl* const)ppage->lParam;

			if( pbase_page->get_id() == idd_page )
				return (__lret)ppage->pResource;
		}

		return -1;
	}

	PROPSHEETPAGE *get_page( int n )
	{
		assert( n >= 0 );
		assert( n < (int)psh.nPages );

		return (PROPSHEETPAGE *)psh.ppsp+n;
	}

	__lret map_page_id( unsigned id ) const {

		for( unsigned n = 0; n < psh.nPages; ++n ) {
			__lret	ptr = psh.ppsp[n].lParam;
			assert( ptr );

			const page_impl *ppage = (const page_impl*)ptr;

			if( ppage->get_id() == id ) {
				return (__lret)psh.ppsp[n].pResource;
			}
		}

		return (__lret)-1;
	}




	__lret do_modal( HWND hwnd )
	{
//		if ( psh.dwFlags & PSH_WIZARD97 )
//			w7_gui::set_aero_wizard( psh );
		__lret ret_code = property_sheet( hwnd );
		//if( m_hwnd )detach();
		return ret_code;
	}
	
	__lret do_modeless( HWND hwnd )
	{
		psh.dwFlags |= PSH_MODELESS;
		return property_sheet( hwnd );
	}

	bool is_aero() const
	{
		return ( (psh.dwFlags & PSH_AEROWIZARD) != 0 );
	}
protected:
	virtual void handle_init()
	{
		_win_impl::handle_init();

		awin_string str, wstr;
		if( id_back )
		{
			if ( is_aero() )
			{
                wstr = utils::load_string( id_back );
				PropSheet_SetButtonText( handle(), PSWIZB_BACK, (LRESULT)wstr.c_str() );
			} else
			{
                str = utils::load_string( id_back );
				::SetDlgItemText( handle(), ID_WIZ_BTN_BACK, str.c_str() );
			}
		}
		if( id_next )
		{
			if ( is_aero() )
			{
                wstr = utils::load_string( id_next );
				PropSheet_SetButtonText( handle(), PSWIZB_NEXT, (LRESULT)wstr.c_str() );
			} else
			{
                str = utils::load_string( id_next );
				::SetDlgItemText( handle(), ID_WIZ_BTN_NEXT, str.c_str() );
			}
		}
		if( id_finish )
		{
			if ( is_aero() )
			{
                wstr = utils::load_string( id_finish );
				PropSheet_SetButtonText( handle(), PSWIZB_FINISH, (LRESULT)wstr.c_str() );
			} else
			{
                str = utils::load_string( id_finish );
				::SetDlgItemText( handle(), ID_WIZ_BTN_FINISH, str.c_str() );
			}
		}
		if( id_cancel )
		{
			if ( is_aero() )
			{
                wstr = utils::load_string( id_cancel );
				PropSheet_SetButtonText( handle(), PSWIZB_CANCEL, (LRESULT)wstr.c_str() );
			} else
			{
                str = utils::load_string( id_cancel );
				::SetDlgItemText( handle(), ID_WIZ_BTN_CANCEL, str.c_str() );
			}
		}
		if( id_help )
		{
            str = utils::load_string( id_help );
			::SetDlgItemText( handle(), ID_WIZ_BTN_HELP, str.c_str() );
		}
		//pre_process_template( handle() );
	}

	virtual __lret on_notify_reflect( NMHDR *pnmhdr, long *pl )
	{
		if( pnmhdr->code == PSN_SETACTIVE )
			_center();

		
		return 0;
	}
	virtual __lret on_notify( __uint32 idc, NMHDR *pnmhdr )
	{
		return _win_impl::on_notify( idc, pnmhdr );
	}

	virtual void on_precreate( DLGTEMPLATE *pdt) {}
	virtual void on_initialized() {}

protected:
	void _center()
	{
		if( is_centered )
			return;
		is_centered = true;
		HWND	hwnd_p = ::GetParent( handle() );

		// не скрыто, и валидно
		if( !hwnd_p || !IsWindow( hwnd_p ) || !IsWindowVisible( hwnd_p ) )
			hwnd_p = ::GetDesktopWindow();

		_rect	rect, rect_p;
		::GetWindowRect( handle(), &rect );
		::GetWindowRect( hwnd_p, &rect_p );

		::MoveWindow( handle(), rect_p.hcenter()-rect.width()/2,
			rect_p.vcenter()-rect.height()/2,
			rect.width(), rect.height(), 1 );	
	}

	virtual __lret property_sheet( HWND hwnd )
	{
		is_centered = false;
		psh.dwFlags |= PSH_USECALLBACK;
		psh.pfnCallback = subclass_func;

		psh.hwndParent = hwnd;
		//pre_process_pages( &psh );
		subclass_ptr() = this;
		return ::PropertySheet( &psh );
	}

	static _property_sheet_impl_t *&subclass_ptr()
	{static _property_sheet_impl_t *pclass = 0;return pclass;}

	static int CALLBACK subclass_func( HWND hwnd, UINT m, LPARAM l )
	{
		_property_sheet_impl_t	*p = subclass_ptr();
		if ( p )
		{
			switch ( m )
			{
				/*case PSCB_BUTTONPRESSED:
					break;*/
				case PSCB_INITIALIZED:
					p->subclass( hwnd );
					p->on_initialized();
					break;
				case PSCB_PRECREATE:
					{
						struct dlgtemplateex_header_t
						{
							WORD dlgVer;
							WORD signature;
						};
						dlgtemplateex_header_t *pdtex = reinterpret_cast<dlgtemplateex_header_t*>( l );
						if ( pdtex->signature != 0xFFFF )
							p->on_precreate( reinterpret_cast<DLGTEMPLATE*>( l ) );
						else
						{
							// callback для DLGTEMPLATEEX
						}
					}
					break;
			}
		}
		return 0;
	}
public:
	PROPSHEETHEADER	psh;
	bool	is_centered;

	void set_wiz_btns_ids( unsigned	back, unsigned next,
							unsigned finish, unsigned cancel, unsigned	help )
	{
		id_back = back;
		id_next = next;
		id_finish = finish;
		id_cancel = cancel;
		id_help = help;
	}
protected:
	unsigned		id_back;
	unsigned		id_next;
	unsigned		id_finish;
	unsigned		id_cancel;
	unsigned		id_help;
	HFONT			m_font;
};

typedef _property_sheet_impl_t<win_impl>	property_sheet_impl;

END_NAMESPACE_AWIN

#endif //AWIN_PAGE_H_INCLUDED