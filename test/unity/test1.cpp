#include "unity.h"
#include "unity_test_runner.h"

#include <embr/prometheus.h>

TEST_CASE("test1", "[synthetic]")
{
    embr::prometheus::synthetic();
}