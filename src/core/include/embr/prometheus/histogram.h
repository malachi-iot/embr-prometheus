#pragma once

#include "../prometheus/fwd.h"
//#include "../cbor/cbor.h"

namespace embr { namespace prometheus {

// buckets MUST appear in ascending order
// TODO: Make specialized version without sum
// DEBT: I think c++17 lets us do an auto variadic here - if so, consider doing it
template <typename Counter, typename Bucket, Bucket... buckets>
class Histogram : metric_tag
{
#if UNIT_TESTING
public:
#endif
    // Do one extra for +Inf
    static constexpr unsigned bucket_count = sizeof...(buckets) + 1;

    Counter buckets_[bucket_count] {};
    Counter sum_ {};

public:
    void get(Counter out[bucket_count]) const
    {
        Counter prev {};

        for(int i = 0; i < bucket_count; i++)
        {
            prev += buckets_[i];
            out[i] = prev;
        }
    }

    void observe_idx(unsigned bucket_idx)
    {
        ++buckets_[bucket_idx];
    }

    bool observe(const Bucket& value)
    {
        // Lots of help from
        // https://www.foonathan.net/2020/05/fold-tricks/

        // "The +Inf bucket must always be present"

        unsigned i = 0;
        unsigned bucket;
        // DEBT: bucket_count -1 is a crude way of "catch all" for INF
        bool valid = (((i++ < (bucket_count - 1) && value <= buckets) && (bucket = i, true)) || ...);

        // DEBT: Quite crude
        if(valid)
            observe_idx(bucket - 1);
        else
            observe_idx(bucket_count - 1);

        sum_ += value;

        return valid;
    }

    void reset()
    {

    }
};



namespace layer1 {

}

}}
