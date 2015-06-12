/**
 * @file   database.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  database add and query functions
 *
 */

#include "database.hpp"

#include "regarray.hpp"
#include "query_lex.cpp"
#include "logger.hpp"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

// pointer to database
Cregarray *g_database;

/**
 * @brief   Lexer compare function
 *
 * This function compare two lexems and throw exception when they different
 *
 * @param [in] lex input lexer
 * @param [in] lex_type expect lex type
 *
 */
void lex_compare( Cqlex &lex, Cqlex::lex_type type )
{
	Cqlex::lex_type l = lex.get_lex();

	// read error
	if( l == Cqlex::READERR )
		throw -1;

	//if( l != type )
	//	throw "syntax error";
}

/**
 * @brief   Query parser
 *
 * This function parse query and send answer
 *
 * @param [in] lex Input lexer
 * @param [in] fd  Client file descriptor
 * @param [in] rw  Accept change RO row in database
 *
 * @retval 0   Successfully.
 * @retval 1   With error.
 *
 */
int query( Cqlex &lex, int fd, int rw )
{
	char buffer[ 128 ], *list;
	int  reg;
	const char *desc;
	int  val, flag;
	
	// init lexer
	lex.get_c();

	// start parser
	try
	{
		switch( lex.get_lex() )
		{
			// read query
			case Cqlex::READ:
				lex_compare( lex, Cqlex::NUMBER );
				reg = lex.get_lex_num();
				lex_compare( lex, Cqlex::END );
				val = g_database->read( reg );

				//if( val == -1 )
				//	throw "reg missing\n";
				//CHANGE %d -> %X, "ok;%X\n" -> "%X\n" .8 to send 8 signs, temporarily changed for 4, need to make short from int
				sprintf( buffer, "%.4X\n", val );
				write( fd, buffer, strlen( buffer ) );
				info(buffer);
				return 0;
			// write query
			case Cqlex::WRITE:
				lex_compare( lex, Cqlex::NUMBER );
				reg = lex.get_lex_num();
				lex_compare( lex, Cqlex::SEMICOLON );
				lex_compare( lex, Cqlex::NUMBER );
				val = lex.get_lex_num();
				lex_compare( lex, Cqlex::END );

				// get flag
				flag = g_database->flag( reg );
				//if( flag  == -1 )
				//	throw "reg missing\n";

				// read only protection
				if( rw || !flag )
					g_database->write( reg, val );
				
				//sprintf( buffer, "ok\n" );
				//write( fd, buffer, 3 );
				return 0;
			// read register description query
			case Cqlex::DESC:
				lex_compare( lex, Cqlex::NUMBER );
				reg = lex.get_lex_num();
				lex_compare( lex, Cqlex::END );
				desc = g_database->desc( reg );

				if( desc )
				{
					list = ( char* ) malloc( strlen( desc ) + 5 );
					sprintf( list, "ok;%s\n", desc );
					write( fd, list, strlen( list ) );
					free( list );
				}
				else
					throw "reg missing\n";
				return 0;
			// read register attributes (a,b,c,d,flag) query
			case Cqlex::ATTR:
				lex_compare( lex, Cqlex::NUMBER );
				reg = lex.get_lex_num();
				lex_compare( lex, Cqlex::END );

				if( g_database->c( reg ) == -1 )
					throw "reg missing\n";

				sprintf( buffer, "ok;%d;%d;%d;%d;%d\n", g_database->a( reg ),
													    g_database->b( reg ),
													    g_database->c( reg ),
													    g_database->d( reg ),
													    g_database->flag( reg ) );
				write( fd, buffer, strlen( buffer ) );
				return 0;
			// read available registers query
			case Cqlex::LIST:
				lex_compare( lex, Cqlex::END );

				list = g_database->list();
				// add ok;

				val = strlen( list );
				list[ val + 3 ] = '\n';

				for( int i = val - 1; i >= 0; i-- )
					list[ i + 3 ] = list[ i ];

				list[ 0 ] = 'o';
				list[ 1 ] = 'k';
				list[ 2 ] = ';';

				write( fd, list, val + 4 );
				free( list );
				return 0;
			// read error on fd
			case Cqlex::READERR:
				// fd error => connection break, atc..?
				return -1;
			default:
				NULL;
				//throw "syntax error\n";
		}
	}
	catch( const char *e )
	{
		// indicates syntax error
		// send error to client
		error( e );
		write( fd, e, strlen( e ) );
		return 1;
	}
	catch( const int e )
	{
		// indicates file descriptor read error unable to write to fd, return error
		return e;
	}

	return 1;
}

/**
 * @brief   Interface to add register to database
 *
 * @param [in] reg 	register number
 * @param [in] desc register description
 * @param [in] a 	parameter a
 * @param [in] b 	parameter b
 * @param [in] c 	parameter c
 * @param [in] d 	parameter d
 * @param [in] flag RW/RO flag
 *
 * @retval 0   Successfully.
 * @retval 1   With error, duplicit name.
 *
 */
int reg_add( const int reg, 
			 const char *desc, 
			 const int a, 
			 const int b, 
			 const int c, 
			 const int d,
			 const int flag )
{
	return g_database->add( reg, desc, a, b, c, d, flag );
}

/**
 * @brief   Initialize database
 *
 * Function alloc database
 *
 */
void init_database()
{
	g_database = new Cregarray();
}

/**
 * @brief   Free database
 *
 * Function free database
 *
 */
void delete_database()
{
	delete g_database;
}