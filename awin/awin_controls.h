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

#ifndef AWIN_CONTROLS_H_INCLUDED
#define AWIN_CONTROLS_H_INCLUDED

#include "awin_window.h"
#include "awin_dlg.h"
#include "awin_utils.h"
//#include "misc_dbg.h"
//#include "awin\w7_helper.h"

NAMESPACE_AWIN

#ifndef 			ListView_SetCheckState
#define ListView_SetCheckState(hwndLV, i, fCheck) \
  ListView_SetItemState(hwndLV, i, INDEXTOSTATEIMAGEMASK((fCheck)?2:1), LVIS_STATEIMAGEMASK)
#endif //			ListView_SetCheckState
#ifndef 			ListView_SortItemsEx
#define LVM_SORTITEMSEX          (LVM_FIRST + 81)
#define ListView_SortItemsEx(hwndLV, _pfnCompare, _lPrm) \
  (BOOL)SNDMSG((hwndLV), LVM_SORTITEMSEX, (WPARAM)(LPARAM)(_lPrm), (LPARAM)(PFNLVCOMPARE)(_pfnCompare))
#endif //			ListView_SortItemsEx


#ifndef TreeView_SetItemState
#define TreeView_SetItemState(hwndTV, hti, data, _mask) \
{ TVITEM _ms_TVi;\
  _ms_TVi.mask = TVIF_STATE; \
  _ms_TVi.hItem = hti; \
  _ms_TVi.stateMask = _mask;\
  _ms_TVi.state = data;\
  SNDMSG((hwndTV), TVM_SETITEM, 0, (LPARAM)(TV_ITEM FAR *)&_ms_TVi);\
}
#endif// TreeView_SetItemState

#ifndef TreeView_SetCheckState
#define TreeView_SetCheckState(hwndTV, hti, fCheck) \
  TreeView_SetItemState(hwndTV, hti, INDEXTOSTATEIMAGEMASK((fCheck)?2:1), TVIS_STATEIMAGEMASK)
#endif// TreeView_SetCheckState

#ifndef TVM_GETITEMSTATE
#define TVM_GETITEMSTATE        (TV_FIRST + 39)
#endif// TVM_GETITEMSTATE


#ifndef TreeView_GetCheckState
#define TreeView_GetCheckState(hwndTV, hti) \
   ((((__uint32)(SNDMSG((hwndTV), TVM_GETITEMSTATE, (WPARAM)(hti), TVIS_STATEIMAGEMASK))) >> 12) -1)
#endif// TreeView_GetCheckState


class listbox_ctrl : public win_impl
{
public:
	int insert_string( const _char *text, int idx = -1 )	{return (int)SendMessage( handle(), LB_INSERTSTRING, (WPARAM)idx, (LPARAM)text );}
	int get_count()											{return (int)SendMessage( handle(), LB_GETCOUNT, 0, 0 );}
	void get_text( int idx, _char *psz )					{SendMessage( handle(), LB_GETTEXT, WPARAM(idx), LPARAM(psz) );}
	int get_text_len( int idx )								{return (int)SendMessage( handle(), LB_GETTEXTLEN, WPARAM(idx), 0 );}
	int get_cur_sel()										{return (int)SendMessage( handle(), LB_GETCURSEL, 0, 0 );}
	void set_cur_sel( int idx )								{SendMessage( handle(), LB_SETCURSEL, WPARAM(idx), 0 );}
	void delete_string( int idx )							{SendMessage( handle(), LB_DELETESTRING, WPARAM(idx), 0 );}
	void delete_content()									{SendMessage( handle(), LB_RESETCONTENT, 0, 0 );}

	void set_item_data( int idx, u_ptr data )				{SendMessage( handle(), LB_SETITEMDATA, idx, (LPARAM)(size_t)data );}
	u_ptr get_item_data( int idx )							{return (u_ptr)SendMessage( handle(), LB_GETITEMDATA, idx, 0 );}
};

class progress_ctrl : public win_impl
{
public:
	__lret	set_range( int lo, int hi )						{return SendMessage( handle(), PBM_SETRANGE, 0, MAKELONG(lo, hi ) );}
	__lret	set_pos( int pos )								{return SendMessage( handle(), PBM_SETPOS, pos, 0 );}
};

class list_ctrl : public win_impl
{
	bool m_use_vista_style ;

