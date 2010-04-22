// ==========================================================================
// Paradice Random
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.
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
#include "paradice/random.hpp"
#include <boost/random.hpp>
#include <ctime>

using namespace odin;
using namespace boost;
using namespace std;

namespace paradice {

struct random_number_generator
{
    random_number_generator()
    {
        // Because of the fixed bit pattern for a lot of the current time, 
        // randomness doesn't really kick in for at least 10 rolls.  So,
        // We seed and run before actually using any numbers.
        rng.seed(static_cast<unsigned int>(time(NULL)));

        uniform_int<> numbers(1,100);
        variate_generator< mt19937&, uniform_int<> > roller(rng, numbers);

        for (u32 roll = 0; roll < 20; ++roll)
        {
            roller();
        }
    }

    u32 operator()(u32 from, u32 to)
    {
        uniform_int<> die(from, to);
        variate_generator< mt19937&, uniform_int<> > roller(rng, die);

        return roller();
    }

    mt19937 rng;
};

odin::u32 random_number(odin::u32 from, odin::u32 to)
{
    static random_number_generator generate;
    return generate(from, to);
}

}
