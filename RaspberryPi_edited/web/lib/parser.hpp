/**
 * @file   parser.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  template parser, return syntax tree
 *
 */

#ifndef PARSER
#define PARSER

#include "lexer.hpp"
#include "array.hpp"
#include "tree.hpp"

class CParser
{
	public:
			/**
			 * @brief   Constructor
			 *
			 * @param [in] file	 Template file path
			 * @param [in] data  Data array from View
			 *
			 */
			CParser( const char *file, const CArray *data );
			~CParser();
		/**
		 * @brief   Method start parsing
		 *
		 * @retval CTree  	Syntax tree
		 *
		 */
		CTree* execParser();
		/**
		 * @brief   Method to get error, that occurred during parsing
		 *
		 * @retval char*  	Error text
		 *
		 */
		char* getError();
	private:
		/**
		 * @brief   Method to save error 
		 *
		 * @param [in] char* 	Text to save
		 *
		 */
		void saveError( const char *x );

		/**
		 * @brief   Start parser point 
		 *
		 */
		CTree* Start();

		/**
		 * @brief   Parse for range values
		 *
		 * This function parse query and send answer
		 *
		 * @param [in] first 	Pointer to save first for value
		 * @param [in] last 	Pointer to save last for value
		 *
		 * @retval Cvar   Tree node Cvar.
		 *
		 */
		CVar* ForRange( int *first, int *last );

		/**
		 * @brief   Parse integer
		 *
		 * @retval int 	Number.
		 *
		 */
		int   Number();

		/**
		 * @brief   Parse Array ... [ x ][][]...
		 *
		 * @retval CVar   Tree node CVar
		 *
		 */
		CVar* Array();

		/**
		 * @brief   Parse array index
		 *
		 * @param [in] id 	pointer to save index name/or identifier
		 * @param [in] type pointer to save index type
		 *
		 */
		void ArrayType( char **id, CVar::IdentType *type );

		/**
		 * @brief   Comparsion lex function
		 *
		 * Compare two lex on input and expected. If they not equal throw error. Return lex identifier.
		 *
		 * @param [in] getNext  If true get next lexem, default true.
		 *
		 * @retval char* 	Lex identifier
		 *
		 */
		char* comparsion( CLexer::SymbolType x, bool getNext );

		/**
		 * @brief   Parse else block
		 *
		 * @retval CTree 	Else block tree.
		 *
		 */
		CTree* Else();

		/**
		 * @brief   Parse expression
		 *
		 * @retval CTree 	Expression tree.
		 *
		 */
		CTree* Expression();

		/**
		 * @brief   Parse expression
		 *
		 * @retval CTree 	Expression tree.
		 *
		 */
		CTree* Opr1();

		/**
		 * @brief   Parse expression
		 *
		 * @retval CTree 	Expression tree.
		 *
		 */
		CTree* Opr2();

		/**
		 * @brief   Parse expression operand
		 *
		 * @retval CTree 	Expression tree.
		 *
		 */
		CTree* Opd();

		/**
		 * @brief   Parse expression logic operation
		 *
		 * @retval CTree 	Expression tree.
		 *
		 */
		CTree* Opr1_( CTree *op );

		/**
		 * @brief   Parse expression plus, minus
		 *
		 * @retval CTree 	Expression tree.
		 *
		 */
		CTree* Opr2_( CTree *op );

		/**
		 * @brief   Parse expression times, devide, modulo
		 *
		 * @retval CTree 	Expression tree.
		 *
		 */
		CTree* Opr3_( CTree *op );
		//==========================
		CTree *m_main;
		//? struct tree *block
		const CArray 		*m_data;
		CLexer 				*m_lexer;
		char 				*m_error;
};

#endif