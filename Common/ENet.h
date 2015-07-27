#ifndef CENET_H
#define CENET_H

const size_t MAX_PLAYERS = 32;

enum NetChannel
{
	NetChan_Data,
	NetChan_File,
	NetChan_Count
};

int ENet_Setup();

#endif //CENET_H