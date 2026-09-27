/*
 * test_editor_view_int.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <gtest/gtest.h>

#include "amazeditor.h"
#include "amazeview.h"
#include "mock_adapter.h"
#include "mock_textboxwrapper.h"

TEST(EditorViewIntegration, NewLevel)
{
    AMazEditor editor;
    AMazeView view;

    view.setup(editor.get_level());

    MockAdapter::button_action = EditorAction::Action::New;
    MockTextboxWrapper::width_content = "12";
    MockTextboxWrapper::height_content = "14";

    EditorAction actions = view.render(
        editor.get_level(),
        editor.get_selected_tile()
    );

    auto result = editor.process(actions);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(
        result->result,
        EditorResult::OperationStatus::NewSuccess
    );

    view.process(*result);

    EXPECT_EQ(editor.get_level().get_width(), 12);
    EXPECT_EQ(editor.get_level().get_height(), 14);
    EXPECT_EQ(view.get_side_size(), 40);
}

TEST(EditorViewIntegration, LoadLevel)
{
    AMazEditor editor;
    AMazeView view;

    view.setup(editor.get_level());

    MockTextboxWrapper::filename_content = "old_level.lvl";
    MockAdapter::button_action = EditorAction::Action::Load;

    EditorAction actions = view.render(
        editor.get_level(),
        editor.get_selected_tile()
    );

    EXPECT_EQ(actions.action, EditorAction::Action::Load);
    EXPECT_EQ(actions.filename, "old_level.lvl");

    auto result = editor.process(actions);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(
        result->result,
        EditorResult::OperationStatus::LoadSuccess
    );

    view.process(*result);

    EXPECT_EQ(editor.get_level().get_width(), 24);
    EXPECT_EQ(editor.get_level().get_height(), 24);
    EXPECT_EQ(view.get_side_size(), 23);
}