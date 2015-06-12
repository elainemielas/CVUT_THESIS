/**
 * @file   lexer.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  lexer
 *
 */

#include "lexer.hpp"


// symbol table meaning
const char *CLexer::SymbolTable[ 45 ] = {
   	"text html", "LPRINT", "RPRINT", "LSTAT", "RSTAT", "string",
	"identifier", "number", "plus", "minus", "times", "divide", "modulo", "and", "or",
	"equal", "not equal", "<", ">", "<=", ">=", "(", ")", "=", "LBRA", "RBRA", "LSBRA", "RSBRA",
	",", ";", "..",
	"key word 'variable'", "key word 'include'", "key word 'if'", "key word 'endif'", "key word 'else'", 
	"key word 'for'", "key word 'endfor'", "key word 'in'",
	"key word 'blockprint'", "key word 'menuprint'", "key word 'block'", "key word 'endblock'", 
	"end of file", "error"
};

// key word table
const struct { const char* word; CLexer::SymbolType symb; } keyWordTable[] = {
   {"if", CLexer::kwIF},
   {"endif", CLexer::kwENDIF},
   {"else", CLexer::kwELSE},
   {"for", CLexer::kwFOR},
   {"in", CLexer::kwIN},
   {"endfor", CLexer::kwENDFOR},
   {"include", CLexer::kwINCLUDE},
   {"blockprint", CLexer::kwBLOCKPRINT},
   {"menuprint", CLexer::kwMENUPRINT},
   {"block", CLexer::kwBLOCK},
   {"endblock", CLexer::kwENDBLOCK},
   {NULL, ( CLexer::SymbolType ) 0}
};

