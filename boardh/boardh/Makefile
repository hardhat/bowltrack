OBJ=bg.rel sprite.rel main.rel

all: result.rom

main.rel: main.c
	sdcc -mz80 -c --std-c99 main.c

sprite.rel: sprite.c
	sdcc -mz80 -c --std-c99 sprite.c

bg.rel: bg.c
	sdcc -mz80 -c --std-c99 bg.c

result.rom: $(OBJ)
	sdcc -mz80 --code-loc 0x8024 --data-loc 0x7000 --no-std-crt0 ../crtcv.rel $(OBJ) ../cvlib.lib ../getput.lib
	psp-objcopy --input-target=ihex --output-target=binary crtcv.ihx result.rom
	ls -l *.rom

clean:
	-rm *.rel *.asm *.lst *.sym *.noi *.map *.lk *.ihx