	// Переменные для сохранения состояния при буферизации прорисовки
	HBITMAP	m_custom_draw_bitmap;
	HBITMAP	m_custom_draw_old_bitmap;
	HDC		m_custom_draw_dc;
	HDC		m_custom_draw_memdc;
	_rect	m_custom_draw_rect;

protected:
	void disable_vista_style()
	{
		m_use_vista_style = false;
	}

public:
	list_ctrl() : m_use_vista_style( true ) {}

	// При включённой для висты двойной буферизации custom draw работает плохо. 
	// Выводить в контекст ListView надо через буфер в memory DC.
	/*HDC begin_custom_draw( HDC hdc, const RECT &rect )
	{
		HDC ret = hdc;

		assert( m_custom_draw_memdc == 0 );
		if ( ListView_GetExtendedListViewStyle( handle() ) &  LVS_EX_DOUBLEBUFFER )
		{
			m_custom_draw_dc = hdc;
			m_custom_draw_rect = rect;
			m_custom_draw_memdc = ::CreateCompatibleDC( m_custom_draw_dc );

			m_custom_draw_bitmap = 
				::CreateCompatibleBitmap( m_custom_draw_dc, m_custom_draw_rect.right, m_custom_draw_rect.bottom );
			m_custom_draw_old_bitmap = (HBITMAP)::SelectObject( m_custom_draw_memdc, m_custom_draw_bitmap );

			// копируем фон
			BitBlt( m_custom_draw_memdc, m_custom_draw_rect.left, m_custom_draw_rect.top, 
				m_custom_draw_rect.width(), m_custom_draw_rect.height(),
				m_custom_draw_dc, m_custom_draw_rect.left, m_custom_draw_rect.top, SRCCOPY );

			ret = m_custom_draw_memdc;
		}

		return ret;
	}

	void end_custom_draw()
	{
		if ( m_custom_draw_memdc )
		{
			// копируем то, что нарисовали
			BitBlt( m_custom_draw_dc, m_custom_draw_rect.left, m_custom_draw_rect.top, 
				m_custom_draw_rect.width(), m_custom_draw_rect.height(),
				m_custom_draw_memdc, m_custom_draw_rect.left, m_custom_draw_rect.top, SRCCOPY );

			::SelectObject( m_custom_draw_memdc, m_custom_draw_old_bitmap );
			::DeleteObject( m_custom_draw_bitmap );
			::DeleteDC( m_custom_draw_memdc );

			m_custom_draw_memdc = 0;
			m_custom_draw_bitmap = 0;
		}
	}*/

	virtual void handle_init()
	{
		m_custom_draw_memdc = 0;
		/*if ( m_use_vista_style && w7_helper::is_vista() )
		{
			HMODULE hdll = ::GetModuleHandle( _T("uxtheme.dll") );
			if ( !hdll )
				hdll = ::LoadLibrary( _T("uxtheme.dll") );
			if ( hdll )
			{
				BOOL (__stdcall *IsAppThemed)();
				(FARPROC&)IsAppThemed = ::GetProcAddress( hdll, "IsAppThemed" );
				if ( IsAppThemed && IsAppThemed() )
				{
					HRESULT (__stdcall *SetWindowTheme)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
					(FARPROC&)SetWindowTheme = ::GetProcAddress( hdll, "SetWindowTheme" );
					if ( SetWindowTheme )
					{
						SetWindowTheme( handle(), L"Explorer", 0 );
						// Для нормальной работы стиля необходима включённая двойная буферизация
						ListView_SetExtendedListViewStyleEx( handle(), LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER );
					}
				}
			}
		}*/

		__super::handle_init();
	}

