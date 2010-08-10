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

#ifndef AWIN_LAYOUT_H_INCLUDED
#define AWIN_LAYOUT_H_INCLUDED

#include "awin_defs.h"

//define own namespace
//#define NAMESPACE_AWIN_LAYOUT	namespace awin123 { namespace layout456 {
//#define END_NAMESPACE_AWIN_LAYOUT		} }


//NAMESPACE_AWIN_LAYOUT
namespace awin123 { 
	namespace layout456 {

enum awin_layout_ancor {
	layout_ancor_left,
	layout_ancor_right,
	layout_ancor_top,
	layout_ancor_bottom,
	layout_ancor_hcenter,
	layout_ancor_vcenter
};

class awin_layout_item;

typedef boost::shared_ptr<awin_layout_item>	awin_layout_item_ptr;

struct awin_layout_arg {
	awin_layout_arg() : hdwp( 0 ) {
		hdwp = BeginDeferWindowPos( 10 );
	}
	~awin_layout_arg() {
		if( hdwp ) {
			::EndDeferWindowPos( hdwp );
		}
	}

	HDWP	hdwp;
};

//
// layout item class
//

class awin_layout_item {
public:
	awin_layout_item( 
		const awin::_rect &rc, 
		awin_layout_ancor l,
		awin_layout_ancor t, 
		awin_layout_ancor r, 
		awin_layout_ancor b )
			: rect_def( rc )
			, ancor_left( l )
			, ancor_top( t )
			, ancor_right( r )
			, ancor_bottom( b )	{
	}

	void insert( awin_layout_item_ptr item ) {
		childs.push_back( item );
	}

	void layout( 
		const awin::_rect &rect_new_parent, 
		const awin::_rect &rect_def_parent, 
		awin_layout_arg &arg ) {

		awin::_rect	rect_new(
			calc_coord( rect_def.left, rect_new_parent, rect_def_parent, ancor_left ),
			calc_coord( rect_def.top, rect_new_parent, rect_def_parent, ancor_top ),
			calc_coord( rect_def.right, rect_new_parent, rect_def_parent, ancor_right ),
			calc_coord( rect_def.bottom, rect_new_parent, rect_def_parent, ancor_bottom ) 
		);

		set_layout( rect_new, arg );
	}

	virtual void set_layout( 
		const awin::_rect &rect_new, 
		awin_layout_arg &arg ) {
	
		for( awin_layout_item_list::iterator i = childs.begin(); i != childs.end(); ++i ) {
			awin_layout_item_ptr	child( *i );
			child->layout( rect_new, rect_def, arg );
		}
	}
protected:
	int calc_coord( 
		int val, 
		const awin::_rect &rect_new_parent, 
		const awin::_rect &rect_def_parent, 
		awin_layout_ancor a ) {

		if( a == layout_ancor_left ) {
			return val - rect_def_parent.left + rect_new_parent.left;
		}
		if( a == layout_ancor_right ) {
			return val - rect_def_parent.right + rect_new_parent.right;
		}
		if( a == layout_ancor_top ) {
			return val - rect_def_parent.top + rect_new_parent.top;
		}
		if( a == layout_ancor_bottom ) {
			return val - rect_def_parent.bottom + rect_new_parent.bottom;
		}
		if( a == layout_ancor_hcenter ) {
			return val - rect_def_parent.hcenter() + rect_new_parent.hcenter();
		}
		if( a == layout_ancor_vcenter ) {
			return val - rect_def_parent.vcenter() + rect_new_parent.vcenter();
		}
		return val;
	}

protected:
	awin_layout_ancor	ancor_left, ancor_top, ancor_right, ancor_bottom;
	awin::_rect			rect_def;
	typedef std::list<awin_layout_item_ptr>	awin_layout_item_list;
	awin_layout_item_list	childs;
};

//
// layout item for window
//

class awin_layout_item_hwnd : public awin_layout_item {
public:
	awin_layout_item_hwnd( 
		HWND h, 
		awin_layout_ancor l, 
		awin_layout_ancor t, 
		awin_layout_ancor r, 
		awin_layout_ancor b )
			: awin_layout_item( awin::_rect(), l, t, r, b )
			, hwnd( h )	{

		::GetWindowRect( hwnd, &rect_def );

		HWND	hparent = ::GetParent( hwnd );
		::MapWindowPoints( 0, hparent, rect_def.as_points(), 2 );
	}

	void set_layout( 
		const awin::_rect &rect_new, 
		awin_layout_arg &arg ) {

		arg.hdwp = ::DeferWindowPos( arg.hdwp, hwnd, 0, rect_new.left, rect_new.top, rect_new.width(), rect_new.height(), SWP_DRAWFRAME|SWP_NOZORDER ); 
		__super::set_layout( rect_new, arg );
	}

private:
	HWND	hwnd;
};

	
struct awin_init_layout {
	UINT ctrl_id;
	awin_layout_ancor	left, top, right, bottom;
};
	
class awin_layout : public awin_layout_item {
public:
	awin_layout() 
		: awin_layout_item( 
			awin::_rect(), 
			layout_ancor_left, 
			layout_ancor_top, 
			layout_ancor_right, 
			layout_ancor_bottom )
		, hwnd_parent( 0 ) {	
	}

	/**
	 * Initialize the layout manager
	 */
	void init( 
		HWND p, 
		awin_init_layout const *items, 
		size_t count ) {
	
		hwnd_parent = p;
		::GetClientRect( hwnd_parent, &rect_def );

		for( size_t n = 0; n < count; ++n ) {
			HWND hwnd = ::GetDlgItem( hwnd_parent, items[n].ctrl_id );

			assert( hwnd );

			insert( awin_layout_item_ptr( new awin_layout_item_hwnd( hwnd, 
				items[n].left, items[n].top, items[n].right, items[n].bottom ) ) );
		}
	}

	/**
	 * call set_layout
	 */
	void set_layout( 
		const awin::_rect &rect_new, 
		awin_layout_arg &arg ) {
	
		__super::set_layout( rect_new, arg );
	}
private:
	HWND	hwnd_parent;
};

}
}
//END_NAMESPACE_AWIN_LAYOUT

#endif //AWIN_LAYOUT_H_INCLUDED