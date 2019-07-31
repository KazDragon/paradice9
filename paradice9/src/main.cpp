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
#include "paradice9/paradice9.hpp"
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    serverpp::port_identifier port = 4000;
    std::string  threads           = "";
    unsigned int concurrency       = 0;
    
    po::options_description description("Available options");
    description.add_options()
        ( "help,h",                                                    "show this help message"                            )
        ( "port,p",    po::value<serverpp::port_identifier>(&port),    "port number"                                       )
        ( "threads,t", po::value<std::string>(&threads),               "number of threads of execution (0 for autodetect)" )
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

        if (vm.count("threads") == 0)
        {
            concurrency = 1;
        }
        else
        {
            try
            {
                concurrency = boost::lexical_cast<unsigned int>(threads);
            }
            catch(...)
            {
                // Failure is to be expected here, since it might be an empty
                // string.  In this case, concurrency will be a detectable 0.
            }
            
            if (concurrency == 0)
            {
                concurrency = std::thread::hardware_concurrency();
            }

            // According to the Boost docs, "thread::hardware_concurrency()" 
            // may return 0 on platforms that don't have information available
            // about cores/hyperthreading units, etc.  In this case, we will
            // default to one thread.
            if (concurrency == 0)
            {
                concurrency = 1;
            }
        }
    }
    catch(po::error &err)
    {
        if (strlen(err.what()) == 0)
        {
            std::cout << boost::format("USAGE: %s <port number>|<options>\n")
                        % argv[0]
                 << description
                 << std::endl;
                 
            return EXIT_SUCCESS;
        }
        else
        {
            std::cerr << boost::format("ERROR: %s\n\nUSAGE: %s <port number>|<options>\n")
                        % err.what()
                        % argv[0]
                 << description
                 << std::endl;
        }
        
        return EXIT_FAILURE;
    }

    boost::asio::io_context io_context;
    paradice9 application{io_context, port};
 
    std::vector<std::thread> threadpool;

    for (unsigned int thr = 0; thr < concurrency; ++thr)
    {
        threadpool.emplace_back([&io_context]{io_context.run();});
    }
    
    for (auto &pthread : threadpool)
    {
        pthread.join();
    }
    
    return EXIT_SUCCESS;
}

