#include <catch2/catch_test_macros.hpp>

#include <embr/prometheus.h>
#include <estd/sstream.h>

using namespace embr::prometheus;

TEST_CASE("TEST")
{
    estd::layer1::ostringstream<512> out;
    const auto& str = out.rdbuf()->str();
    Gauge<int> g;
    Histogram<int, 0, 10, 20, 30> h;
    OutAssist<decltype(out)> oa(out);

    oa.name("metric1");

    h.observe(15);
    h.observe(21);
    h.observe(25);

    REQUIRE(h.buckets_[0] == 0);
    REQUIRE(h.buckets_[2] == 1);
    REQUIRE(h.buckets_[3] == 2);

    REQUIRE(h.observe(35) == false);

    g.add(23);

    oa.label("instance", "1");
    oa.metric(g);

    REQUIRE(str == "metric1{instance=\"1\"} 23");

    out.rdbuf()->clear();

    const char* label_names[] { "instance", "poop" };
    const char* label_values[] { "abc", "def" };
    OutAssist2<decltype(out), 2> oa2(out, "metric2", label_names, label_values);

    // Working pretty well, just hard to compare output
    oa2.metric(h);

    //REQUIRE(str == "metric2");
}