#ifndef SPECS_HPP
#define SPECS_HPP

#define FULL_FORM "Simple P...... Entity Component System"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

namespace specs {

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

        //        std::unordered_map<SystemType, std::vector<std::function<void(World&)>>> Systems;
};

/*
 * Don't use this, work in progress
 */
template <typename... Types> class new_World {

        struct __entity {
                uint64_t mask;
                std::vector<size_t> compIndex;
        };

        std::tuple<std::vector<__entity>, std::vector<Types>...> Components {};
        template <typename... CompTypes> void new_entity(CompTypes&&... components)
        {
                (is_valid_component<std::decay_t<CompTypes>>(), ...);

                uint64_t entity_mask = (get_mask<std::decay_t<CompTypes>>() | ...);

                (std::get<std::vector<std::decay_t<CompTypes>>>(Components)
                        .push_back(std::forward<CompTypes>(components)),
                    ...);

                std::vector<size_t> compIndex { (
                    std::get<std::vector<std::decay_t<CompTypes>>>(Components).size() - 1)... };

                std::get<std::vector<__entity>>(Components).emplace_back(entity_mask, compIndex);
        }

        template <typename... Args, typename SystemFn> void run_system(SystemFn func)
        {

                (is_valid_component<std::decay_t<Args>>(), ...);

                uint64_t system_mask = ((get_mask<Args>()) | ...);
                for (auto entity : std::get<std::vector<__entity>>(Components)) {
                        if ((entity.mask & system_mask) != system_mask) { continue; }
                        auto get_component_ref = [&](auto type) -> auto& {
                                using T = decltype(type);

                                // Get index of component type in the Components tuple
                                size_t typeIndex = get_index<T>();

                                // Get component position in storage
                                size_t componentStorageIndex = entity.compIndex[typeIndex];

                                // Fetch component
                                return std::get<std::vector<T>>(Components)[componentStorageIndex];
                        };

                        // Unpack all components in correct order
                        auto component_refs = std::forward_as_tuple(
                            get_component_ref(std::__type_identity<Args> {})...);

                        // Call the system function with the correct components
                        std::apply(func, component_refs);
                }
        }

        std::tuple<std::vector<uint64_t>, std::vector<std::unique_ptr<Types>>...> ComponentStore {};
        //        std::unordered_map<SystemType, std::vector<std::function<void(World&)>>> Systems;

        template <typename T> constexpr void is_valid_component()
        {
                if constexpr (get_mask<T>() == 0) {
                        static_assert(get_mask<T>() != 0, "Component type is not registered!");
                }
        }

    public:
        static_assert(sizeof...(Types) <= 64, "Too many types! Maximum allowed is 64.");

        template <typename... CompTypes> void add_entity(CompTypes&&... components)
        {

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

        template <typename... Args, typename SystemFn> void trigger(SystemFn func)
        {
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
};
}; // namespace specs

#endif // SPECS_HPP

/*-----------------------------------------------------------------------------.
 ============================= Trash ===========================================


        template <typename Func> void add_system(SystemType type, Func&& func)
        {
                size_t index = static_cast<size_t>(type);

                using FuncType = decltype(&std::decay_t<Func>::operator()); // Get lambda type
                using ArgsTuple =
                    typename function_traits<FuncType>::args; // Extract argument types

                std::apply(
                    [this, index, &func](auto&&... args) {
                            this->add_system_impl(index, func,
                                std::__type_identity<std::decay_t<decltype(args)>> {}...);
                    },
                    ArgsTuple {});
        }

        template <typename Func> void invoke_system(Func&& func)
        {
                using FuncType = decltype(&std::decay_t<Func>::operator()); // Get lambda type
                using ArgsTuple =
                    typename function_traits<FuncType>::args; // Extract argument types

                std::apply(
                    [this, &func](auto&&... args) {
                            this->invoke_system_impl(
                                func, std::type_identity<std::decay_t<decltype(args)>> {}...);
                    },
                    ArgsTuple {});
        }

        void run_systems(SystemType type)
        {
                for (auto& system : SystemStore[static_cast<size_t>(type)]) { system(*this); }
        }

        std::array<std::vector<std::function<void(World&)>>, SystemTypeCount> SystemStore;

        template <typename... Args>
        void invoke_system_impl(std::function<void(Args&...)> func, std::type_identity<Args>...)
        {
                uint64_t id = ((get_mask<Args>()) | ...);

                auto& entity_ids = std::get<0>(ComponentStore);
                for (size_t i = 0; i < entity_ids.size(); ++i) {
                        if ((entity_ids[i] & id) == id) {
                                func(*std::get<std::vector<std::unique_ptr<Args>>>(
                                    ComponentStore)[i]...);
                        }
                }
        }

        template <typename... Args>
        void add_system_impl(
            std::size_t index, std::function<void(Args&...)> func, std::__type_identity<Args>...)
        {
                uint64_t id = ((get_mask<Args>()) | ...);
                SystemStore[index].emplace_back(
                    [this, id, func = std::forward(func)](
                        World& world) { world.invoke_system_with_id(id, func); });
        }

        template <typename Func> void invoke_system_with_id(uint64_t id, Func&& func)
        {
                using FuncType = decltype(&std::decay_t<Func>::operator()); // Get lambda type
                using ArgsTuple =
                    typename function_traits<FuncType>::args; // Extract argument types

                std::apply(
                    [this, id, &func](auto&&... args) {
                            this->invoke_system_with_id(
                                id, func, std::__type_identity<std::decay_t<decltype(args)>> {}...);
                    },
                    ArgsTuple {});
        }
        template <typename... Args>
        void invoke_system_with_id_impl(
            uint64_t id, std::function<void(Args&...)> func, std::__type_identity<Args>...)
        {

                auto& entity_ids = std::get<0>(ComponentStore);
                for (size_t i = 0; i < entity_ids.size(); ++i) {
                        if ((entity_ids[i] & id) == id) {
                                func(*std::get<std::vector<std::unique_ptr<Args>>>(
                                    ComponentStore)[i]...);
                        }
                }
        }

        //  uint64_t system_id = ((get_mask<Args>()) | ...);
        //  auto& entity_ids   = std::get<0>(ComponentStore);
        //  for (size_t i = 0; i < entity_ids.size(); ++i) {
        //          if ((entity_ids[i] & system_id) == system_id) {
        //                  func(*std::get<std::vector<std::unique_ptr<Args>>>(
        //                      ComponentStore)[i]...);
        //          }
        //  }
        //
        // Do not use
        // Will cause undefined behaviour if @Args are not in same order as Provided in @Types
        //



 `-----------------------------------------------------------------------------*/
