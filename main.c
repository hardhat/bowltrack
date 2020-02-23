#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL.h>

#include "render.h"
#include "bowl.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

//The window we'll be rendering to
SDL_Window *rootWindow = NULL;
//Render context
SDL_Renderer *renderer = NULL;


int main(int argc,char **argv)
{
	//Init
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return 10;
    }
	
	//Create window
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &rootWindow, &renderer);
	SDL_SetWindowTitle(rootWindow, "Bowl Track");	
	
	int quit=0;
	while(!quit) {
		SDL_RenderClear(renderer);
		draw(renderer);
		SDL_RenderPresent(renderer);
	
		// message loop
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				quit=1;
				break;
			case SDL_KEYUP:
				if(event.key.keysym.sym==27) quit=1;
				break;
			}
		}
		
		SDL_Delay(50);
		
		update(50);
	}
	SDL_DestroyWindow(rootWindow);
		
	return 0;
}
