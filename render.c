#include <stdio.h>

#include <SDL.h>

#include "render.h"
#include "bowl.h"

double localTranslate[3]={0.0,-16.0,0.0};
double worldScale[3]={8.0,64.0,1.0};
double worldTranslate[3]={0.0,32.0,0.0};
int screenTranslate[2]={128,96};
double screenD=32.0;
double frameYIncrement=-23.3333333333;
int frameCycle=3;
int frame=0;
int mapNode=0;
int mapPos=0;
int mapPosMax=6;

void mapToWorld(int mx,int my,int mz,double *wx,double *wy,double *wz)
{
	double curl=0.0;
	switch(map[mapNode].type) {
	case MT_TURNLEFT:
		curl=my*my*4;
		//printf("curl left: %lf (y=%d)\n",curl,my);
		break;
	case MT_TURNRIGHT:
		curl=-my*my*4;
		//printf("curl right: %lf (y=%d)\n",curl,my);
		break;
	default:
		break;
	}

	*wx=(mx+curl+localTranslate[0])*worldScale[0]+worldTranslate[0];
	*wy=(my+localTranslate[1])*worldScale[1]+worldTranslate[1];
	*wz=(mz+localTranslate[2])*worldScale[2]+worldTranslate[2];
	
	//printf("map (%d,%d,%d) -> world (%lf,%lf,%lf)\n",mx,my,mz,*wx,*wy,*wz);
}

void worldToScreen(double wx,double wy,double wz,SDL_Point *point)
{
	if(wy==0.0) wy=0.01;
	double sx=wx*screenD/wy,sy=wz*screenD/wy;

	point->x=screenTranslate[0]+(int)sx;
	point->y=screenTranslate[1]-(int)sy;
	//printf("world (%lf,%lf,%lf) -> screen(%d,%d)\n",wx,wy,wz,point->x,point->y);
}

int mapToClippedScreen(struct Bowl *bowl,int mx,int my,int mz,SDL_Point *point)
{
	point[0].x=0;
	point[0].y=0;
	if(my+1>=bowl->length) return 0;
	
	double wx,wy,wz;
	mapToWorld(mx,my,mz,&wx,&wy,&wz);
	if(wy<=0.0) return 0;
	if(wy<screenD) {
		// clip to screenD.
		//double per=(screenD-wy)/(wy1-wy);
		wy=screenD;
		//wx=wx*per+wx1*(1-per);
		//wz=wz*per+wz1*(1-per);
	}
	worldToScreen(wx,wy,wz,point);
	
	return 1;
}


