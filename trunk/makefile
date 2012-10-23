# TOOLS
CC           = g++
AR           = ar -r
RM           = rm -f
MAKE         = make -C

# COMPILER FLAGS
DEBUG        = #-ggdb
OPTIM        = -O2
PROF         = #-pg
CRYPT        = -DPARADICE_USE_CRYPTOPP
D_FLAGS      = -DNDEBUG
C_FLAGS      = -ansi -pedantic -Wall $(D_FLAGS) $(DEBUG) $(OPTIM) $(PROF) \
               $(CRYPT)
I_DIRS       = -I.
L_DIRS       = -L.
L_FLAGS      = $(OPTIM) $(PROF)

ODIN_NAMES        = ansi/protocol \
                    ansi/ansi_parser \
                    net/server \
                    net/socket \
                    telnet/detail/parser \
                    telnet/client_option \
                    telnet/command_router \
                    telnet/input_visitor \
                    telnet/negotiation_router \
                    telnet/protocol \
                    telnet/server_option \
                    telnet/subnegotiation_router \
                    telnet/stream \
                    telnet/options/naws_client \
                    telnet/options/terminal_type_client \
                    tokenise
ODIN_O_FILES      = $(ODIN_NAMES:%=odin/%.o)
ODIN_D_FILES      = $(ODIN_O_FILES:.o=.d)
ODIN_LIB          = libodin.a

MUNIN_NAMES       = algorithm \
                    aligned_layout \
                    ansi/protocol \
                    attribute \
                    basic_component \
                    basic_container \
                    basic_frame \
                    button \
                    canvas \
                    card \
                    clock \
                    compass_layout \
                    component \
                    composite_component \
                    container \
                    context \
                    dropdown_list \
                    edit \
                    filled_box \
                    frame \
                    framed_component \
                    glyph \
                    grid_layout \
                    horizontal_scroll_bar \
                    horizontal_squeeze_layout \
                    horizontal_strip_layout \
                    image \
                    layout \
                    list \
                    named_frame \
                    scroll_pane \
                    solid_frame \
                    status_bar \
                    text_area \
                    text/default_multiline_document \
                    text/default_singleline_document \
                    text/document \
                    toggle_button \
                    types \
                    vertical_scroll_bar \
                    vertical_squeeze_layout \
                    viewport \
                    window
MUNIN_O_FILES     = $(MUNIN_NAMES:%=munin/%.o)
MUNIN_D_FILES     = $(MUNIN_O_FILES:.o=.d)
MUNIN_LIB         = libmunin.a

HUGIN_NAMES       = account_creation_screen \
                    character_creation_screen \
                    character_selection_screen \
                    command_prompt \
                    intro_screen \
                    main_screen \
                    password_change_screen \
                    user_interface \
                    wholist
HUGIN_O_FILES     = $(HUGIN_NAMES:%=hugin/%.o)
HUGIN_D_FILES     = $(HUGIN_O_FILES:.o=.d)
HUGIN_LIB         = libhugin.a

GUIBUILDER_NAMES  = client \
                    main \
                    ui
GUIBUILDER_O_FILES= $(GUIBUILDER_NAMES:%=guibuilder/%.o)
GUIBUILDER_D_FILES= $(GUIBUILDER_O_FILES:.o=.d)
GUIBUILDER_LIBS   = -lmunin -lodin -lboost_program_options -lboost_system \
                    -lboost_signals
G_NAME            = guibuilder.exe

PARADICE_NAMES    = account \
                    admin \
                    character \
                    client \
                    communication \
                    configuration \
                    connection \
                    cryptography \
                    dice_roll_parser \
                    help \
                    random \
                    rules \
                    utility \
                    who
PARADICE_O_FILES  = $(PARADICE_NAMES:%=paradice/%.o)
PARADICE_D_FILES  = $(PARADICE_O_FILES:.o=.d)
PARADICE_LIB      = libparadice.a
LIBS              = -lparadice -lhugin -lmunin -lodin -lboost_program_options \
                    -lboost_filesystem -lboost_system -lboost_signals \
                    -lboost_serialization -lboost_thread -lcryptopp

