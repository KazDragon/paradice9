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
LIBS         = -lodin -lparadice -lboost_system-gcc34-mt \
               -lboost_program_options-gcc34-mt -lws2_32 -lwsock32
E_NAME       = paradice.exe

ODIN_NAMES        = telnet/completed_negotiation \
                    telnet/initiated_negotiation \
                    telnet/stream
ODIN_O_FILES      = $(ODIN_NAMES:%=odin/%.o)
ODIN_LIB          = libodin.a

PARADICE_NAMES    = server socket client connection communication \
                    dice_roll_parser
PARADICE_O_FILES  = $(PARADICE_NAMES:%=paradice/%.o)
PARADICE_LIB      = libparadice.a

PARADICE9_NAMES   = Paradice9
PARADICE9_O_FILES = $(PARADICE9_NAMES:%=paradice9/%.o)

TEST_L_DIRS       = -L.
TEST_LIBS         = -lodin -lparadice -lcppunit
TEST_L_FLAGS      = --enable-auto-import
TEST_FIXTURES     = dice_parser
TEST_NAMES        = paradice_test $(TEST_FIXTURES:%=%_fixture)
TEST_O_FILES      = $(TEST_NAMES:%=test/%.o)
TEST_EXE          = paradice_test.exe

$(E_NAME): $(ODIN_LIB) $(PARADICE_LIB) $(TEST_EXE) $(PARADICE9_O_FILES)
	$(CC) $(NAME_IS) $@ $(PARADICE9_O_FILES) $(L_DIRS) $(LIBS)
    
clean: 
	$(RM) $(E_NAME) $(PARADICE_LIB) $(PARADICE9_O_FILES) $(PARADICE_O_FILES) \
          $(ODIN_O_FILES) $(ODIN_LIB) $(TEST_O_FILES) $(TEST_EXE)
    
libodin.a: $(ODIN_O_FILES)
	$(AR) $@ $(ODIN_O_FILES)
    
libparadice.a: $(PARADICE_O_FILES)
	$(AR) $@ $(PARADICE_O_FILES)

$(TEST_EXE): $(TEST_O_FILES) $(ODIN_LIB) $(PARADICE_LIB)    
	$(CC) $(NAME_IS) $@ $(TEST_O_FILES) $(TEST_L_DIRS) $(TEST_LIBS)
	$@    
    
%.o: %.cpp
	$(CC) $(NAME_IS) $@ $(NOLINK) $(C_FLAGS) $(I_DIRS) $<
