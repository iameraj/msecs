#ifndef MSECS_HPP
#define MSECS_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

namespace msecs {

template <typename... Types> class World {

        std::tuple<std::vector<uint64_t>, std::vector<std::unique_ptr<Types>>...> ComponentStore {};

    public:
        static_assert(sizeof...(Types) <= 64, "Too many types! Maximum allowed is 64.");

        template <typename... CompTypes> void add_entity(CompTypes&&... components)
        {

                (is_valid_component<std::decay_t<CompTypes>>(), ...);

                uint64_t id = (get_mask<std::decay_t<CompTypes>>() | ...);

                auto& entity_ids = std::get<0>(ComponentStore);
                entity_ids.push_back(id);

                // Store each component inside the tuple of vectors
                (std::get<std::vector<std::unique_ptr<std::decay_t<CompTypes>>>>(ComponentStore)
                        .push_back(std::make_unique<std::decay_t<CompTypes>>(
                            std::forward<CompTypes>(components))),
                    ...);
                auto fill_missing = [&](auto& component_vector) {
                        if (component_vector.size() < entity_ids.size()) {
                                component_vector.push_back(nullptr);
                        }
                };
                (fill_missing(std::get<std::vector<std::unique_ptr<Types>>>(ComponentStore)), ...);
        }

        template <typename... Args, typename SystemFn> void run_system(SystemFn func)
        {
                (is_valid_component<std::decay_t<Args>>(), ...);

                uint64_t system_id = ((get_mask<Args>()) | ...);
                auto& entity_ids   = std::get<0>(ComponentStore);
                for (size_t i = 0; i < entity_ids.size(); ++i) {
                        if ((entity_ids[i] & system_id) == system_id) {
                                func(*std::get<std::vector<std::unique_ptr<Args>>>(
                                    ComponentStore)[i]...);
                        }
                }
        }

    private:
        template <typename T> static constexpr int get_mask() { return typeToMask[get_index<T>()]; }

        static constexpr std::array<int, sizeof...(Types) + 1> typeToMask = [] {
                std::array<int, sizeof...(Types) + 1> arr = {};
                for (std::size_t i = 0; i < sizeof...(Types); ++i) { arr[i + 1] = 1 << i; }
                return arr;
        }();

        template <typename T> static constexpr std::size_t get_index()
        {
                return get_index_impl<T>(std::index_sequence_for<Types...> {});
        }

        template <typename T, std::size_t... I>
        static constexpr std::size_t get_index_impl(std::index_sequence<I...>)
        {
                return ((std::is_same<T, Types>::value ? (I + 1) : 0) + ...);
        }

        template <typename T> constexpr void is_valid_component()
        {
                if constexpr (get_mask<T>() == 0) {
                        static_assert(get_mask<T>() != 0, "Component type is not registered!");
                }
        }
};
}; // namespace msecs

#endif // MSECS_HPP
