#CC          = g++-4
CC           = g++
AR           = ar -r
RM           = rm -f
MAKE         = make -C
DEBUG        = -ggdb
OPTIM        = #-O2 -DNDEBUG
PROF         = #-pg
D_FLAGS      = -D_WIN32_WINNT=0x0501 -D__USE_W32_SOCKETS
C_FLAGS      = -ansi -pedantic -Wall $(D_FLAGS) $(DEBUG) $(OPTIM) $(PROF)
NAME_IS      = -o
I_DIRS       = -I.
NOLINK       = -c
L_DIRS       = -L.
LIBS         = -lparadice -lmunin -lodin -lboost_program_options \
               -lboost_system -lws2_32 -lwsock32
E_NAME       = paradice.exe

ODIN_NAMES        = telnet/client_option \
                    telnet/command_router \
                    telnet/input_visitor \
                    telnet/negotiation_router \
                    telnet/protocol \
                    telnet/server_option \
                    telnet/subnegotiation_router \
                    telnet/stream \
                    telnet/options/naws_client \
                    tokenise
ODIN_O_FILES      = $(ODIN_NAMES:%=odin/%.o)
ODIN_LIB          = libodin.a

MUNIN_NAMES       = algorithm \
                    ansi/attribute \
                    ansi/graphics_context \
                    ansi/types \
                    ansi/window \
                    types
MUNIN_O_FILES     = $(MUNIN_NAMES:%=munin/%.o)
MUNIN_LIB         = libmunin.a

PARADICE_NAMES    = client \
                    communication \
                    configuration \
                    connection \
                    dice_roll_parser \
                    rules \
                    server \
                    socket \
                    utility \
                    who
PARADICE_O_FILES  = $(PARADICE_NAMES:%=paradice/%.o)
PARADICE_LIB      = libparadice.a

PARADICE9_NAMES   = context_impl Paradice9
PARADICE9_O_FILES = $(PARADICE9_NAMES:%=paradice9/%.o)

TEST_L_DIRS       = -L.
TEST_LIBS         = -lparadice -lmunin -lodin -lcppunit -lboost_system \
                    -lboost_signals -lws2_32 -lwsock32
TEST_L_FLAGS      = --enable-auto-import
TEST_FIXTURES     = buffered_datastream \
                    dice_parser \
                    munin_algorithm \
                    munin_ansi_graphics_context \
                    munin_ansi_window \
                    munin_component \
                    munin_container \
                    munin_graphics_context \
                    munin_layout \
                    router \
                    runtime_array \
                    small_buffer \
                    telnet_client_option \
                    telnet_command_router \
                    telnet_generator \
                    telnet_negotiation_router \
                    telnet_options_echo_client \
                    telnet_options_naws_client \
                    telnet_options_subnegotiationless_client \
                    telnet_options_subnegotiationless_server \
                    telnet_options_suppress_goahead_client \
                    telnet_options_suppress_goahead_server \
                    telnet_parser \
                    telnet_server_option \
                    telnet_stream \
                    telnet_subnegotiation_router \
                    telnet_input_visitor
TEST_NAMES        = paradice_test $(TEST_FIXTURES:%=%_fixture)
TEST_O_FILES      = $(TEST_NAMES:%=test/%.o)
TEST_EXE          = paradice_test.exe

$(E_NAME): $(ODIN_LIB) $(MUNIN_LIB) $(PARADICE_LIB) $(TEST_EXE) $(PARADICE9_O_FILES)
	$(CC) $(NAME_IS) $@ $(PARADICE9_O_FILES) $(L_DIRS) $(LIBS)
    
clean: 
	$(RM) $(E_NAME) $(PARADICE9_O_FILES) \
          $(PARADICE_LIB) $(PARADICE_O_FILES) \
          $(MUNIN_LIB) $(MUNIN_O_FILES) \
          $(ODIN_LIB) $(ODIN_O_FILES) \
          $(TEST_EXE) $(TEST_O_FILES)
    
libodin.a: $(ODIN_O_FILES)
	$(AR) $@ $(ODIN_O_FILES)

libmunin.a: $(MUNIN_O_FILES)
	$(AR) $@ $(MUNIN_O_FILES)

libparadice.a: $(PARADICE_O_FILES)
	$(AR) $@ $(PARADICE_O_FILES)

$(TEST_EXE): $(TEST_O_FILES) $(ODIN_LIB) $(PARADICE_LIB) $(MUNIN_LIB)
	$(CC) $(NAME_IS) $@ $(TEST_O_FILES) $(TEST_L_DIRS) $(TEST_LIBS)
	./$@    
    
%.o: %.cpp
	$(CC) $(NAME_IS) $@ $(NOLINK) $(C_FLAGS) $(I_DIRS) $<
