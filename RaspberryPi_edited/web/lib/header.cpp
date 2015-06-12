#include "header.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//--------------------------------
CHeader::CHeader()
{
	m_headers = ( char** ) malloc( sizeof( char* ) );
	*m_headers = NULL;
}
//--------------------------------
CHeader::~CHeader()
{
	char **x = m_headers;

	while( *x )
	{
		free( *x );
		x++;
	}

	free( m_headers );
}
//--------------------------------
void CHeader::AddItem( const char *x )
{
	char **addr;

	// find similar headers
	if( ( addr = isSimilar( x ) ) )
	{
		free( *addr );
		*addr = ( char* ) malloc( strlen( x ) + 1 );
		strcpy( *addr, x );
		return;
	}

	int size = itemSize();

	m_headers = ( char** ) realloc( m_headers, sizeof( char* ) * ( size + 2 ) );
	m_headers[ size ] = ( char* ) malloc( strlen( x ) + 1 );
	strcpy( m_headers[ size ], x );
	m_headers[ size + 1 ] = NULL;
}
//--------------------------------
ostream& operator << ( ostream &os, const CHeader &x )
{
	x.Print( os );

	return os;
}
//--------------------------------
char** CHeader::isSimilar( const char *x )
{
	char **y = m_headers;

	while( *y )
	{
		if( compare( *y, x ) )
			return y;

		y++;
	}

	return NULL;
}
//--------------------------------
//porovnava hlavicku po ':' nezavisle na velkost pismen
int CHeader::compare( const char *x, const char *y )
{
	int diff;

	while( *x && *y )
	{
		diff = abs( *x - *y );

		// compare char case insensitive
		if( diff && diff != ( 'a' - 'A' ) )
			break;

		if( *x == ':' )
			return 1;

		x++;
		y++;
	}

	return 0;
}
//--------------------------------
int CHeader::itemSize()
{
	int size = 0;
	char **x = m_headers;

	while( *x )
	{
		size++;
		x++;
	}

	return size;
}
//--------------------------------
void CHeader::Print( ostream &os ) const
{
	char **x = m_headers;

	while( *x )
	{
		os << ( *x ) << endl;
		x++;
	}

	os << endl;
}