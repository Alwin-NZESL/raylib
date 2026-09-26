/*
 * test_grid.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "level.h"

#include <sstream>
#include <stdexcept>

TEST(GridTest, SetACellValue)
{
    Grid<int> grid(5, 5);

    grid.cell(2, 3) = 42;

    EXPECT_EQ(grid.cell(2, 3), 42);
}

TEST(GridTest, SetTopLeftCell)
{
    Grid<int> grid(5, 5);

    grid.cell(0, 0) = 11;

    EXPECT_EQ(grid.cell(0, 0), 11);
}

TEST(GridTest, SetTopRightCell)
{
    Grid<int> grid(5, 5);

    grid.cell(4, 0) = 12;

    EXPECT_EQ(grid.cell(4, 0), 12);
}

TEST(GridTest, SetBottomLeftCell)
{
    Grid<int> grid(5, 5);

    grid.cell(0, 4) = 13;

    EXPECT_EQ(grid.cell(0, 4), 13);
}

TEST(GridTest, SetBottomRightCell)
{
    Grid<int> grid(5, 5);

    grid.cell(4, 4) = 14;

    EXPECT_EQ(grid.cell(4, 4), 14);
}

TEST(GridTest, CellsAreIndependent)
{
    Grid<int> grid(5, 5);

    grid.cell(2, 3) = 42;

    EXPECT_EQ(grid.cell(2, 3), 42);
    EXPECT_EQ(grid.cell(3, 3), 0);
    EXPECT_EQ(grid.cell(2, 2), 0);
    EXPECT_EQ(grid.cell(2, 4), 0);
}

TEST(GridTest, DimensionsAreCorrect)
{
    Grid<int> grid(7, 11);

    EXPECT_EQ(grid.get_width(), 7);
    EXPECT_EQ(grid.get_height(), 11);
}

TEST(GridTest, NonSquareGrid)
{
    Grid<int> grid(3, 7);

    grid.cell(2, 6) = 42;

    EXPECT_EQ(grid.cell(2, 6), 42);
    EXPECT_EQ(grid.cell(0, 0), 0);
}

TEST(GridTest, CellCanBeOverwritten)
{
    Grid<int> grid(5, 5);

    grid.cell(2, 3) = 42;
    grid.cell(2, 3) = 99;

    EXPECT_EQ(grid.cell(2, 3), 99);
}

TEST(GridTest, CopyIsIndependent)
{
    Grid<int> grid(5, 5);

    grid.cell(2, 3) = 42;

    Grid<int> copy = grid;

    copy.cell(2, 3) = 99;

    EXPECT_EQ(grid.cell(2, 3), 42);
    EXPECT_EQ(copy.cell(2, 3), 99);
}

TEST(GridTest, ConstGridCanBeRead)
{
    const Grid<int> grid(5, 5);

    EXPECT_EQ(grid.cell(2, 3), 0);
}

TEST(GridTest, OutOfBoundsAccessThrows)
{
    Grid<int> grid(5, 5);

    EXPECT_THROW(grid.cell(5, 0), std::out_of_range);
    EXPECT_THROW(grid.cell(0, 5), std::out_of_range);
    EXPECT_THROW(grid.cell(-1, 0), std::out_of_range);
    EXPECT_THROW(grid.cell(0, -1), std::out_of_range);
}

TEST(GridTest, SaveAndLoadACellValue)
{
    std::stringstream stream;

    Grid<int> grid(5, 5);
    grid.cell(2, 3) = 42; // Set tile at (2, 3) to value 42
    stream << grid;

    Grid<int> loaded_grid(1, 1);
    stream >> loaded_grid;

    EXPECT_EQ(loaded_grid.get_width(), 5);
    EXPECT_EQ(loaded_grid.get_height(), 5);
    EXPECT_EQ(loaded_grid.cell(2, 3), 42);
}

TEST(GridTest, SaveAndLoadMultipleCellValues)
{
    std::stringstream stream;

    Grid<int> grid(5, 5);
    grid.cell(0, 0) = 1;
    grid.cell(4, 0) = 2;
    grid.cell(0, 4) = 3;
    grid.cell(4, 4) = 4;
    grid.cell(2, 2) = 42;
    stream << grid;

    Grid<int> loaded_grid(1, 0);
    stream >> loaded_grid;

    EXPECT_EQ(loaded_grid.get_width(), 5);
    EXPECT_EQ(loaded_grid.get_height(), 5);
    EXPECT_EQ(loaded_grid.cell(0, 0), 1);
    EXPECT_EQ(loaded_grid.cell(4, 0), 2);
    EXPECT_EQ(loaded_grid.cell(0, 4), 3);
    EXPECT_EQ(loaded_grid.cell(4, 4), 4);
    EXPECT_EQ(loaded_grid.cell(2, 2), 42);
}
