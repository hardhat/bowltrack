#include <SDL.h>

#include "render.h"
#include "bowl.h"

double localTranslate[3]={0.0,0.0,0.0};
double worldScale[3]={1.0,1.0,1.0};
int screenTranslate[2]={128,64};

void calcPoint(SDL_Point *point,int x,int y,int z)
{
	double wx=x*8.0-8*32,wy=y*64.0,wz=z;
	if(wy==0.0) wy=0.01;
	double sx=wx/wy,sy=wz/wy;


	point->x=screenTranslate[0]+(int)sx;
	point->y=screenTranslate[1]+(int)sy;
}

void draw(SDL_Renderer *renderer)
{
	SDL_Rect rect={0,0,256,192};
	
	// Sky
	SDL_SetRenderDrawColor(renderer,0,128,255,255);
	SDL_RenderFillRect(renderer,&rect);
	
	// Draw Snow
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	rect.y=64;
	rect.h=128;
	SDL_RenderFillRect(renderer,&rect);

	// Draw Track
	struct Bowl *bowl=generateBowl(MT_STRAIGHT,0);
	localTranslate[0]=bowl->width/2;
	localTranslate[1]=0;
	worldScale[0]=256.0/bowl->width;
	worldScale[1]=256.0/bowl->length;
	worldScale[2]=1.0;
	
	int x,y;
	SDL_SetRenderDrawColor(renderer,0,0,191,255);
	for(y=0;y<bowl->length;y++) {
		for(x=1;x<bowl->width;x++) {
			SDL_Point point[2];
			
			calcPoint(point+0,x-1,y,bowl->elevation[x-1+y*bowl->width]);
			calcPoint(point+1,x,y,bowl->elevation[x+y*bowl->width]);
			SDL_RenderDrawLines(renderer,point,2);
		}
	}

	SDL_SetRenderDrawColor(renderer,0,0,0,255);
}
