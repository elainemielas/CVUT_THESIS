/**
 * @file   query_lex.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  query lexer, use in database.cpp and listeners.cpp 
 *
 */

#ifndef QLEXER
#define QLEXER

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "logger.hpp"

#include <sys/select.h>

/**
 * @brief   Query lexer class
 *
 * Lexer for input clien queryes.
 *
 */
class Cqlex
{
public:
	enum lex_type
	{
		READ, WRITE, DESC, ATTR, NUMBER, LIST,
		SEMICOLON, END, ERR, READERR
	};

	/**
	 * @brief   Constructor
	 *
	 * @param [in] fd 		Client file descriptor
	 * @param [in] timeout 	Set timeout during reading, 0 means without timeout
	 *
	 */
	Cqlex( int fd, int timeout = 0 )
	{
		m_fd = fd;
		m_len = m_pos = 0;
		m_buffer = ( char* ) malloc( m_buffer_len * sizeof( char ) );

		// select timeout
		m_timeout = timeout;
		m_tv.tv_sec = timeout;
		m_tv.tv_usec = 0;

		if( timeout )
		{
			FD_ZERO( &m_fd_set );
			FD_SET( m_fd, &m_fd_set );
		}
		//get_c();
	}

	/**
	 * @brief   Destructor
	 *
	 */
	~Cqlex()
	{
		free( m_buffer );
	}

	/**
	 * @brief   Automat to recognition individually lexems from input characters
	 *
	 * @retval lex_type   Lexem.
	 *
	 */
	lex_type get_lex()
	{
		switch( m_c )
		{
			case (char) EOF:
				return READERR;
			case '@':
				// RW operation
				get_c();
				goto q1;
			case '\n':
				return END;
			case '#':
				get_c();
				return DESC;
			case '$':
				get_c();
				return ATTR;
			case ';':
				get_c();
				return SEMICOLON;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				m_number = m_c - '0';
				get_c();
				goto q2;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				m_number = m_c - 'A' + 10;
				get_c();
				goto q2;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				m_number = m_c - 'a' + 10;
				get_c();
				goto q2;
			default:
				return ERR;
		}

q1:
	// read @R, write @W, list @@
		switch( m_c )
		{
			case 'R':
				get_c();
				return READ;
			case 'W':
				get_c();
				return WRITE;
			case '@':
				get_c();
				return LIST;
			default:
				return ERR;
		}

q2:
	// number
		switch( m_c )
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				//m_number = m_number * 10 + m_c - '0';
				m_number = m_number * 16 + m_c - '0';
				get_c();
				goto q2;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				m_number = m_number * 16 + m_c - 'A' + 10;
				get_c();
				goto q2;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				m_number = m_number * 16 + m_c - 'a' + 10;
				get_c();
				goto q2;
			default:
				return NUMBER;
		}

	}

	/**
	 * @brief   Function to get number from lexem if lexem type is NUMBER
	 *
	 * @retval int   number.
	 *
	 */
	int get_lex_num()
	{
		return m_number;
	}

	/**
	 * @brief   Function to get next character from file descriptor
	 * Also use to initialise lexer.
	 * 
	 */
	void get_c()
	{
		if( m_pos == m_len )
		{
			// when timeout set
			if( m_timeout && ( select( m_fd + 1, &m_fd_set, NULL, NULL, &m_tv ) <= 0 ) )
            {
            	m_c = ( char ) EOF;
            	return;
            }
			m_len = read( m_fd, m_buffer, m_buffer_len );
			//debug log
			char buffer2[100];
			int offset = sprintf(buffer2, "received message of length %d ", m_len);
			for(int i = 0; i < m_len; ++i)
			{
				offset += sprintf(buffer2 + offset, "%d ", m_buffer[i]);
			}
			info(buffer2);
			info(m_buffer);
			//end debug log
			m_pos = 0;
		}

		// 0 char read, meybe timeout error or file descriptor error 
		if( m_len <= 0 )
		{
			m_c = ( char ) EOF;
			return;
		}

		// set actual character
		m_c = m_buffer[ m_pos++ ];
	}
private:

	static const int m_buffer_len = 128;
	int   m_fd;
	char *m_buffer;
	int   m_len;
	int   m_pos;

	fd_set  m_fd_set;
	timeval m_tv;
	int     m_timeout;

	char m_c;
	int  m_number;
};

#endif