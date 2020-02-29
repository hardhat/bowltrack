#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "export.h"

/* Coleco Palette
* Calculated from the following table
* Mixing values from technical documentation and visualy expressed values

Table source at URL : http://theadamresource.com/articles/misc/wk970202.h
tml

==================================================
|     |              |     RFD     |   MF & MdK  |
|     |              =============================
|     |              | 8-bit value | 8-bit value |
|     |   TMS9118    =============================
| hex |    color     |  R   G   B  |  R   G   B  |
==================================================
|  0  | Transparent  |   0   0   0 |   0   0   0 |
|  1  | Black        |   0   0   0 |   0   0   0 |
|  2  | Medium Green |  71 183  59 |  32 192  32 |
|  3  | Light Green  | 124 207 111 |  96 224  96 |
|  4  | Dark Blue    |  93  78 255 |  32  32 224 |
|  5  | Light Blue   | 128 114 255 |  64  96 224 |
|  6  | Dark Red     | 182  98  71 | 160  32  32 |
|  7  | Cyan         |  93 200 237 |  64 192 224 |
|  8  | Medium Red   | 215 107  72 | 224  32  32 |
|  9  | Light Red    | 251 143 108 | 224  96  96 |
|  A  | Dark Yellow  | 195 205  65 | 192 192  32 |
|  B  | Light Yellow | 211 218 118 | 192 192 128 |
|  C  | Dark Green   |  62 159  47 |  32 128  32 |
|  D  | Magenta      | 182 100 199 | 192  64 160 |
|  E  | Grey         | 204 204 204 | 160 160 160 |
|  F  | White        | 255 255 255 | 224 224 224 |
==================================================
*/
const unsigned char colecoPalette[16][3]={
	{0,0,0},	// transparent
	{0,0,0},	// black
	{32,192,32},	// medium green
	{96,224,96},	// light green
	{32,32,224},	// dark blue
	{64,96,224},    // light blue
	{160,32,32},	// dark red
	{64,192,224},	// cyan
	{224,32,32},	// medium red
	{224,96,96},	// light red
	{192,192,32},	// dark yellow
	{192,192,128},	// light yellow
	{32,128,32},	// dark green
	{192,64,160},	// magenta
	{160,160,160},	// grey
	{224,224,224},	// white
};

unsigned char nameTable[3][256];
unsigned char patternTable[3][2048];
unsigned char colorTable[3][2048];
int nextCard[3]={0,0,0};
int mergeDuplicates=1;

int colorTracker[16];

void resetExport()
{
	int j;
	for(j=0;j<3;j++) {
		nextCard[j]=0;
		int i;
		for(i=0;i<256;i++) {
			nameTable[j][i]=0;
		}
		for(i=0;i<2048;i++) {
			patternTable[j][i]=0;
		}
	}
    for(j=0;j<16;j++) colorTracker[j]=0;
    mergeDuplicates=1;
}

int rgbToColecoPalette(unsigned char *pixel)
{
	int bestDist=255*255*3;
	int bestColor=0;
	int i;

	for(i=1;i<16;i++) {
		int dist=0;
		int delta;
		int j;

		for(j=0;j<3;j++) {
			delta=pixel[j]-colecoPalette[i][j];
			dist+=delta*delta;
		}
		if(dist<=bestDist) {
			bestColor=i;
			bestDist=dist;
		}
	}
	colorTracker[bestColor]++;

	return bestColor;
}

void encodeSegment(unsigned char *pixels,unsigned char *pattern,unsigned char *color)
{
	int i;
	int pix[8];
	int freq[16];
	int fg=-1,bg=-1;
	int fgFreq=0,bgFreq=0;

	for(i=0;i<16;i++) freq[i]=0;
	// map colors to coleco palette
	for(i=0;i<8;i++) {
		pix[i]=rgbToColecoPalette(pixels+i*4);
		freq[pix[i]]++;
	}
	// survey colors
	for(i=0;i<16;i++) {
		if(freq[i]>=bgFreq) {
			bg=i;
			bgFreq=freq[i];
		}
		if(bgFreq>=fgFreq) {
			int temp;

			temp=fgFreq;
			fgFreq=bgFreq;
			bgFreq=temp;

			temp=fg;
			fg=bg;
			bg=temp;
		}
	}

	// encode segment
	*color=bg+(fg<<4);
	*pattern=0;
	for(i=0;i<8;i++) {
		if(pix[i]==fg) {
			*pattern|=1<<(7-i);
		}
	}
}

int mergeDuplicateCard(int region,int card)
{
	int i;
	unsigned char *targetPattern=&patternTable[region][card<<3];
	unsigned char *targetColor=&colorTable[region][card<<3];

    if(mergeDuplicates==0) return card;

	for(i=0;i<card;i++) {
		int offset=i<<3;
		int j;
		int match=1;
		for(j=0;j<8;j++) {
			if(targetPattern[j]!=patternTable[region][offset+j] ||
			targetColor[j]!=colorTable[region][offset+j]) {
				match=0;
				break;
			}
		}
		if(match) return i;
	}

	return card;
}