	int find_item( LPARAM param )							{LV_FINDINFO info;info.flags = LVFI_PARAM;info.lParam = param;return (int)ListView_FindItem( handle(), -1, &info );}
	int insert_item( LVITEM *pitem )							{return (int)ListView_InsertItem( handle(), pitem );	}
	void set_item( LVITEM *pitem )								{ListView_SetItem( handle(), pitem );	}
	void set_item_text( int item, int subitem, const _char *psz )		{ListView_SetItemText( handle(), item, subitem, (_char*)psz );	}
	void set_item_state( int item, unsigned state, unsigned mask ){ListView_SetItemState( handle(), item, state, mask );}
	void get_item_text( int item, int subitem, const _char *psz, __uint32 size_text )		{ListView_GetItemText( handle(), item, subitem, (_char*)psz, size_text );	}
	void delete_item( int item )								{ListView_DeleteItem( handle(), item );}
	void delete_all_items()										{ListView_DeleteAllItems( handle() );}
	u_ptr get_item_data( int item )							{LV_ITEM	i;i.iItem = item;i.lParam = 0;i.mask = LVIF_PARAM;ListView_GetItem( handle(), &i );return (u_ptr)i.lParam;}
	void get_item_rect( int item, int code , RECT *prect)		{ListView_GetItemRect( handle(), item, prect, code );}
	void get_subitem_rect( int item, int subitem, int code, RECT *prect)	{ListView_GetSubItemRect( handle(), item, subitem, code, prect );}
	int get_selected_item()
	{
		LVITEM	item;
		item.mask = LVIF_STATE;
		item.iItem = ListView_GetSelectionMark( handle() );
		item.state = item.stateMask = LVIS_SELECTED|LVIS_FOCUSED;
		if( item.iItem == -1 )return -1;

//		ListView_GetItem( handle(), &item );
		return (item.state & (LVIS_SELECTED|LVIS_FOCUSED))?item.iItem:-1;
	}
	int get_selected_item_ex()
	{
		return ListView_GetNextItem( handle(), -1, LVIS_SELECTED);
	}
	__lret get_item( LVITEM *pitem )								{return ListView_GetItem( handle(), pitem );	}
	void set_selected_item( int index )							
	{
		ListView_SetItemState( handle(), index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );	
		ListView_SetSelectionMark( handle(), index );
	}
	void clear_selection()
	{
		int idx = -1;
		while( true )
		{
			idx = (int)list_ctrl::get_next_item( idx, LVNI_SELECTED );
			if( idx == -1 )break;
			list_ctrl::set_item_state( idx, 0, LVIS_SELECTED|LVIS_FOCUSED );
		}
	}

//++++++++ Added by Sabelnikov Andrey N. mailto:a.sabelnikov@agnitum.com 14.10.2005 11:54:22 +++++++++++++++++
	int insert_item( int i, const _char* pstr, LPARAM lParam = NULL)
	{
		assert(pstr);
		LVITEM lv_item = {0};
		lv_item.mask = LVIF_TEXT|LVIF_PARAM;
		lv_item.iItem = i;
		lv_item.pszText = (_char*)pstr;
		lv_item.lParam = lParam;
		return insert_item(&lv_item);	
	}

	int add_item( const _char* pstr, LPARAM lParam = 0 )
	{
		int count = (int)get_items_count();
		return insert_item( count, pstr, lParam );
	}

	int add_column( const _char* pstr, int width )
	{
		LVCOLUMN col = {0};
		col.mask = LVCF_TEXT|LVCF_WIDTH;
		col.cx = width;
		col.pszText = (_char*)pstr;
		return insert_column( get_columns_count(), &col );
	}

	bool set_item_data( int item , LPARAM lParam )
	{
		LV_ITEM	i = {0};
		i.iItem = item;
		i.lParam = lParam;
		i.mask = LVIF_PARAM;
		BOOL res = ListView_SetItem( handle(), &i );
		return res ? true:false;
	}
	bool trim_columns_width(int flags = LVSCW_AUTOSIZE_USEHEADER)
	{
		__lret col_count = get_columns_count();
		for (int i = 0; i< col_count; i++)
			ListView_SetColumnWidth( handle(), i, flags );
		return true;
	}

	void get_subitem_by_point(const _point &point, int &item, int &subitem) 
	{
		LVHITTESTINFO hit = {point};
		ListView_SubItemHitTest(handle(), &hit);
		item = hit.iItem;
		subitem = hit.iSubItem;
	}

	int get_next_item( int idx, unsigned flags )				{return ListView_GetNextItem( handle(), idx, flags );}
	int get_items_count()										{return ListView_GetItemCount( handle() );}
	int get_top_index()										{return ListView_GetTopIndex( handle() );}
	int get_count_per_page()									{return ListView_GetCountPerPage( handle() );}

