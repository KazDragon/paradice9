// ==========================================================================
// Odin Finite State Machine.
//
// Copyright (C) 2006 Matthew Chaplain, All Rights Reserved.
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
#ifndef ODIN_STATE_MACHINE_HPP_
#define ODIN_STATE_MACHINE_HPP_

#include <boost/function.hpp>
#include <map>

namespace odin {
    
template <typename State, typename Transition>
class state_machine
{
    public :
        struct state_impl
        {
            boost::function<void ()>                         on_entry;
            boost::function<void ()>                         on_exit;

            std::map< Transition, boost::function<void ()> > transition;
            
            boost::function<void (Transition)> undefined_transition;
        };
        
        state_machine(State initial_state)
            : state_(initial_state)
        {
        }
        
        state_machine &operator()(Transition transition)
        {
            typename std::map<
                Transition
              , boost::function<void ()> 
            >::iterator transition_function_iterator =
                state[state_].transition.find(transition);

            if (transition_function_iterator != state[state_].transition.end()
             && transition_function_iterator->second)
            {
                transition_function_iterator->second();
            }
            else if (state[state_].undefined_transition)
            {
                state[state_].undefined_transition(transition);
            }
                
            return *this;
        }
        
        State get_state() const
        {
            return state_;
        }
        
    protected :
        std::map<State, state_impl> state;

        void set_state(State new_state)
        {
            if (state[state_].on_exit)
            {
                state[state_].on_exit();
            }
            
            state_ = new_state;
            
            if (state[state_].on_entry)
            {
                state[state_].on_entry();
            }
        }

    private :
        State state_;
        
};

}

#endif