int encodeCard(SDL_Renderer *renderer,int x,int y)
{
	if(x>256-8 || y>192-8 || x<0 || y<0) {
		printf("*** Internal error, encodeCard location out of range: %d,%d\n",x,y);
		return -1;
	}

	SDL_Rect rect={x,y,8,8};
	unsigned char pixels[64*4];
	int i;
	for(i=0;i<sizeof(pixels);i++) pixels[i]=0;
	SDL_RenderReadPixels(renderer,&rect,SDL_PIXELFORMAT_ABGR8888,pixels,8*4);

	int region=y/64;
	int nameCell=x/8+32*(y/8-region*8);
	if(nextCard[region]>255) {
		printf("Card overflow at %d,%d\n",x,y);
		return -2;
	}
	int pcCell=nextCard[region]*8;

	for(i=0;i<8;i++) {
		encodeSegment(pixels+i*8*4,patternTable[region]+pcCell+i,colorTable[region]+pcCell+i);
    }

	int card=mergeDuplicateCard(region,nextCard[region]);
	nameTable[region][nameCell]=card;
	if(card==nextCard[region]) nextCard[region]++;

	return 0;
}

void encodeScreen(SDL_Renderer *renderer)
{
    int i,j;
	for(j=0;j<192;j+=8) {
		for(i=0;i<256;i+=8) {
			encodeCard(renderer,i,j);
		}
	}
}

void exportPC(SDL_Renderer *renderer,const char *filename)
{
	resetExport();
	//mergeDuplicates=0;
	encodeScreen(renderer);

	int j;
	for(j=0;j<16;j++) {
        if(colorTracker[j]>0) {
            printf("freq %d: %d\n",j,colorTracker[j]);
        }
	}

	FILE *file=fopen(filename,"wb");
	if(!file) {
		printf("*** Cannot create file '%s'\n",filename);
		return;
	}

	for(j=0;j<3;j++) {
		fwrite(patternTable[j],256,8,file);
	}

	for(j=0;j<3;j++) {
		fwrite(colorTable[j],256,8,file);
	}
	fclose(file);
}

void exportPC2C(const char *filename)
{
	int i,j;
	char base[256];
	FILE *file=fopen(filename,"w");
	if(!file) {
		printf("*** Cannot create file '%s'\n",filename);
		return;
	}
	strcpy(base,filename);
	if(strchr(base,'.')) {
        strchr(base,'.')[0]=0;
	}

	for(j=0;j<3;j++) {
		fprintf(file,"const int %s_region%d_size=%d;\n",filename,j,nextCard[j]*8);

		fprintf(file,"const unsigned char %s_pattern_region%d[]={\n	",base,j);
		for(i=0;i<nextCard[j]*8;i++) {
			fprintf(file,"0x%02x,",patternTable[j][i]);
			if((i%8)==7) fprintf(file,"\n	");
		}
		fprintf(file,"}; // region %d\n\n",j);

		fprintf(file,"const unsigned char %s_color_region%d[]={\n	",base,j);
		for(i=0;i<nextCard[j]*8;i++) {
			fprintf(file,"0x%02x,",colorTable[j][i]);
			if((i%8)==7) fprintf(file,"\n	");
		}
		fprintf(file,"}; // region %d\n\n",j);
	}
	fclose(file);
}

void exportName2C(const char *filename)
{
	char base[256];
	FILE *file=fopen(filename,"w");
	if(!file) {
		printf("*** Cannot create file '%s'\n",filename);
		return;
	}
	strcpy(base,filename);
	if(strchr(base,'.')) {
        strchr(base,'.')[0]=0;
	}

	int i,j;
	for(j=0;j<3;j++) {
		fprintf(file,"const unsigned char %s_name%d[]={\n	",base,j);
		for(i=0;i<256;i++) {
			fprintf(file,"0x%02x,",nameTable[j][i]);
			if((i%8)==7) fprintf(file,"\n	");
		}
		fprintf(file,"}; // region %d\n\n",j);
	}
	fclose(file);
}

extern SDL_Window *rootWindow;

void exportBMP(SDL_Renderer *renderer,const char *filename)
{
   SDL_Surface* screenShot = SDL_CreateRGBSurface(0, 256, 192, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

   if(screenShot)
   {
      SDL_RenderReadPixels(renderer, NULL, SDL_GetWindowPixelFormat(rootWindow), screenShot->pixels, screenShot->pitch);
      SDL_SaveBMP(screenShot, filename);
      SDL_FreeSurface(screenShot);
   }
}
