# makefile.wat -- Osuwari Multi for ArcaOS/OpenWatcom 2.0
#
# Usage: wmake -f makefile.wat          (see compile-wat.cmd)

CC     = wcc386
RC     = wrc
LINK   = wlink

CFLAGS = -bt=os2 -mf -5 -fpi -Oaxt -W3 -ze -d0 -i=src -i=$(%OS2TK)\h
RCFLAGS = -r -i=src -i=src\images -i=$(%OS2TK)\h

TARGET = bin\osmulti2.exe

OBJS = bin\osmulti2.obj bin\bitmap.obj bin\window.obj bin\track.obj &
       bin\anime.obj bin\balloon.obj bin\shapewin.obj bin\setup.obj &
       bin\about.obj bin\profile.obj bin\ostext.obj bin\uni.obj bin\ulang.obj

IMAGES = src\images\osmulti.ico src\images\nade.ptr src\images\funi.ptr &
         src\images\mul_norm.bmp src\images\mul_ohno.bmp src\images\mul_psyu.bmp &
         src\images\mul_urur.bmp src\images\mul_uru2.bmp src\images\mul_clos.bmp

HDRS = src\osmulti2.h src\osmulres.h src\shapewin.h src\ostext.h src\uni.h src\ulang.h

all: $(TARGET)

.c: src

.c{bin}.obj:
	$(CC) $(CFLAGS) -fo=$@ $[@

bin\osmulti2.obj: src\osmulti2.c $(HDRS)
bin\bitmap.obj:   src\bitmap.c   $(HDRS)
bin\window.obj:   src\window.c   $(HDRS)
bin\track.obj:    src\track.c    $(HDRS)
bin\anime.obj:    src\anime.c    $(HDRS)
bin\balloon.obj:  src\balloon.c  $(HDRS)
bin\shapewin.obj: src\shapewin.c $(HDRS)
bin\setup.obj:    src\setup.c    $(HDRS)
bin\about.obj:    src\about.c    $(HDRS)
bin\profile.obj:  src\profile.c  $(HDRS)
bin\ostext.obj:   src\ostext.c   $(HDRS)
bin\uni.obj:      src\uni.c      $(HDRS)
bin\ulang.obj:    src\ulang.c    $(HDRS)

bin\osmulres.res: src\osmulres.rc src\osmulres.h $(IMAGES)
	$(RC) $(RCFLAGS) -fo=bin\osmulres.res src\osmulres.rc

$(TARGET): $(OBJS) bin\osmulres.res
	$(LINK) system os2v2_pm name $(TARGET) option stack=65536 option map=bin\osmulti2.map file { $(OBJS) }
	$(RC) -q bin\osmulres.res $(TARGET)

clean: .SYMBOLIC
	-@if exist bin\*.obj del bin\*.obj
	-@if exist bin\osmulres.res del bin\osmulres.res
	-@if exist bin\osmulti2.exe del bin\osmulti2.exe
	-@if exist bin\osmulti2.map del bin\osmulti2.map
