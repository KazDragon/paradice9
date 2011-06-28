// ==========================================================================
// Paradice Cryptography
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#include "paradice/cryptography.hpp"

// Add the define PARADICE_USE_CRYPTOPP in order to use Crypto++ as a method
// of securely hashing passwords.
#ifdef PARADICE_USE_CRYPTOPP
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

// Add the define PARADICE_NOCRYPT in order to use the well-known ROT26 cypher
// to store passwords
#elif defined(PARADICE_NOCRYPT)
// Intentionally blank
#else
#error No encryption method selected
#endif

#include <boost/lexical_cast.hpp>
#include <boost/typeof/typeof.hpp>
#include <cstdlib>

using namespace std;

namespace paradice {

// ==========================================================================
// ENCRYPT
// ==========================================================================
string encrypt(string const &text)
{
#ifdef PARADICE_USE_CRYPTOPP
    CryptoPP::SHA hash;
    
    string result;
    CryptoPP::StringSource(
        text
      , true
      , new CryptoPP::HashFilter(
            hash
          , new CryptoPP::HexEncoder(new CryptoPP::StringSink(result))));
    
#else
    string result = text;
#endif

    return result;
}

}

