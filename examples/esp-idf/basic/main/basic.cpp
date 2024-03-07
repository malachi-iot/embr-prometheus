#include <stdio.h>

#include <embr/prometheus.h>
#include <embr/posix/streambuf.h>

extern "C" void app_main(void)
{
    embr::prometheus::posix_ostream out(0);

    out << "Hello" << estd::endl;
}
