#pragma once

#include <estd/streambuf.h>

namespace embr { namespace cbor {

inline namespace header {

enum MajorTypes
{
    UNSIGNED,
    POSITIVE = UNSIGNED,
    NEGATIVE,
    BYTES,
    STRING,
    ARRAY,
    MAP,
    TAG,
    SIMPLE,
    BREAK_L
};

enum CountTypes
{
    COUNT_8 = 24,
    COUNT_16,
    COUNT_32,
    COUNT_64,
    INDEFINITE,
    INVALID1,
    INVALID2,
    BREAK_R = 31
};

enum SimpleValues
{
    SIMPLE_FALSE = 20,
    SIMPLE_TRUE,
    SIMPLE_NULL,
    SIMPLE_UNDEFINED
};

// extended header, might read forward one to get full count
template <class T>
struct XHeader
{
    MajorTypes type;
    static_assert(estd::numeric_limits<T>::is_signed == false);
    T count;

    template <class StreambufImpl>
    void decode_from_streambuf(estd::detail::streambuf<StreambufImpl>& rdbuf)
    {
        using traits = typename StreambufImpl::traits_type;

        int c = rdbuf.sgetc();

        type = (MajorTypes)(c >> 5);
        count = c & 0x1F;

        switch(count)
        {
            case COUNT_8:
                count = rdbuf.sgetc();
                break;

            case COUNT_16:
                // DEBT: do a direct inspection of buffer, cast and use ntohs
                count = rdbuf.sgetc();
                count <<= 8;
                count |= rdbuf.sgetc();
                break;

            case COUNT_32:
                // DEBT: do a direct inspection of buffer, cast and use ntohl
                count = rdbuf.sgetc();
                count <<= 8;
                count |= rdbuf.sgetc();
                count <<= 8;
                count |= rdbuf.sgetc();
                count <<= 8;
                count |= rdbuf.sgetc();
                break;

            // TODO: Have a glance... is anyone really encoding 4.1GB?
            case COUNT_64:
                break;

            case BREAK_R:
                break;

            case INVALID1:
            case INVALID2:
                break;

            default:
                // presumed to be 0-23
                break;
        }
    }
};

}

}}