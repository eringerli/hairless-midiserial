#pragma once

#include <QMetaType>
#include <QString>
#include <cstdint>
#include <vector>

using MidiMsg = std::vector< unsigned char >;

Q_DECLARE_METATYPE( MidiMsg );

constexpr const char* NAME_MIDI_IN  = "MIDI->Serial";
constexpr const char* NAME_MIDI_OUT = "Serial->MIDI";

constexpr const char* TEXT_NOT_CONNECTED = "(Not Connected)";
constexpr const char* TEXT_NEW_PORT      = "(Create new port)";
