/**
 * @file   menu.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  menu class
 *
 */

#include "menu.hpp"

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

char *g_module_name;

CMenu::CMenu()
{
	m_root = NULL;
}
//---------------------------------------------
CMenu::~CMenu()
{
	TSubMenu *node = m_root;

	while( node )
	{
		node = m_root->m_next;
		delete m_root;
		m_root = node;
	}

}
//---------------------------------------------
void CMenu::AddSubmenu( const char *name, const char *url, const bool isSet )
{
	m_root = new TSubMenu( name, url, isSet, m_root );
}
//---------------------------------------------
CMenu::TSubMenu::TSubMenu( const char *name, const char *url, const bool isSet, TSubMenu *next )
{
	m_name = ( char* ) malloc( strlen( name ) + 1 );
	m_url  = ( char* ) malloc( strlen( url ) + 1 );

	strcpy( m_name, name );
	strcpy( m_url, url );

	m_isset = isSet;
	m_next  = next;
}
//---------------------------------------------
CMenu::TSubMenu::~TSubMenu()
{
	free( m_name );
	free( m_url );
}
//---------------------------------------------
ostream& operator << ( ostream &os, const CMenu &x )
{
	x.Print( os );

	return os;
}
//---------------------------------------------
void CMenu::Print( ostream &os ) const
{
	DIR *dp;
	struct dirent *ep;     
	dp = opendir ( "./modules" );
	TSubMenu *node = m_root;

	// overview ./modules directory
	if ( dp != NULL )
	{
		os << "<ul class=\"menu\">\n";
		while ( ( ep = readdir( dp ) ) )
		{
			// filter .so files
			if( !is_so( ep->d_name ) )
				continue;

			// remove .so
		    *strrchr( ep->d_name, '.' ) = 0;
		    // is active?
		    if( strcmp( g_module_name, ep->d_name ) )
				os << "\t<li><a href=\"/index.cgi/" << ep->d_name << "\">" << ep->d_name << "</a></li>\n";
			else
			{
				// active also print submenu
				os << "\t<li class=\"menu_selected\"><a href=\"/index.cgi/" << ep->d_name << "\">" << ep->d_name << "</a>\n";
					if( m_root )
						os <<"\t<ul class=\"submenu\">\n";
					// print submenu
					while( node )
					{
						os << "\t\t<li" << ( node->m_isset ? " class=\"submenu_selected\"" : "" ) 
						   << "><a href=\"" << node->m_url << "\">" << node->m_name << "</a></li>\n";
						node = node->m_next;
					}
					if( m_root )
						os << "\t</ul>\n";
				os << "</li>\n";
			}
		}
		os << "</ul>\n";

		(void) closedir( dp );
	}
	else
		os << "Couldn't open the directory\n";
}
//---------------------------------------------
int CMenu::is_so( const char *s ) const
{
	int len = strlen( s );

	return ( s[ len - 1 ] == 'o' && s[ len - 2 ] == 's' && s[ len - 3 ] == '.' );
}