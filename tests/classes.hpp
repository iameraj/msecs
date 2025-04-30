#include <string>
#include <vector>

struct Position {
        float x, y;

        Position(float __x = 0.0f, float __y = 0.0f)
            : x(__x)
            , y(__y)
        {
        }

        static Position default_() { return Position(); }
};

struct Velocity {
        float dx, dy;

        Velocity(float __dx = 0.0f, float __dy = 0.0f)
            : dx(__dx)
            , dy(__dy)
        {
        }

        static Velocity default_() { return Velocity(); }
};

struct Health {
        int current, max;

        Health(int __current = 100, int __max = 100)
            : current(__current)
            , max(__max)
        {
        }

        static Health default_() { return Health(); }
};

struct Name {
        std::string value;

        Name(const std::string& __value = "Unnamed")
            : value(__value)
        {
        }

        static Name default_() { return Name(); }
};

struct Inventory {
        std::vector<std::string> items;

        Inventory(const std::vector<std::string>& __items = {})
            : items(__items)
        {
        }

        static Inventory default_() { return Inventory(); }
};

struct AIState {
        enum class State { Idle, Patrol, Attack };
        State current;

        AIState(State __state = State::Idle)
            : current(__state)
        {
        }

        static AIState default_() { return AIState(); }
};
