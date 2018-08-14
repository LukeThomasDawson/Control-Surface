#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <Settings/SettingsWrapper.h>

const uint8_t NOTE_OFF = 0x80;
const uint8_t NOTE_ON = 0x90;
const uint8_t KEY_PRESSURE = 0xA0;
const uint8_t CC = 0xB0, CONTROL_CHANGE = CC;
const uint8_t PROGRAM_CHANGE = 0xC0;
const uint8_t CHANNEL_PRESSURE = 0xD0;
const uint8_t PITCH_BEND = 0xE0;

const uint8_t SysExStart = 0xF0;
const uint8_t SysExEnd = 0xF7;

const uint8_t TuneRequest = 0xF6;

struct MIDI_message {
    uint8_t header;
    uint8_t data1;
    uint8_t data2;

    bool operator==(const MIDI_message &other) const {
        return this->header == other.header && this->data1 == other.data1 &&
               this->data2 == other.data2;
    }

    bool operator!=(const MIDI_message &other) const {
        return !(*this == other);
    }
};

enum MIDI_read_t {
    NO_MESSAGE = 0,
    CHANNEL_MESSAGE = 1,
    SYSEX_MESSAGE = 2,
};

// --------------------------------------------------------------------------- //

class MIDI_Parser {
  public:
    /** Get the last MIDI channel message */
    MIDI_message getChannelMessage();
    /** Get the pointer to the SysEx data buffer. */
    const uint8_t *getSysExBuffer();
    /** Get the length of the last SysEx message. */
    size_t getSysExLength();

  protected:
    MIDI_message midimsg = {};

#ifndef IGNORE_SYSEX
    // Edit this in ../Settings/Settings.h
    constexpr static size_t bufferSize = SYSEX_BUFFER_SIZE;
    uint8_t SysExBuffer[bufferSize];
    size_t SysExLength = 0;
    bool receivingSysEx = false;

    /** Start a new SysEx message. */
    void startSysEx();
    /** Finish the current SysEx message. */
    void endSysEx();
    /** Add a byte to the current SysEx message. */
    bool addSysExByte(uint8_t data);
    /** Check if the buffer has at least 1 byte of free space available. */
    bool hasSpaceLeft();
#endif

  public:
    /** Check if the given byte is a MIDI header byte. */
    static bool isHeader(uint8_t data);
    /** Check if the given byte is a MIDI data byte. */
    static bool isData(uint8_t data);
};
