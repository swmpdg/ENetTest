#include <enet/enet.h>

#include "ENet.h"

int ENet_Setup()
{
	int iStartupResult = enet_initialize();

	if( iStartupResult == 0 )
	{
		atexit( enet_deinitialize );
	}

	return iStartupResult;
}
