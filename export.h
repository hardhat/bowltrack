#ifndef EXPORT_H
#define EXPORT_H

extern const unsigned char colecoPalette[16][3];
extern SDL_Surface *spriteSurface;

int rgbToColecoPalette(unsigned char *pixel);

void resetExport();
void encodeScreen(SDL_Renderer *renderer);
void exportPC2C(const char *filename);
void exportName2C(const char *filename);


// All in 1 function
void exportPC(SDL_Renderer *renderer,const char *filename,int mergeDuplicates);
void exportBMP(SDL_Renderer *renderer,const char *filename);

// sprite export
void spriteReset();
void sprites2C(const char *filename);
void sprites2PPSPR(const char *filename);


#endif
