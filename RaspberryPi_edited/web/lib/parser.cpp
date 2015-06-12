/**
 * @file   parser.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  template parser, return syntax tree
 *
 */

#include "parser.hpp"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

CLexer::LexSymbol g_s;


CParser::CParser( const char *file, const CArray *data )
{
	m_data  = data;
	m_error = NULL;
	m_lexer = new CLexer( file );
}
//-----------------------------------------------
CParser::~CParser()
{
	delete m_lexer;
	free( m_error );
}
//-----------------------------------------------
CTree* CParser::execParser()
{
	// some lexer error?
	if( m_lexer->getError() )
		return NULL;

	// init parser
	g_s = m_lexer->getLexem();

	// start parser
	m_main = Start();
	
	// check if an error occurred
	if( m_lexer->getError() || m_error )
	{
		delete m_main;
		delBlocks();
		return NULL;
	}

	return m_main;
}
//-----------------------------------------------
void CParser::saveError( const char *x )
{
	// already has error
	if( m_lexer->getError() || m_error )
		return;

	m_error = ( char* ) malloc( 32 + strlen( m_lexer->getFileName() ) + strlen( x ) );
	sprintf( m_error, "Parser error %s:%d %s", m_lexer->getFileName(), m_lexer->getLineNum(), x );
}
//-----------------------------------------------
char *CParser::getError()
{
	if( m_lexer->getError() )
		return m_lexer->getError();

	return m_error;
}
//-----------------------------------------------
char* CParser::comparsion( CLexer::SymbolType x, bool getNext = true )
{
	char *tmp;

	// not equal, save error
	if( g_s.type != x )
	{
		free( g_s.ident );
		char err[ 128 ];
		sprintf( err, "expect %s instead %s", CLexer::SymbolTable[ x ], CLexer::SymbolTable[ g_s.type ] );
		saveError( err );
		g_s = m_lexer->getLexem();
		return NULL;
	}

	tmp = g_s.ident;
	if( getNext )	
		g_s = m_lexer->getLexem();

	return tmp;
}
//-----------------------------------------------
CTree* CParser::Start()
{
	char *id;
	CLexer *tmp;
	CTree *tree, *op;
	CVar  *var;

	switch( g_s.type )
	{
		// html text
		case CLexer::HTML:
			id = g_s.ident;
			g_s = m_lexer->getLexem();
			return new CHtml( id, Start() );
		// error
		case CLexer::EOI:
			return NULL;
		// identifier -> variable
		case CLexer::IDENT:
			id = g_s.ident;
			g_s = m_lexer->getLexem();
			var = Array();
			if( var )
				var->appendVar( new CVar( id, CVar::IDENT, NULL, NULL ) );
			else 
				var = new CVar( id, CVar::IDENT, NULL, NULL );

			return ( CTree* ) ( var->append( Start() ) );
		// blockprint
		case CLexer::kwBLOCKPRINT:
			g_s = m_lexer->getLexem();
			id = comparsion( CLexer::STRING );
			return new CPrintblock( id, Start() );
		// menuprint
		case CLexer::kwMENUPRINT:
			g_s = m_lexer->getLexem();
			return new CPrintmenu( Start() );
		// declare new block
		case CLexer::kwBLOCK:
			g_s = m_lexer->getLexem();
			id = comparsion( CLexer::STRING );
			addBlock( id, Start() );
			comparsion( CLexer::kwENDBLOCK );
			return Start();
		// declare include -> save lexer and parse other file
		case CLexer::kwINCLUDE:
			g_s = m_lexer->getLexem();
			id = comparsion( CLexer::STRING, false );
			//ked id == NULL /doslo k chybe 
			if( !id )
				return NULL;
			//parsing new file
			tmp = m_lexer;
			m_lexer = new CLexer( id );
			free( id );
			//check errors
			if( m_lexer->getError() )
			{	
				delete tmp;
				return NULL;
			}
			g_s = m_lexer->getLexem();
			tree = Start();
			//return old file lexer
			delete m_lexer;
			m_lexer = tmp;
			g_s = m_lexer->getLexem();
			return tree->append( Start() );
		// for loop 
		case CLexer::kwFOR:
			int first, last;
			g_s = m_lexer->getLexem();
			id = comparsion( CLexer::IDENT );
			comparsion( CLexer::kwIN );
			var  = ForRange( &first, &last );
			tree = Start();
			comparsion( CLexer::kwENDFOR );
			// check if for or foreach
			if( var )
				return new CForeach( id, var, tree, Start() );
			else
				return new CFor( id, first, last, tree, Start() );
		// if/else condition
		case CLexer::kwIF:
			g_s = m_lexer->getLexem();
			op = Expression();
			tree = Start();
			return new CIf( op, tree, Else(), Start() );
		default:
			break;
	}

	//saveError( "start error" );
	free( g_s.ident );
	return NULL;
}
//-----------------------------------------------
CTree* CParser::Else()
{
	CTree *tree;

	switch( g_s.type )
	{
		// else block
		case CLexer::kwELSE:
			g_s = m_lexer->getLexem();
			tree = Start();
			comparsion( CLexer::kwENDIF );
			return tree;
		// if without else block
		case CLexer::kwENDIF:
			g_s = m_lexer->getLexem();
			return NULL;
		default:
			free( g_s.ident );
			g_s = m_lexer->getLexem();
			saveError( "expect else or endif" );
	}
	return NULL;
}
//-----------------------------------------------
CTree* CParser::Expression()
{
	CTree *op;	//operand 1,2 

	op = Opr1();
	return Opr1_( op );
}
//-----------------------------------------------
CTree* CParser::Opr1()
{
	CTree *op;

	op = Opr2();
	return Opr2_( op );
}
//-----------------------------------------------
CTree* CParser::Opr1_( CTree *op )
{
	switch( g_s.type )
	{
		// parse ==
		case CLexer::EQ:
			g_s = m_lexer->getLexem();
			return new CLOperation( CLOperation::EQ, op, Expression() );
		// parse !=
		case CLexer::NEQ:
			g_s = m_lexer->getLexem();
			return new CLOperation( CLOperation::NEQ, op, Expression() );
		// parse <
		case CLexer::LT:
			g_s = m_lexer->getLexem();
			return new CLOperation( CLOperation::LT, op, Expression() );
		// parse >
		case CLexer::GT:
			g_s = m_lexer->getLexem();
			return new CLOperation( CLOperation::GT, op, Expression() );
		// parse <=
		case CLexer::LTE:
			g_s = m_lexer->getLexem();
			return new CLOperation( CLOperation::LTE, op, Expression() );
		// parse >=
		case CLexer::GTE:
			g_s = m_lexer->getLexem();
			return new CLOperation( CLOperation::GTE, op, Expression() );
		// parse &&
		case CLexer::AND:
			g_s = m_lexer->getLexem();
			return new CLOperation( CLOperation::AND, op, Expression() );
		// parse ||
		case CLexer::OR:
			g_s = m_lexer->getLexem();
			return new CLOperation( CLOperation::OR, op, Expression() );
		default:
			return op;
	}
}
//-----------------------------------------------
CTree* CParser::Opr2()
{
	CTree *op;

	switch( g_s.type )
	{
		// unarny minus
		case CLexer::MINUS:
			g_s = m_lexer->getLexem();
			op = new CUnMinus( Opd() );
			return Opr3_( op );
		default:
		op = Opd();
		return Opr3_( op );
	}
}
//-----------------------------------------------
CTree* CParser::Opr2_( CTree *op )
{
	switch( g_s.type )
	{
		// parse +
		case CLexer::PLUS:
			g_s = m_lexer->getLexem();
			return new CAOperation( CAOperation::PLUS, op, Opr1() );
		// parse -
		case CLexer::MINUS:
			g_s = m_lexer->getLexem();
			return new CAOperation( CAOperation::MINUS, op, Opr1() );
		default:
			return op;
	}
}
//-----------------------------------------------
CTree* CParser::Opr3_( CTree *op )
{
	switch( g_s.type )
	{
		// parse *
		case CLexer::TIMES:
			g_s = m_lexer->getLexem();
			return new CAOperation( CAOperation::TIMES, op, Opr2() );
		// parse /
		case CLexer::DIVIDE:
			g_s = m_lexer->getLexem();
			return new CAOperation( CAOperation::DIVIDE, op, Opr2() );
		// parse %
		case CLexer::MOD:
			g_s = m_lexer->getLexem();
			return new CAOperation( CAOperation::MODULO, op, Opr2() );
		default:
			return op;
	}
}
//-----------------------------------------------
CTree* CParser::Opd()
{
	char  *id;
	CVar  *var;
	CTree *tree = NULL;
	int    num;

	switch( g_s.type )
	{
		// parse variable
		case CLexer::IDENT:
			id = g_s.ident;
			g_s = m_lexer->getLexem();
			var = Array();
			if( var )
				var->appendVar( new CVar( id, CVar::IDENT, NULL, NULL ) );
			else 
				var = new CVar( id, CVar::IDENT, NULL, NULL );

			return var;
		// parse number
		case CLexer::NUMB:
			num = g_s.number;
			g_s = m_lexer->getLexem();
			return new CVal( num );
		// parse string "xyz"
		case CLexer::STRING:
			tree  = new CVal( g_s.ident );
			free( g_s.ident );
			g_s = m_lexer->getLexem();
			return tree;
		// parse left parenthesis
		case CLexer::LPAR:
			g_s = m_lexer->getLexem();
			tree = Expression();
			comparsion( CLexer::RPAR );
			return tree;
		default:
			free( g_s.ident );
			g_s = m_lexer->getLexem();
			saveError( "expect operand" );
			return NULL;
	}
}
//-----------------------------------------------
CVar* CParser::Array()
{
	char  *id;
	CVar  *tree;
	CVar::IdentType type;

	switch( g_s.type )
	{
		// expect left parenthesis [
		case CLexer::LSBRA:
			g_s = m_lexer->getLexem();
			ArrayType( &id, &type );
			comparsion( CLexer::RSBRA );
			tree = Array();
			return ( tree ? tree->appendVar( new CVar( id, type, NULL, NULL ) ) : new CVar( id, type, NULL, NULL ) );
		default:
			return NULL;
	}
}
//-----------------------------------------------
void CParser::ArrayType( char **id, CVar::IdentType *type )
{
	switch( g_s.type )
	{
		// index by string
		case CLexer::STRING:
			*type = CVar::STRING;
			*id   = g_s.ident;
			g_s = m_lexer->getLexem();
			return;
		// index by variable
		case CLexer::IDENT:
			*type = CVar::IDENT;
			*id   = g_s.ident;
			g_s = m_lexer->getLexem();
			return;
		default:
			free( g_s.ident );
			g_s = m_lexer->getLexem();
			break;
	}
	*id = ( char* ) malloc( sizeof( char ) );
	**id = 0;

	saveError( "expect string or identifier in array index" );
}
//-----------------------------------------------
CVar* CParser::ForRange( int *first, int *last )
{
	char *id;
	CVar *var;
	//var 
	switch( g_s.type )
	{
		// indicates foreach
		case CLexer::IDENT:
			id = g_s.ident;
			g_s = m_lexer->getLexem();
			var = Array();
			if( var )
				var->appendVar( new CVar( id, CVar::IDENT, NULL, NULL ) );
			else 
				var = new CVar( id, CVar::IDENT, NULL, NULL );
			return var;
		// indicate for loop
		case CLexer::NUMB:
			*first = g_s.number;
			g_s = m_lexer->getLexem();
			comparsion( CLexer::RANGE );
			*last = Number();
			return NULL;
		case CLexer::MINUS:
			g_s = m_lexer->getLexem();
			comparsion( CLexer::NUMB );
			*first = -g_s.number;
			comparsion( CLexer::RANGE );
			*last = Number();
			return NULL;
		default:
			free( g_s.ident );
			g_s = m_lexer->getLexem();
			break;
	}

	saveError( "expect number or identifier in for iteration" );
	return NULL;
}

int CParser::Number()
{
	switch( g_s.type )
	{
		// negativ minus
		case CLexer::MINUS:
			g_s = m_lexer->getLexem();
			comparsion( CLexer::NUMB );
			return ( -g_s.number );
		// positive number
		case CLexer::NUMB:
			g_s = m_lexer->getLexem();
			return ( g_s.number );
		default:
			g_s = m_lexer->getLexem();
			saveError( "expect number" );
	}

	return 0;
}

