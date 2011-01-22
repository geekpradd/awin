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

#ifndef AWIN_AUTO_PTR_H_INCLUDED
#define AWIN_AUTO_PTR_H_INCLUDED

#include "awin_defs.h"


NAMESPACE_AWIN

//-------------------------------------------------------------------------------------------------
//smart pointer
//-------------------------------------------------------------------------------------------------


template <class T>
struct basic_auto_ptr_value_traits {

	static void zero( T &h ){
		h = 0;
	}
	static void copy( T &h_to, T &h ){
		h_to = h;
	}
};


class handle_traits : public basic_auto_ptr_value_traits<HANDLE> {
public:
	static void free( HANDLE &h ){
		if( h && h != INVALID_HANDLE_VALUE ) {
			::CloseHandle( h );
		}
	}
};

class hfind_traits : public basic_auto_ptr_value_traits<HANDLE> {
public:
	static void free( HANDLE &h ){
		if( h && h != INVALID_HANDLE_VALUE ) {
			::FindClose( h );
		}
	}
};

class hkey_traits : public basic_auto_ptr_value_traits<HKEY> {
public:
	static void free( HKEY &h ){
		if( h )::RegCloseKey( h );
	}
};

class hmodule_traits : public basic_auto_ptr_value_traits<HMODULE> {
public:
	static void free( HMODULE &h ){
		if( h )::FreeLibrary( h );
	}
};

class handle_change_notify : public basic_auto_ptr_value_traits<HANDLE> {
public:
	static void free( HANDLE &h ){
		if( h )::FindCloseChangeNotification( h );
	}
};

class map_ptr_traits: public basic_auto_ptr_value_traits<void *> {
public:
	static void free( void *&ptr ){
		if( ptr )::UnmapViewOfFile( ptr );
	}
};

class hgdiobj_traits : public basic_auto_ptr_value_traits<HGDIOBJ> {
public:
	static void free( HGDIOBJ &h ){
		if( h )::DeleteObject( h );
	}
};


template <class T>
class reference_traits
{
public:
	static void zero( T &obj ){
		obj = 0;
	}
	static void free( T &obj ){
		_release( obj );
	}
	static void copy( T &obj_to, T &obj ){
		obj_to = obj;
		_add_ref( obj_to );
	}
	static void weak_copy( T &obj_to, T &obj ){
		obj_to = obj;
	}
};

template <class T>
class auto_init_traits
{
public:
	static void zero( T &obj ){
		obj = 0;
	}
	static void free( T &obj ){
		zero( obj );
	}
	static void copy( T &obj_to, T &obj ){
		obj_to = obj;
	}
};

template <class T, class U>
class auto_ptr
{
public:
	typedef	T	object_type;
	typedef	U	traits;
public:
	auto_ptr()
	{
		traits::zero( m_object );
	}
	auto_ptr( T obj )
	{
		traits::zero( m_object );
		attach( obj );
	}
	auto_ptr( T obj, bool strong_ref )
	{
		traits::zero( m_object );
		if( strong_ref )
			attach( obj );
		else
			attach_no_ref( obj );
	}
	auto_ptr( const auto_ptr& ptr )
	{
		traits::zero( m_object );
		attach( ptr );
	}
	~auto_ptr( )
	{
		clear();
	}
	auto_ptr& attach( T obj )//with [+]add_ref
	{
		clear();
		traits::copy( m_object, obj );

		return *this;
	}
	auto_ptr& attach_no_ref( T obj )//with [+]add_ref
	{
		clear();
		traits::weak_copy( m_object, obj );

		return *this;
	}
	void clear()
	{
		traits::free( m_object );
		traits::zero( m_object );
	}
	T detach( )//without [-]release
	{
		T obj = m_object;
		traits::zero( m_object );
		
		return obj;
	}
	auto_ptr &operator =( T obj )
	{
		attach( obj );
		return *this;
	}
	auto_ptr &operator =( const auto_ptr& obj )
	{
		attach( obj );
		return *this;
	}
	T &reference()
	{
		return m_object;
	}
	operator T() const
	{
		return m_object;
	}


	auto_ptr &create_instance( )	{
		clear();m_object = new awin::types::traits<T>::class_type;return *this;
	}

	template <class R1>
	auto_ptr &create_instance( R1 r1 )	{
		clear();	m_object = new awin::types::traits<T>::class_type( r1 );	return *this;
	}
	
	template <class R1, class R2>
	auto_ptr &create_instance( R1 r1, R2 r2 )	{
		clear();	m_object = new awin::types::traits<T>::class_type( r1, r2 );	return *this;
	}
	
	template <class R1, class R2, class R3>
	auto_ptr &create_instance( R1 r1, R2 r2, R3 r3 )	{
		clear();	m_object = new awin::types::traits<T>::class_type( r1, r2, r3 );	return *this;
	}

	template <class R1, class R2, class R3, class R4>
	auto_ptr &create_instance( R1 r1, R2 r2, R3 r3, R4 r4)	{
		clear();	m_object = new awin::types::traits<T>::class_type( r1, r2, r3, r4 );	return *this;
	}

	template <class R1, class R2, class R3, class R4, class R5>
	auto_ptr &create_instance( R1 r1, R2 r2, R3 r3, R4 r4, R5 r5)	{
		clear();	m_object = new awin::types::traits<T>::class_type( r1, r2, r3, r4, r5 );	return *this;
	}
	object_type operator->( ) const     { 
		// this misc_dbg.h binding is unwanted that's why i't commented out
		//_assert( m_object );
        return m_object; 
    }


protected:
	T	m_object;
};

typedef	auto_ptr<HMODULE, hmodule_traits>			auto_module;
typedef	auto_ptr<HANDLE, handle_traits>				auto_handle;
typedef	auto_ptr<HANDLE, hfind_traits>				auto_hfind;
typedef	auto_ptr<HKEY, hkey_traits>					auto_hkey;
typedef	auto_ptr<HANDLE, handle_change_notify>		auto_change_notify;
typedef	auto_ptr<void*, map_ptr_traits>				auto_unmap;
typedef auto_ptr<HGDIOBJ, hgdiobj_traits>			auto_hgdiobj;

//-------------------------------------------------------------------------------------------------
//simple auto handle with destroying API function specified
//
//usage: 
//	auto_handle_t< HDESK, CloseDesktop > hd;
//	auto_handle_t< HANDLE, CloseHandle > h;
//-------------------------------------------------------------------------------------------------

template< typename handle, BOOL WINAPI free_func( handle ) >
class auto_handle_t
{
	handle	m_handle;
public:
	auto_handle_t() : m_handle( 0 ) {}
	auto_handle_t( const handle &h ){
		m_handle = h;
	}

	auto_handle_t &operator=( const handle &h ){
		m_handle = h;
		return *this;
	}

	~auto_handle_t(){
		if( m_handle )
			free_func( m_handle );
	}

	operator handle(){
		return m_handle;
	}

	handle *ptr(){
		return &m_handle;
	}
};


END_NAMESPACE_AWIN

#define DEFINE_SMART_PTR( __class_name )	\
	typedef	awin::auto_ptr<__class_name*, awin::reference_traits<__class_name*> >		__class_name##_ptr;

#endif//AWIN_AUTO_PTR_H_INCLUDED