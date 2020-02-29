#ifndef EXPORT_H
#define EXPORT_H

void resetExport();
void encodeScreen(SDL_Renderer *renderer);
void exportPC2C(const char *filename);
void exportName2C(const char *filename);


// All in 1 function
void exportPC(SDL_Renderer *renderer,const char *filename);
void exportBMP(SDL_Renderer *renderer,const char *filename);

#endif
