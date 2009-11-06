#CC          = g++-4
CC           = g++
AR           = ar -r
RM           = rm -f
MAKE         = make -C
DEBUG        = -ggdb
OPTIM        = #-O2
PROF         = #-pg
D_FLAGS      = -D_WIN32_WINNT=0x0501 -D__USE_W32_SOCKETS
C_FLAGS      = -ansi -pedantic -Wall $(D_FLAGS) $(DEBUG) $(OPTIM) $(PROF)
NAME_IS      = -o
I_DIRS       = -I.
NOLINK       = -c
LIB_O_FILES  = server.o
MAIN_O_FILES = main.o
L_DIRS       = -L.
LIBS         = -lparadice -lboost_system-gcc34-mt \
               -lboost_program_options-gcc34-mt -lws2_32 -lwsock32 
E_NAME       = paradice.exe

PARADICE_NAMES    = server socket client connection communication
PARADICE_O_FILES  = $(PARADICE_NAMES:%=paradice/%.o)
PARADICE_LIB      = libparadice.a

PARADICE9_NAMES   = Paradice9
PARADICE9_O_FILES = $(PARADICE9_NAMES:%=paradice9/%.o)

$(E_NAME): $(PARADICE_LIB) $(PARADICE9_O_FILES)
	$(CC) $(NAME_IS) $(E_NAME) $(PARADICE9_O_FILES) $(L_DIRS) $(LIBS) $(L_FLAGS)

clean: 
	$(RM) $(E_NAME) $(PARADICE_LIB) $(PARADICE9_O_FILES) $(PARADICE_O_FILES)
    
libparadice.a: $(PARADICE_O_FILES)
	$(AR) $@ $(PARADICE_O_FILES)

%.o: %.cpp
	$(CC) $(NAME_IS) $@ $(NOLINK) $(C_FLAGS) $(I_DIRS) $<
