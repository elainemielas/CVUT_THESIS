#include "../lib/controller.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/sysinfo.h>

using namespace std;

class status : public CController {
public:
    virtual CView* Run()
    {
    	CArray &data = *( new CArray() );

    	//system info
    	struct sysinfo info;

    	sysinfo( &info );		
		
		header->AddItem( "Refresh: 10;" );
		
        /* temperature in /sys/class/thermal/thermal_zone0/temp */
	    FILE *fr = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
	    data[ "temp" ] = ( fgetc( fr ) - '0' ) * 10 + ( fgetc( fr ) - '0' );
	    fclose( fr ); 

        // system info
		data[ "usedram" ] = (int) ( 100 - info.freeram / ( info.totalram / 100 ) );
		data[ "load" ] = (int) info.loads[ 0 ] / 655;
		data[ "day" ] = (int) info.uptime / ( 3600 * 24 );
		data[ "hour" ] = (int) ( info.uptime % ( 3600 * 24 ) ) / 3600;
		data[ "min" ] = (int) info.uptime % ( 3600 * 24 ) % 3600 / 60;

    	return new CView( "status/status.view", &data );
    }
};

// the class factories

extern "C" CController* create()
{
    return new status;
}

extern "C" void destroy( CController* p )
{
    delete p;
}