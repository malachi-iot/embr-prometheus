#include <catch2/catch_test_macros.hpp>

#include <embr/prometheus.h>
#include <estd/sstream.h>

using namespace embr;
using namespace embr::prometheus;

const char* r1 =
    "# HELP metric2 helper text" HTTP_ENDL
    "# TYPE metric2 histogram" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="0"} 0)" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="10"} 0)" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="20"} 2)" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="30"} 3)" HTTP_ENDL
    R"(metric2_bucket{instance="abc", poop="def", le="+Inf"} 4)" HTTP_ENDL
    R"(metric2_sum{instance="abc", poop="def"} 95)" HTTP_ENDL
    R"(metric2_count{instance="abc", poop="def"} 4)" HTTP_ENDL;

const char* r2 =
    "# TYPE metric3 gauge" HTTP_ENDL
    R"(metric3{instance="1", poop="5"} 23)" HTTP_ENDL;

TEST_CASE("ostream")
{
    estd::layer1::ostringstream<512> out;
    const auto& str = out.rdbuf()->str();

    // DEBT: Clunky
    const char* labels[] = { "instance", "vendor" };
    embr::prometheus::label_names = labels;

    SECTION("basics")
    {
        Gauge<int> g;
        Histogram<int, int, 0, 10, 20, 30> h;
        internal::OutAssist oa(out);

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

        oa2.metric(h, "helper text");

        REQUIRE(str == r1);

        out.rdbuf()->clear();

        Labels l(label_names, 1, 5);
        out << put_metric(g, "metric3", nullptr, l);

        REQUIRE(str == r2);
    }
    SECTION("raw metric_put")
    {
        SECTION("gauge")
        {
            Gauge<int> g;

            Labels<int, const char*> l(label_names, 1, "hello");

            internal::metric_put<Gauge<int>, int, const char*>
                mp1(g, "metric1", "help", l);

            mp1(out);
        }
    }
    SECTION("labels")
    {
        const char* names[] { "val1", "val2" };
        Labels<int, const char*> labels(names, 0, "Hello");
        write(out, labels);

        REQUIRE(str == "val1=\"0\", val2=\"Hello\"");
    }
}
