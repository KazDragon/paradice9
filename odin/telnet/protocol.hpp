// ==========================================================================
// Odin Telnet Protocol Constants.
//
// Copyright (C) 2003 Matthew Chaplain, All Rights Reserved.
//
// Permission to reproduce, distribute, perform, display, and to prepare
// derivitive works from this file under the following conditions:
//
// 1. Any copy, reproduction or derivitive work of any part of this file 
//    contains this copyright notice and licence in its entirety.
//
// 2. The rights granted to you under this license automatically terminate
//    should you attempt to assert any patent claims against the licensor 
//    or contributors, which in any way restrict the ability of any party 
//    from using this software or portions thereof in any form under the
//    terms of this license.
//
// Disclaimer: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
//             KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//             WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//             PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//             OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//             OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//             OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//             SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
// ==========================================================================
#ifndef ODIN_TELNET_PROTOCOL_HPP_
#define ODIN_TELNET_PROTOCOL_HPP_

#include "odin/types.hpp"
#include <iosfwd>
#include <vector>

namespace odin { namespace telnet {

typedef odin::u8 option_id_type;
typedef odin::u8 negotiation_request_type;

struct command
{
    command()
        : command_(0)
    {
    }

    command(odin::u8 value)
        : command_(value)
    {
    }

    odin::u8 command_;

    bool operator<(command const &other) const
    {
        return command_ < other.command_;
    }

    bool operator==(command const &other) const
    {
        return command_ == other.command_;
    }
};

std::ostream &operator<<(std::ostream &out, command const &cmd);

// Special characters recognised by Telnet.
enum characters
{
    NUL = 0   // Nothing
  , LF  = 10  // Line Feed
  , CR  = 13  // Carriage Return
  , BEL = 7   // Bell
  , VT  = 11  // Vertical Tab
  , FF  = 12  // Form Feed
};

// Telnet commands.
static odin::u8 const EOR(239); // End Of Record
static odin::u8 const SE(240);  // Subnegotiation End
static odin::u8 const NOP(241); // No Operation
static odin::u8 const DM(242);  // Data Mark
static odin::u8 const BRK(243); // Break
static odin::u8 const IP(244);  // Interrupt Process
static odin::u8 const AO(245);  // Abort Output
static odin::u8 const AYT(246); // Are You There?
static odin::u8 const EC(247);  // Erase Character
static odin::u8 const EL(248);  // Erase Line
static odin::u8 const GA(249);  // Go Ahead
static odin::u8 const SB(250);  // Subnegotiation Begin

