#include "PortLatency.h"

#include <utility>

PortLatency::PortLatency( QString portName )
    : portName( portName )
    , isSwitched( false ) {}
