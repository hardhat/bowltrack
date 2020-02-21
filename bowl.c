#include <stdlib.h>
#include <math.h>

#include "render.h"
#include "map.h"
#include "bowl.h"

#define BOWL_WIDTH 64
#define BOWL_HEIGHT 8

int elevation[BOWL_WIDTH*BOWL_HEIGHT];

struct Bowl bowl={
	BOWL_WIDTH,
	BOWL_HEIGHT,
	elevation
};

void setElevation(struct Bowl *bowl,int x,int y,int z)
{
	if(x<0 || y<0 || x>=bowl->width || y>=bowl->length) return;
	bowl->elevation[x+bowl->width*y]=z;
}

struct Bowl *generateBowl(enum MapType type, int frame)
{
	int x,y;
	
	for(y=0;y<bowl.length;y++) {
		for(x=0;x<bowl.width;x++) {
			double angle=x*M_PI;
			double z=cos(angle)*-64.0;
			
			setElevation(&bowl,x,y,(int)z);
		}
	}

	return &bowl;
}

void freeBowl(struct Bowl *bowl)
{

}