void draw(SDL_Renderer *renderer)
{
	SDL_Rect rect={0,0,256,192};
	
	// Draw Snow
	SDL_SetRenderDrawColor(renderer,64,96,224,255);
	rect.y=64;
	rect.h=128;
	SDL_RenderFillRect(renderer,&rect);

	// Draw Track
	struct Bowl *bowl=generateBowl(map[mapNode].type,0);
	localTranslate[0]=-bowl->width/2;
	localTranslate[1]=0;
	localTranslate[2]=0;
	worldScale[0]=512.0/bowl->width;
	worldScale[1]=512.0/bowl->length;
	worldScale[2]=0.8;
	frameCycle=5;
	frameYIncrement=-worldScale[1]/frameCycle*2.0;
	
	int x,y;
	for(y=bowl->length-1;y>=0;y--) {
		double wx,wy,wz;
		double wx1,wy1,wz1;
		SDL_Point point[2];
		
		if((y%2)==0) {
			SDL_SetRenderDrawColor(renderer,64,192,224,255);
		} else {
			SDL_SetRenderDrawColor(renderer,224,224,224,255);
		}
		//printf("length %d (%s)\n",y,(y%2)==0?"blue":"white");

		if(y+1>=bowl->length) continue;
		
		// fill horizontal left
		mapToWorld(0,y+1,bowl->elevation[0+(y-1)*bowl->width],&wx1,&wy1,&wz1);
		if(wy1<screenD) continue;
		mapToWorld(0,y,bowl->elevation[0+y*bowl->width],&wx,&wy,&wz);
		if(wy<screenD) {
			// clip to screenD.
			double per=(screenD-wy)/(wy1-wy);
			wy=screenD;
			wx=wx*per+wx1*(1-per);
			wz=wz*per+wz1*(1-per);
			//SDL_SetRenderDrawColor(renderer,255,255,255,255);
		}
		worldToScreen(wx,wy,wz,point+0);
		SDL_Rect left={0,point[0].y,point[0].x,191};
		if(left.w>0) {
			SDL_RenderFillRect(renderer,&left);
		}
		//printf("left (%d,%d)+%dx+%d\n",left.x,left.y,left.w,left.h);

		// fill horizontal right
		mapToWorld(bowl->width-1,y+1,bowl->elevation[bowl->width-1+(y-1)*bowl->width],&wx1,&wy1,&wz1);
		if(wy1<screenD) continue;
		mapToWorld(bowl->width-1,y,bowl->elevation[bowl->width-1+y*bowl->width],&wx,&wy,&wz);
		if(wy<screenD) {
			// clip to screenD.
			double per=(screenD-wy)/(wy1-wy);
			wy=screenD;
			wx=wx*per+wx1*(1-per);
			wz=wz*per+wz1*(1-per);
		}
		worldToScreen(wx,wy,wz,point+0);
		SDL_Rect right={point[0].x,point[0].y,256-point[0].x,192-point[0].y};
		if(point[0].x<256) {
			SDL_RenderFillRect(renderer,&right);
		}
		//printf("right (%d,%d)+%dx+%d\n",right.x,right.y,right.w,right.h);
		
		int lastsx=256;
		
		for(x=0;x<bowl->width;x++) {
			if(y+1>=bowl->length) continue;
			mapToWorld(x,y+1,bowl->elevation[x+(y-1)*bowl->width],&wx1,&wy1,&wz1);
			if(wy1<screenD) continue;
			mapToWorld(x,y,bowl->elevation[x+y*bowl->width],&wx,&wy,&wz);
			if(wy<0.0) {
				// clip to screenD.
				double per=(screenD-wy)/(wy1-wy);
				wy=screenD;
				wx=wx*per+wx1*(1-per);
				wz=wz*per+wz1*(1-per);
			}
			worldToScreen(wx,wy,wz,point+0);
			
			//printf("line (%d,%d)-(%d,%d)\n",point[0].x,point[0].y,point[0].x,191);
			SDL_RenderDrawLine(renderer,point[0].x,point[0].y,point[0].x,191);
			if(lastsx+1<point[0].x) {
				// Densify.
				SDL_RenderDrawLine(renderer,point[0].x-1,point[0].y,point[0].x-1,191);
			}
			lastsx=point[0].x;
		}
	}

	// Sky
	SDL_SetRenderDrawColor(renderer,64,96,224,255);
	rect.x=0;
	rect.y=0;
	rect.w=256;
	rect.h=64;
	SDL_RenderFillRect(renderer,&rect);
	
	// Hud
	SDL_SetRenderDrawColor(renderer,224,224,224,255);
	rect.x=64;
	rect.y=16;
	rect.h=8;
	rect.w=128;
	SDL_RenderFillRect(renderer,&rect);

	SDL_SetRenderDrawColor(renderer,192,192,128,255);
	rect.x=65;
	rect.y=17;
	rect.w=126*(mapPos*frameCycle+frame)/(mapPosMax*frameCycle);
	rect.h=6;
	SDL_RenderFillRect(renderer,&rect);

	SDL_SetRenderDrawColor(renderer,224,32,32,255);
	rect.x=64+(64-8)*mapPos/mapSize;
	rect.y=24;
	rect.w=8;
	rect.h=8;
	SDL_RenderFillRect(renderer,&rect);

	SDL_SetRenderDrawColor(renderer,192,192,128,255);
	rect.x=128;
	if(map[mapNode].type==MT_TURNLEFT) rect.x=128+16;
	if(map[mapNode].type==MT_TURNRIGHT) rect.x=128-16;
	SDL_RenderDrawLine(renderer,128,48,rect.x,32);


	SDL_SetRenderDrawColor(renderer,0,0,0,255);
}

void update(int elapsed)
{
	worldTranslate[1]+=frameYIncrement;
	frame++;
	if(frame>=frameCycle) {
		worldTranslate[1]=0;
		frame=0;
		
		mapPos++;
		if(mapPos>=mapPosMax) {
			mapPos=0;
			mapNode++;
			if(mapNode>=mapSize) mapNode=0;
		}
	}
}