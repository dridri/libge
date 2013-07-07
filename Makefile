ifeq ($(ARCH),)
	ARCH = $(shell getconf LONG_BIT)
endif

platform = $(target)

ifeq ($(platform),)
	SYS = $(shell uname)
	ifeq ($(SYS), Linux)
		platform = linux_low
	endif
	ifeq ($(SYS), windows32)
		platform = win_low
		ifeq ($(ARCH),)
			ARCH = 32
		endif
	endif
	ifeq ($(SYS), windows64)
		platform = win_low
		ifeq ($(ARCH),)
			ARCH = 64
		endif
	endif
endif

vid_platform = $(platform)
ifneq (,$(filter $(platform),win_low linux_low))
vid_platform = opengl21
endif
ifneq (,$(filter $(platform),win linux))
vid_platform = opengl30
endif

audio_platform = $(platform)
ifneq (,$(filter $(platform),win_low))
audio_platform = win
endif
ifneq (,$(filter $(platform),linux_low))
audio_platform = linux
endif

ifeq (,$(filter $(platform),win_low win linux_low linux psp android))
$(error Platform not supported by LibGE ($(platform)))
endif

ifneq (,$(filter $(platform),win_low linux_low))
$(info Compiling for OpenGL 2.1 $(ARCH)bits platform)
endif

ifneq (,$(filter $(platform),win linux))
$(info Compiling for OpenGL 3.0+ $(ARCH)bits platform)
endif

$(shell mkdir -p obj obj/gui obj/lua obj/system obj/video obj/audio obj/system/$(platform) obj/audio/$(audio_platform) obj/video/$(vid_platform))

include Makefile.objs

DEFS = -DPLATFORM=$(platform) -DPLATFORM_$(platform) -DARCH=$(ARCH)

include Makefile.$(platform)


obj/%.o: %.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@ -DLIBGE_STATIC

obj/system/$(platform)/%.o: system/$(platform)/%.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@ -DLIBGE_STATIC

obj/video/$(vid_platform)/%.o: video/$(vid_platform)/%.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@ -DLIBGE_STATIC

obj/audio/$(platform)/%.o: audio/$(platform)/%.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@ -DLIBGE_STATIC

obj/lua/$(platform)/%.o: lua/$(platform)/%.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@ -DLIBGE_STATIC
	
	
ifneq ($(platform),android)
clean:
	cd obj && $(RM) *.o
	cd obj/gui && $(RM) *.o
	cd obj/lua && $(RM) *.o
	cd obj/system && $(RM) *.o
	cd obj/video && $(RM) *.o
	cd obj/audio && $(RM) *.o
	cd obj/system/$(platform) && $(RM) *.o
	cd obj/video/$(vid_platform) && $(RM) *.o
	cd obj/audio/$(audio_platform) && $(RM) *.o
endif