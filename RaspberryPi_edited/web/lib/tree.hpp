/**
 * @file   tree.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  class represents syntactic structure of template file
 *
 */

#ifndef TREE
#define TREE

#include "array.hpp"

/**
 * @brief   Main virtual class
 *
 */
class CTree
{
	public:
		/**
		 * @brief   Method to print element to stream
		 *
		 * This function parse query and send answer
		 *
		 * @param [in] os 	Output stream
		 * @param [in] data Data from View class, used in template
		 *
		 */
		virtual void Print( ostream &os, CArray *data ){}

		/**
		 * @brief   Destructor
		 *
		 */
		virtual ~CTree(){}

		/**
		 * @brief   Method to determinate is element true/false, used in If/For condition 
		 *
		 * @param [in] data Data from View
		 *
		 */
		virtual int isTrue( CArray *data ){ return 0; }

		/**
		 * @brief   Return reference to the array with one dereference
		 *
		 * @param [in] data 	Data from View
		 *
		 * @retval CArray*   Array data
		 *
		 */
		virtual CArray* getArray( CArray *data ){ return NULL; }

		/**
		 * @brief   Append new node to end
		 *
		 * @param [in] x 	New node
		 *
		 * @retval CTree*   Return pointer to ifself
		 *
		 */
		virtual CTree* append( CTree *x );
	protected:
		CTree *m_next;
};

/**
 * @brief   Function to add template variable to variable-table
 *
 * @param [in] id 	Variable identifier (variable name)
 * @param [in] arr 	Variable data (content)
 *
 */
void addVar( char *id, CArray *arr );

/**
 * @brief   Delete variable from variable-table
 *
 * @param [in] id 	Variable identifier
 *
 */
void delVar( char *id );

/**
 * @brief   Get variable content
 *
 * @param [in] id 	Variable identifier
 *
 * @retval CArray*   Pointer to variable data
 *
 */
CArray* getVar( const char *id );

/**
 * @brief   Add template block 
 *
 * Template block is separate syntax tree
 *
 * @param [in] id 	Block identifier
 * @param [in] tree Block tree structure
 *
 */
void addBlock( char *id, CTree *tree );

/**
 * @brief   Get block root node
 *
 * @param [in] id 	Block identifier
 *
 * @retval CTree*   Return root node pointer
 *
 */
CTree* getBlock( const char *id );

/**
 * @brief   Delete blocks
 *
 * Delete all insert template blocks
 *
 */
void delBlocks();


/**
 * @brief   Html node class
 *
 */
class CHtml : public CTree
{
	public:
		CHtml( char *html, CTree *next );
		~CHtml();
		virtual void Print( ostream &os, CArray *data );
	protected:
		char *m_html;
};

/**
 * @brief   Variable node class, print one value from data Array from View, indexed by string or variable
 *
 */
class CVar : public CTree
{
	public:
		typedef enum { IDENT, STRING } IdentType;
		/**
		 * @brief   Constructor
		 *
		 * @param [in] var 		Index identifier
		 * @param [in] type		Index identifier is string or varible identifier?
		 * @param [in] array 	Next index or NULL if array end.
		 * @param [in] next 	Next template node.
		 *
		 */
		CVar( char *var, IdentType type, CVar *array, CTree *next );
		~CVar();
		virtual CVar* appendVar( CVar *x );
		virtual void Print( ostream &os, CArray *data );
		virtual int isTrue( CArray *data );
		virtual CArray* getArray( CArray *data );
	protected:
		CVar 	 *m_array; 
		char  	 *m_var;
		IdentType m_type;
};

/**
 * @brief   Class PrintBlock to render separated block in this node
 *
 */
class CPrintblock : public CTree
{
	public:
		/**
		 * @brief   Constructor
		 *
		 * @param [in] block 	Block identifier.
		 * @param [in] next 	Next template node.
		 *
		 */
		CPrintblock( char *block, CTree *next );
		~CPrintblock();
		virtual void Print( ostream &os, CArray *data );
	protected: 
		char  *m_block;
};

/**
 * @brief   Class PrintMenu to print menu (Cmenu) in this node
 *
 */
class CPrintmenu : public CTree
{
	public:
		/**
		 * @brief   Constructor
		 *
		 * @param [in] next 	Next template identifier.
		 *
		 */
		CPrintmenu( CTree *next );
		~CPrintmenu();
		virtual void Print( ostream &os, CArray *data );
};

