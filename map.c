#include "map.h"

struct Map map[]={
	{MT_STRAIGHT},
	{MT_TURNLEFT},
	{MT_STRAIGHT},
	{MT_TURNRIGHT},
	{MT_TUNNEL},
	{MT_TURNLEFT},
	{MT_TURNRIGHT},
	{MT_END},
};

int mapSize=8;
