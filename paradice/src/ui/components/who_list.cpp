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
#include "paradice/ui/components/who_list.hpp"

#include <algorithm>
#include <sstream>
#include <munin/render_surface.hpp>
#include <terminalpp/virtual_key.hpp>

namespace paradice::ui {

namespace {

constexpr std::size_t column_count = 2;
constexpr std::size_t visible_name_row_count = 3;
constexpr std::size_t names_per_page = column_count * visible_name_row_count;
constexpr auto left_column_margin = terminalpp::coordinate_type{1};
constexpr auto page_text_right_margin = terminalpp::coordinate_type{2};
constexpr auto page_row =
    terminalpp::coordinate_type{visible_name_row_count};

}  // namespace

void who_list::set_player_characters(std::vector<terminalpp::string> names)
{
    names_ = std::move(names);

    auto const total_pages =
        names_.empty() ? std::size_t{0}
                       : (names_.size() + names_per_page - 1) / names_per_page;

    if (total_pages == 0)
    {
        current_page_ = 0;
    }
    else
    {
        current_page_ = std::min(current_page_, total_pages - 1);
    }

    on_redraw({terminalpp::rectangle{{0, 0}, get_size()}});
}

void who_list::set_current_page(std::size_t page)
{
    current_page_ = page;
    on_redraw({terminalpp::rectangle{{0, 0}, get_size()}});
}

bool who_list::do_can_receive_focus() const
{
    return true;
}

terminalpp::extent who_list::do_get_preferred_size() const
{
    return {0, 4};
}

void who_list::do_event(std::any const &event)
{
    if (auto const *key = std::any_cast<terminalpp::virtual_key>(&event);
        key != nullptr && has_focus())
    {
        auto const total_pages =
            names_.empty() ? std::size_t{0}
                           : (names_.size() + names_per_page - 1) / names_per_page;

        if (total_pages == 0)
        {
            return;
        }

        if (key->key == terminalpp::vk::cursor_right)
        {
            set_current_page((current_page_ + 1) % total_pages);
            return;
        }

        if (key->key == terminalpp::vk::cursor_left)
        {
            set_current_page((current_page_ + total_pages - 1) % total_pages);
            return;
        }
    }

    basic_component::do_event(event);
}

void who_list::do_draw(
    munin::render_surface &surface, terminalpp::rectangle const &) const
{
    for (auto row = terminalpp::coordinate_type{0}; row < get_size().height_;
         ++row)
    {
        for (auto column = terminalpp::coordinate_type{0};
             column < get_size().width_;
             ++column)
        {
            surface[column][row] = ' ';
        }
    }

    if (names_.empty())
    {
        return;
    }

    auto draw_name = [&surface](
                         terminalpp::string const &name,
                         terminalpp::coordinate_type x,
                         terminalpp::coordinate_type y,
                         std::size_t max_width) {
        auto const truncated = name.size() > max_width;
        auto const visible_columns =
            std::min<std::size_t>(name.size(), max_width);

        for (std::size_t column = 0; column < visible_columns;
             ++column)
        {
            auto &cell =
                surface[x + static_cast<terminalpp::coordinate_type>(column)][y];

            if (truncated && max_width >= 3 && column >= max_width - 3)
            {
                cell = '.';
            }
            else
            {
                cell =
                    name[static_cast<terminalpp::string::size_type>(column)];
            }
        }
    };

    auto const right_column = get_size().width_ / column_count +
                              left_column_margin;
    auto const left_column_width = static_cast<std::size_t>(
        right_column - left_column_margin - terminalpp::coordinate_type{1});
    auto const right_column_width = static_cast<std::size_t>(
        get_size().width_ - right_column - terminalpp::coordinate_type{1});
    auto const first_visible_index = current_page_ * names_per_page;

    if (first_visible_index >= names_.size())
    {
        return;
    }

    auto const visible_names =
        std::min<std::size_t>(names_.size() - first_visible_index, names_per_page);

    for (std::size_t index = 0; index < visible_names; ++index)
    {
        auto const &name = names_[first_visible_index + index];
        auto const column =
            index % column_count == 0 ? left_column_margin : right_column;
        auto const row =
            static_cast<terminalpp::coordinate_type>(index / column_count);
        auto const max_width =
            column == left_column_margin ? left_column_width
                                         : right_column_width;

        draw_name(name, column, row, max_width);
    }

    if (names_.size() > names_per_page)
    {
        auto const total_pages =
            (names_.size() + names_per_page - 1) / names_per_page;

        std::ostringstream stream;
        stream << current_page_ + 1 << " / " << total_pages;

        auto const page_text = stream.str();
        auto const page_x = static_cast<terminalpp::coordinate_type>(
            get_size().width_ - page_text.size() - page_text_right_margin);

        for (std::size_t index = 0; index < page_text.size(); ++index)
        {
            surface[page_x + static_cast<terminalpp::coordinate_type>(index)]
                   [page_row] = page_text[index];
        }
    }
}

std::shared_ptr<who_list> make_who_list()
{
    return std::make_shared<who_list>();
}

}  // namespace paradice::ui
