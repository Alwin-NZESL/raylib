#include <gtest/gtest.h>

#include "level.h"

#include <fstream>
#include <utility>

TEST(LevelTest, SaveAndLoadLevel)
{
    Level level(5, 5);
    {
        std::ofstream file("test_level.lvl");
        file << level;
    }

    Level loaded_level(0, 0);
    {
        std::ifstream file("test_level.lvl");
        file >> loaded_level;
    }

    EXPECT_EQ(level, loaded_level);
}

TEST(LevelTest, CopyIsIndependent)
{
    Level level(5, 5);

    level.tile(2, 3) = 42;

    Level copy = level;

    copy.tile(2, 3) = 99;

    EXPECT_EQ(level.tile(2, 3), 42);
    EXPECT_EQ(copy.tile(2, 3), 99);
}

    
TEST(LevelTest, SetPlayerSpawnAndAngle)
{
    Level level(5, 5);

    level.set_player_origin( std::make_pair(2.0F, 2.0F) );
    level.set_player_angle( 90.0F );

    EXPECT_EQ(level.get_player_origin(), std::make_pair(2.0f, 2.0f) );
    EXPECT_EQ(level.get_player_angle(), 90.0f );
}

TEST(ConversionTest, ParseOldMakeNew)
{
	std::string old_level =
        "444444444444444477777777"
        "400000000000000070000007"
        "401000000000000000000007"
        "402000000000000000000007"
        "403000000000000070000007"
        "404000055555555577077777"
        "405000050505050570007771"
        "406000050000000570000008"
        "407000000000000000007771"
        "408000050000000570000008"
        "400000050000000570007771"
        "400000055550555577777771"
        "666666666660666666666666"
        "800000000000000000000004"
        "666666066660666666666666"
        "444444044460622222223333"
        "400000000460620000020002"
        "400000000000620050020002"
        "400000000460620000022022"
        "406060000460000050000002"
        "400500000460620000022022"
        "406060000460620050020002"
        "400000000460620000020002"
        "444444444411122222233333";

    Level level(24,24);

    for( size_t y = 0; y < 24; ++y )
        for( size_t x = 0; x < 24; ++x )
            level.tile(x,y) = old_level[y * 24 + x] - '0';

    level.set_player_origin( std::make_pair(2.0F, 2.0F) );
    level.set_player_angle( 0.0F );

    std::ofstream file("old_level.lvl");
    file << level;
}

TEST(ConversionTest, ParseFirstLevel)
{
	std::string old_level =
	    "111111111111111111111111"
        "100000000000000000000001"
        "100000000000000000000001"
        "100000000000000000000001"
        "100000222220000303030001"
        "100000200020000000000001"
        "100000200020000300030001"
        "100000200020000000000001"
        "100000220220000303030001"
        "100000000000000000000001"
        "100000000000000000000001"
        "100001020304050607080001"
        "100000000000000000000001"
        "100000000000000000000001"
        "100000000000000000000001"
        "100000000000000000000001"
        "144444444000000000000001"
        "140400004000000000000001"
        "140000504000000000000001"
        "140400004000000000000001"
        "140444444000000000000001"
        "140000000000000000000001"
        "144444444000000000000001"
        "111111111111111111111111";

    Level level(24,24);

    for( size_t y = 0; y < 24; ++y )
        for( size_t x = 0; x < 24; ++x )
            level.tile(x,y) = old_level[y * 24 + x] - '0';

    level.set_player_origin( std::make_pair(2.0F, 2.0F) );
    level.set_player_angle( 0.0F );
    
    std::ofstream file("first_level.lvl");
    file << level;
}
