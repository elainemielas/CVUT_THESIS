/**
 * @file   view.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  view class
 *
 */

#include "view.hpp"
#include "tree.hpp"

#include "parser.hpp"

	 CView::CView( const char *path, CArray *data )
{
	m_path = ( char* ) malloc( strlen( path ) + 1 );
	strcpy( m_path, path );

	m_data = data;
}
//-----------------------------------------
	 CView::~CView()
{
	free( m_path );
	delete m_data;
}
//-----------------------------------------
ostream& operator << ( ostream &os, const CView &x )
{
	
	CParser parser( x.m_path, x.m_data );
	
	CTree *tree = parser.execParser();

	if( parser.getError() )
	{
		os << parser.getError();
		return os;
	}

	tree->Print( os, x.m_data );

	delete tree;
	delBlocks();
	
	return os;
}
//-----------------------------------------
