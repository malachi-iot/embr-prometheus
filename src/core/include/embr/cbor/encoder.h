#pragma once

#include "header.h"

namespace embr { namespace cbor {


enum EncoderStates
{
    ENCODER_HEADER,
};

template <class StreambufImpl>
class StatefulEncoder
{
    EncoderStates state_;

public:
};

class Encoder
{

};

}}