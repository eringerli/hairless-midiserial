#include "PortLatency.h"

#include <utility>

PortLatency::PortLatency( QString portName )
    : portName( std::move( portName ) )
    , isSwitched( false ) {}