	int get_columns_count()									{return Header_GetItemCount( get_header() );}
	int insert_column( int ncol, LVCOLUMN *pcol )				{return ListView_InsertColumn( handle(), ncol, pcol );}
	void delete_column( int col )								{ListView_DeleteColumn( handle(), col );}
	void delete_all_columns()									{for( int col = get_columns_count(); col; col-- )delete_column( col-1 );}
	int  get_column_width( int col )							{return ListView_GetColumnWidth( handle(), col );}
	void	set_column_width( int col, int width )				{ListView_SetColumnWidth( handle(), col, width );}

	
	HWND get_header()											{return ListView_GetHeader( handle() );}
	void set_imagelist( HIMAGELIST hil, int code )				{ListView_SetImageList( handle(), hil, code );}
	void set_extended_style( unsigned style, unsigned mask )	{ListView_SetExtendedListViewStyleEx( handle(), style, mask );}
	void ensure_visible( int item, bool partial_ok )			{ListView_EnsureVisible( handle(), item, partial_ok );}
	void get_view_rect( RECT *prect )							{ListView_GetViewRect( handle(), prect );}

	__lret set_items_count( size_t c, int f = 0 )                  {return ListView_SetItemCountEx( handle(), c, f );}

public:
	static const _char *window_class()							{return WC_LISTVIEW;}
};

inline
void TreeView_ExpandAll( HWND htree, int code, HTREEITEM hti_p = 0 )
{
	HTREEITEM	hti = TreeView_GetNextItem( htree, hti_p, TVGN_CHILD );
	while( hti )
	{
		TreeView_ExpandAll( htree, code, hti );
		hti = TreeView_GetNextItem( htree, hti, TVGN_NEXT );
	}
	TreeView_Expand( htree, hti_p, code ); 
}


class tree_ctrl : public win_impl
{
public:
	HTREEITEM insert_item( TVINSERTSTRUCTA* pinsert )			{return (HTREEITEM)::SendMessage( handle(), TVM_INSERTITEMA, 0, (LPARAM)pinsert );}
	void get_item( TVITEMA *pitem )								{::SendMessage( handle(), TVM_GETITEMA, 0, (LPARAM)pitem );}
	void set_item( TVITEMA *pitem )								{::SendMessage( handle(), TVM_SETITEMA, 0, (LPARAM)pitem );}
	
	HTREEITEM insert_item( TVINSERTSTRUCTW* pinsert )			{return (HTREEITEM)::SendMessage( handle(), TVM_INSERTITEMW, 0, (LPARAM)pinsert );}
	void get_item( TVITEMW *pitem )								{::SendMessage( handle(), TVM_GETITEMW, 0, (LPARAM)pitem );}
	void set_item( TVITEMW *pitem )								{::SendMessage( handle(), TVM_SETITEMW, 0, (LPARAM)pitem );}
	
	void delete_item( HTREEITEM hti )							{TreeView_DeleteItem( handle(), hti );}

	void get_item_rect( HTREEITEM hti, RECT *prect, bool fitem_rect )	{TreeView_GetItemRect( handle(), hti, prect, fitem_rect );}


	void set_imagelist( HIMAGELIST hi, int code = TVSIL_NORMAL )	{TreeView_SetImageList( handle(), hi, code );}
	HTREEITEM get_next_item( HTREEITEM hti, int code )			{return TreeView_GetNextItem( handle(), hti, code );}
	void select_item( HTREEITEM hti )							{TreeView_SelectItem( handle(), hti );}
	HTREEITEM get_selected_item() const							{return TreeView_GetSelection( handle() );}
	void delete_all_items()										{TreeView_DeleteAllItems( handle() ); }

	void expand( HTREEITEM hti, int code )						{TreeView_Expand( handle(), hti, code );}
	void expand_all( int code )									{TreeView_ExpandAll( handle(), code );}

	HTREEITEM	hit_test( TVHITTESTINFO *p )					{return TreeView_HitTest( handle(), p ); }
	void ensure_visible( HTREEITEM hti )						{TreeView_EnsureVisible( handle(), hti );}

	__lret get_item_data( HTREEITEM hti )	{
		TVITEM	item = {TVIF_PARAM, hti};
		TreeView_GetItem( handle(), &item );
		return item.lParam;
	}

	HTREEITEM	find_item( LPARAM l, HTREEITEM hti = 0 )	{
		for( HTREEITEM h = get_next_item( hti, TVGN_CHILD ); h; h = get_next_item( h, TVGN_NEXT ) )	{
			if( get_item_data( h ) == l )return h;
			HTREEITEM h2 = find_item( l, h );
			if( h2 )return h2;
		}
		return 0;
	}


public:
	static const _char *window_class()							{return WC_TREEVIEW;}
};

