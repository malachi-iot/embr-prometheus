#include <catch2/catch_test_macros.hpp>

#include <embr/prometheus.h>
#include <embr/cbor/cbor.h>
#include <estd/sstream.h>

using namespace embr;
using namespace embr::prometheus;

TEST_CASE("histogram")
{
    Histogram<int, int, 0, 10, 20, 30> h;

    // +1 for +Inf
    int v1[5];

    h.observe(15);
    h.observe(20);
    h.observe(25);

    REQUIRE(h.buckets_[0] == 0);
    REQUIRE(h.buckets_[2] == 2);
    REQUIRE(h.buckets_[3] == 1);

    bool b = h.observe(35);

    REQUIRE(b == false);

    h.get(v1);

    REQUIRE(v1[1] == 0);
    REQUIRE(v1[2] == 2);
    REQUIRE(v1[3] == 3);
    REQUIRE(v1[4] == 4);
}


TEST_CASE("cbor")
{
    estd::layer1::istringstream<128> in("hi");
    auto rdbuf = in.rdbuf();

    cbor::header::XHeader<unsigned> xh;

    xh.decode_from_streambuf(*rdbuf);
}
