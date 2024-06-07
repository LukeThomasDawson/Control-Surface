#pragma once

#include <MIDI_Outputs/Abstract/MIDITouchWheel.hpp>
#include <MIDI_Senders/RelativeCCSender.hpp>
#include <MIDI_Senders/DigitalNoteSender.hpp>

BEGIN_CS_NAMESPACE

/**
 * @brief   A class of MIDIOutputElement%s that read the input of a **quadrature
 *          (rotary) encoder** and send out relative MIDI **Control Change**
 *          events.
 * 
 * This version cannot be banked.
 *
 * @ingroup MIDIOutputElements
 */
class CCTouchWheel : public MIDITouchWheel<RelativeCCSender, DigitalNoteSender> {
  public:

    CCTouchWheel(MIDIAddress address)
        : MIDITouchWheel<RelativeCCSender, DigitalNoteSender>(address, {}, {}) {}
};


END_CS_NAMESPACE