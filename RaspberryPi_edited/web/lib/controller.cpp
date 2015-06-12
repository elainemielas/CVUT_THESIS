#include "controller.hpp"

#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

//extern char **environ;

CController::CController()
{
	menu    = new CMenu;
    header  = new CHeader;
    
    header->AddItem( "Content-Type: text/html" );

    fill_SERVER();
    fill_POST();
    fill_GET();

}

CController::~CController()
{
	//cout << _SERVER << _GET << _POST;

    delete header;
    delete menu;
}

void CController::fill_SERVER()
{
    char *key, *value;
    bool end = false;
    char **env = environ;

    while( *env )
    {
        key = value = *env;

        while( *value != '=' )
            value++;

        *value = 0;
        value++;

        _SERVER[ key ] = value;

        value--;
        *value = '=';
        env++;
    }

    key = value = getenv( "PATH_INFO" );

    //default 
    if( !key )
    {
        _SERVER[ "MODULE" ] = "status";
        _SERVER[ "MODULE_PATH" ] = "";
        return;
    }

    value++;
    key++;
    while( *value != 0 && *value != '/' )
        value++;

    if( *value )
    {
        *value = 0;
        value++;
        end = true;
    }

    _SERVER[ "MODULE" ] = key;
    _SERVER[ "MODULE_PATH" ] = value;

    if( end )
    {
        value--;
        *value = '/';
    }

}

void CController::fill_POST()
{
	string key;
	string value;
	char c;
	
	c = getchar();

q0:
	switch( c )
	{
		case '=':
			c = getchar();
			goto q1;
		case (char) EOF:
			return;
		default:
			key += c;
			c = getchar();
			goto q0;
	}

q1:
	switch( c )
	{
		case '&':
			c = getchar();
			_POST[ key.c_str() ] = value.c_str();
			key.erase();
			value.erase();
			goto q0;
		case (char) EOF:
			_POST[ key.c_str() ] = value.c_str();
			return;
		default:
			value += c;
			c = getchar();
			goto q1;
	}

}

void CController::fill_GET()
{
	char *x = getenv( "QUERY_STRING" );
	char *key = x, *value = x;

	if( !x )
		return;

q00:
	switch( *x )
	{
		case '=':
			*x = 0;
			x++;
			value = x;			
			goto q01;
		case 0:
			return;
		default:
			x++;
			goto q00;
	}

q01:
	switch( *x )
	{
		case '&':
			*x = 0;
			_GET[ key ] = value;
			x++;
			key = value = x;
			goto q00;
		case 0:
			_GET[ key ] = value;
			return;
		default:
			x++;
			goto q01;
	}

}