PARADICE9_NAMES   = context_impl paradice9 main
PARADICE9_O_FILES = $(PARADICE9_NAMES:%=paradice9/%.o)
PARADICE9_D_FILES = $(PARADICE9_O_FILES:.o=.d)
E_NAME            = paradice.exe

TEST_L_DIRS       = -L.
TEST_LIBS         = -lparadice -lmunin -lodin -lcppunit -lboost_signals \
                    -lboost_system -lboost_system
TEST_L_FLAGS      = --enable-auto-import
TEST_FIXTURES     = ansi_parser \
                    dice_parser \
                    munin_string_to_elements \
                    router \
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
                    telnet_options_terminal_type_client \
                    telnet_parser \
                    telnet_server_option \
                    telnet_stream \
                    telnet_subnegotiation_router \
                    telnet_input_visitor
TEST_NAMES        = paradice_test $(TEST_FIXTURES:%=%_fixture)
TEST_O_FILES      = $(TEST_NAMES:%=test/%.o)
TEST_D_FILES      = $(TEST_O_FILES:.o=.d)
TEST_EXE          = paradice_test.exe

ALL_D_FILES = $(ODIN_D_FILES) $(MUNIN_D_FILES) $(HUGIN_D_FILES) \
              $(PARADICE_D_FILES) $(PARADICE9_D_FILES) $(TEST_D_FILES) \
              $(GUIBUILDER_D_FILES)
              
# Add rules for specific platform
# To anyone trying to build Paradice that is failing here, simply create
# an empty file with the name given in the error message.  Then add lines of
# makefile to that file in order to fix any other problems (extra libraries,
# etc.) that crop up.
UNAME = $(shell uname)
-include Makefile.$(UNAME)

# Add user rules
# To anyond trying to build Paradice that needs specific rules to be added,
# for example non-standard include or library locations, then you can add
# those rules to your own user.mk file, and it will be included automatically.
-include user.mk

# Build Rules              
$(E_NAME): $(ODIN_LIB) $(MUNIN_LIB) $(HUGIN_LIB) $(PARADICE_LIB) \
           $(TEST_EXE) $(PARADICE9_O_FILES)
	./$(TEST_EXE)
	$(CC) -o $@ $(PARADICE9_O_FILES) $(L_DIRS) $(LIBS) $(L_FLAGS)
    
$(G_NAME): $(ODIN_LIB) $(MUNIN_LIB) $(GUIBUILDER_O_FILES)
	$(CC) -o $@ $(GUIBUILDER_O_FILES) $(L_DIRS) $(GUIBUILDER_LIBS) $(L_FLAGS)
	
.PHONY: clean
clean: 
	$(RM) $(E_NAME) $(PARADICE9_O_FILES) \
	      $(G_NAME) $(GUIBUILDER_O_FILES) \
          $(PARADICE_LIB) $(PARADICE_O_FILES) \
          $(HUGIN_LIB) $(HUGIN_O_FILES) \
          $(MUNIN_LIB) $(MUNIN_O_FILES) \
          $(ODIN_LIB) $(ODIN_O_FILES) \
          $(TEST_EXE) $(TEST_O_FILES) \
          $(ALL_D_FILES)
    
libodin.a: $(ODIN_O_FILES)
libmunin.a: $(MUNIN_O_FILES)
libhugin.a: $(HUGIN_O_FILES)
libparadice.a: $(PARADICE_O_FILES)
%.a:
	$(AR) $@ $?
	
$(TEST_EXE): $(TEST_O_FILES) $(ODIN_LIB) $(PARADICE_LIB) \
             $(MUNIN_LIB) $(HUGIN_LIB)
	$(CC) -o $@ $(TEST_O_FILES) $(TEST_L_DIRS) $(TEST_LIBS) $(L_FLAGS)

-include $(ALL_D_FILES)              

%.o: %.cpp
	$(CC) -MMD -MF $*.d -MT '$@' -o $@ -c $(C_FLAGS) $(I_DIRS) $<

