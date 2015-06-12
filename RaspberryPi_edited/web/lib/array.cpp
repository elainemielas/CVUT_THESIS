#include "array.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//===================================
CArray::Tnode::Tnode( const char *x )
{
	next = NULL;
	arr = new CArray;
	id = ( char* ) malloc( strlen( x ) + 1 );
	strcpy( id, x );
}
//-----------------------------------
CArray::Tnode::~Tnode()
{
	delete arr;
	free( id );
}
//===================================
CArray::CArray()
{
	root 	= NULL;
	str 	= ( char* ) malloc( sizeof( char ) );
	*str 	= 0;
	getnext = NULL;
}
//===================================
CArray::CArray( const CArray &x )
{
	root 	= NULL;
	getnext = NULL;
	str 	= ( char* ) malloc( strlen( x.str ) + 1 );
	strcpy( str, x.str );
}
//===================================
CArray::CArray( const int x )
{
	char buffer[ 12 ];

	root 	= NULL;
	getnext = NULL;

	sprintf( buffer, "%d", x );

	str = ( char* ) malloc( strlen( buffer ) + 1 );
	strcpy( str, buffer );
}
//===================================
CArray::CArray( const char *x )
{
	root 	= NULL;
	getnext = NULL;

	str = ( char* ) malloc( strlen( x ) + 1 );
	strcpy( str, x );
}
//-----------------------------------
CArray::CArray( const float x )
{
	char buffer[ 16 ];

	root 	= NULL;
	getnext = NULL;

	sprintf( buffer, "%g", x );

	str = ( char* ) malloc( strlen( buffer ) + 1 );
	strcpy( str, buffer );
}
//-----------------------------------
CArray::~CArray()
{
	Tnode *node;

	while( root )
	{
		node = root->next;
		delete root;
		root = node;
	}

	free( str );
}
//-----------------------------------
int CArray::empty()
{
	if( str[ 0 ] == 0 || ( str[ 1 ] == 0 && str[ 0 ] == '0' ) )
		return 1;

	return 0;
}
//-----------------------------------
int CArray::length()
{
	return strlen( str );
}
//-----------------------------------
CArray& CArray::operator [] ( const char *x )
{
	Tnode *node = root;

	//prehladavanie hodnoty pola
	while( node )
	{
		if( !strcmp( x, node->id ) )
			return *node->arr;

		node = node->next;
	}

	// ak taka hodnota neexistuje vytvori sa
	node = root;
	root = new Tnode( x );
	root->next = node;
	return *root->arr;
}
//-----------------------------------
CArray& CArray::operator [] ( const int x )
{
	char buffer[ 11 ];

	sprintf( buffer, "%d", x );

	return ( *this )[ buffer ];
}
//------------------------------------
CArray& CArray::operator = ( const CArray &x )
{
	if( this == &x )
		return *this;

	free( str );
	str = ( char* ) malloc( strlen( x.str ) + 1 );
	strcpy( str, x.str );

	return *this;
}
//-----------------------------------
CArray CArray::operator * ( const CArray &x )
{
	int n1, n2;
	CArray r;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return ( r = ( n1 * n2 ) );
}
//-----------------------------------
CArray CArray::operator / ( const CArray &x )
{
	int n1, n2;
	CArray r;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return ( r = ( n1 / n2 ) );
}
//-----------------------------------
CArray CArray::operator % ( const CArray &x )
{
	int n1, n2;
	CArray r;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return ( r = ( n1 % n2 ) );
}
//-----------------------------------
//numeric +
CArray CArray::operator + ( const CArray &x )
{
	int n1, n2;
	CArray r;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return ( r = ( n1 + n2 ) );
}
//-----------------------------------
CArray CArray::operator - ( const CArray &x )
{
	int n1, n2;
	CArray r;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return ( r = ( n1 - n2 ) );
}
//-----------------------------------
//concat string
CArray CArray::operator & ( const CArray &x )
{
	char *tmp;
	CArray r;

	tmp = ( char* ) malloc( strlen( str ) + strlen( x.str ) + 1 );
	strcpy( tmp, str );
	strcat( tmp, x.str );
	r = tmp;
	free( tmp );
	return r;
}
//-----------------------------------
int CArray::operator == ( const CArray &x )
{
	if( !str && !x.str )
		return 1;
	if( !str || !x.str )
		return 0;

	return !strcmp( str, x.str );
}
//-----------------------------------
int CArray::operator != ( const CArray &x )
{
	return !( *this == x );
}
//-----------------------------------
int CArray::operator < ( const CArray &x )
{
	if( !isNumber() || !x.isNumber() )
	{
		if( !x.str )
			return 0;
		else if( !str )
			return 1;
		else 
		{
			return  ( ( int ) strlen( str ) - ( int ) strlen( x.str ) ) < 0;
		}
	}

	int n1, n2;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return n1 < n2;
}
//-----------------------------------
int CArray::operator <= ( const CArray &x )
{
	if( !isNumber() || !x.isNumber() )
	{
		if( !x.str )
			return ( !str );
		else if( !str )
			return 1;
		else 
		{
			return  ( ( int ) strlen( str ) - ( int ) strlen( x.str ) ) <= 0;
		}
	}

	int n1, n2;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return n1 <= n2;
}
//-----------------------------------
int CArray::operator > ( const CArray &x )
{
	if( !isNumber() || !x.isNumber() )
	{
		if( !str )
			return 0;
		else if( !x.str )
			return 1;
		else 
		{
			return  ( ( int ) strlen( str ) - ( int ) strlen( x.str ) ) > 0;
		}
	}

	int n1, n2;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return n1 > n2;
}
//-----------------------------------
int CArray::operator >= ( const CArray &x )
{
	if( !isNumber() || !x.isNumber() )
	{
		if( !str )
			return ( !x.str );
		else if( !x.str )
			return 1;
		else 
		{
			return  ( ( int ) strlen( str ) - ( int ) strlen( x.str ) ) >= 0;
		}
	}

	int n1, n2;

	sscanf( str, "%d", &n1 );
	sscanf( x.str, "%d", &n2 );

	return n1 >= n2;
}
//-----------------------------------
ostream& operator << ( ostream &os, const CArray &x )
{
	if( x.str )
		os << x.str;
		//printf( "%s", str );
	
	if( x.root )
	{
		os << "is array { \n";
		CArray::Tnode *node = x.root;
		while( node )
		{
			os << "\t[\"" << node->id << "\"]=>" << *node->arr << endl;

			node = node->next;
		}
		os << " }\n";
	}
	
	if( !x.root && !x.str )
		os << "no set\n";

	return os;
}
//-----------------------------------
CArray* CArray::getNext()
{
	if( !getnext )
		getnext = root;
	else
		getnext = getnext->next;

	return ( getnext ? getnext->arr : NULL );
}
//-----------------------------------
char* CArray::getstr() const
{
	return str;
}
//-----------------------------------
void CArray::Print()
{
	if( str )
		printf( "%s", str );
	
	if( root )
	{
		printf("is array { \n" );
		Tnode *node = root;
		while( node )
		{
			printf( "\t[\"%s\"]=>", node->id );
			node->arr->Print();

			node = node->next;
		}
		printf( " }\n" );
	}
	
	if( !root && !str )
		printf("no set\n");
}
//====================================
int CArray::isNumber() const
{
	char *c = str;

	if( !str )
		return 0;

	if( *c == '-' )
		c++;

	while( *c )
	{
		if( *c < '0' || *c > '9' )
			return 0;

		c++;
	}

	return 1;
}