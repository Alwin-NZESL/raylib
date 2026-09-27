/*
 * test_amazeditor.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <filesystem>
#include <fstream>

#include "amazeditor.h"

namespace
{
    const std::filesystem::path test_file = std::filesystem::temp_directory_path() / "amazeditor_test_level.txt";

    void remove_test_file()
        { std::filesystem::remove(test_file); }

    UICapture input(UICapture::Type type, int x, int y)
        { return UICapture{type, x, y, std::make_pair(x, y)}; }

    UICapture input_outside(UICapture::Type type, int x, int y)
        { return UICapture{type, x, y, std::nullopt}; }
}


// -----------------------------------------------------------------------------
// Creation
// -----------------------------------------------------------------------------

TEST(EditorTest, Creation)
{
    AMazEditor editor;
    Level expected;

    EXPECT_EQ(editor.get_level(), expected);
    EXPECT_EQ(editor.get_selected_tile(), 1);
}


// -----------------------------------------------------------------------------
// New level
// -----------------------------------------------------------------------------

TEST(EditorTest, NewLevel)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::New;
    action.new_width = "10";
    action.new_height = "15";

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::NewSuccess);
    EXPECT_EQ(result->width, 10);
    EXPECT_EQ(result->height, 15);

    EXPECT_EQ(editor.get_level().get_width(), 10);
    EXPECT_EQ(editor.get_level().get_height(), 15);
}


TEST(EditorTest, NewLevelTooSmall)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::New;
    action.new_width = "4";
    action.new_height = "10";

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::MinExceeded);

    // Existing level must remain unchanged.
    EXPECT_EQ(editor.get_level(), Level());
}


TEST(EditorTest, NewLevelTooLarge)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::New;
    action.new_width = "29";
    action.new_height = "10";

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::MaxExceeded);

    EXPECT_EQ(editor.get_level(), Level());
}


TEST(EditorTest, NewLevelInvalidWidth)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::New;
    action.new_width = "abc";
    action.new_height = "10";

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::InvalidDimensions);

    EXPECT_EQ(editor.get_level(), Level());
}


TEST(EditorTest, NewLevelInvalidHeight)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::New;
    action.new_width = "10";
    action.new_height = "abc";

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::InvalidDimensions);

    EXPECT_EQ(editor.get_level(), Level());
}


// -----------------------------------------------------------------------------
// Editor actions
// -----------------------------------------------------------------------------

TEST(EditorTest, NoneActionProducesNoResult)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::None;

    auto result = editor.process(action);

    EXPECT_FALSE(result.has_value());
}


TEST(EditorTest, SelectingTileChangesSelectedTile)
{
    AMazEditor editor;

    EditorAction action;
    action.select_tile = 7;

    editor.process(action);

    EXPECT_EQ(editor.get_selected_tile(), 7);
}


// -----------------------------------------------------------------------------
// Painting
// -----------------------------------------------------------------------------

TEST(EditorTest, LeftDownPaintsTile)
{
    AMazEditor editor;

    EditorAction action;
    action.select_tile = 5;
    editor.process(action);

    editor.process(input(UICapture::Type::LeftPressed, 3, 4));
    editor.process(input(UICapture::Type::LeftDown, 3, 4));

    EXPECT_EQ(editor.get_level().tile(3, 4), 5);
}


TEST(EditorTest, LeftDownPaintsMultipleTiles)
{
    AMazEditor editor;

    EditorAction action;
    action.select_tile = 6;
    editor.process(action);

    editor.process(input(UICapture::Type::LeftDown, 3, 4));
    editor.process(input(UICapture::Type::LeftDown, 4, 4));
    editor.process(input(UICapture::Type::LeftDown, 5, 4));

    EXPECT_EQ(editor.get_level().tile(3, 4), 6);
    EXPECT_EQ(editor.get_level().tile(4, 4), 6);
    EXPECT_EQ(editor.get_level().tile(5, 4), 6);
}


TEST(EditorTest, PaintingOutsideLevelDoesNothing)
{
    AMazEditor editor;

    EditorAction action;
    action.select_tile = 5;
    editor.process(action);

    editor.process(input(UICapture::Type::LeftDown, -1, 4));
    editor.process(input(UICapture::Type::LeftDown, 24, 4));

    // No crash and existing level remains unchanged.
    EXPECT_EQ(editor.get_level().tile(0, 4), 0);
}


// -----------------------------------------------------------------------------
// Erasing
// -----------------------------------------------------------------------------

TEST(EditorTest, RightPressedErasesTile)
{
    AMazEditor editor;

    EditorAction action;
    action.select_tile = 5;
    editor.process(action);

    editor.process(input(UICapture::Type::LeftDown, 3, 4));
    ASSERT_EQ(editor.get_level().tile(3, 4), 5);

    editor.process(input(UICapture::Type::RightPressed, 3, 4));

    EXPECT_EQ(editor.get_level().tile(3, 4), 0);
}


TEST(EditorTest, ErasingOutsideLevelDoesNothing)
{
    AMazEditor editor;

    editor.process(input(UICapture::Type::RightPressed, -1, 4));
    editor.process(input(UICapture::Type::RightPressed, 24, 4));
}


// -----------------------------------------------------------------------------
// Spawn
// -----------------------------------------------------------------------------

TEST(EditorTest, LeftPressOnSpawnStartsDragging)
{
    AMazEditor editor;

    // The default player position is (0, 0).
    editor.process(input(UICapture::Type::LeftPressed, 0, 0));
    editor.process(input(UICapture::Type::LeftDown, 5, 6));

    auto [x, y] = editor.get_level().get_player_origin();

    EXPECT_EQ(x, 5);
    EXPECT_EQ(y, 6);
}


TEST(EditorTest, LeftPressOnNonSpawnDoesNotStartDragging)
{
    AMazEditor editor;

    editor.process(input(UICapture::Type::LeftPressed, 5, 6));
    editor.process(input(UICapture::Type::LeftDown, 7, 8));

    auto [x, y] = editor.get_level().get_player_origin();

    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
}


TEST(EditorTest, DraggingSpawnClearsOriginalTile)
{
    AMazEditor editor;

    editor.process(input(UICapture::Type::LeftPressed, 0, 0));
    editor.process(input(UICapture::Type::LeftDown, 5, 6));
    editor.process(input(UICapture::Type::LeftReleased, 5, 6));

    EXPECT_EQ(editor.get_level().tile(0, 0), 0);
}


TEST(EditorTest, ReleasingOutsideGridStopsDragging)
{
    AMazEditor editor;

    editor.process(input(UICapture::Type::LeftPressed, 0, 0));
    editor.process(input(UICapture::Type::LeftDown, 5, 6));

    // Release outside the grid.
    editor.process(input_outside(UICapture::Type::LeftReleased, 100, 100));

    // A subsequent LeftDown should paint rather than continue moving
    // the player spawn.
    EditorAction action;
    action.select_tile = 7;
    editor.process(action);

    editor.process(input(UICapture::Type::LeftDown, 10, 10));

    EXPECT_EQ(editor.get_level().tile(10, 10), 7);
}


// -----------------------------------------------------------------------------
// Save / Load
// -----------------------------------------------------------------------------

TEST(EditorTest, SaveLevel)
{
    remove_test_file();

    AMazEditor editor;

    EditorAction new_action;
    new_action.action = EditorAction::Action::New;
    new_action.new_width = "10";
    new_action.new_height = "12";
    ASSERT_TRUE(editor.process(new_action).has_value());

    EditorAction save_action;
    save_action.action = EditorAction::Action::Save;
    save_action.filename = test_file.string();

    auto result = editor.process(save_action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::SaveSuccess);
    EXPECT_TRUE(std::filesystem::exists(test_file));

    remove_test_file();
}


TEST(EditorTest, SaveWithEmptyFilename)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::Save;

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::EmptyFilename);
}


TEST(EditorTest, SaveWithInvalidFilename)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::Save;
    action.filename =
        (std::filesystem::temp_directory_path() /
         "directory_that_does_not_exist" /
         "level.txt").string();

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::OpenFailed);
}


TEST(EditorTest, LoadWithEmptyFilename)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::Load;

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::EmptyFilename);
}


TEST(EditorTest, LoadNonexistentFile)
{
    AMazEditor editor;

    EditorAction action;
    action.action = EditorAction::Action::Load;
    action.filename =
        (std::filesystem::temp_directory_path() /
         "this_file_does_not_exist.txt").string();

    auto result = editor.process(action);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->result, EditorResult::OperationStatus::OpenFailed);
}


TEST(EditorTest, SaveAndLoadLevel)
{
    remove_test_file();

    AMazEditor editor;

    // Create a level.
    EditorAction new_action;
    new_action.action = EditorAction::Action::New;
    new_action.new_width = "10";
    new_action.new_height = "12";
    ASSERT_TRUE(editor.process(new_action).has_value());

    // Modify it.
    EditorAction select_action;
    select_action.select_tile = 8;
    editor.process(select_action);

    editor.process(input(UICapture::Type::LeftDown, 3, 4));

    auto original = editor.get_level();

    // Save it.
    EditorAction save_action;
    save_action.action = EditorAction::Action::Save;
    save_action.filename = test_file.string();

    auto save_result = editor.process(save_action);

    ASSERT_TRUE(save_result.has_value());
    EXPECT_EQ(save_result->result,
              EditorResult::OperationStatus::SaveSuccess);

    // Change the editor's level.
    EditorAction another_level;
    another_level.action = EditorAction::Action::New;
    another_level.new_width = "20";
    another_level.new_height = "20";
    ASSERT_TRUE(editor.process(another_level).has_value());

    EXPECT_NE(editor.get_level(), original);

    // Load the saved level.
    EditorAction load_action;
    load_action.action = EditorAction::Action::Load;
    load_action.filename = test_file.string();

    auto load_result = editor.process(load_action);

    ASSERT_TRUE(load_result.has_value());
    EXPECT_EQ(load_result->result,
              EditorResult::OperationStatus::LoadSuccess);

    EXPECT_EQ(editor.get_level(), original);

    remove_test_file();
}