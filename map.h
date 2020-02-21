#ifndef MAP_H
#define MAP_H

enum MapType {
	MT_STRAIGHT,
	MT_TURNLEFT,
	MT_TURNRIGHT,
	MT_TUNNEL,
	MT_END,
};

struct Map {
	enum MapType type;	
};

extern struct Map map[];
extern int mapSize;

#endif
