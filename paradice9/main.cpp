// ==========================================================================
// Paradice9
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#include "paradice9.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace boost;

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned int>(time(NULL)));

    unsigned int port = 4000;
    
    po::options_description description("Available options");
    description.add_options()
        ( "help,h", "show this help message" )
        ( "port,p", po::value<unsigned int>(&port), "port number" )
        ;

    po::positional_options_description pos_description;
    pos_description.add("port", -1);
    
    try
    {
        po::variables_map vm;
        po::store(
            po::command_line_parser(argc, argv)
                .options(description)
                .positional(pos_description)
                .run()
          , vm);
        
        po::notify(vm);
        
        if (vm.count("help") != 0)
        {
            throw po::error("");
        }
        else if (vm.count("port") == 0)
        {
            throw po::error("Port number must be specified");
        }
    }
    catch(po::error &err)
    {
        if (strlen(err.what()) == 0)
        {
            cout << format("USAGE: %s <port number>|<options>\n")
                        % argv[0]
                 << description
                 << endl;
                 
            return EXIT_SUCCESS;
        }
        else
        {
            cerr << format("ERROR: %s\n\nUSAGE: %s <port number>|<options>\n")
                        % err.what()
                        % argv[0]
                 << description
                 << endl;
        }
        
        return EXIT_FAILURE;
    }

    asio::io_service io_service;
    
    paradice9 application(io_service, port);
    io_service.run();

    return EXIT_SUCCESS;
}
