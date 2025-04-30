#include "../msecs.hpp"
#include "classes.hpp"
#include <gtest/gtest.h>

TEST(WorldTests, AddEntities)
{
        msecs::World<Position> ecs;

        const int final_count = 10;

        // Add `final count` entities
        for (int i = 0; i < final_count; i++) { ecs.add_entity(Position::default_()); }

        // Count the number of entites using `run_system`
        int entity_count = 0;
        ecs.run_system<Position>([&entity_count](Position _pos) { entity_count++; });

        EXPECT_EQ(entity_count, final_count);
}
