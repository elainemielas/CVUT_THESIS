/**
 * @file   regarray.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  register array database class
 *
 */

#include "regarray.hpp"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "logger.hpp"

using namespace std;

#define SAVE_FILE "/regsave"

	Cregarray::Cregarray()
{
	m_len   = 0;
	m_alloc = 0;
	m_root  = NULL;

	char *pwd = getenv( "REGD_RUN_DIR" );
	char *filename;
	int   size = 0;

	// concat RUN_DIR and SAVE_FILE
	size += strlen( SAVE_FILE ) + 2;

	if( pwd )
		size += strlen( pwd );

	filename = ( char* ) malloc( size * sizeof( char ) );

	if( pwd )
	{
		strcpy( filename, pwd );
		strcat( filename, SAVE_FILE );
	}
	else
	{
		filename[ 0 ] = '.';
		strcpy( filename + 1, SAVE_FILE );
	}

	// open reg save file
	m_savefile = fopen( filename, "r+b" );
	// when file doewsn't exist create
	if( !m_savefile )
		m_savefile = fopen( filename, "w+b" );
	// file not create
	if( !m_savefile )
		error( "unable to create file 'regsave'" );

	free( filename );
}
//---------------------------------------------
	Cregarray::~Cregarray()
{
	for( int i = 0; i < m_len; i++ )
		delete m_root[ i ];

	fclose( m_savefile );
	free( m_root );
}
//---------------------------------------------
int Cregarray::add( const int reg, const char *desc, const int a, 
				    const int b, const int c, const int d, const int flag )
{
	long int offset = 0;
	int      value;

	// je v databaze?
	if( find( reg ) )
		return 1;

	// realoc
	if( m_len == m_alloc )
	{
		m_alloc = ( m_alloc ? m_alloc * 2 : 5 );
		m_root = ( Treg** ) realloc( m_root, sizeof( Treg* ) * m_alloc );
	}

	// find register in file
	if( m_savefile )
		offset = seek( reg, &value );

	// add register
	m_root[ m_len++ ] = new Treg( reg, desc, a, b, c, d, flag, offset, value );

	return 0;
}
//---------------------------------------------
int Cregarray::write( int reg, int value )
{
	// find value
	Treg *r = find( reg );

	if( !r )
		return 0;

	// lock register
	pthread_mutex_lock( &r->m_mux );

	// save into memory
	r->m_value = value;

	if( m_savefile )
	{
		// save into file
		fseek( m_savefile, r->m_seek + sizeof( int ), SEEK_SET );	// seek to position
		fwrite( &value, sizeof( int ), 1, m_savefile );		// write to file
	}

	// unlock register
	pthread_mutex_unlock( &r->m_mux );

	return 1;
}
//---------------------------------------------
int Cregarray::read( int reg )
{
	// find value
	Treg *r = find( reg );

	return ( r ? r->m_value : -1 );
}
//---------------------------------------------
const char* Cregarray::desc( int reg )
{
	// find value
	Treg *r = find( reg );

	return ( r ? r->m_desc : NULL );
}
//---------------------------------------------
int Cregarray::a( int reg )
{
	// find value
	Treg *r = find( reg );

	return ( r ? r->m_a : 0 );
}
//---------------------------------------------
int Cregarray::b( int reg )
{
	// find value
	Treg *r = find( reg );

	return ( r ? r->m_b : 0 );
}
//---------------------------------------------
int Cregarray::c( int reg )
{
	// find value
	Treg *r = find( reg );

	return ( r ? r->m_c : -1 );
}
//---------------------------------------------
int Cregarray::d( int reg )
{
	// find value
	Treg *r = find( reg );

	return ( r ? r->m_d : -1 );
}
//---------------------------------------------
int Cregarray::flag( int reg )
{
	// find value
	Treg *r = find( reg );

	return ( r ? r->m_flag : -1 );
}
//---------------------------------------------
char* Cregarray::list()
{
	int l = 0;
	char *s;
	char  buffer[ 16 ];

	for( int i = 0; i < m_len; i++ )
		if( m_root[ i ]->m_reg )
			l += log10( abs( m_root[ i ]->m_reg ) ) + 2;
		else 
			// log( 0 ) !!!
			l += 2;

	l += 5;
	s = ( char* ) malloc( l * sizeof( char ) );

	s[ 0 ] = 0;

	for( int i = 0; i < m_len; i++ )
	{
		if( i < m_len - 1 )
			sprintf( buffer, "%X;", m_root[ i ]->m_reg );
		else
			sprintf( buffer, "%X", m_root[ i ]->m_reg );
		
		strcat( s, buffer );
	}

	return s;
}
//---------------------------------------------
Cregarray::Treg* Cregarray::find( int reg )
{
	for( int i = 0; i < m_len; i++ )
	{
		if( m_root[ i ]->m_reg == reg )
			return m_root[ i ];
	}

	return NULL;
}
//---------------------------------------------
long int Cregarray::seek( int reg, int *value )
{
	int id;
	long int seek_pos = 0;

	// seek to begin
	fseek( m_savefile, seek_pos, SEEK_SET );

	// search register in file
	while( fread( &id, sizeof( int ), 1, m_savefile ) == 1 )
	{
		if( id == reg )
		{
			// register found
			fread( value, 1, sizeof( int ), m_savefile );
			return seek_pos;
		}
		seek_pos += 2 * sizeof( int );
		fseek( m_savefile, seek_pos, SEEK_SET );
	}
	// register not found
	*value = 0;
	// write new reg
	fwrite( &reg, sizeof( int ), 1, m_savefile );
	fwrite( value, sizeof( int ), 1, m_savefile );

	fseek( m_savefile, 0, SEEK_SET );
	return seek_pos;
}
//=============================================
	Cregarray::Treg::Treg( const int reg, const char *desc, const int a, 
						   const int b, const int c, const int d, const int flag, 
						   const long int seek, const int value )
{
	m_reg 	= reg;
	m_a 	= a;
	m_b 	= b;
	m_c 	= c;
	m_d 	= d;
	m_flag	= flag;
	m_seek  = seek;
	m_value = value;

	m_desc = ( char* ) malloc( strlen( desc ) + 1 );
	strcpy( m_desc, desc );

	// inti mutex
	pthread_mutex_init( &m_mux, NULL );
}
//---------------------------------------------
	Cregarray::Treg::~Treg()
{
	free( m_desc );
	pthread_mutex_destroy( &m_mux );
}
