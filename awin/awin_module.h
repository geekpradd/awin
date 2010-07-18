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

#ifndef AWIN_MODULE_H_INCLUDED
#define AWIN_MODULE_H_INCLUDED

#include "awin_defs.h"
//#include "misc_string2.h"

#ifndef DISABLE_COMCTL_INIT
#ifdef _WINDOWS_
#pragma comment(lib, "comctl32.lib")
#endif //_WINDOWS_
#endif //DISABLE_COMCTL_INIT

NAMESPACE_AWIN

class module : public _static_ptr_t<module>
{
public:
	module( HMODULE h );
	virtual ~module();

	virtual bool init( HMODULE h );
	virtual bool deinit();

	void set_resource_handle( HINSTANCE h )	{m_hrc = h;}

	virtual bool run();

//if program fault here, you have to define global or static instance of module
	static HINSTANCE hrc()				{return instance()->m_hrc;}
	static HINSTANCE hinst()			{return instance()->m_hinst;}
	static awin_string const &language()			{return instance()->m_ui_language; }
	static awin_string const &common_help_file()	{return instance()->m_ui_common_file_name; }
	static awin_string const &get_common_help_folder(){return instance()->m_common_help_folder;}
	
	static void set_common_help_folder( awin_string const &psz ) {
        instance()->m_common_help_folder = psz;
    }

	static void set_language( const awin_string &str_lang )	{ 
        instance()->m_ui_language = str_lang; 
    }

	static void set_common_help_file( const awin_string &str_hlp_file ){ 
        instance()->m_ui_common_file_name = str_hlp_file; 
    }


protected:
	HINSTANCE	m_hinst, m_hrc;
	awin_string	m_ui_language;
	awin_string	m_ui_common_file_name;
	awin_string	m_common_help_folder;
};

inline module::module( HMODULE h )
{
	init( h );
}

inline module::~module()
{
}



inline bool module::init( HINSTANCE h )
{
	m_hinst = h;
	m_hrc = h;

	/*HMODULE	hmodule_comctl = ::GetModuleHandle( _t("comctl32.dll"));

	if( hmodule_comctl )
	{
		INITCOMMONCONTROLSEX	init;
		init.dwSize = sizeof( INITCOMMONCONTROLSEX );
		init.dwICC = ICC_COOL_CLASSES|ICC_WIN95_CLASSES;


		BOOL (__stdcall *pfnInitCommonControlsEx)( LPINITCOMMONCONTROLSEX ) = 0;

		(FARPROC &)pfnInitCommonControlsEx = ::GetProcAddress( hmodule_comctl, "InitCommonControlsEx" );
		if( pfnInitCommonControlsEx )pfnInitCommonControlsEx( &init );
	}*/

#ifndef DISABLE_COMCTL_INIT
	INITCOMMONCONTROLSEX	init;
	init.dwSize = sizeof( INITCOMMONCONTROLSEX );
	init.dwICC = ICC_COOL_CLASSES|ICC_WIN95_CLASSES;
	InitCommonControlsEx( &init );
#endif 
	return true;
}

inline bool module::deinit()
{
	m_hinst = 0;
	m_hrc = 0;
	return true;
}

inline bool module::run()
{
	MSG	msg;
	while( ::GetMessage( &msg, 0, 0, 0 ) )
	{
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}

	return deinit();
}

END_NAMESPACE_AWIN

#endif //AWIN_MODULE_H_INCLUDED
