// tests/test.cpp
// R9 - Unit tests for the stochastic simulation library using proper doctest

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "../include/Stochastic/stochastic.h"
#include <fstream>
#include <algorithm>

using namespace Stochastic;

// R9 - Test symbol table functionality
TEST_CASE("SymbolTable functionality")
{
    SymbolTable<std::string, int> table;

    SUBCASE("Adding and retrieving symbols")
    {
        table.add("test1", 42);
        table.add("test2", 100);

        REQUIRE(table.get("test1") == 42);
        REQUIRE(table.get("test2") == 100);
        REQUIRE(table.contains("test1"));
        REQUIRE_FALSE(table.contains("nonexistent"));
    }

    SUBCASE("Duplicate symbol detection")
    {
        table.add("test1", 42);
        REQUIRE_THROWS_AS(table.add("test1", 50), SymbolTableException);
    }

    SUBCASE("Nonexistent symbol access")
    {
        REQUIRE_THROWS_AS(table.get("nonexistent"), SymbolTableException);
    }

    SUBCASE("Symbol update")
    {
        table.add("test1", 42);
        table.update("test1", 99);
        REQUIRE(table.get("test1") == 99);

        REQUIRE_THROWS_AS(table.update("nonexistent", 50), SymbolTableException);
    }

    SUBCASE("Keys retrieval")
    {
        table.add("key1", 1);
        table.add("key2", 2);
        auto keys = table.keys();
        REQUIRE(keys.size() == 2);
        REQUIRE(std::find(keys.begin(), keys.end(), "key1") != keys.end());
        REQUIRE(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    }
}

// R9 - Test species functionality
TEST_CASE("Species functionality")
{
    Species s1("A", 10, 0);
    Species s2("B", 5, 1);

    SUBCASE("Basic properties")
    {
        REQUIRE(s1.name() == "A");
        REQUIRE(s1.count() == 10);
        REQUIRE(s1.id() == 0);

        REQUIRE(s2.name() == "B");
        REQUIRE(s2.count() == 5);
        REQUIRE(s2.id() == 1);
    }

    SUBCASE("Count manipulation")
    {
        s1.increment_count(5);
        REQUIRE(s1.count() == 15);

        s1.decrement_count(3);
        REQUIRE(s1.count() == 12);

        s1.set_count(20);
        REQUIRE(s1.count() == 20);
    }

    SUBCASE("Underflow protection")
    {
        REQUIRE_THROWS_AS(s2.decrement_count(10), SimulationException);
        // Count should remain unchanged after failed decrement
        REQUIRE(s2.count() == 5);
    }

    SUBCASE("Dereference operators")
    {
        const Species &ref1 = *s1;
        Species &ref2 = *s1;
        REQUIRE(ref1.name() == "A");
        REQUIRE(ref2.name() == "A");
    }
}

// R9 - Test reaction building
TEST_CASE("ReactionBuilder functionality")
{
    Species A("A", 10, 0);
    Species B("B", 5, 1);
    Species C("C", 0, 2);

    SUBCASE("Simple reaction: A >> rate >>= B")
    {
        ReactionBuilder builder = A >> 2.0 >>= B;

        REQUIRE(builder.has_rate());
        REQUIRE(builder.rate() == doctest::Approx(2.0));
        REQUIRE(builder.reactants().size() == 1);
        REQUIRE(builder.products().size() == 1);
        REQUIRE(builder.reactants()[0].first == 0); // A's ID
        REQUIRE(builder.products()[0].first == 1);  // B's ID
    }

    SUBCASE("Complex reaction: A + B >> rate >>= C")
    {
        ReactionBuilder builder = (A + B) >> 3.5 >>= C;

        REQUIRE(builder.has_rate());
        REQUIRE(builder.rate() == doctest::Approx(3.5));
        REQUIRE(builder.reactants().size() == 2);
        REQUIRE(builder.products().size() == 1);
    }

    SUBCASE("Multiple products")
    {
        ReactionBuilder products = B + C;
        ReactionBuilder builder = A >> 1.0 >>= products;

        REQUIRE(builder.has_rate());
        REQUIRE(builder.rate() == doctest::Approx(1.0));
        REQUIRE(builder.reactants().size() == 1);
        REQUIRE(builder.products().size() == 2);
    }

    SUBCASE("No rate specified")
    {
        ReactionBuilder builder = A >>= B;
        REQUIRE_FALSE(builder.has_rate());
    }
}

// R9 - Test vessel functionality
TEST_CASE("Vessel functionality")
{
    Vessel v("Test Vessel");

    SUBCASE("Species management")
    {
        auto A = v.add("A", 100);
        auto B = v.add("B", 50);
        auto C = v.add("C", 0);

        REQUIRE(A->count() == 100);
        REQUIRE(B->count() == 50);
        REQUIRE(C->count() == 0);

        // Test species retrieval
        auto retrieved_A = v.get_species("A");
        REQUIRE(retrieved_A->name() == "A");
        REQUIRE(retrieved_A->count() == 100);

        // Test environment
        auto env = v.environment();
        REQUIRE(env->name() == "environment");
    }

    SUBCASE("Duplicate species detection")
    {
        v.add("A", 100);
        REQUIRE_THROWS_AS(v.add("A", 10), SymbolTableException);
    }

    SUBCASE("Reaction addition")
    {
        auto A = v.add("A", 100);
        auto B = v.add("B", 0);

        // This should not throw
        REQUIRE_NOTHROW(v.add(*A >> 1.0 >>= *B));

        // Reaction without rate should throw
        REQUIRE_THROWS_AS(v.add(*A >>= *B), SimulationException);
    }

    SUBCASE("String representations")
    {
        auto A = v.add("A", 100);
        auto B = v.add("B", 50);
        v.add(*A >> 1.0 >>= *B);

        std::string desc = v.to_string();
        std::string dot = v.to_dot();

        REQUIRE_FALSE(desc.empty());
        REQUIRE_FALSE(dot.empty());
        REQUIRE(dot.find("digraph") != std::string::npos);
        REQUIRE(desc.find("Test Vessel") != std::string::npos);
    }
}

// R9 - Test simple simulation
TEST_CASE("Simple simulation")
{
    Vessel v("Simple Test");
    auto A = v.add("A", 100);
    auto B = v.add("B", 0);
    auto env = v.environment();

    SUBCASE("Decay simulation")
    {
        // Simple decay: A >> 0.1 >>= env
        v.add(*A >> 0.1 >>= *env);

        size_t initial_A = A->count();

        try
        {
            v.simulate(10.0);
        }
        catch (const SimulationException &e)
        {
            // Simulation may end early if all A is consumed
            MESSAGE("Simulation ended early: " << e.what());
        }

        // A should decrease over time (unless simulation ended very early)
        CHECK(A->count() <= initial_A);
    }

    SUBCASE("Conversion simulation")
    {
        // Conversion: A >> 1.0 >>= B
        v.add(*A >> 1.0 >>= *B);

        size_t initial_total = A->count() + B->count();

        try
        {
            v.simulate(5.0);
        }
        catch (const SimulationException &e)
        {
            MESSAGE("Simulation ended early: " << e.what());
        }

        // Total should be conserved
        CHECK(A->count() + B->count() == initial_total);
        // Some conversion should have occurred
        CHECK(B->count() > 0);
    }
}

// R9 - Test observer functionality
TEST_CASE("Observer functionality")
{
    Vessel v("Observer Test");
    auto A = v.add("A", 100);
    auto B = v.add("B", 0);
    v.add(*A >> 1.0 >>= *B);

    SUBCASE("TrajectoryObserver")
    {
        TrajectoryObserver traj_obs;

        try
        {
            Simulator::runSimulation(v, 5.0, &traj_obs);
        }
        catch (const SimulationException &e)
        {
            MESSAGE("Simulation ended early: " << e.what());
        }

        const auto &trajectory = traj_obs.getTrajectory();
        REQUIRE_FALSE(trajectory.empty());

        // Check that we have time points
        REQUIRE(trajectory.begin()->first >= 0.0);

        // Check that species are recorded
        if (!trajectory.empty())
        {
            const auto &first_snapshot = trajectory.begin()->second;
            REQUIRE(first_snapshot.find("A") != first_snapshot.end());
            REQUIRE(first_snapshot.find("B") != first_snapshot.end());
        }
    }

    SUBCASE("PeakObserver")
    {
        PeakObserver peak_obs("B");
        Vessel v_copy = v; // Copy for independent simulation

        try
        {
            Simulator::runSimulation(v_copy, 5.0, &peak_obs);
        }
        catch (const SimulationException &e)
        {
            MESSAGE("Simulation ended early: " << e.what());
        }

        auto [peak_time, peak_value] = peak_obs.getPeak();
        REQUIRE(peak_time >= 0.0);
        // Peak value might be 0 if simulation ended immediately
        REQUIRE(peak_value >= 0);
    }

    SUBCASE("TrajectoryObserver file save")
    {
        TrajectoryObserver traj_obs;

        // Add some manual observations for testing
        std::vector<Species *> species_ptrs = {A.get(), B.get()};
        traj_obs.observe(0.0, species_ptrs);
        traj_obs.observe(1.0, species_ptrs);

        REQUIRE_NOTHROW(traj_obs.saveToFile("test_trajectory.csv"));

        // Check file exists and has content
        std::ifstream file("test_trajectory.csv");
        REQUIRE(file.is_open());

        std::string line;
        std::getline(file, line);
        REQUIRE(line.find("time") != std::string::npos);
        REQUIRE(line.find("A") != std::string::npos);
        REQUIRE(line.find("B") != std::string::npos);

        file.close();
    }
}

// R9 - Test SEIHR model specifically
TEST_CASE("SEIHR model validation")
{
    const size_t N = 1000;
    Vessel model = seir(N);

    SUBCASE("Initial conditions")
    {
        auto S = model.get_species("S");
        auto E = model.get_species("E");
        auto I = model.get_species("I");
        auto H = model.get_species("H");
        auto R = model.get_species("R");

        // Check that species exist
        REQUIRE(S != nullptr);
        REQUIRE(E != nullptr);
        REQUIRE(I != nullptr);
        REQUIRE(H != nullptr);
        REQUIRE(R != nullptr);

        // Check initial population sums to N (approximately)
        size_t total_pop = S->count() + E->count() + I->count() + H->count() + R->count();
        REQUIRE(total_pop == N);

        // Initial conditions: some S, E, I; no H, R
        REQUIRE(S->count() > 0);
        REQUIRE(E->count() > 0);
        REQUIRE(I->count() > 0);
        REQUIRE(H->count() == 0);
        REQUIRE(R->count() == 0);
    }

    SUBCASE("Short simulation run")
    {
        auto H = model.get_species("H");
        size_t initial_H = H->count();

        try
        {
            model.simulate(5.0); // Very short simulation
        }
        catch (const SimulationException &e)
        {
            MESSAGE("Simulation ended early: " << e.what());
        }

        // Hospitalized count should not decrease (only I->H and H->R transitions)
        CHECK(H->count() >= initial_H);
    }
}

// R9 - Test circadian rhythm model
TEST_CASE("Circadian rhythm model validation")
{
    Vessel model = circadian_rhythm();

    SUBCASE("Initial conditions")
    {
        auto DA = model.get_species("DA");
        auto DR = model.get_species("DR");
        auto MA = model.get_species("MA");
        auto A = model.get_species("A");

        REQUIRE(DA != nullptr);
        REQUIRE(DR != nullptr);
        REQUIRE(MA != nullptr);
        REQUIRE(A != nullptr);

        // Initial: DNA sites available, no mRNA or proteins
        REQUIRE(DA->count() == 1);
        REQUIRE(DR->count() == 1);
        REQUIRE(MA->count() == 0);
        REQUIRE(A->count() == 0);
    }

    SUBCASE("Model structure")
    {
        std::string desc = model.to_string();
        REQUIRE(desc.find("Circadian Rhythm") != std::string::npos);

        std::string dot = model.to_dot();
        REQUIRE(dot.find("digraph") != std::string::npos);
    }
}

// R9 - Additional edge case tests
TEST_CASE("Edge cases and error handling")
{
    SUBCASE("Empty vessel simulation")
    {
        Vessel empty_vessel("Empty");

        // Should handle empty vessel gracefully
        REQUIRE_NOTHROW(empty_vessel.simulate(1.0));
    }

    SUBCASE("Zero rate reactions")
    {
        Vessel v("Zero Rate Test");
        auto A = v.add("A", 100);
        auto B = v.add("B", 0);

        // Zero rate should still be valid
        REQUIRE_NOTHROW(v.add(*A >> 0.0 >>= *B));
    }

    SUBCASE("Large population handling")
    {
        Vessel v("Large Pop Test");
        auto A = v.add("A", 1000000);
        auto B = v.add("B", 0);

        REQUIRE(A->count() == 1000000);
        REQUIRE_NOTHROW(v.add(*A >> 0.001 >>= *B));
    }
}

// R9 - Performance and stress tests
TEST_CASE("Performance tests")
{
    SUBCASE("Many species vessel")
    {
        Vessel v("Many Species");

        // Add many species
        for (int i = 0; i < 100; ++i)
        {
            std::string name = "Species_" + std::to_string(i);
            v.add(name, i + 1);
        }

        // Verify all species exist
        for (int i = 0; i < 100; ++i)
        {
            std::string name = "Species_" + std::to_string(i);
            auto species = v.get_species(name);
            REQUIRE(species != nullptr);
            REQUIRE(species->count() == static_cast<size_t>(i + 1));
        }
    }

    SUBCASE("Complex reaction network")
    {
        Vessel v("Complex Network");
        auto A = v.add("A", 100);
        auto B = v.add("B", 50);
        auto C = v.add("C", 25);
        auto D = v.add("D", 0);
        auto E = v.add("E", 0);

        // Create a complex reaction network
        v.add(*A >> 1.0 >>= *B);
        v.add(*B >> 0.5 >>= *C);
        v.add(*C >> 2.0 >>= *D);
        v.add((*A + *B) >> 0.1 >>= *E);
        v.add(*D >> 0.8 >>= *A); // Feedback loop

        // Should handle complex networks
        REQUIRE_NOTHROW(v.simulate(1.0));
    }
}