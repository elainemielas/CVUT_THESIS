#include "tree.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controller.hpp"

CController *g_controller;

// block structure
struct TBlock
{
	TBlock( char *id, CTree *tree );
	~TBlock();
	char   *m_id;
	CTree  *m_tree;
	TBlock *m_next;
};

// var structure
struct TVar
{
	TVar( char *id, CArray *arr );
	char   *m_id;
	CArray *m_arr;
	TVar   *m_next;
};

// block root node
TBlock *g_block;

// var root node
TVar   *g_var;


//-----------------------------------
TVar::TVar( char *id, CArray *arr )
{
	m_id   = id;
	m_arr  = arr;
	m_next = g_var;
}
//-----------------------------------
void addVar( char *id, CArray *arr )
{
	TVar *node = g_var;

	// exist?
	while( node )
	{
		if( !strcmp( id, node->m_id ) )
			break;

		node = node->m_next;
	}

	if( node )
		node->m_arr = arr;
	else
		g_var = new TVar( id, arr );
}
//-----------------------------------
void delVar( char *id )
{
	TVar *node = g_var;
	TVar *prev = NULL;

	// find var
	while( node )
	{
		if( !strcmp( node->m_id, id ) )
		{
			if( !prev )
				g_var = node->m_next;
			else
				prev->m_next = node->m_next;

			delete node;
			break;
		}

		prev = node;
		node = node->m_next;
	}
}
//-----------------------------------
CArray* getVar( const char *id )
{
	TVar *node = g_var;

	// find var
	while( node )
	{
		if( !strcmp( id, node->m_id ) )
			return ( node->m_arr );

		node = node->m_next;
	}
	
	return NULL;
}
//------------------
TBlock::TBlock( char *id, CTree *tree )
{
	m_id   = id;
	m_tree = tree;
	m_next = g_block;
}
//-----------------------------------
TBlock::~TBlock()
{
	free( m_id );
	delete m_tree;
}
//-----------------------------------
void addBlock( char *id, CTree *tree )
{
	TBlock *block = new TBlock( id, tree );
	g_block 	  = block;
}
//-----------------------------------
CTree* getBlock( const char *id )
{
	TBlock *node = g_block;

	// find block
	while( node )
	{
		if( !strcmp( id, node->m_id ) )
			return ( node->m_tree );

		node = node->m_next;
	}
	
	return NULL;
}
//-----------------------------------
void delBlocks()
{
	TBlock *node;
	// find block
	while( g_block )
	{
		node = g_block->m_next;
		delete g_block;
		g_block = node;
	}
}
//= CTree ==============================
CTree* CTree::append( CTree *x )
{
	CTree *node = this;
	
	// get to the end
	while( node->m_next )
		node = node->m_next;

	// append
	node->m_next = x;

	return this;
}
//= CHtml ==============================
CHtml::CHtml( char *html, CTree *next )
{  
	m_next = next;
	m_html = html;
}
//--------------------------------------
CHtml::~CHtml()
{ 
	free( m_html ); 
	
	if( m_next )
		delete m_next; 
}
//--------------------------------------
void CHtml::Print( ostream &os, CArray *data )
{
	os << m_html;
	// print next
	if( m_next )
		m_next->Print( os, data );
}
//= CVar ===============================
CVar::CVar( char *var, IdentType type, CVar *array, CTree *next )
{
	m_next  = next;
	m_var   = var;
	m_array = array;
	m_type  = type;
}
//--------------------------------------
CVar::~CVar()
{ 
	free( m_var ); 
	
	if( m_next )
		delete m_next; 

	if( m_array )
		delete m_array; 
}
//--------------------------------------
CVar* CVar::appendVar( CVar *x )
{
	CVar *node = this;
	
	// get to the end
	while( node->m_array )
		node = node->m_array;

	// append
	node->m_array = x;

	return this;
}
//--------------------------------------
void CVar::Print( ostream &os, CArray *data )
{
	// last array ident iterator ?
	if( m_type == IDENT && !m_array && getVar( m_var ) )
		os << ( *getVar( m_var ) );
	else
		// string iterator
		os << ( *getArray( data ) );

	// print next node
	if( m_next )
		m_next->Print( os, data );
}
//--------------------------------------
int CVar::isTrue( CArray *data )
{
	// ident iterate ( by varible )
	if( m_type == IDENT && !m_array && getVar( m_var ) )
		return ( !getVar( m_var )->empty() );
	else
		// string iterator
		return ( !getArray( data )->empty() );
}
//--------------------------------------
CArray* CVar::getArray( CArray *data )
{
	char *tmp = m_var;

	// varible iteration
	if( m_type == IDENT && getVar( m_var ) )
	{
		tmp = getVar( m_var )->getstr();
	}

	if( m_array )
		// variable iteration ^^
		return &( ( *( m_array->getArray( data ) ) )[ tmp ] );
	else if( getVar( m_var ) )
		// varible print
		return getVar( m_var );
	else
		// string iteration
		return &( ( *data )[ tmp ] );
}
//= CPrintblock ========================
CPrintblock::CPrintblock( char *block, CTree *next )
{
	m_next  = next;
	m_block = block;
}
//--------------------------------------
CPrintblock::~CPrintblock()
{ 
	free( m_block ); 
	
	if( m_next )
		delete m_next; 
}
//--------------------------------------
void CPrintblock::Print( ostream &os, CArray *data )
{ 
	CTree *block = getBlock( m_block );
	if( block )
		block->Print( os, data );

	if( m_next )
		m_next->Print( os, data );
}
//======================================
CPrintmenu::CPrintmenu( CTree *next )
{
	m_next  = next;
}
//--------------------------------------
CPrintmenu::~CPrintmenu()
{	
	if( m_next )
		delete m_next; 
}
//--------------------------------------
void CPrintmenu::Print( ostream &os, CArray *data )
{ 
	//menu
	os << *( g_controller->menu );

	if( m_next )
		m_next->Print( os, data );
}
//= CForeach ===========================
CForeach::CForeach( char *id, CVar *var, CTree *block, CTree *next )
{
	m_id	= id;
	m_var	= var;
	m_block = block;
	m_next  = next;
}
//--------------------------------------
CForeach::~CForeach()
{ 
	delete m_block;
	delete m_var;
	free( m_id );

	if( m_next )
		delete m_next; 
}
//--------------------------------------
void CForeach::Print( ostream &os, CArray *data )
{
	CArray *tmp;
	CArray *var = m_var->getArray( data );

	// foreach all element from array
	while( ( tmp = ( var->getNext() ) ) )
	{
		addVar( m_id, tmp );
		m_block->Print( os, data );
	}

	delVar( m_id );

	if( m_next )
		m_next->Print( os, data );
}
//= CFor ===============================
CFor::CFor( char *id, int first, int last, CTree *block, CTree *next )
{
	m_id	= id;
	m_first = first;
	m_last  = last;
	m_block = block;
	m_next  = next;
}
//--------------------------------------
CFor::~CFor()
{ 
	delete m_block;
	free( m_id );

	if( m_next )
		delete m_next; 
}
//--------------------------------------
void CFor::Print( ostream &os, CArray *data )
{
	int    i = m_first;
	CArray a;

	// for loop
	while( 1 )
	{
		a = i;
		addVar( m_id, &a );

		m_block->Print( os, data );

		// loop condition, loop may go up or down
		if( i == m_last )
			break;
		else if( m_first < m_last )
			i++;
		else
			i--;

	}
	delVar( m_id );

	if( m_next )
		m_next->Print( os, data );
}
//= CVal ===============================
CVal::CVal( const int x )
{
	m_val  = new CArray( x );
	m_next = NULL;
}
//--------------------------------------
CVal::CVal( const char *x )
{
	m_val  = new CArray( x );
	m_next = NULL;
}
//--------------------------------------
CVal::~CVal()
{ 
	delete m_val;
}
//--------------------------------------
CArray* CVal::getArray( CArray *data )
{
	return m_val;
} 
//--------------------------------------
int CVal::isTrue( CArray *data )
{
	return ( !m_val->empty() );
}
//= CIf ================================
CIf::CIf( CTree *cond, CTree *tree, CTree *else_tree, CTree *next )
{
	m_condition = cond;
	m_tree 	 = tree;
	m_else 	 = else_tree;
	m_next   = next;
}
//--------------------------------------
CIf::~CIf()
{ 
	delete m_condition;
	delete m_tree;

	if( m_else )
		delete m_else;

	if( m_next )
		delete m_next;
}
//--------------------------------------
void CIf::Print( ostream &os, CArray *data )
{
	// if print
	if( m_condition->isTrue( data ) )
		m_tree->Print( os, data );
	//else print
	else if ( m_else )
		m_else->Print( os, data );

	if( m_next )
		m_next->Print( os, data );

}
//= CAOperation ========================
CAOperation::CAOperation( Operation o, CTree *op1, CTree *op2 )
{
	m_o   = o;
	m_op1 = op1;
	m_op2 = op2;
}
//--------------------------------------
CAOperation::~CAOperation()
{ 
	delete m_op1;
	delete m_op2;
}
//--------------------------------------
CArray* CAOperation::getArray( CArray *data )
{
	switch( m_o )
	{
		// opration *
		case TIMES:
			m_val = *( m_op1->getArray( data ) ) * *( m_op2->getArray( data ) );
			break;
		// operation /
		case DIVIDE:
			m_val = *( m_op1->getArray( data ) ) / *( m_op2->getArray( data ) );
			break;
		// operation %
		case MODULO:
			m_val = *( m_op1->getArray( data ) ) % *( m_op2->getArray( data ) );
			break;
		// operation -
		case MINUS:
			m_val = *( m_op1->getArray( data ) ) - *( m_op2->getArray( data ) );
			break;
		// operation +
		case PLUS:
			m_val = *( m_op1->getArray( data ) ) + *( m_op2->getArray( data ) );
			break;
		default: 
			break;
	}

	return ( &m_val );
}
//--------------------------------------
int CAOperation::isTrue( CArray *data )
{
	getArray( data );
	return ( !m_val.empty() );
}
//= CLOperation ========================
CLOperation::CLOperation( Operation o, CTree *op1, CTree *op2 )
{
	m_o   = o;
	m_op1 = op1;
	m_op2 = op2;
}
//--------------------------------------
CLOperation::~CLOperation()
{ 
	delete m_op1;
	delete m_op2;
}
//--------------------------------------
CArray* CLOperation::getArray( CArray *data )
{
	switch( m_o )
	{
		// operation ==
		case EQ:
			m_val = ( *( m_op1->getArray( data ) ) == *( m_op2->getArray( data ) ) );
			break;
		// operation !=
		case NEQ:
			m_val = ( *( m_op1->getArray( data ) ) != *( m_op2->getArray( data ) ) );
			break;
		// operation <
		case LT:
			m_val = ( *( m_op1->getArray( data ) ) < *( m_op2->getArray( data ) ) );
			break;
		// operation <=
		case LTE:
			m_val = ( *( m_op1->getArray( data ) ) <= *( m_op2->getArray( data ) ) );
			break;
		// operation >
		case GT:
			m_val = ( *( m_op1->getArray( data ) ) > *( m_op2->getArray( data ) ) );
			break;
		// operation >=
		case GTE:
			m_val = ( *( m_op1->getArray( data ) ) >= *( m_op2->getArray( data ) ) );
			break;
		// operation &&
		case AND:
			m_val = ( ( !m_op1->getArray( data )->empty() ) && ( !m_op2->getArray( data )->empty() ) );
			break;
		// operation ||
		case OR:
			m_val = ( ( !m_op1->getArray( data )->empty() ) || ( !m_op2->getArray( data )->empty() ) );
			break;
		default: 
			break;
	}

	return ( &m_val );
}
//--------------------------------------
int CLOperation::isTrue( CArray *data )
{
	getArray( data );
	return ( !m_val.empty() );
}
//= CUnMinus ===========================
CUnMinus::CUnMinus( CTree *tree )
{
	m_tree = tree;
	m_next = NULL;
}
//--------------------------------------
CUnMinus::~CUnMinus()
{ 
	if( m_tree )
		delete m_tree;
}
//--------------------------------------
CArray* CUnMinus::getArray( CArray *data )
{
	m_val = 0;
	m_val = m_val - *( m_tree->getArray( data ) );

	return &m_val;
} 
//--------------------------------------
int CUnMinus::isTrue( CArray *data )
{
	getArray( data );
	return ( !m_val.empty() );
}
//======================================