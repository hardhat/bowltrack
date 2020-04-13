#include<stdio.h>
#include<string.h>

#include<SDL.h>
#include<SDL_image.h>

#include "export.h"

#define WIDTH 32
#define HEIGHT 43

struct Color {
	unsigned char y;
	unsigned char x;
	unsigned char patt;
	unsigned char color;
} color[64];
int nextColor;

extern SDL_Surface *spriteSurface;
int spriteFrame;
int binaryMode=0;

void spriteReset()
{
	nextColor=0;
}

int getSpritePixel(int x,int y)
{
	if(!spriteSurface) spriteSurface=IMG_Load("sprites.tga");

	unsigned char *p=((unsigned char *)spriteSurface->pixels)+spriteSurface->pitch*y+(x+spriteFrame*WIDTH)*spriteSurface->format->BytesPerPixel;
	uint32_t pixel=*(uint32_t *)p;
	Uint8 rgba[4];
	SDL_GetRGBA(pixel,spriteSurface->format,rgba+0,rgba+1,rgba+2,rgba+3);
	if(rgba[3]<128) return 0;	// transparent
	return rgbToColecoPalette(rgba);
}

int buildSprites(FILE *file,int ch,int coleco)
{
	unsigned char sprite[WIDTH*HEIGHT];
	int i,j;
	int xmin=-1,xmax=-1,ymin=-1,ymax=-1;
	int freq=0;
	for(i=0;i<HEIGHT;i++) {
		for(j=0;j<WIDTH;j++) {
			int c=getSpritePixel(j,i);
			if(c==ch) c=1; else c=0;
			sprite[i*WIDTH+j]=c;
			freq+=c;
			if(c) {
				if(j<xmin || xmin==-1) xmin=j;
				if(j>xmax || xmax==-1) xmax=j;
				if(i<ymin || ymin==-1) ymin=i;
				if(i>ymax || ymax==-1) ymax=i;
			}
		}
	}
	if(freq==0) return 0;
	// Measure boundbox of that sprite and output it.
	if(!binaryMode) fprintf(file,"/* Sprite at %d,%d -> %d,%d (%d x %d) */\n",xmin,ymin,xmax,ymax,xmax-xmin,ymax-ymin);
	int sw=((xmax-xmin+15)/16);
	int sh=((ymax-ymin+15)/16);
	if(!binaryMode) fprintf(file,"/* sprite count=%d (%d x %d) */\n",sw*sh,sw,sh);
	if(sw*sh+nextColor>64) {
		printf("*** Sprite buffer overflow.  Export aborted\n");
		return 0;
	}
	int sx,sy,cx,ry,bx;
	for(sy=0;sy<sh;sy++) {	// sprite y
		for(sx=0;sx<sw;sx++) {	// sprite x
			color[nextColor].x=xmin+sx*16;
			color[nextColor].y=ymin+sy*16;
			color[nextColor].color=coleco;
			color[nextColor].patt=4*nextColor;
			nextColor++;
			for(cx=0;cx<2;cx++) {	// sprite column
				for(ry=0;ry<16;ry++) {	// sprite row
					int c=0;
					for(bx=0;bx<8;bx++) {
						int x=xmin+sx*16+cx*8+bx;
						int y=ymin+sy*16+ry;
						int pos=x+y*WIDTH;
						if(x<WIDTH && y<HEIGHT) {
							c=c*2+sprite[pos];
						} else c<<=1;
					}
					if(!binaryMode) {
						fprintf(file,"0x%02x%s",c,ry==15?",\n":",");
					} else {
						fprintf(file,"%c",c);
					}
				}
			}
		}
	}
	return sw*sh;
}

void sprites2C(const char *filename)
{
	FILE *file;
	int i;
	int frame;
	int count=0;
	char base[256];
	int frameIndex[256];
	int frameCount[256];

	file=fopen(filename,"w");
	if(!file) {
		printf("Could not create '%s'\n",filename);
		return;
	}
	strcpy(base,filename);
	if(strchr(base,'.')) {
        strchr(base,'.')[0]=0;
	}

	fprintf(file,"const byte SPATT[]={\n");

	for(frame=0;frame<8;frame++) {
		spriteFrame=frame;
		fprintf(file,"// Frame %d ==========\n",frame);
		frameIndex[frame]=count;
		frameCount[frame]=0;
		for(i=1;i<16;i++) {
			//fprintf(file,"/* Output color %d */\n",i);
			int built=buildSprites(file,i,i);
			count+=built;
			frameCount[frame]+=built;
		}
	}
	fprintf(file,"};\n\nconst byte STABLE[]={\n");
	for(i=0;i<nextColor;i++) {
	fprintf(file,"0x%02x,0x%02x,0x%02x,0x%02x",color[i].y,color[i].x,color[i].patt,color[i].color);
		if((i%4)==3) fprintf(file,",\n"); else fprintf(file,",");
	}

	fprintf(file,"};\n/* Made %d sprites */\nconst int SPATT_LENGTH=%d;\n",count,count*32);
	fprintf(file,"const int STABLE_LENGTH=%d;\n",count*4);

	fprintf(file,"\nconst int frameIndex[]={\n");
	for(i=0;i<frame;i++) {
		fprintf(file,"%d,",frameIndex[i]);
	}
	fprintf(file,"\n};\n\nconst int frameCount[]={\n");
	for(i=0;i<frame;i++) {
		fprintf(file,"%d,",frameCount[i]);
	}
	fprintf(file,"\n};\nconst int FRAME_LENGTH=%d;",frame);

	fclose(file);
}

void sprites2PPSPR(const char *filename)
{
	FILE *file;
	int frame;
	int count=0;
	int i;
	int frameIndex[256];
	int frameCount[256];

	file=fopen(filename,"wb");	// binary write
	if(!file) {
		printf("Could not create '%s'\n",filename);
		return;
	}

	binaryMode=1;
	for(frame=0;frame<8;frame++) {
		spriteFrame=frame;
		frameIndex[frame]=count;
		frameCount[frame]=0;
		for(i=1;i<16;i++) {
			//fprintf(file,"/* Output color %d */\n",i);
			int built=buildSprites(file,i,i);
			count+=built;
			frameCount[frame]+=built;
		}
	}
	binaryMode=0;
	// Fill to 2k
	for(i=count*32;i<0x800;i++) fprintf(file,"%c",0);
	// Now output the active index of the animation frame, number of frames
	fprintf(file,"%c%c",2,frame);
	for(i=0;i<frame;i++) {
		int j;
		fprintf(file,"%c",frameCount[i]);
		for(j=frameIndex[i];j<frameIndex[i]+frameCount[i];j++) {
			fprintf(file,"%c%c%c%c",color[j].y,color[j].x,color[j].patt,color[j].color);
		}
	}
	fprintf(file,"%c%c",0,1);
	fclose(file);
}
