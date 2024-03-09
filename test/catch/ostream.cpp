#include <catch2/catch_test_macros.hpp>

#include <embr/prometheus.h>
#include <estd/sstream.h>

using namespace embr;
using namespace embr::prometheus;

const char* r1 =
    R"(metric2_bucket{instance="abc", poop="def", le="0"} 0)" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="10"} 0)" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="20"} 2)" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="30"} 3)" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="+Inf"} 4)" HTTP_ENDL;

TEST_CASE("ostream")
{
    estd::layer1::ostringstream<512> out;
    const auto& str = out.rdbuf()->str();

    SECTION("basics")
    {
        Gauge<int> g;
        Histogram<int, int, 0, 10, 20, 30> h;
        internal::OutAssist<decltype(out)> oa(out);

        // +1 for +Inf
        int v1[5];

        h.observe(15);
        h.observe(20);
        h.observe(25);
        h.observe(35);

        g.add(23);

        oa.name("metric1");
        oa.label("instance", "1");
        oa.metric(g);

        REQUIRE(str == "metric1{instance=\"1\"} 23");

        out.rdbuf()->clear();

        const char* label_names[] { "instance", "poop" };
        OutAssist2<decltype(out), const char*, const char*> oa2(out, "metric2",
            label_names,
            "abc", "def");

        oa2.metric(h);

        REQUIRE(str == r1);

        out.rdbuf()->clear();

        out << put_metric(g, "metric3");

        REQUIRE(str == "# TYPE metric3 gauge\r\nmetric3 23\r\n");
    }
    SECTION("labels")
    {
        const char* names[] { "val1", "val2" };
        Labels<int, const char*> labels(names, 0, "Hello");
        write(out, labels);

        REQUIRE(str == "val1=\"0\", val2=\"Hello\"");
    }
}
