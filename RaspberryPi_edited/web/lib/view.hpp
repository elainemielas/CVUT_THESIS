/**
 * @file   view.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  view class
 *
 */

#ifndef VIEW
#define VIEW

#include "array.hpp"
#include <iostream>

/**
 * @brief   View class
 *
 * Class render html code to client, use lexer, parser and syntax tree to generate html code.
 *
 */
class CView 
{
	public:
		/**
		 * @brief   Constructor
		 *
		 * @param [in] path		Path to template file
		 * @param [in] data 	Generated data used in template
		 *
		 */
	    CView( const char *path, CArray *data );

	    /**
		 * @brief   Destructor
		 *
		 */
	    ~CView();

		/**
		 * @brief   Operator << 
		 *
		 * @param [in] os		Output stream
		 * @param [in] x 		View
		 *
		 */
	    friend ostream & operator << ( ostream &os, const CView &x );
	protected:
		CArray *m_data;
		char   *m_path;
		
};

#endif