  // WILL and WONT require a response of DO or DONT
  // DO and DONT require a response of WILL or WONT
static negotiation_request_type const WILL = 251;
static negotiation_request_type const WONT = 252;
static negotiation_request_type const DO   = 253;
static negotiation_request_type const DONT = 254;
  
static odin::u8 const IAC(255); // Interpret As Command

struct negotiation_type
{
    negotiation_request_type request_;
    option_id_type           option_id_;
};

bool operator<(negotiation_type const &lhs, negotiation_type const &rhs);

typedef std::vector<odin::u8> subnegotiation_content_type;

struct subnegotiation_type
{
    option_id_type              option_id_;
    subnegotiation_content_type content_;
};

// A fairly exhaustive list of telnet negotiations and subnegotiations follows:

// RFC856/STD27 - Transmit Binary
static option_id_type const TRANSMIT_BINARY                           = 0;

// RFC857/STD28 - Echo
static option_id_type const ECHO                                      = 1;

// RFCUnknown   - ReConnect Protocol
static option_id_type const RCP                                       = 2;

// RFC858/STD29 - Suppress Go-Ahead
static option_id_type const SUPPRESS_GOAHEAD                          = 3;

// RFCUnknown   - Negotiate About Message Size
static option_id_type const NAMS                                      = 4;

// RFC859/STD30 - Status
static option_id_type const STATUS                                    = 5;
    static option_id_type const STATUS_IS                             = 0;
    static option_id_type const STATUS_SEND                           = 1;
    
// RFC860/STD31 - Timing Mark
static option_id_type const TIMING_MARK                               = 6;

// RFC726       - Remote Controlled Transmission and Echoing
static option_id_type const RCTE                                      = 7;

// RFCUnknown   - Negotiate About Output Line Width
static option_id_type const NAOL                                      = 8;

// RFCUnknown   - Negotiate About Output Page Size
static option_id_type const NAOP                                      = 9;

// RFC652       - Negotiate About Carriage Return Disposition
static option_id_type const NAOCRD                                    = 10;
    static option_id_type const NAOCRD_DR                             = 0;
    static option_id_type const NAOCRD_DS                             = 1;
    
// RFC653       - Negotiate About Output Horizontal Tab Stops
static option_id_type const NAOHTS                                    = 11;
    static option_id_type const NAOHTS_DR                             = 0;
    static option_id_type const NAOHTS_DS                             = 1;
 
// RFC654       - Negotiate About Output Horizontal Tab Disposition
static option_id_type const NAOHTD                                    = 12;
    static option_id_type const NAOHTD_DR                             = 0;
    static option_id_type const NAOHTD_DS                             = 1;

// RFC655       - Negotiate About Output Form Feed Disposition
static option_id_type const NAOFFD                                    = 13;
    static option_id_type const NAOFFD_DR                             = 0;
    static option_id_type const NAOFFD_DS                             = 1;

// RFC656       - Negotiate About Output Vertical Tab Stops
static option_id_type const NAOVTS                                    = 14;
    static option_id_type const NAOVTS_DR                             = 0;
    static option_id_type const NAOVTS_DS                             = 1;
 
// RFC657       - Negotiate About Output Vertical Tab Disposition
static option_id_type const NAOVTD                                    = 15;
    static option_id_type const NAOVTD_DR                             = 0;
    static option_id_type const NAOVTD_DS                             = 1;

// RFC658       - Negotiate About Output Line Feed Disposition
static option_id_type const NAOLFD                                    = 16;
    static option_id_type const NAOLFD_DR                             = 0;
    static option_id_type const NAOLFD_DS                             = 1;
    
// RFC698       - Extended ASCII
static option_id_type const EXTEND_ASCII                              = 17; 

// RFC727       - Logout
static option_id_type const LOGOUT                                    = 18;

// RFC735       - Byte Macro
static option_id_type const BM                                        = 19;
    static option_id_type const BM_DEFINE                             = 1;
    static option_id_type const BM_ACCEPT                             = 2; 
    static option_id_type const BM_REFUSE                             = 3;
        static option_id_type const BM_REFUSE_OTHER_REASON            = 0;
        static option_id_type const BM_REFUSE_BAD_CHOICE              = 1;
        static option_id_type const BM_REFUSE_TOO_LONG                = 2;
        static option_id_type const BM_REFUSE_WRONG_LENGTH            = 3;
    static option_id_type const BM_LITERAL                            = 3;
    static option_id_type const BM_CANCEL                             = 4;
        static option_id_type const BM_CANCEL_OTHER_REASON            = 0;
        static option_id_type const BM_CANCEL_BAD_CHOICE              = 1;
        static option_id_type const BM_CANCEL_TOO_LONG                = 2;
        static option_id_type const BM_CANCEL_WRONG_LENGTH            = 3;
    
// RFC732       - Data Entry Terminal
static option_id_type const DET                                       = 20;
    static option_id_type const DET_EDIT_FACILITIES                   = 1;
        static option_id_type const DET_TOROIDAL_CURSOR_ADDRESSING    = 6;
        static option_id_type const DET_INCREMENTAL_CURSOR_ADDRESSING = 5;
        static option_id_type const DET_READ_CURSOR_ADDRESS           = 4;
        static option_id_type const DET_LINE_INSERT_OR_DELETE         = 3;
        static option_id_type const DET_CHAR_INSERT_OR_DELETE         = 2;
        static option_id_type const DET_BACK_TAB                      = 1;
        static option_id_type const DET_POSITIVE_ADDRESSING_ONLY      = 0;
    static option_id_type const DET_ERASE_FACILITIES                  = 2;
        static option_id_type const DET_EF_ERASE_FIELD                = 4;
        static option_id_type const DET_EF_ERASE_LINE                 = 3;
        static option_id_type const DET_EF_ERASE_REST_OF_SCREEN       = 2;
        static option_id_type const DET_EF_ERASE_REST_OF_LINE         = 1;
        static option_id_type const DET_EF_ERASE_REST_OF_FIELD        = 0;
    static option_id_type const DET_TRANSMIT_FACILITIES               = 3;
        static option_id_type const DET_TF_DATA_TRANSMIT              = 5;
        static option_id_type const DET_TF_TRANSMIT_FIELD             = 4;
        static option_id_type const DET_TF_TRANSMIT_LINE              = 3;
        static option_id_type const DET_TF_TRANSMIT_REST_OF_SCREEN    = 2;
        static option_id_type const DET_TF_TRANSMIT_REST_OF_LINE      = 1;
        static option_id_type const DET_TF_TRANSMIT_REST_OF_FIELD     = 0;
    static option_id_type const DET_FORMAT_FACILITIES                 = 4;
        static option_id_type const DET_FF_FN                         = 7;
        static option_id_type const DET_FF_MODIFIED                   = 6;
        static option_id_type const DET_FF_LIGHT_PEN                  = 5;
        static option_id_type const DET_FF_REPEAT                     = 4;
        static option_id_type const DET_FF_BLINKING                   = 3;
        static option_id_type const DET_FF_REVERSE_VIDEO              = 2;
        static option_id_type const DET_FF_RIGHT_JUSTIFICATION        = 1;
        static option_id_type const DET_FF_OVERSTRIKE                 = 0;
        static option_id_type const DET_FF_PROTECTION_ON_OR_OFF       = 6;
        static option_id_type const DET_FF_PROTECTION                 = 5;
        static option_id_type const DET_FF_ALPHABETIC_ONLY_PROTECTION = 4;
        static option_id_type const DET_FF_NUMERIC_ONLY_PROTECTION    = 3;
        static option_id_type const DET_FF_INTENSITY2                 = 2;
        static option_id_type const DET_FF_INTENSITY1                 = 1;
        static option_id_type const DET_FF_INTENSITY0                 = 0;
    static option_id_type const DET_MOVE_CURSOR                       = 5;
    static option_id_type const DET_SKIP_TO_LINE                      = 6;
    static option_id_type const DET_SKIP_TO_CHAR                      = 7;
    static option_id_type const DET_UP                                = 8;
    static option_id_type const DET_DOWN                              = 9;
    static option_id_type const DET_LEFT                              = 10;
    static option_id_type const DET_RIGHT                             = 11;
    static option_id_type const DET_HOME                              = 12;
    static option_id_type const DET_LINE_INSERT                       = 13;
    static option_id_type const DET_LINE_DELETE                       = 14;
    static option_id_type const DET_CHAR_INSERT                       = 15;
    static option_id_type const DET_CHAR_DELETE                       = 16;
    static option_id_type const DET_READ_CURSOR                       = 17;
    static option_id_type const DET_CURSOR_POSITION                   = 18;
    static option_id_type const DET_REVERSE_TAB                       = 19;
    static option_id_type const DET_TRANSMIT_SCREEN                   = 20;
    static option_id_type const DET_TRANSMIT_UNPROTECTED              = 21;
    static option_id_type const DET_TRANSMIT_LINE                     = 22;
    static option_id_type const DET_TRANSMIT_FIELD                    = 23;
    static option_id_type const DET_TRANSMIT_REST_OF_SCREEN           = 24;
    static option_id_type const DET_TRANSMIT_REST_OF_LINE             = 25;
    static option_id_type const DET_TRANSMIT_REST_OF_FIELD            = 26;
    static option_id_type const DET_TRANSMIT_MODIFIED                 = 27;
    static option_id_type const DET_DATA_TRANSMIT                     = 28;
    static option_id_type const DET_ERASE_SCREEN                      = 29;
    static option_id_type const DET_ERASE_LINE                        = 30;
    static option_id_type const DET_ERASE_FIELD                       = 31;
    static option_id_type const DET_ERASE_REST_OF_SCREEN              = 32;
    static option_id_type const DET_ERASE_REST_OF_LINE                = 33;
    static option_id_type const DET_ERASE_REST_OF_FIELD               = 34;
    static option_id_type const DET_ERASE_UNPROTECTED                 = 35;
    static option_id_type const DET_FORMAT_DATA                       = 36;
        static option_id_type const DET_FD_BLINKING                   = 7;
        static option_id_type const DET_FD_REVERSE_VIDEO              = 6;
        static option_id_type const DET_FD_RIGHT_JUSTIFICATION        = 5;
        static option_id_type const DET_FD_PROTECTION1                = 4;
        static option_id_type const DET_FD_PROTECTION0                = 3;
        static option_id_type const DET_FD_INTENSITY2                 = 2;
        static option_id_type const DET_FD_INTENSITY1                 = 1;
        static option_id_type const DET_FD_INTENSITY0                 = 0;
        static option_id_type const DET_FD_MODIFIED                   = 1;
        static option_id_type const DET_FD_PEN_SELECTABLE             = 0;
    static option_id_type const DET_REPEAT                            = 37;
    static option_id_type const DET_SUPPRESS_PROTECTION               = 38;
    static option_id_type const DET_FIELD_SEPARATOR                   = 39;
    static option_id_type const DET_FN                                = 40;
    static option_id_type const DET_ERROR                             = 41;
               
// RFC734       - Supdup          
static option_id_type const SUPDUP                                    = 21;

// RFC749       - Supdup Output
static option_id_type const SUPDUP_OUTPUT                             = 22;

// RFC779       - Send Location
static option_id_type const SEND_LOCATION                             = 23;

// RFC1091      - Terminal Type
static option_id_type const TERMINAL_TYPE                             = 24;
    static option_id_type const TERMINAL_TYPE_IS                      = 0;
    static option_id_type const TERMINAL_TYPE_SEND                    = 1;

// RFC885       - End Of Record
static option_id_type const END_OF_RECORD                             = 25;

// RFC927       - Telnet User Identification
static option_id_type const TUID                                      = 26;
 
// RFC933       - Output Marking
static option_id_type const OUTMRK                                    = 27;
    static option_id_type const OUTMRK_ACK                            = 6;
    static option_id_type const OUTMRK_NAK                            = 21;
    static option_id_type const OUTMRK_GS                             = 29;
    static option_id_type const OUTMRK_CNTL_D                         = 'D';
    static option_id_type const OUTMRK_CNTL_T                         = 'T';
    static option_id_type const OUTMRK_CNTL_B                         = 'B';
    static option_id_type const OUTMRK_CNTL_L                         = 'L';
    static option_id_type const OUTMRK_CNTL_R                         = 'R';
    
// RFC946       - Terminal Location Number
static option_id_type const TTYLOC                                    = 28;

// RFC1041      - 3270 Regime
static option_id_type const REGIME_3270                               = 29;
    static option_id_type const REGIME_3270_IS                        = 0;
    static option_id_type const REGIME_3270_ARE                       = 1;
    
// RFC1053      - X.3 PAD
static option_id_type const X_3_PAD                                   = 30;
    static option_id_type const X_3_PAD_SET                           = 0;
    static option_id_type const X_3_PAD_RESPONSE_SET                  = 1;
    static option_id_type const X_3_PAD_IS                            = 2;
    static option_id_type const X_3_PAD_RESPONSE_IS                   = 3;
    static option_id_type const X_3_PAD_SEND                          = 4;
    
// RFC1073      - Negotiate About Window Size
static option_id_type const NAWS                                      = 31;

// RFC1079      - Terminal Speed
static option_id_type const TERMINAL_SPEED                            = 32;
    static option_id_type const TERMINAL_SPEED_IS                     = 0;
    static option_id_type const TERMINAL_SPEED_SEND                   = 1;
    
// RFC1372      - Remote Flow Control
static option_id_type const TOGGLE_FLOW_CONTROL                       = 33;
    static option_id_type const TOGGLE_FLOW_CONTROL_OFF               = 0;
    static option_id_type const TOGGLE_FLOW_CONTROL_ON                = 1;
    static option_id_type const TOGGLE_FLOW_CONTROL_RESTART_ANY       = 2;
    static option_id_type const TOGGLE_FLOW_CONTROL_RESTART_XON       = 3;
    
// RFC1184      - Line Mode
static option_id_type const LINEMODE                                  = 34;
    static option_id_type const LINEMODE_MODE                         = 1;
        static option_id_type const LINEMODE_MODE_EDIT                = 1;
        static option_id_type const LINEMODE_MODE_TRAPSIG             = 2;
        static option_id_type const LINEMODE_MODE_MODE_ACK            = 3;
    static option_id_type const LINEMODE_FORWARDMASK                  = 2;
    static option_id_type const LINEMODE_SLC                          = 3;
        static option_id_type const LINEMODE_SLC_SYNCH                = 1;
        static option_id_type const LINEMODE_SLC_BRK                  = 2;
        static option_id_type const LINEMODE_SLC_IP                   = 3;
        static option_id_type const LINEMODE_SLC_AO                   = 4;
        static option_id_type const LINEMODE_SLC_AYT                  = 5;
        static option_id_type const LINEMODE_SLC_EOR                  = 6;
        static option_id_type const LINEMODE_SLC_ABORT                = 7;
        static option_id_type const LINEMODE_SLC_EOF                  = 8;
        static option_id_type const LINEMODE_SLC_SUSP                 = 9;
        static option_id_type const LINEMODE_SLC_EC                   = 10;
        static option_id_type const LINEMODE_SLC_EL                   = 11;
        static option_id_type const LINEMODE_SLC_EW                   = 12;
        static option_id_type const LINEMODE_SLC_RP                   = 13;
        static option_id_type const LINEMODE_SLC_LNEXT                = 14;
        static option_id_type const LINEMODE_SLC_XON                  = 15;
        static option_id_type const LINEMODE_SLC_XOFF                 = 16;
        static option_id_type const LINEMODE_SLC_FORW1                = 17;
        static option_id_type const LINEMODE_SLC_FORW2                = 18;
        static option_id_type const LINEMODE_SLC_MCL                  = 19;
        static option_id_type const LINEMODE_SLC_MCR                  = 20;
        static option_id_type const LINEMODE_SLC_MCWL                 = 21;
        static option_id_type const LINEMODE_SLC_MCWR                 = 22;
        static option_id_type const LINEMODE_SLC_MCBOL                = 23;
        static option_id_type const LINEMODE_SLC_MCEOL                = 24;
        static option_id_type const LINEMODE_SLC_INSRT                = 25;
        static option_id_type const LINEMODE_SLC_OVER                 = 26;
        static option_id_type const LINEMODE_SLC_ECR                  = 27;
        static option_id_type const LINEMODE_SLC_EWR                  = 28;
        static option_id_type const LINEMODE_SLC_EBOL                 = 29;
        static option_id_type const LINEMODE_SLC_EEOL                 = 30;
        static option_id_type const LINEMODE_SLC_DEFAULT              = 3;
        static option_id_type const LINEMODE_SLC_VALUE                = 2;
        static option_id_type const LINEMODE_SLC_CANTCHANGE           = 1;
        static option_id_type const LINEMODE_SLC_NOSUPPORT            = 0;
        static option_id_type const LINEMODE_SLC_LEVELBITS            = 3;
        static option_id_type const LINEMODE_SLC_ACK                  = 128;
        static option_id_type const LINEMODE_SLC_FLUSHIN              = 64;
        static option_id_type const LINEMODE_SLC_FLUSHOUT             = 32;
    static option_id_type const LINEMODE_EOF                          = 236;
    static option_id_type const LINEMODE_SUSP                         = 237;
    static option_id_type const LINEMODE_ABORT                        = 238;
    
// RFC1096      - X Display Location
static option_id_type const XDISPLOC                                  = 35;
    static option_id_type const XDISPLOC_IS                           = 0;
    static option_id_type const XDISPLOC_SEND                         = 1;
    
// RFC1572      - Environment
static option_id_type const ENVIRON                                   = 36;
    static option_id_type const ENVIRON_IS                            = 0;
    static option_id_type const ENVIRON_SEND                          = 1;
    static option_id_type const ENVIRON_INFO                          = 2;
    static option_id_type const ENVIRON_VAR                           = 0;
    static option_id_type const ENVIRON_VALUE                         = 1;
    static option_id_type const ENVIRON_ESC                           = 2;
    static option_id_type const ENVIRON_USERVALUE                     = 3;
    
// RFC2941      - Authentication
static option_id_type const AUTHENTICATION                            = 37;
    static option_id_type const AUTHENTICATION_IS                     = 0;
    static option_id_type const AUTHENTICATION_SEND                   = 1;
    static option_id_type const AUTHENTICATION_REPLY                  = 2;
    static option_id_type const AUTHENTICATION_NAME                   = 3;
    static option_id_type const AUTHENTICATION_NULL                   = 0;
    static option_id_type const AUTHENTICATION_KERBEROS_V4            = 1;
    static option_id_type const AUTHENTICATION_KERBEROS_V5            = 2;
    static option_id_type const AUTHENTICATION_SPX                    = 3;
    static option_id_type const AUTHENTICATION_MINK                   = 4;
    static option_id_type const AUTHENTICATION_SRP                    = 5;
    static option_id_type const AUTHENTICATION_RSA                    = 6;
    static option_id_type const AUTHENTICATION_SSL                    = 7;
    static option_id_type const AUTHENTICATION_LOKI                   = 10;
    static option_id_type const AUTHENTICATION_SSA                    = 11;
    static option_id_type const AUTHENTICATION_KEA_SJ                 = 12;
    static option_id_type const AUTHENTICATION_KEA_SJ_INTEG           = 13;
    static option_id_type const AUTHENTICATION_DSS                    = 14;
    static option_id_type const AUTHENTICATION_NTLM                   = 15;
    static option_id_type const AUTHENTICATION_WHO_MASK               = 1;
    static option_id_type const AUTHENTICATION_CLIENT_TO_SERVER       = 0;
    static option_id_type const AUTHENTICATION_SERVER_TO_CLIENT       = 1;
    static option_id_type const AUTHENTICATION_HOW_MASK               = 2;
    static option_id_type const AUTHENTICATION_HOW_ONE_WAY            = 0;
    static option_id_type const AUTHENTICATION_HOW_MUTUAL             = 2;
    static option_id_type const AUTHENTICATION_ENCRYPT_MASK           = 20;
    static option_id_type const AUTHENTICATION_OFF                    = 0;
    static option_id_type const AUTHENTICATION_USING_TELOPT           = 4;
    static option_id_type const AUTHENTICATION_AFTER_EXCHANGE         = 16;
    static option_id_type const AUTHENTICATION_RESERVED               = 20;
    static option_id_type const AUTHENTICATION_INI_CRED_FWD_MASK      = 8;
    static option_id_type const AUTHENTICATION_INI_CRED_FWD_OFF       = 0;
    static option_id_type const AUTHENTICATION_INI_CRED_FWD_ON        = 8;
    
// RFC2946      - Data Encryption
static option_id_type const ENCRYPT                                   = 38;
    static option_id_type const ENCRYPT_IS                            = 0;
    static option_id_type const ENCRYPT_SUPPORT                       = 1;
    static option_id_type const ENCRYPT_REPLY                         = 2;
    static option_id_type const ENCRYPT_START                         = 3;
    static option_id_type const ENCRYPT_REQUEST_START                 = 4;
    static option_id_type const ENCRYPT_REQUEST_END                   = 5;
    static option_id_type const ENCRYPT_ENC_KEYID                     = 6;
    static option_id_type const ENCRYPT_DEC_KEYID                     = 7;
    static option_id_type const ENCRYPT_NULL                          = 0;
    static option_id_type const ENCRYPT_DES_CFB64                     = 1;
    static option_id_type const ENCRYPT_DES_OFB64                     = 2;
    static option_id_type const ENCRYPT_DES3_CFB64                    = 3;
    static option_id_type const ENCRYPT_DES3_OFB64                    = 4;
    static option_id_type const ENCRYPT_CAST5_40_CDB64                = 8;
    static option_id_type const ENCRYPT_CAST5_40_OFB64                = 9;
    static option_id_type const ENCRYPT_CAST128_CFB64                 = 10;
    static option_id_type const ENCRYPT_CAST128_OFB64                 = 11;
    
// RFC1572      - Environment
static option_id_type const NEW_ENVIRON                               = 39;
    static option_id_type const NEW_ENVIRON_IS                        = 0;
    static option_id_type const NEW_ENVIRON_SEND                      = 1;
    static option_id_type const NEW_ENVIRON_INFO                      = 2;
    static option_id_type const NEW_ENVIRON_VAR                       = 0;
    static option_id_type const NEW_ENVIRON_VALUE                     = 1;
    static option_id_type const NEW_ENVIRON_ESC                       = 2;
    static option_id_type const NEW_ENVIRON_USERVAR                   = 3;

// RFC2066      - Character Set    
static option_id_type const CHARSET                                   = 42;
    static option_id_type const CHARSET_REQUEST                       = 1;
    static option_id_type const CHARSET_ACCEPTED                      = 2;
    static option_id_type const CHARSET_REJECTED                      = 3;
    static option_id_type const CHARSET_TTABLE_IS                     = 4;
    static option_id_type const CHARSET_TTABLE_REJECTED               = 5;
    static option_id_type const CHARSET_TTABLE_ACK                    = 7;
    static option_id_type const CHARSET_TTABLE_NAK                    = 8;

// RFC2217      - Com Port Control
static option_id_type const COM_PORT                                  = 44;
    static option_id_type const COM_PORT_C2S_SET_BAUDRATE             = 1;
    static option_id_type const COM_PORT_C2S_SET_DATASIZE             = 2;
    static option_id_type const COM_PORT_C2S_SET_PARITY               = 3;
    static option_id_type const COM_PORT_C2S_SET_STOPSIZE             = 4;
    static option_id_type const COM_PORT_C2S_SET_CONTROL              = 5;
    static option_id_type const COM_PORT_C2S_NOTIFY_LINESTATE         = 6;
    static option_id_type const COM_PORT_C2S_NOTIFY_MODEMSTATE        = 7;
    static option_id_type const COM_PORT_C2S_FLOWCONTROL_SUSPEND      = 8;
    static option_id_type const COM_PORT_C2S_FLOWCONTROL_RESUME       = 9;
    static option_id_type const COM_PORT_C2S_SET_LINESTATE_MASK       = 10;
    static option_id_type const COM_PORT_C2S_SET_MODEMSTATE_MASK      = 11;
    static option_id_type const COM_PORT_C2S_PURGE_DATA               = 12;
    static option_id_type const COM_PORT_S2C_SET_BAUDRATE             = 101;
    static option_id_type const COM_PORT_S2C_SET_DATASIZE             = 102;
    static option_id_type const COM_PORT_S2C_SET_PARITY               = 103;
    static option_id_type const COM_PORT_S2C_SET_STOPSIZE             = 104;
    static option_id_type const COM_PORT_S2C_SET_CONTROL              = 105;
    static option_id_type const COM_PORT_S2C_NOTIFY_LINESTATE         = 106;
    static option_id_type const COM_PORT_S2C_NOTIFY_MODEMSTATE        = 107;
    static option_id_type const COM_PORT_S2C_FLOWCONTROL_SUSPEND      = 108;
    static option_id_type const COM_PORT_S2C_FLOWCONTROL_RESUME       = 109;
    static option_id_type const COM_PORT_S2C_SET_LINESTATE_MASK       = 110;
    static option_id_type const COM_PORT_S2C_SET_MODEMSTATE_MASK      = 111;
    static option_id_type const COM_PORT_S2C_PURGE_DATA               = 112;
    
// RFC2840      - Kermit
static option_id_type const KERMIT                                    = 47;
    static option_id_type const KERMIT_START_SERVER                   = 0;
    static option_id_type const KERMIT_STOP_SERVER                    = 1;
    static option_id_type const KERMIT_REQ_START_SERVER               = 2;
    static option_id_type const KERMIT_REQ_STOP_SERVER                = 3;
    static option_id_type const KERMIT_SOP                            = 4;
    static option_id_type const KERMIT_RESP_START_SERVER              = 5;
    static option_id_type const KERMIT_RESP_STOP_SERVER               = 6;
    
// http://www.zuggsoft.com/zmud/mcp.htm - Mud Client Compression Protocol    
static option_id_type const COMPRESS2                                 = 86;  

// http://www.zuggsoft.com/zmud/msp.htm - Mud Sound Protocol    
static option_id_type const MSP                                       = 90;     

// http://www.zuggsoft.com/zmud/mxp.htm - Mud eXtension Protocol
static option_id_type const MXP                                       = 91;
    
// http://mud.blogdns.org/wiki.cgi?MudSoundProtocolTwo - Mud Sound Protocol 2
static option_id_type const MSP2                                      = 92;
    static option_id_type const MSP2_MUSIC                            = 0;
    static option_id_type const MSP2_SOUND                            = 1;
    static option_id_type const MSP2_VOLUME                           = 'V';
    static option_id_type const MSP2_REPEATS                          = 'L';
    static option_id_type const MSP2_PRIORITY                         = 'P';
    static option_id_type const MSP2_CONTINUE                         = 'C';
    static option_id_type const MSP2_TYPE                             = 'T';
    static option_id_type const MSP2_URL                              = 'U';
    
// RFC861/STD32 - Extended Options List
static option_id_type const EXOPL                                     = 255;

// RFC748       - Randomly Lose
// static option_id_type const RANDOMLY_LOSE                             = 266;

// RFC1097      - Subliminal Message.
// static option_id_type const SUBLIMINAL_MESSAGE                        = 257;
                
}}

#endif

