#include "../include/msecs.hpp"
#include "classes.hpp"
#include <cstdlib>
#include <gtest/gtest.h>

TEST(WorldTests, AddEntities)
{
        msecs::World<Position> ecs;

        const int final_count = 10;

        // Add `final count` entities
        for (int i = 0; i < final_count; i++) { ecs.add_entity(Position::default_()); }

        // Empty template matches every entity so this function iterates over all entites
        int entity_count = 0;
        ecs.run_system([&]() { entity_count++; });

        EXPECT_EQ(entity_count, final_count);
}

TEST(WorldTests, RunSystems)
{
        auto ecs = msecs::World<Position, Velocity, Name>();

        ecs.add_entity(Name("Demo"), Position(0, 0), Velocity(1.1, 1.2));

        for (int i = 0; i < 11; i++) {
                ecs.run_system<Position, Velocity>(
                    [](Position& pos, Velocity& vel) { pos.x += vel.dx, pos.y += vel.dy; });
        }

        int did_test_run = false;
        ecs.run_system<Position>([&](Position& pos) {
                ASSERT_FLOAT_EQ(pos.x, 12.1);
                ASSERT_FLOAT_EQ(pos.y, 13.2);
                did_test_run = true;
        });

        ASSERT_TRUE(did_test_run);
}

TEST(WorldTests, FilterSystems)
{

        auto ecs = msecs::World<Name>();

        const int total_entites = 100;
        const std::string name  = "john";

        for (int i = 0; i < total_entites; ++i) { ecs.add_entity(Name(name)); }

        ecs.filter_entities<Name>([&name](Name& n) { return n.value != name; });

        int alive_players = 0;
        ecs.run_system([&]() { alive_players++; });

        ASSERT_EQ(alive_players, 0);
}
TEST(WorldTests, FilterSystemsSenarioTest)
{
        auto ecs = msecs::World<Name, Health>();

        std::srand(time(nullptr));

        const int total_entites = rand() % 100;

        // Populating world with... players?
        for (int i = 0; i < total_entites; ++i) { ecs.add_entity(Name(), Health()); }

        // Doing two random attacks to players
        ecs.run_system<Health>([](Health& h) {
                h.current -= rand() % 50;
                h.current -= rand() % 100;
        });

        // Counting players that have <1 hp
        int dead_players = 0;
        ecs.run_system<Health>([&](Health& h) { dead_players += (h.current < 1) & 1; });

        // Filtering out dead players from the world
        ecs.filter_entities<Health>([](Health& h) { return h.current > 0; });

        // Counting remaining players and checking they all have health >=1
        int alive_players = 0;
        ecs.run_system<Health>([&](Health& h) {
                ASSERT_GE(h.current, 1);
                alive_players++;
        });

        // Final sanity check
        ASSERT_EQ(alive_players + dead_players, total_entites);
}