//=========================================
CLexer::SymbolType CLexer::keyWord() 
{
	int i = 0;

	// is indent key word?
	while( keyWordTable[ i ].word ) 
		if( !strcmp( s.ident, keyWordTable[ i ].word ) )
		{
			free( s.ident );
			s.ident = NULL;
			return keyWordTable[ i ].symb;
		}
		else
			i++;

	return IDENT;
}
//----------------------------------------
void CLexer::readInput()
{
	character = fgetc( inputFile );

	// translate input chat to char type
	if( ( character >= 'A' && character <= 'Z' ) || ( character >= 'a' && character <= 'z') )
		charType = LETTER;
	else if ( character >= '0' && character <= '9' )
		charType = NUMBER;
	else if ( character == '_' )
		charType = UNDERSCORE;
	else if ( (char) character ==  (char) EOF )
		charType = END;
	else if ( character == '\n' )
	{
		charType = WHITE_SPACE;
		m_lineNum++;
	}
	else if ( character <= ' ' )
		charType = WHITE_SPACE;
	else
		charType = NO_TYPE;
}
//----------------------------------------
CLexer::CLexer( const char *fileName )
{
	m_filename = ( char* ) malloc( strlen( fileName ) + 1 + 5 );
	strcpy( m_filename, "view/" );
	strcat( m_filename, fileName );

	// open file
	inputFile = fopen( m_filename, "rt" );

	m_lineNum = 1;
	m_error = NULL;
	isHtml = true;

	if( !inputFile )
	{
		m_error = ( char* ) malloc( 32 + strlen( m_filename ) );
		sprintf( m_error, "Lexer error file '%s' not found!", m_filename );
	}
	else
		readInput();
}
//----------------------------------------
CLexer::~CLexer()
{
	if( inputFile )
		fclose( inputFile );

	free( m_error );
	free( m_filename );
}
//----------------------------------------
int CLexer::getLineNum()
{
	return m_lineNum;
}
//----------------------------------------
char* CLexer::getError()
{
	return m_error;
}
//----------------------------------------
char* CLexer::getFileName()
{
	return m_filename;
}
//----------------------------------------
void CLexer::saveError( char c )
{
	// only first error save
	if( m_error )
		return;

	m_error = ( char* ) malloc( 64 + strlen( m_filename ) );
	sprintf( m_error, "Lexer error in file %s:%d unknown character '%c'", m_filename, m_lineNum, c );
}
//----------------------------------------
void CLexer::insertChar( const char c )
{
	unsigned int len = ( s.ident ? strlen( s.ident ) : 0 );

	if( ( unsigned int ) identSize <= ( len + 1 ) )
	{
		identSize = ( identSize ? identSize * 2 : 32 );
		s.ident = ( char* ) realloc( s.ident, identSize );
	}

	s.ident[ len ]     = c;
	s.ident[ len + 1 ] = 0;
}
//----------------------------------------
CLexer::LexSymbol CLexer::getLexem()
{
	s.ident = NULL;
	identSize = 0;

	// automat
q0:
	// html code only copy, lexems between {{  }}
	if( !isHtml )
		goto q2;
	else if( character == '{' )
	{
		readInput();
		goto q1;
	}
	else if( charType == END )
	{
		s.type = HTML;
		isHtml = false;
		return s;
	}
	else
	{
		insertChar( character );
		readInput();
		goto q0;
	}

// lexems start? 
q1:
	switch( character )
	{
		case '{':
			s.type = HTML;
			isHtml = false;
			readInput();
			//empty html code throw
			if( !s.ident )
				goto q2;
			return s;
		default:
			insertChar( '{' );
			insertChar( character );
			readInput();
			goto q0;
	}
	saveError( character );

// lexems
q2:
	switch( charType )
	{
		case WHITE_SPACE:
			readInput();
			goto q2;
		case END:
			s.type = EOI;
			return s;
		case NUMBER:
			s.type = NUMB;
			s.number = character - '0';
			readInput();
			goto q9;
		case LETTER:
			insertChar( character );
			readInput();
			goto q5;
	}
	switch( character )
	{
		case '.':
			readInput();
			goto q8;
		case '[':
			s.type = LSBRA;
			readInput();
			return s;
		case ']':
			s.type = RSBRA;
			readInput();
			return s;
		case '{':
			readInput();
			goto q6;
		case '}':
			readInput();
			goto q3;
		case '(':
			s.type = LPAR;
			readInput();
			return s;
		case ')':
			s.type = RPAR;
			readInput();
			return s;
		case '&':
			readInput();
			goto q14;
		case '|':
			readInput();
			goto q15;
		case '<':
			readInput();
			goto q12;
		case '>':
			readInput();
			goto q13;
		case '=':
			readInput();
			goto q10;
		case '!':
			readInput();
			goto q11;
		case '-':
			s.type = MINUS;
			readInput();
			return s;
		case '+':
			s.type = PLUS;
			readInput();
			return s;
		case '*':
			s.type = TIMES;
			readInput();
			return s;
		case '/':
			s.type = DIVIDE;
			readInput();
			return s;
		case '%':
			s.type = MOD;
			readInput();
			return s;
		case '"':
			s.type = STRING;
			readInput();
			goto q7;
	}
	saveError( character );

q3:
	switch( character )
	{
		case '}':
			readInput();
			isHtml = true;
			goto q0;
		default:
			readInput();
			s.type = RPAR;
			return s;
	}
	saveError( character );

q6:
	switch( character )
	{
		case '{':
			readInput();
			s.type = LPRINT;
			return s;
		case '%':
			readInput();
			s.type = LSTAT;
			return s;
		default:
			readInput();
			s.type = LBRA;
			return s;
	}
	saveError( character );

//identifikator
q5:	
	switch( charType )
	{
		case LETTER:
		case NUMBER:
		case UNDERSCORE:
			insertChar( character );
			readInput();
			goto q5;
		default:
			s.type = keyWord();
			return s;
	}
	saveError( character );

//string
q7:
	switch( character )
	{
		case '"':
			readInput();
			if( !s.ident )
				insertChar( 0 );
			return s;
		default:
			insertChar( character );
			readInput();
			goto q7;
	}
	saveError( character );
//range
q8:
	switch( character )
	{
		case '.':
			readInput();
			s.type = RANGE;
			return s;
		default:
			readInput();
			s.type = ERR;
			return s;
	}
	saveError( character );

//number
q9:
	switch( charType )
	{
		case NUMBER:
			s.number = s.number * 10 + ( character - '0' );
			readInput();
			goto q9;
		default:
			return s;
	}
	saveError( character );

//EQ == 
q10:
	switch( character )
	{
		case '=':
			s.type = EQ;
			readInput();
			return s;
		default:
			s.type = ERR;
			return s;
	}

//NEQ !=
q11:
	switch( character )
	{
		case '=':
			s.type = NEQ;
			readInput();
			return s;
		default:
			s.type = ERR;
			return s;
	}

//LTE or LT <= <
q12:
	switch( character )
	{
		case '=':
			s.type = LTE;
			readInput();
			return s;
		default:
			s.type = LT;
			return s;
	}

//GTE or GT >= >
q13:
	switch( character )
	{
		case '=':
			s.type = GTE;
			readInput();
			return s;
		default:
			s.type = GT;
			return s;
	}

//AND &&
q14:
	switch( character )
	{
		case '&':
			s.type = AND;
			readInput();
			return s;
		default:
			s.type = ERR;
			return s;
	}

//OR &&
q15:
	switch( character )
	{
		case '|':
			s.type = OR;
			readInput();
			return s;
		default:
			s.type = ERR;
			return s;
	}

}