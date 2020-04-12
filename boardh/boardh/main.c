#include <coleco.h>
#include <getput1.h>

extern const byte bitmapColRLE[];
extern const byte bitmapPatRLE[];
extern const byte SPATT[];
extern const int SPATT_LENGTH;
extern const byte STABLE[];
extern const int STABLE_LENGTH;
extern const int frameIndex[];
extern const int frameCount[];
extern const int FRAME_LENGTH;

struct Player {
	byte x,y;
	byte speed;
	int trackPosition;
	byte frame;
} player;

void resetGame()
{
	player.x=128;
	player.y=96;
	player.speed=0;
	player.trackPosition=0;
	player.frame=0;
}

void showSpriteFrame()
{
	int i;
	int *sprtab=(int *)0x73f2;

	// Sprite 0 is for debugging.
	for(i=0;i<=frameCount[player.frame];i++) {
		int base=i+frameIndex[player.frame];
		sprites[i+1].y=player.y+STABLE[base<<2];
		sprites[i+1].x=player.x+STABLE[(base<<2)+1];
		sprites[i+1].pattern=STABLE[(base<<2)+2];
		sprites[i+1].colour=STABLE[(base<<2)+3];
	}
	for(;i<32;i++) {
		sprites[i].y=208;
	}
	//update_sprites(10,0);
	put_vram(*sprtab,sprites,40);
}

void handleJoypad()
{
	player.frame=3;
	if(joypad_1&LEFT) {
		player.frame=0;
		player.x--;
		if(player.x<64) player.x=64;	// should wipe out.
	} else if(joypad_1&RIGHT) {
		player.frame=2;
		player.x++;
		if((int)player.x>256-65) player.x=256-64;
	}
	if(joypad_1&UP) {
		player.frame=7;
	}
	if(joypad_1&DOWN) {
		player.frame=1;
	}
}

void update()
{
	sprites[0].y=64;
	sprites[0].x++;
	sprites[0].colour=1;
	sprites[0].pattern=sprites[0].x>>4;
}

void render()
{
	showSpriteFrame();
}

void init()
{
	unsigned int *spatt_table=(unsigned int *)0x73f4;
        screen_off();
        screen_mode_2_bitmap();
        sprites_16x16();

        load_colorrle(bitmapColRLE);
        load_patternrle(bitmapPatRLE);
	put_vram(*spatt_table,SPATT,SPATT_LENGTH);
        screen_on();
        enable_nmi();

//        disable_nmi();  
        
//      screen(name_table1,name_table2);
//      load_namerle(NAMERLE);
//      rle2vram(NAMERLE,0x1800);
//      rle2vram(NAMERLE,0x1b00);
//      screen_on();
}


void main()
{
	init();

	resetGame();

	for(;;) {
		render();
		delay(1);
		disable_nmi();
		//writevram(surface,0x1800+96,512);
		//put_frame(surface,0,4,S_WIDTH,S_HEIGHT);
		enable_nmi();
		handleJoypad();
		update();
	}

}

void nmi()
{
}

const byte sound1[]={0xff};

const struct SoundTable {
        const byte *data;
        byte *area;
}snd_table[]={{sound1,SOUNDAREA1}};