/**
 * @brief   Foreach node class, passes all elements in array (CArray)
 *
 */
class CForeach : public CTree
{
	public:
		/**
		 * @brief   Constructor
		 *
		 * @param [in] id 		Iteration variables identifier
		 * @param [in] var 		Iterated variable
		 * @param [in] block 	Iterated block
		 * @param [in] next 	Next template node
		 *
		 */
		CForeach( char *id, CVar *var, CTree *block, CTree *next );
		~CForeach();
		virtual void Print( ostream &os, CArray *data );
	protected: 
		char   *m_id;
		CVar   *m_var;
		CTree  *m_block;
};

/**
 * @brief   For node class, iterated loop
 *
 */
class CFor : public CTree
{
	public:
		/**
		 * @brief   Constructor
		 *
		 * @param [in] id 		Iteration variables identifier
		 * @param [in] first 	Iterate from
		 * @param [in] last 	Iterate to
		 * @param [in] block 	Iterated block
		 * @param [in] next 	Next template node
		 *
		 */
		CFor( char *id, int first, int last, CTree *block, CTree *next );
		~CFor();
		virtual void Print( ostream &os, CArray *data );
	protected: 
		char   *m_id;
		int 	m_first;
		int 	m_last;
		CTree  *m_block;
};

/**
 * @brief   Value node class, integer or string
 *
 */
class CVal : public CTree
{
	public:
		/**
		 * @brief   Constructor
		 *
		 * @param [in] x int	Numeric value
		 * @param [in] x char*	String value
		 *
		 */
		CVal( const int x );
		CVal( const char *x );
		~CVal();
		virtual CArray* getArray( CArray *data );
		virtual int isTrue( CArray *data );
	protected: 
		CArray *m_val;
};

/**
 * @brief   If/else condition node class
 *
 */
class CIf : public CTree
{
	public:
		/**
		 * @brief   Constructor
		 *
		 * @param [in] cond			Condition expression block 
		 * @param [in] tree			True statement block
		 * @param [in] else_tree	False statement block
		 * @param [in] next			Next template node
		 *
		 */
		CIf( CTree *cond, CTree *tree, CTree *else_tree, CTree *next );
		~CIf();
		virtual void Print( ostream &os, CArray *data );
	protected: 
		CTree *m_condition;
		CTree *m_tree;
		CTree *m_else;
};

/**
 * @brief   Arithmetic operations node class
 *
 */
class CAOperation : public CTree
{
	public:
		typedef enum { TIMES, DIVIDE, MODULO, MINUS, PLUS } Operation;
		/**
		 * @brief   Constructor
		 *
		 * @param [in] o	Operation type
		 * @param [in] op1	First operand
		 * @param [in] op2 	Second operand
		 *
		 */
		CAOperation( Operation o, CTree *op1, CTree *op2 );
		~CAOperation();
		virtual CArray* getArray( CArray *data );
		virtual int isTrue( CArray *data );
	protected:
		Operation m_o;
		CArray    m_val;
		CTree    *m_op1;
		CTree    *m_op2;
};


/**
 * @brief   Logic operations node class
 *
 */
class CLOperation : public CTree
{
	public:
		typedef enum { EQ, NEQ, LT, LTE, GT, GTE, AND, OR } Operation;
		/**
		 * @brief   Constructor
		 *
		 * @param [in] o	Operation type
		 * @param [in] op1	First operand
		 * @param [in] op2 	Second operand
		 *
		 */
		CLOperation( Operation o, CTree *op1, CTree *op2 );
		~CLOperation();
		virtual CArray* getArray( CArray *data );
		virtual int isTrue( CArray *data );
	protected:
		Operation m_o;
		CArray    m_val;
		CTree    *m_op1;
		CTree    *m_op2;
};

/**
 * @brief   Unary minus node class
 *
 */
class CUnMinus : public CTree
{
	public:
		/**
		 * @brief   Constructor
		 *
		 * @param [in] tree 	Expression.
		 *
		 */
		CUnMinus( CTree *tree );
		~CUnMinus();
		virtual CArray* getArray( CArray *data );
		virtual int isTrue( CArray *data );
	protected: 
		CArray  m_val;
		CTree  *m_tree;
};

#endif