#include "unity.h"
#include "unity_test_runner.h"

extern "C" void app_main(void)
{
    // DEBT: Deviation from pytest approach.  Apparently that one automatically runs these
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();

    unity_run_menu();
}
