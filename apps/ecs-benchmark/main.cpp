// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef ENABLE_CU_PROFILE
#define ENABLE_CU_PROFILE
#endif

#include <qce/ecs/ecs.hpp>
#include <cu/profile-utils.hpp>

#include <cstdlib>
#include <utility>

using namespace QCE;

// utils
bool random_bool() {
    return (std::rand() % 2) == 0;
}
entity_id_t random_id(entity_id_t max_id) {
    return static_cast<entity_id_t>(std::rand()) % max_id;
}

// components simulation
template<std::size_t N> struct C {};
static constexpr size_t COMPONENTS_COUNT = 64;

template<size_t CacheLimit, typename Seq> struct MakeEntityManager;
template<size_t CacheLimit, std::size_t... I>
struct MakeEntityManager<CacheLimit, std::index_sequence<I...>> {
    using type = EntityManager<CacheLimit, C<I>...>;
};

template <size_t CacheLimit>
using EntityManagerT = MakeEntityManager<CacheLimit, std::make_index_sequence<COMPONENTS_COUNT>>::type;

template <size_t CacheLimit, size_t... I>
void add_random_components(
        EntityManagerT<CacheLimit>& ecm,
        entity_id_t entity_id,
        std::index_sequence<I...>) {
    (
        (
            random_bool() ?
            ecm.template AddComponent<C<I>>(entity_id, C<I>{}) :
            ErrorCode::SUCCESS
        ),
        ...
    );
}

template <size_t CacheLimit, size_t... I>
std::set<entity_id_t> run_query(
        EntityManagerT<CacheLimit>& ecm,
        std::index_sequence<I...>) {
    return ecm.template QueryEntities<C<I>...>();
}

static constexpr size_t ENTITIES_COUNT = 32768;
static constexpr size_t QUERY_REPEATS = 1000;

template <size_t InvalidatePeriod, size_t CacheLimit, size_t QuerySize>
std::string GenerateTestID() {
    std::string test_id = "";
    if constexpr (!InvalidatePeriod)
        test_id += "Static ";
    else
        test_id += "Dynamic_" + std::to_string(InvalidatePeriod) + " ";

    if constexpr (CacheLimit == DISABLE_ENTITY_QUERY_CACHE)
        test_id += "NoCache ";
    else if constexpr (CacheLimit == INFINITE_ENTITY_QUERY_CACHE)
        test_id += "InfiniteCache ";
    else
        test_id += "CacheLimit_" + std::to_string(CacheLimit) + " ";

    test_id += "Components_" + std::to_string(QuerySize);
    return test_id;
}

template <size_t CacheLimit, size_t InvalidatePeriod>
void query_performance_benchmark() {
    std::srand(777);

    std::set<int> empty_results{};

    EntityManagerT<CacheLimit> ecm;

    for (size_t i = 0; i < ENTITIES_COUNT; ++i) {
        auto entity_id = ecm.AddEntity();
        add_random_components<CacheLimit>(ecm, entity_id, std::make_index_sequence<COMPONENTS_COUNT>{});
    }

    auto test_id_2 =  GenerateTestID<InvalidatePeriod, CacheLimit, 2>();
    auto test_id_4 =  GenerateTestID<InvalidatePeriod, CacheLimit, 4>();
    auto test_id_8 =  GenerateTestID<InvalidatePeriod, CacheLimit, 8>();
    auto test_id_64 = GenerateTestID<InvalidatePeriod, CacheLimit, 64>();

    for (size_t i = 0; i < QUERY_REPEATS; ++i) {
        {
            CU_PROFILE_CHECKBLOCK(Components_2, test_id_2);
            auto res = run_query<CacheLimit>(ecm, std::make_index_sequence<2>{});
            if (res.size() == 0) {
                empty_results.insert(2);
            }
        }
        {
            CU_PROFILE_CHECKBLOCK(Components_4, test_id_4);
            auto res = run_query<CacheLimit>(ecm, std::make_index_sequence<4>{});
            if (res.size() == 0) {
                empty_results.insert(4);
            }
        }
        {
            CU_PROFILE_CHECKBLOCK(Components_8, test_id_8);
            auto res = run_query<CacheLimit>(ecm, std::make_index_sequence<8>{});
            if (res.size() == 0) {
                empty_results.insert(8);
            }
        }
        {
            CU_PROFILE_CHECKBLOCK(Components_64, test_id_64);
            auto res = run_query<CacheLimit>(ecm, std::make_index_sequence<64>{});
            if (res.size() == 0) {
                empty_results.insert(64);
            }
        }


        if (InvalidatePeriod && i % InvalidatePeriod == 0) {
            // invalidate cache
            auto entity_id = random_id(ENTITIES_COUNT);
            QCE_THROW_CRITICAL(ecm.RemoveEntity(entity_id));
            entity_id = ecm.AddEntity();
            add_random_components<CacheLimit>(ecm, entity_id, std::make_index_sequence<COMPONENTS_COUNT>{});
        }
    }

    std::cout << "Empty queries: ";
    for (const auto& empty_results_value : empty_results) {
        std::cout << empty_results_value << " ";
    }
    std::cout << std::endl;

    auto bench_res = CU_PROFILE_GET_RESULTS(
        std::vector{ test_id_2, test_id_4, test_id_8, test_id_64 }
    );

    for (const auto& [res_id, timer_result] : bench_res) {
        std::cout << res_id << ":" << std::endl;
        std::cout << timer_result << std::endl;
    }

    std::cout << "===================================================================" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    query_performance_benchmark<DISABLE_ENTITY_QUERY_CACHE, 0>();
    query_performance_benchmark<64, 0>();
    query_performance_benchmark<INFINITE_ENTITY_QUERY_CACHE, 0>();

    query_performance_benchmark<DISABLE_ENTITY_QUERY_CACHE, 30>();
    query_performance_benchmark<64, 30>();
    query_performance_benchmark<INFINITE_ENTITY_QUERY_CACHE, 30>();

    query_performance_benchmark<DISABLE_ENTITY_QUERY_CACHE, 1>();
    query_performance_benchmark<64, 1>();
    query_performance_benchmark<INFINITE_ENTITY_QUERY_CACHE, 1>();

    return 0;
}