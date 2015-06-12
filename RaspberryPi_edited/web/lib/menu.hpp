/**
 * @file   menu.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  menu class
 *
 */

#ifndef MENU
#define MENU

#include <iostream>

using namespace std;

/**
 * @brief   Menu class
 *
 * Generate html menu from availavble modules in dir ./modules/\*.so
 *
 */
class CMenu
{
	public:
		CMenu();
		~CMenu();
		/**
		 * @brief   Add submenu entry
		 *
		 * Show under active module
		 *
		 * @param [in] name 	Submenu name
		 * @param [in] url 		Submenu url
		 * @param [in] isSet 	Is this item active now?
		 *
		 */
		void AddSubmenu( const char *name, const char *url, const bool isSet );
		/**
		 * @brief   Operator <<
		 *
		 * Print menu to stream 
		 *
		 * @param [in] ostream 	Output stream
		 * @param [in] x 		Menu
		 *
		 */
		friend ostream& operator << ( ostream &os, const CMenu &x );
	protected:
		// submenu structure 
		struct TSubMenu
		{
			TSubMenu( const char *name, const char *url, const bool isSet, TSubMenu *next );
			~TSubMenu();
			char 	 *m_name;
			char 	 *m_url;
			bool 	  m_isset;
			TSubMenu *m_next;
		};
		/**
		 * @brief   Is shared object?
		 *
		 * This method say if file is *.so (shared object file)
		 *
		 * @param [in] s 	File name
		 *
		 * @retval 0   Successfully.
		 * @retval 1   With error.
		 *
		 */
		int is_so( const char *s ) const;
		
		/**
		 * @brief   Print menu item to stream
		 *
		 * This function parse query and send answer
		 *
		 * @param [in] os 	Output stream
		 *
		 */
		void Print( ostream &os ) const;
		TSubMenu *m_root;
};

#endif