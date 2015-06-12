/**
 * @file   database.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  database add and query functions
 *
 */

#include "query_lex.cpp"

/**
 * @brief   Initialize database
 *
 * Function alloc database
 *
 */
void init_database();

/**
 * @brief   Free database
 *
 * Function free database
 *
 */
void delete_database();

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
int query( Cqlex &lex, int fd, int rw = 0 );

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
			 const int flag );