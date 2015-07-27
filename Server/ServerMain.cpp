#include <cstdio>

#include <enet/enet.h>

#include "Enet.h"

int main( int iArgc, char** ppArgv )
{
	if( ENet_Setup() != 0 )
	{
		printf( "An error occurred while initializing ENet server.\n" );
		return EXIT_FAILURE;
	}

	ENetAddress address;
	ENetHost* pServer;

	address.host = ENET_HOST_ANY;
	address.port = 1234;

	pServer = enet_host_create( &address, MAX_PLAYERS, NetChan_Count, 0, 0 );

	if( !pServer )
	{
		printf( "An error occurred while creating the server host.\n" );
		exit( EXIT_FAILURE );
	}

	ENetEvent event;

	size_t uiClientId = 0;

	enet_uint32 uiWaitTime = 60000;

	while( enet_host_service( pServer, &event, uiWaitTime ) > 0 )
	{
		switch( event.type )
		{
		case ENET_EVENT_TYPE_CONNECT:
			{
				printf( "Client connected from %x:%u\n", event.peer->address.host, event.peer->address.port );
				event.peer->data = ( void* ) uiClientId;
				++uiClientId;
				uiWaitTime = 5000;
				break;
			}

		case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf( "Client %u disconnected\n", ( size_t ) event.peer->data );

				event.peer->data = nullptr;
				uiWaitTime = 5000;
				break;
			}

		case ENET_EVENT_TYPE_RECEIVE:
			{
				printf( "A packet of length %u containing %s was received from %u on channel %u.\n",
						event.packet->dataLength,
						event.packet->data,
						( size_t ) event.peer->data,
						event.channelID );

				enet_packet_destroy( event.packet );
				ENetPacket* pPacket = enet_packet_create( "packet2", strlen( "packet2" ) + 1, ENET_PACKET_FLAG_RELIABLE );
				enet_peer_send( event.peer, NetChan_Data, pPacket );
				break;
			}
		}
	}

	enet_host_destroy( pServer );

	fflush( stdout );
	fflush( stderr );

	getchar();

	return 0;
}