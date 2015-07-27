#include <cstdio>

#include <enet/enet.h>

#include "Enet.h"

int main( int iArgc, char** ppArgv )
{
	if( ENet_Setup() != 0 )
	{
		fprintf( stderr, "An error occurred while initializing ENet client.\n" );
		return EXIT_FAILURE;
	}

	ENetHost* pClient;

	pClient = enet_host_create( nullptr, 1, NetChan_Count, 0, 0 );

	if( !pClient )
	{
		fprintf( stderr, "An error occurred while creating the client host.\n" );
		exit( EXIT_FAILURE );
	}

	ENetAddress address;

	enet_address_set_host( &address, "localhost" );

	address.port = 1234;

	ENetPeer* pServer = enet_host_connect( pClient, &address, NetChan_Count, 0 );

	if( !pServer )
	{
		fprintf( stderr, "An error occurred while connecting to the server\n" );
		return EXIT_FAILURE;
	}

	ENetEvent event;

	size_t uiServerId = 0;

	if( enet_host_service( pClient, &event, 5000 ) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT )
	{
		fprintf( stderr, "Connection to some.server.net:1234 succeeded." );

		printf( "Client connected to %x:%u\n", event.peer->address.host, event.peer->address.port );
		event.peer->data = ( void* ) uiServerId;
		++uiServerId;

		ENetPacket* pPacket = enet_packet_create( "packet", strlen( "packet" ) + 1, ENET_PACKET_FLAG_RELIABLE );
		enet_peer_send( pServer, NetChan_Data, pPacket );
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset( pServer );
		fprintf( stderr, "Connection to some.server.net:1234 failed." );

		return EXIT_FAILURE;
	}

	bool fConnected = true;

	while( fConnected && enet_host_service( pClient, &event, 5000 ) > 0 )
	{
		switch( event.type )
		{
		case ENET_EVENT_TYPE_CONNECT:
			{
				printf( "Client connected to %x:%u\n", event.peer->address.host, event.peer->address.port );
				event.peer->data = ( void* ) uiServerId;
				++uiServerId;

				ENetPacket* pPacket = enet_packet_create( "packet", strlen( "packet" ) + 1, ENET_PACKET_FLAG_RELIABLE );
				enet_peer_send( pServer, NetChan_Data, pPacket );
				break;
			}

		case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf( "Client disconnected from server %u\n", ( size_t ) event.peer->data );

				event.peer->data = nullptr;
				pServer = nullptr;
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
				enet_peer_disconnect( pServer, 0 );
				fConnected = false;
				break;
			}
		}
	}

	bool fDisconnected = false;

	while( !fDisconnected && enet_host_service( pClient, &event, 3000 ) > 0 )
	{
		switch( event.type )
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy( event.packet );
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			fprintf( stderr, "Disconnection succeeded.\n" );
			fDisconnected = true;
			break;
		}
	}

	enet_host_destroy( pClient );

	fflush( stdout );
	fflush( stderr );

	getchar();

	return 0;
}