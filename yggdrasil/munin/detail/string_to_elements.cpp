// ==========================================================================
// Munin String-to-elements.
//
// Copyright (C) 2014 Matthew Chaplain, All Rights Reserved.
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
#include "yggdrasil/munin/detail/string_to_elements.hpp"
#include "yggdrasil/munin/detail/string_to_elements_parser.hpp"
#include "yggdrasil/munin/element.hpp"

namespace {

// ==========================================================================
// DIRECTIVE_VISITOR
// ==========================================================================
class directive_visitor
  : public boost::static_visitor<>
{
public :
    // ======================================================================
    // GET_ELEMENTS
    // ======================================================================
    auto get_elements()
    {
        return elements_;
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(char ch)
    {
        current_glyph_.character_ = ch;
        elements_.emplace_back(current_glyph_, current_attribute_);
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        yggdrasil::munin::detail::default_directive const &directive)
    {
        current_glyph_     = {};
        current_attribute_ = {};
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        yggdrasil::munin::detail::character_set_directive const &directive)
    {
        current_glyph_.character_set_ = directive.character_set_;
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        yggdrasil::munin::detail::locale_directive const &directive)
    {
        current_glyph_.locale_ = directive.locale_;
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        yggdrasil::munin::detail::intensity_directive const &directive)
    {
        current_attribute_.intensity_ = directive.intensity_;
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        yggdrasil::munin::detail::polarity_directive const &directive)
    {
        current_attribute_.polarity_ = directive.polarity_;
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        yggdrasil::munin::detail::underlining_directive const &directive)
    {
        current_attribute_.underlining_ = directive.underlining_;
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        yggdrasil::munin::detail::foreground_colour_directive const &directive)
    {
        current_attribute_.foreground_colour_ = directive.foreground_colour_;
    }
    
    // ======================================================================
    // OPERATOR()
    // ======================================================================
    void operator()(
        yggdrasil::munin::detail::background_colour_directive const &directive)
    {
        current_attribute_.background_colour_ = directive.background_colour_;
    }
    
private :
    std::vector<yggdrasil::munin::element> elements_;
    yggdrasil::munin::glyph                current_glyph_;
    yggdrasil::munin::attribute            current_attribute_;
};

}

namespace yggdrasil { namespace munin { namespace detail {

// ==========================================================================
// STRING_TO_ELEMENTS_IMPL
// ==========================================================================
template <class Iterator>
std::vector<element> string_to_elements_impl(
    Iterator &&parse_begin,
    Iterator &&parse_end)
{
    static yggdrasil::munin::detail::string_to_elements_parser<Iterator> const 
        parser;

    auto parsed_result = std::vector<
        yggdrasil::munin::detail::string_to_elements_directive
    >();

    boost::spirit::qi::parse(parse_begin, parse_end, parser, parsed_result);
    
    directive_visitor visitor;
    
    for (auto const &directive : parsed_result)
    {
        apply_visitor(visitor, directive);
    }
    
    return visitor.get_elements();
}

// ==========================================================================
// STRING_TO_ELEMENTS
// ==========================================================================
std::vector<element> string_to_elements(std::string const& text)
{
    return string_to_elements_impl(text.begin(), text.end());
}

// ==========================================================================
// STRING_TO_ELEMENTS
// ==========================================================================
std::vector<element> string_to_elements(char const *text)
{
    return string_to_elements_impl(text + 0, text + strlen(text));
}

// ==========================================================================
// STRING_TO_ELEMENTS
// ==========================================================================
std::vector<element> string_to_elements(char const *text, size_t len)
{
    return string_to_elements_impl(text + 0, text + len);
}

}}}