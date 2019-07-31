// ==========================================================================
// Paradice Help
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.  
// This file is covered by the MIT Licence:
//
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.
// ==========================================================================
#include "paradice/rules.hpp"
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "paradice/connection.hpp"
#include "paradice/tokenise.hpp"
#include "hugin/user_interface.hpp"
#include "terminalpp/encoder.hpp"
#include <boost/format.hpp>

namespace {
    static std::string const help_header =
        "Type HELP CLOSE to close this window.\n"
        "\n";
        
        static std::string const help_repeat =
        "COMMAND: !\n"
        "\n"
        "Repeats the previous command.\n";
        
    static std::string const help_say =
        "COMMAND: SAY, .\n"
        "\n"
        " USAGE:    [say|.] <text>\n"
        " EXAMPLE:  say hello, everyone!\n"
        " EXAMPLE:  . What-ho!\n"
        "\n"
        "Sends a message to the room.\n";
        
    static std::string const help_whisper =
        "COMMAND: WHISPER, >\n"
        "\n"
        " USAGE:    [whisper|>] <player> <text>\n"
        " EXAMPLE:  whisper perdix What's with the tentacles?\n"
        " EXAMPLE:  > manesni So how much weird should be expected?\n"
        "\n"
        "Sends a message to a specific player.\n";
        
    static std::string const help_emote =
        "COMMAND: EMOTE, :\n"
        "\n"
        " USAGE:    [emote|:] <message>\n"
        " EXAMPLE:  emote wants a hug.\n"
        " EXAMPLE:  : kicks a puppy.\n"
        "\n"
        "Sends a message to the room in the form of an action.\n";
        
    static std::string const help_set =
        "COMMAND: SET\n"
        "\n"
        " USAGE:    set <option> <value>\n"
        " EXAMPLE:  set commandmode mmo\n"
        "\n"
        "Sets a particular configuration option.  Currently, the only option "
        "available is 'commandmode', which can be set to either 'mud' or "
        "'mmo'.  The commandmode option defaults to mud.  Setting it to mmo "
        "means that text entered into the command field will use the 'say' "
        "command by default, unless prefixed with a / character (e.g. /quit)";
        
    static std::string const help_title =
        "COMMAND: TITLE, SURNAME\n"
        "\n"
        " USAGE:   [title|surname] <text>\n"
        " EXAMPLE: title lord of the flies\n"
        " EXAMPLE: surname McFly\n"
        "\n"
        "Sets the text that comes after your name in the list of currently "
        "active players.\n";
        
    static std::string const help_prefix =
        "COMMAND: PREFIX, HONORIFIC\n"
        "\n"
        " USAGE:   [prefix|honorific] <text>\n"
        " EXAMPLE: prefix Sir\n"
        " EXAMPLE: honorific Overlord\n"
        "\n"
        "Sets the text that comes before your name in the list of currently "
        "active players.\n";

    static std::string const help_roll =
        "COMMAND: ROLL\n"
        "\n"
        " USAGE:   roll [n*]<dice>d<sides>[<bonuses...>] [<category>]\n"
        " EXAMPLE: roll 2d6+3-20\n"
        " EXAMPLE: roll 20*2d6\n"
        " EXAMPLE: roll 1d10+4 initiative\n"
        "\n"
        "Rolls the specified number of dice.  For example, 2d6+3 rolls two "
        "six-sided dice and adds three to the total.  2*1d10+4 rolls a single "
        "ten-sided die, adds four to the result, then repeats the process a "
        "second time.\n"
        "If a category is selected, then that roll is added to the selected "
        "category for later retrieval.\n"
        "These rolls are visible to all.";

