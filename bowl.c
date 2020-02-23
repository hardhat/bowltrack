#include <stdlib.h>
#include <math.h>

#include "render.h"
#include "map.h"
#include "bowl.h"

#define BOWL_WIDTH 512
#define BOWL_HEIGHT 16

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
			double z=-128.0;
			if(x<bowl.width/4 || x>=bowl.width*3/4) {
				int pos=x<bowl.width/4?x:x-bowl.width/2;
				double angle=pos/(double)bowl.width*M_PI*4;
				z=cos(angle);
				z=z*128.0;
			}
			double angle=x/(double)bowl.width*M_PI*2;
			z+=cos(angle)*128.0;
			z=z/2.0;
			
			setElevation(&bowl,x,y,(int)z);
			//if(y==7) printf("(%d,%d,%d)\n",x,y,(int)z);
		}
	}

	return &bowl;
}

void freeBowl(struct Bowl *bowl)
{

}
