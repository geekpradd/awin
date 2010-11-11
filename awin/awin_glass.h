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

#ifndef AWIN_GLASS_H_INCLUDED
#define AWIN_GLASS_H_INCLUDED

#include "awin_dlg.h"
#include "awin_utils.h"

#include <Dwmapi.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

NAMESPACE_AWIN

class glass_dlg : public dlg_impl {
public:
	glass_dlg( UINT idd )
		: dlg_impl( idd )
		, margins()
		, glass_effect_enabled( false ) {
	}
    
	void set_extended_caption_height( int h ) {
		margins.cyTopHeight = h;
	}

	inline COLORREF get_transparent_color() {
		return RGB( 255, 0, 255 );//::GetSysColor( COLOR_3DFACE );
	}
protected:
	long_t on_erasebkgnd( HDC hdc ) {
		if( !glass_effect_enabled ) {
			return __super::on_erasebkgnd( hdc );
		}

		_rect   rect;
		::GetClientRect( handle(), &rect );

		_rect   rect_top( rect );
		rect_top.bottom = margins.cyTopHeight;
		HBRUSH  hbrush = ::CreateSolidBrush( get_transparent_color() );
		::FillRect( hdc, &rect_top, hbrush );
		::DeleteObject( hbrush );

		rect.top = margins.cyTopHeight;
		::FillRect( hdc, &rect, (HBRUSH)::GetSysColorBrush( COLOR_3DFACE ) );

		return 1;
	}
    
	long_t on_initdialog() {
		on_dwm_compositino_changed();
		return __super::on_initdialog();
	}

	long_t on_notify( __uint32 idc, NMHDR *pnmhdr ) {
		if( pnmhdr->code == NM_CUSTOMDRAW ) {
			if( glass_effect_enabled ) {
				awin_string   class_name = utils::get_window_class( pnmhdr->hwndFrom );
				if( boost::iequals( class_name, "button" ) ) {
					NMCUSTOMDRAW  *pcd = (NMCUSTOMDRAW  *)pnmhdr;
					if( pcd->dwDrawStage == CDDS_PREERASE ) {
						HBRUSH  hbrush = ::CreateSolidBrush( get_transparent_color() );
						::FillRect( pcd->hdc, &pcd->rc, hbrush );
						::DeleteObject( hbrush );
					}
					return CDRF_DODEFAULT;
				}
			}
		}
		return __super::on_notify( idc, pnmhdr );
	}

	long_t handle_message( UINT m, WPARAM w, LPARAM l ) {
		if( m == WM_DWMCOMPOSITIONCHANGED ) {
			on_dwm_compositino_changed();
		}
		else if( m == WM_UPDATEUISTATE ) {
			return true;
			/*_rect   rect_all;
			::GetWindowRect( handle(), &rect_all );

			rect_all.left += margins.cxLeftWidth;
			rect_all.right -= margins.cxRightWidth;
			rect_all.top += margins.cyTopHeight;
			rect_all.bottom -= margins.cyBottomHeight;

			for( HWND    hwnd = ::GetWindow( handle(), GW_CHILD ); hwnd != 0; hwnd = ::GetWindow( hwnd, GW_HWNDNEXT ) ) {
				if( !IsWindowVisible( hwnd ) ) {
					continue;
				}
				_rect   rect;
				::GetWindowRect( hwnd, &rect );

				if( rect.top >=  rect_all.top &&
					rect.bottom <= rect_all.bottom &&
					rect.left >= rect_all.left &&
					rect.right <= rect_all.right ) {
					continue;
				}

				::InvalidateRect( hwnd, 0, true );
			}*/
		}
		/*if( m == WM_CTLCOLORSTATIC ) {
			if( glass_effect_enabled ) {
				HDC hdc = (HDC)w;
				::SetBkMode( hdc, TRANSPARENT );
				::SetTextColor( hdc, RGB(0,0,0) );//::GetSysColor( COLOR_WINDOWTEXT ) );
				HBRUSH hbr = (HBRUSH)GetStockObject( NULL_BRUSH );
				return (long_t)hbr;//::CreateSolidBrush( get_transparent_color() );
			}
		}*/
		return __super::handle_message( m, w, l );
	}

private:
	void on_dwm_compositino_changed() {
		bool    old_glass_effect = false;

		HRESULT (WINAPI *_DwmExtendFrameIntoClientArea)( HWND hWnd, const MARGINS *pMarInset );
		HRESULT (WINAPI *_DwmSetWindowAttribute)( HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute );

		HMODULE hdmw = LoadLibrary( fcm_text("Dwmapi.dll") );

		glass_effect_enabled = false;

		if( hdmw ) {
			(FARPROC&)_DwmExtendFrameIntoClientArea = ::GetProcAddress( hdmw, "DwmExtendFrameIntoClientArea" );
			(FARPROC&)_DwmSetWindowAttribute = ::GetProcAddress( hdmw, "DwmSetWindowAttribute" );

			if( _DwmExtendFrameIntoClientArea ) {

				if( _DwmExtendFrameIntoClientArea( handle(), &margins ) == S_OK ) {
					::SetWindowLong( handle(), GWL_EXSTYLE, ::GetWindowLong( handle(), GWL_EXSTYLE ) | WS_EX_LAYERED );
					::SetLayeredWindowAttributes( handle(), get_transparent_color(), 0, LWA_COLORKEY );

					glass_effect_enabled = true;
				}

			}

			::FreeLibrary( hdmw );
		}

		if( !glass_effect_enabled ) {
			::SetWindowLong( handle(), GWL_EXSTYLE, ::GetWindowLong( handle(), GWL_EXSTYLE ) & ~WS_EX_LAYERED );
			//::SetLayeredWindowAttributes( handle(), cr_transparent, 0, LWA_COLORKEY );
		}

		if( old_glass_effect != glass_effect_enabled ) {
			::InvalidateRect( handle(), 0, true );
		}
	}
private:    
    MARGINS margins;
    bool    glass_effect_enabled;
};


END_NAMESPACE_AWIN

#endif //AWIN_GLASS_H_INCLUDED