class header_ctrl : public win_impl
{
public:
	int insert_item( int col, HDITEM *phdi )	{return Header_InsertItem( handle(), col, phdi ); }
	int	get_items_count()						{return Header_GetItemCount( handle() );}
	int layout( HDLAYOUT *phdl )				{return Header_Layout( handle(), phdl ); }
	bool	get_item_rect( int item, RECT *prect )	{return Header_GetItemRect( handle(), item, prect )!=0;}
	bool	get_item( int item, HDITEM *phd )		{return Header_GetItem( handle(), item , phd )!=0;}
public:
	static const _char *window_class()							{return WC_HEADER;}
};

class static_path_ctrl : public win_impl
{
public:
	virtual __lret on_paint()
	{
		PAINTSTRUCT	paint;
		HDC	hdc = ::BeginPaint( m_hwnd, &paint );

		::SelectObject( hdc, (HFONT)::SendMessage(m_hwnd, WM_GETFONT, 0, 0) );
		::SetBkMode( hdc, TRANSPARENT );

		_rect rc;
		::GetClientRect( m_hwnd, &rc );
		rc.right -=2;

        awin_string	str = utils::get_window_text( handle() );
		::DrawText( hdc, str.c_str(), -1, &rc, DT_PATH_ELLIPSIS );

		::EndPaint( m_hwnd, &paint );
		return 1;
	}
};

class tooltiped_ctrl : public win_impl
{
	HWND		m_hparent;

	UINT_PTR	last_tool_uid;
public:
	void create_ctrl( HWND hparent )
	{
		m_hparent = hparent;
		
		win_impl::create_ex( WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
			_rect(),
			0, 
			hparent,
			0,
			TOOLTIPS_CLASS,
			WS_EX_TOPMOST );

		::SetWindowPos( handle(), HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	
	void add_tool( HWND hwnd, const _char *psz_text )
	{
		TOOLINFO ti = {0};
		RECT rect = { 0 };

		::GetWindowRect( hwnd, &rect );
		::ScreenToClient( m_hparent, (LPPOINT)&rect );
		::ScreenToClient( m_hparent, (LPPOINT)&rect + 1 );

		
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_SUBCLASS;
		ti.hwnd = m_hparent;
		ti.hinst = module::hinst();
		ti.lpszText = (LPTSTR)psz_text;

		ti.rect.left = rect.left;    
		ti.rect.top = rect.top;
		ti.rect.right = rect.right;
		ti.rect.bottom = rect.bottom;
	    
		::SendMessage( handle(), TTM_ADDTOOL, 0, (LPARAM)&ti );	
		// так делать нельзя потому что портится буфер строки
		//::SendMessage( m_tooltip, TTM_ENUMTOOLS, 0, (LPARAM)&ti );
		//last_tool_uid = ti.uId;
	}

	void add_tool( RECT rc_area, const _char *psz_text )
	{
		TOOLINFO ti = {0};

		
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_SUBCLASS;
		ti.hwnd = m_hparent;
		ti.hinst = module::hinst();
		ti.lpszText = (LPTSTR)psz_text;

		ti.rect.left = rc_area.left;    
		ti.rect.top = rc_area.top;
		ti.rect.right = rc_area.right;
		ti.rect.bottom = rc_area.bottom;
	    
		::SendMessage( handle(), TTM_ADDTOOL, 0, (LPARAM)&ti );	
		// так делать нельзя потому что портится буфер строки
		//::SendMessage( m_tooltip, TTM_ENUMTOOLS, 0, (LPARAM)&ti );
		//last_tool_uid = ti.uId;
	}
	void del_tool( UINT_PTR uid, HWND hwnd ){
		TOOLINFO ti = {0};
		ti.uId = uid;
		ti.hwnd = hwnd;
		::SendMessage( handle(), TTM_DELTOOL, 0, (LPARAM)&ti );
	}

	tooltiped_ctrl()
	{
		last_tool_uid = 0;
	}

	virtual ~tooltiped_ctrl()
	{
	}
};


class static_path_with_tip_ctrl : public static_path_ctrl
{
	tooltiped_ctrl m_tooltip_ctrl;
public:
	virtual void	handle_init()
	{
		m_tooltip_ctrl.create_ctrl( ::GetParent( handle() ) );
	}

	virtual __lret	handle_message( UINT m, WPARAM w, LPARAM l )
	{
		if( m == WM_SETTEXT )
			m_tooltip_ctrl.add_tool( handle(), (_char*)l );
		return __super::handle_message( m, w, l );
	}
};

END_NAMESPACE_AWIN

#endif // AWIN_CONTROLS_H_INCLUDED