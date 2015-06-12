/**
 * @file   lexer.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  lexer
 *
 */

#ifndef LEXER
#define LEXER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief   Lexer class
 *
 * Read input file and return lexems.
 *
 */
class CLexer
{
	public:
		// lexems symbol types
		typedef enum {
			HTML, LPRINT, RPRINT, LSTAT, RSTAT, STRING,
			IDENT, NUMB, PLUS, MINUS, TIMES, DIVIDE, MOD, AND, OR,
			EQ, NEQ, LT, GT, LTE, GTE, LPAR, RPAR, ASSIGN, LBRA, RBRA, LSBRA, RSBRA,
			COMMA, SEMICOLON, RANGE,
			kwVAR, kwINCLUDE, kwIF, kwENDIF, kwELSE, kwFOR, kwENDFOR, kwIN,
			kwBLOCKPRINT, kwMENUPRINT, kwBLOCK, kwENDBLOCK, 
			EOI, ERR
		} SymbolType;

		static const char *SymbolTable[ 45 ];

		// symbol structure
		typedef struct LexSymbol
		{
			SymbolType type;
			char      *ident;  	/* atribut symbolu IDENT */
			int        number; 	/* atribut symbolu NUMB */
		} LexSymbol;
		//================
						/**
						 * @brief   Constructor
						 *
						 * @param [in] fileName 	Template file path.
						 *
						 */
		 		  		  CLexer( const char *fileName );
				 		 ~CLexer();
		/**
		 * @brief   Method to get next lexem
		 *
		 * @retval LexSymbol   Lex Symbol
		 *
		 */
		CLexer::LexSymbol getLexem();

		/**
		 * @brief   Get processing line number
		 *
		 * @retval int   Line number.
		 *
		 */
		int 			  getLineNum();

		/**
		 * @brief   Get error report.
		 *
		 * @retval char* 	Error report.
		 *
		 */
		char* 			  getError();

		/**
		 * @brief   Get template file name
		 *
		 * @retval char* 	Template name
		 *
		 */
		//vrati meno suboru ktore spracovava
		char*			  getFileName();
	private:
		bool  isHtml;
		char *m_error;
		char *m_filename;
		FILE *inputFile;
		int   m_lineNum;
		char  character;
		int   charType;
		//char *ident;
		int   identSize;
		LexSymbol s;
		typedef enum {LETTER, NUMBER, UNDERSCORE, WHITE_SPACE, END, NO_TYPE} InputCharType;
		//================
		/**
		 * @brief   Read character from file
		 *
		 */
		void readInput();

		/**
		 * @brief   Concat character to identifier name
		 *
		 * @retval c 	Character to concat.
		 *
		 */
		void insertChar( const char c );

		/**
		 * @brief   Save error character
		 *
		 * @retval c 	Error character.
		 *
		 */
		void saveError( char c );

		/**
		 * @brief   Is identifier key word?
		 *
		 * Return lex type 
		 *
		 * @retval SymbolType	Lex symbol type.
		 *
		 */
		SymbolType keyWord();
};

#endif