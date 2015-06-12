/**
 * @file   header.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  Html header
 *
 */

#ifndef HEADER
#define HEADER

/*
 * trieda hlaviciek html
 */
#include <iostream>

using namespace std;

class CHeader
{
	public:
		CHeader();
		~CHeader();

		/**
		 * @brief   Add new html header
		 *
		 * @param [in] x 	Header
		 *
		 */
		void AddItem( const char *x );
		
		/**
		 * @brief   Operator << 
		 *
		 * Print headers to output stream
		 *
		 * @param [in] os 	Output stream
		 * @param [in] x 	Cheader
		 *
		 */
		friend ostream& operator << ( ostream &os, const CHeader &x );
	protected:
		/**
		 * @brief   Find similar header
		 *
		 * Check if that head exist
		 *
		 * @param [in] x 	Header
		 *
		 * @retval NULL   	Not found.
		 * @retval char**   Pointer to header row.
		 *
		 */
		char **isSimilar( const char *x );

		/**
		 * @brief   Case insensitive header compare
		 *
		 * @param [in] x 	First header
		 * @param [in] y 	Second header
		 *
		 */
		int    compare( const char *x, const char *y );

		/**
		 * @brief   Return header count.
		 *
		 * @retval int 	Size.
		 *
		 */
		int    itemSize();

		/**
		 * @brief   Print header to stream
		 *
		 * @param [in] os 	Output stream
		 *
		 */
		void Print( ostream &os ) const;

		// root node
		char **m_headers;
};

#endif