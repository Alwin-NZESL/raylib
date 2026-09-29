/*
 * test_amazeview.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "amazeview.h"
#include "mock_adapter.h"
#include "mock_textboxwrapper.h"

class AMazeViewTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        MockAdapter::reset();
        MockTextboxWrapper::reset();
    }
};


TEST_F(AMazeViewTest, Creation)
{
    AMazeView view;
}


TEST_F(AMazeViewTest, SetupCalculatesSideSize)
{
    AMazeView view;
    Level level(20, 10);

    view.setup(level);

    EXPECT_EQ(view.get_side_size(), 28);
}


TEST_F(AMazeViewTest, SetupUsesSmallestDimension)
{
    AMazeView view;
    Level level(10, 20);

    view.setup(level);

    EXPECT_EQ(view.get_side_size(), 28);
}


TEST_F(AMazeViewTest, SetupForSquareLevel)
{
    AMazeView view;
    Level level(20, 20);

    view.setup(level);

    EXPECT_EQ(view.get_side_size(), 28);
}


TEST_F(AMazeViewTest, TransformCoordinates)
{
    AMazeView view;
    Level level(20, 20);

    view.setup(level);

    UICapture capture{
        UICapture::Type::LeftPressed,
        48,
        76,
        std::nullopt
    };

    view.transform_coords(capture);

    ASSERT_TRUE(capture.grid_coords.has_value());

    auto [x, y] = *capture.grid_coords;

    EXPECT_EQ(x, 1);
    EXPECT_EQ(y, 2);
}


TEST_F(AMazeViewTest, TransformCoordinatesOutsideGridTopLeft)
{
    AMazeView view;
    Level level(20, 20);

    view.setup(level);

    UICapture capture{
        UICapture::Type::LeftPressed,
        19,
        19,
        std::nullopt
    };

    view.transform_coords(capture);

    EXPECT_FALSE(capture.grid_coords.has_value());
}


TEST_F(AMazeViewTest, TransformCoordinatesAtGridOrigin)
{
    AMazeView view;
    Level level(20, 20);

    view.setup(level);

    UICapture capture{
        UICapture::Type::LeftPressed,
        20,
        20,
        std::nullopt
    };

    view.transform_coords(capture);

    EXPECT_FALSE(capture.grid_coords.has_value());
}


TEST_F(AMazeViewTest, ProcessNewSuccessRecalculatesSideSize)
{
    AMazeView view;
    Level initial(20, 20);

    view.setup(initial);
    ASSERT_EQ(view.get_side_size(), 28);

    EditorResult result{
        EditorResult::OperationStatus::NewSuccess,
        10,
        20
    };

    view.process(result);

    EXPECT_EQ(view.get_side_size(), 28);
}


TEST_F(AMazeViewTest, ProcessLoadSuccessRecalculatesSideSize)
{
    AMazeView view;
    Level initial(20, 20);

    view.setup(initial);
    ASSERT_EQ(view.get_side_size(), 28);

    EditorResult result{
        EditorResult::OperationStatus::LoadSuccess,
        10,
        10
    };

    view.process(result);

    EXPECT_EQ(view.get_side_size(), 56);
}


TEST_F(AMazeViewTest, LoadSuccessUpdatesDimensions)
{
    AMazeView view;
    Level initial(20, 20);

    view.setup(initial);

    EditorResult result{
        EditorResult::OperationStatus::LoadSuccess,
        12,
        14
    };

    view.process(result);

    EditorAction actions = view.render(initial, 1);

    EXPECT_EQ(actions.new_width, "12");
    EXPECT_EQ(actions.new_height, "14");
}


TEST_F(AMazeViewTest, RenderReturnsCurrentControlValues)
{
    AMazeView view;
    Level level(20, 10);

    view.setup(level);

    EditorAction actions = view.render(level, 3);

    EXPECT_EQ(actions.action, EditorAction::Action::None);
    EXPECT_EQ(actions.new_width, "20");
    EXPECT_EQ(actions.new_height, "10");
    EXPECT_EQ(actions.filename, "");
    EXPECT_EQ(actions.select_tile, 1);
}


TEST_F(AMazeViewTest, RenderReturnsButtonAction)
{
    AMazeView view;
    Level level(20, 20);

    view.setup(level);

    MockAdapter::button_action = EditorAction::Action::New;

    EditorAction actions = view.render(level, 1);

    EXPECT_EQ(actions.action, EditorAction::Action::New);
}


TEST_F(AMazeViewTest, RenderReturnsSelectedTile)
{
    AMazeView view;
    Level level(20, 20);

    view.setup(level);

    MockAdapter::selected_tile = 7;

    EditorAction actions = view.render(level, 1);

    EXPECT_EQ(actions.select_tile, 7);
}


TEST_F(AMazeViewTest, RenderReturnsAllEditorActionInformation)
{
    AMazeView view;
    Level level(20, 20);

    view.setup(level);

    MockAdapter::button_action = EditorAction::Action::Save;
    MockAdapter::selected_tile = 4;
    MockTextboxWrapper::filename_content = "test.level";

    EditorAction actions = view.render(level, 4);

    EXPECT_EQ(actions.action, EditorAction::Action::Save);
    EXPECT_EQ(actions.new_width, "20");
    EXPECT_EQ(actions.new_height, "20");
    EXPECT_EQ(actions.filename, "test.level");
    EXPECT_EQ(actions.select_tile, 4);
}

TEST_F(AMazeViewTest, RenderReturnsFilename)
{
    AMazeView view;
    Level level(20, 10);

    view.setup(level);

    MockTextboxWrapper::filename_content = "test.level";

    EditorAction actions = view.render(level, 3);

    EXPECT_EQ(actions.filename, "test.level");
}


TEST_F(AMazeViewTest, RenderReturnsAllTextBoxValues)
{
    AMazeView view;
    Level level(20, 10);

    view.setup(level);

    MockTextboxWrapper::filename_content = "my_level.dat";
    MockTextboxWrapper::width_content = "15";
    MockTextboxWrapper::height_content = "17";

    EditorAction actions = view.render(level, 3);

    EXPECT_EQ(actions.filename, "my_level.dat");
    EXPECT_EQ(actions.new_width, "15");
    EXPECT_EQ(actions.new_height, "17");
}