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

#ifndef AWIN_UTILS_H_INCLUDED
#define AWIN_UTILS_H_INCLUDED

#include "awin_defs.h"

NAMESPACE_AWIN_UTILS

inline
awin_string load_string( unsigned ids, HMODULE h = 0 ) {
    awin_string     str;

    if( !h ) {
        h = module::hrc();
    }

    str.resize( 65536 );
    ::LoadString( h, ids, &str[0], (DWORD)str.size() );

    return str.c_str();
}

inline
awin_string get_window_text( HWND hwnd ) {
    size_t  cb = ::SendMessage( hwnd, WM_GETTEXTLENGTH, 0, 0 );
    if( !cb ) {
        return awin_string();
    }

    awin_string  tempo;
    tempo.resize( cb+10 );
    ::SendMessage( hwnd, WM_GETTEXT, tempo.size(), (LPARAM)&tempo[0] );
    return tempo.c_str();
}

inline
awin_string get_control_text( HWND hwnd, UINT id ) {
	HWND	hwnd_child = ::GetDlgItem( hwnd, id );
	if( !hwnd_child ) {
		return awin_string();
	}

    return get_window_text( hwnd_child );
}

inline
awin_string get_window_class( HWND hwnd ) {
    int n = 1024;

    awin_string tempo;
    tempo.resize( n );

    GetClassName( hwnd, &tempo[0], n );

    return tempo.c_str();
}

inline
awin_string get_combo_text( HWND combo, int line ) {
    size_t cb = ::SendMessage( combo, CB_GETLBTEXTLEN, line, 0 );
    awin_string  tempo;
    tempo.resize( cb+10 );
    ::SendMessage( combo, CB_GETLBTEXT, line, LPARAM(tempo.c_str()) );
    return tempo.c_str();
}


END_NAMESPACE_AWIN_UTILS

#endif //AWIN_UTILS_H_INCLUDED