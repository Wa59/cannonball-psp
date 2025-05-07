TARGET = cannonball-psp

OBJS = src/main/romloader.o \
        src/main/roms.o \
        src/main/trackloader.o \
        src/main/utils.o \
        src/main/video.o \
        src/main/sdl/audio.o \
        src/main/sdl/input.o \
        src/main/sdl/renderbase.o \
        src/main/sdl/rendersw.o \
        src/main/sdl/timer.o \
        src/main/engine/oanimseq.o \
        src/main/engine/oattractai.o \
        src/main/engine/obonus.o \
        src/main/engine/ocrash.o \
        src/main/engine/oferrari.o \
        src/main/engine/ohiscore.o \
        src/main/engine/ohud.o \
        src/main/engine/oinitengine.o \
        src/main/engine/oinputs.o \
        src/main/engine/olevelobjs.o \
        src/main/engine/ologo.o \
        src/main/engine/omap.o \
        src/main/engine/omusic.o \
        src/main/engine/ooutputs.o \
        src/main/engine/opalette.o \
        src/main/engine/oroad.o \
        src/main/engine/osmoke.o \
        src/main/engine/osprite.o \
        src/main/engine/osprites.o \
        src/main/engine/ostats.o \
        src/main/engine/otiles.o \
        src/main/engine/otraffic.o \
        src/main/engine/outils.o \
        src/main/engine/outrun.o \
        src/main/engine/audio/osound.o \
        src/main/engine/audio/osoundint.o \
        src/main/hwaudio/segapcm.o \
        src/main/hwaudio/soundchip.o \
        src/main/hwaudio/ym2151.o \
        src/main/hwvideo/hwroad.o \
        src/main/hwvideo/hwsprites.o \
        src/main/hwvideo/hwtiles.o \
        src/main/frontend/cabdiag.o \
        src/main/frontend/config.o \
        src/main/frontend/menu.o \
        src/main/frontend/ttrial.o \
        src/main/cannonboard/asyncserial.o \
        src/main/cannonboard/interface.o \
        src/main/directx/ffeedback.o \
        src/main/callback.o \
        src/main/main.o

INCDIR += src/main src/boost
CFLAGS = -DPSP -DCOMPILE_SOUND_CODE -G0 -Wall -g -O3
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lstdc++ -lSDL_mixer -lSDL_ttf -lfreetype -lvorbisfile -lvorbis -logg -lSDL -lpng -lpspgum -lpspgu \
    -lg -lc -lGLU -lglut -lGL -lm  -lpspaudiolib -lpspaudio \
    -lpspvfpu  \
    -L${PSPDEV}/psp/sdk/lib \
    -L${PSPDEV}/psp \
    -lpspdebug -lpspctrl -lpspge -lpspirkeyb -lpsppower -lpsppower_driver \
    -lpspdisplay -lpsphprm  -lpspsdk -lpsprtc -lpsputility \
    -lpspnet_inet -lpspuser -lpspvram

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Cannonball
PSP_EBOOT_ICON= ICON0.PNG
PSP_EBOOT_PIC1= PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
