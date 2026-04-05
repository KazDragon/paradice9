// ==========================================================================
// Paradice Who List
//
// Copyright (C) 2026 Matthew Chaplain, All Rights Reserved.
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
#ifndef PARADICE_UI_WHO_LIST_HPP_
#define PARADICE_UI_WHO_LIST_HPP_

#include <paradice/export.hpp>

#include <munin/basic_component.hpp>
#include <terminalpp/string.hpp>

#include <memory>
#include <vector>

namespace paradice::ui {

class PARADICE_EXPORT who_list final : public munin::basic_component
{
public:
    void set_player_characters(std::vector<terminalpp::string> names);

protected:
    [[nodiscard]] bool do_can_receive_focus() const override;
    [[nodiscard]] terminalpp::extent do_get_preferred_size() const override;
    void do_event(std::any const &event) override;
    void do_draw(
        munin::render_surface &surface,
        terminalpp::rectangle const &region) const override;

private:
    void set_current_page(std::size_t page);

    std::size_t current_page_ = 0;
    std::vector<terminalpp::string> names_;
};

std::shared_ptr<who_list> make_who_list();

}  // namespace paradice::ui

#endif
