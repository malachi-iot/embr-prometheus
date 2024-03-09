#include <catch2/catch_test_macros.hpp>

#include <embr/prometheus.h>
#include <embr/cbor/cbor.h>
#include <estd/sstream.h>

using namespace embr;
using namespace embr::prometheus;

TEST_CASE("histogram")
{

}


TEST_CASE("cbor")
{
    estd::layer1::istringstream<128> in("hi");
    auto rdbuf = in.rdbuf();

    cbor::header::XHeader<unsigned> xh;

    xh.decode_from_streambuf(*rdbuf);
}
