#include "msecs.hpp"
#include <cstdio>
#include <vector>

struct Position {
        int x, y;
};

struct Velocity {
        int x, y;
};

struct Identity {
        const char* name;
};

void apply_velocity(Position& pos, Velocity& vel)
{
        pos.x += vel.x;
        pos.y += vel.y;
}

int main(void)
{
        auto world = msecs::World<Position, Velocity, Identity> {};
        world.add_entity(Position { 10, 10 }, Velocity { 2, 2 }, Identity { "Point One" });
        world.add_entity(Position { 20, 10 }, Velocity { 2, 3 }, Identity { "Point Two" });

        for (int i = 0; i < 100; ++i) {
                world.run_system<Position, Velocity>([&](Position& pos, Velocity& vel) {
                        pos.x += vel.x;
                        pos.y += vel.y;
                });
                world.run_system<Position, Identity>([&](Position& pos, Identity& id) {
                        printf(" %s is at (%d, %d)\t", id.name, pos.x, pos.y);
                });

                /**
                 *  // Causes error due to mismatched template and function args
                 *
                 *  world.run_system<Position, Identity>([&](Position& pos, Velocity& vel) {
                 *          pos.x += vel.x;
                 *          pos.y += vel.y;
                 *  });
                 *
                 */

                /**
                 *   // Causes error due to misplaced template and function args
                 *   world.run_system<Identity, Position>([&](Position& pos, Identity& id) {
                 *           printf(" %s is at (%d, %d)\t", id.name, pos.x, pos.y);
                 *   });
                 */

                // Works fine as long as both args are in same order
                world.run_system<Identity, Position>([&](Identity& id, Position& pos) {
                        printf(" %s is at (%d, %d)\t", id.name, pos.x, pos.y);
                });
                world.run_system<Identity, std::vector<int>>(
                    [&](Identity& _id, std::vector<int> _vec) {});

                printf("\n");
        }
}
