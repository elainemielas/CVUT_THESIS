#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <iostream>
#include <string.h>
#include <string>

#include <iomanip>
#include <sstream>
#include <fstream>

#include "lib/controller.hpp"

using namespace std;

CController *g_controller;
void        *g_module;
destroy_t   *g_destroy_module;
char        *g_module_name;

// functon return module name
char* get_module()
{
    char *path = getenv( "PATH_INFO" );

    if( !path )
    {
        path = ( char* ) malloc( 7 * sizeof( char ) );
        strcpy( path, "status" );
        return path;
    }

    // module length
    int   l = 0;
    char *x = path + 1;
    while( *x != '/' )
    {
        l++;
        x++;
    }

    x = ( char* ) malloc( ( l + 1 ) * sizeof( char ) );
    for( int i = 0; i < l; i++ )
        x[ i ] = path[ i + 1 ];

    x[ l ] = 0;
    return x;
}

char* module_path()
{
	char *str = ( char* ) malloc( strlen( g_module_name ) + 1 + 3 + 10 );

	strcpy ( str, "./modules/" );
	strcat ( str, g_module_name );
    strcat ( str, ".so" );

	return str;
}

//function to load shared module object
int load_module( ostream &os )
{
	char *path = module_path();

    // load the triangle library
    g_module = dlopen( path, RTLD_LAZY );
    if( !g_module )
    {
        os << "Cannot load library: " << dlerror() << endl;
        return 1;
    }
    free( path );
    // reset errors
    dlerror();
    
    // load the symbols
    create_t* create_module = ( create_t* ) dlsym( g_module, "create" );
    const char* dlsym_error = dlerror();
    if( dlsym_error )
    {
        os << "Cannot load symbol create: " << dlsym_error << endl;
        return 1;
    }
    
    //destroy_t* 
    g_destroy_module = ( destroy_t* ) dlsym( g_module, "destroy" );
    dlsym_error = dlerror();
    if ( dlsym_error )
    {
        os << "Cannot load symbol destroy: " << dlsym_error << endl;
        return 1;
    }

    // create an instance of the class
    g_controller = create_module();

	return 0;
}

int main ( int argc, char **argv, char **envp )
{
    stringstream out;
    CView       *view;

    //get module
    g_module_name = get_module();

	if( load_module( out ) )
    {
        cout << "Content-Type: text/html\n\n";
        cout << out.str();
        free( g_module_name );
        return 0;
    }

    // use the class
    view = g_controller->Run();

    //print headers
    cout << *( g_controller->header );

    //print content
    cout << (*view);

    // destroy the class
    g_destroy_module( g_controller );

    // unload the triangle library
    dlclose( g_module );

    free( g_module_name );

    delete view;

	return 0;
}
