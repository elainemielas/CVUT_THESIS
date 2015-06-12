/**
 * @file   array.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  infinite associative array
 *
 */

#ifndef ARRAY
#define ARRAY

#include <iostream>

using namespace std;

class CArray
{
	public:
		CArray();
		CArray( const CArray &x );
		CArray( const int x );
		CArray( const float x );
		CArray( const char *x );
		~CArray();

		void Print();

		/**
		 * @brief   Method to crossing item array
		 *
		 * This function return item by item, at the end return NULL
		 *
		 * @retval NULL   	End items.
		 * @retval CArray   Item
		 *
		 */
		CArray* getNext();
		CArray& operator [] ( const char *x );
		CArray& operator [] ( const int x );
		// operator =
		CArray& operator = ( const CArray &x );
		
		/**
		 * @brief   Times two item if they are interger
		 *
		 * @retval CArray 	Return in CArray
		 *
		 */
		CArray operator * ( const CArray &x );
		
		/**
		 * @brief   Divide two item if they are interger
		 *
		 * @retval CArray 	Return in CArray
		 *
		 */
		CArray operator / ( const CArray &x );

		/**
		 * @brief   Modulo two item if they are interger
		 *
		 * @retval CArray 	Return in CArray
		 *
		 */
		CArray operator % ( const CArray &x );
		
		/**
		 * @brief   Plus two item if they are interger
		 *
		 * @retval CArray 	Return in CArray
		 *
		 */
		CArray operator + ( const CArray &x );
		
		/**
		 * @brief   Minus two item if they are interger
		 *
		 * @retval CArray 	Return in CArray
		 *
		 */
		CArray operator - ( const CArray &x );
		
		/**
		 * @brief   String concat two items 
		 *
		 * @retval CArray 	Return in CArray
		 *
		 */
		CArray operator & ( const CArray &x );

		/**
		 * @brief   Compare two items
		 *
		 * @retval 1 	when equal
		 * @retval 0 	when not equal
		 *
		 */
		int operator == ( const CArray &x );

		/**
		 * @brief   Compare two items
		 *
		 * @retval 0 	when equal
		 * @retval 1 	when not equal
		 *
		 */
		int operator != ( const CArray &x );
		
		/**
		 * @brief   Compare length two items
		 *
		 * If items are numbers compare by value, otherwise compare by string length
		 * 
		 * @retval 1 	Item lower, or smaller length
		 * @retval 0 	Item greater, or greater length
		 *
		 */
		int operator < ( const CArray &x );

		/**
		 * @brief   Compare length two items
		 *
		 * If items are numbers compare by value, otherwise compare by string length
		 * 
		 * @retval 0 	Item lower, or smaller length
		 * @retval 1 	Item greater, or greater length
		 *
		 */
		int operator > ( const CArray &x );

		/**
		 * @brief   Compare length two items
		 *
		 * If items are numbers compare by value, otherwise compare by string length
		 * 
		 * @retval 1 	Item lower or equal, or smaller or equal length
		 * @retval 0 	Item greater, or greater length
		 *
		 */
		int operator <= ( const CArray &x );

		/**
		 * @brief   Compare length two items
		 *
		 * If items are numbers compare by value, otherwise compare by string length
		 * 
		 * @retval 0 	Item lower, or smaller length
		 * @retval 1 	Item greater or equal, or greater or equal length
		 *
		 */
		int operator >= ( const CArray &x );
		friend ostream & operator << ( ostream &os, const CArray &x );
		
		/**
		 * @brief   Return c string ( char* )
		 *
		 * @retval char* 	C string
		 *
		 */
		char* getstr() const;
		
		/**
		 * @brief   Is empty?
		 *
		 * If items are numbers compare by value, otherwise compare by string length
		 * 
		 * @retval 1 	Item is empty string or 0
		 * @retval 0 	Else
		 *
		 */
		int empty();
		
		/**
		 * @brief   Return string length
		 * 
		 * @retval int String length
		 *
		 */
		int length();
	protected:
		// array node
		struct Tnode
		{
			Tnode( const char *x );
			~Tnode();
			Tnode  *next;
			CArray *arr;
			char   *id;
		};
		// root node
		Tnode  *root;
		Tnode  *getnext;
		// value
		char   *str;
		int     isNumber() const;
};

#endif