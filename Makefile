# Library metadata.
PROJECTNAME := OrbisUtil

# relistic it'd be better to just move INTDIR up before this, but nah...
TARGETSTATIC  = ORBIS_Debug/lib$(PROJECTNAME).a

# Libraries linked into the ELFs
LIBS        := -lc -lkernel -lc++

# Root vars
TOOLCHAIN   := $(OO_PS4_TOOLCHAIN)
INTDIR      := ORBIS_Debug
EXTRAFLAGS  := 

# Define objects to build
CFILES      := $(wildcard source/**/*.c)
CPPFILES    := $(wildcard source/**/*.cpp)
OBJS        := $(patsubst %.c, %.o, $(CFILES)) $(patsubst %.cpp, %.o, $(CPPFILES))

# Define final C/C++ flags
CFLAGS      := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c -D__OPENORBIS__=1 -o0 -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include
CXXFLAGS    := $(CFLAGS) -D__OPENORBIS__=1 -o0 -isystem $(TOOLCHAIN)/include/c++/v1
LDFLAGS     := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crt1.o

# make the dir...
_unused     := $(shell mkdir -p $(INTDIR))

# Check for linux vs macOS and account for clang/ld path
UNAME_S     := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
		AR      := llvm-ar
		CC      := clang
		CCX     := clang++
		LD      := ld.lld
		CDIR    := linux
endif
ifeq ($(UNAME_S),Darwin)
		AR      := /usr/local/opt/llvm/bin/llvm-ar
		CC      := /usr/local/opt/llvm/bin/clang
		CCX     := /usr/local/opt/llvm/bin/clang++
		LD      := /usr/local/opt/llvm/bin/ld.lld
		CDIR    := macos
endif

%.o: %.cpp
	$(CCX) $(CXXFLAGS) -o $@ $<
	
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(TARGETSTATIC): $(ODIR) $(OBJS)
	$(AR) rcs $(TARGETSTATIC) $(OBJS)
	rm -r $(OBJS) 
	
clean:
	rm -f $(OBJS)
	rm $(TARGETSTATIC)