    static std::string const help_rollprivate =
        "COMMAND: ROLLPRIVATE\n"
        "\n"
        " USAGE:   rollprivate [n*]<dice>d<sides>[<bonuses...>] [<category>]\n"
        " EXAMPLE: rollprivate 2d6+3-20\n"
        " EXAMPLE: rollprivate 20*2d6\n"
        "\n"
        "Rolls the specified number of dice.  For example, 2d6+3 rolls two "
        "six-sided dice and adds three to the total.  2*1d10+4 rolls a single "
        "ten-sided die, adds four to the result, then repeats the process a "
        "second time.\n"
        "These rolls are visible only to the roller.";
        
    static std::string const help_showrolls =
        "COMMAND: SHOWROLLS\n"
        "\n"
        " USAGE:   showrolls <category> [asc|desc]\n"
        " EXAMPLE: showrolls initiative\n"
        " EXAMPLE: showrolls initiative desc\n"
        "\n"
        "Shows the list of rolls that have been made in the specified "
        "category.\n"
        "If an ordering is chosen, then the rolls are listed in either "
        "ascending or descending order, as appropriate.";
        
    static std::string const help_clearrolls =
        "COMMAND: CLEARROLLS\n"
        "\n"
        " USAGE:   clearrolls <category>\n"
        " EXAMPLE: clearrolls initiative\n"
        "\n"
        "Clears all rolls from the specified category.\n";
        
    static std::string const help_password =
        "COMMAND: PASSWORD\n"
        "\n"
        " USAGE: password\n"
        "\n"
        "Switches to the password change screen.  Fill in the details "
        "requested on that screen to change your password.\n";
        
    static std::string const help_help = 
        "COMMAND: HELP\n" 
        "  USAGE:   help <subject>\n"
        "  EXAMPLE: help say\n"
        "\n"
        "Shows help on the specified subject.\n"
        "\n"
        "  USAGE:   help close\n"
        "\n"
        "Closes the help window.\n";
        
    static std::string const help_gm =
        "COMMAND: GM\n"
        "  USAGE:   gm <command>\n"
        "  EXAMPLE: gm tools\n"
        "\n"
        "Performs a GM action.  These include:\n"
        "  TOOLS - brings up an screen of tools for GMs.\n"
        "  ENCOUNTER <command> - manipulate the live encounters (todo).\n";

    static struct help_item 
    {
        std::string command;
        std::string text;
    } const help_table[] =
    {
        { "!",           help_repeat      }
      , { "say",         help_say         }
      , { ".",           help_say         }
      , { "whisper",     help_whisper     }
      , { ">",           help_whisper     }
      , { "emote",       help_emote       }
      , { ":",           help_emote       }
      , { "set",         help_set         }
      , { "title",       help_title       }
      , { "surname",     help_title       }
      , { "prefix",      help_prefix      }
      , { "honorific",   help_prefix      }
      , { "roll",        help_roll        }
      , { "rollprivate", help_rollprivate }
      , { "showrolls",   help_showrolls   }
      , { "clearrolls",  help_clearrolls  }
      , { "password",    help_password    }
      , { "gm",          help_gm          }
      , { "help",        help_help        }
    };
}

namespace paradice {

// ==========================================================================
// PARADICE COMMAND: HELP
// ==========================================================================
PARADICE_COMMAND_IMPL(help)
{
    auto argument = paradice::tokenise(arguments).first;
    auto user_interface = player->get_user_interface();
    
    if (argument == "close")
    {
        user_interface->hide_help_window();
    }
    else
    {
        for (size_t index = 0;
             index < (sizeof(help_table) / sizeof(*help_table));
             ++index)
        {
            if (argument == help_table[index].command)
            {
                user_interface->set_help_window_text(
                    terminalpp::encode(
                        help_header
                      + help_table[index].text));
                user_interface->show_help_window();
                return;
            }
        }
        
        auto text = 
            help_header
          + "No help on that subject was found.  Available subjects are:\n";
            
        for (size_t index = 0;
             index < (sizeof(help_table) / sizeof(*help_table));
             ++index)
        {
            text += help_table[index].command;
            text += " ";
        }
        
        user_interface->set_help_window_text(terminalpp::encode(text));
        user_interface->show_help_window();
    }
}

}

