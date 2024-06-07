#pragma once

#include <AH/Settings/Warnings.hpp>
AH_DIAGNOSTIC_WERROR()

#include <Banks/BankAddresses.hpp>
#include <MIDI_Outputs/Bankable/Abstract/MIDIFilteredMotorisedFader.hpp>
#include <MIDI_Senders/PitchBendSender.hpp>
#include <MIDI_Senders/DigitalNoteSender.hpp>

BEGIN_CS_NAMESPACE

namespace Bankable {

/**
 * @brief   A class of MIDIOutputElement%s that read the analog input from a
 *          **potentiometer or fader**, and send out 14-bit MIDI **Pitch Bend** 
 *          events.
 * 
 * The analog input is filtered and hysteresis is applied for maximum
 * stability.  
 * The actual precision is "only" 10 bits, because this is the resolution of the
 * built-in ADC, and this is the default resolution used by the Mackie Control
 * Universal protocol.  
 * This version can be banked.
 *
 * @ingroup BankableMIDIOutputElements
 */
class PBMotorisedFader
    : public MIDIFilteredMotorisedFader<SingleAddress, PitchBendSender<10>, DigitalNoteSender> {
  public:
    /** 
     * @brief   Create a new Bankable PBMotorisedFader object with the given 
     *          analog pin and channel.
     * 
     * @param   config
     *          The bank configuration to use: the bank to add this element to,
     *          and whether to change the address, channel or cable number.
     * @param   faderIndex
     *          The analog input pin to read from.
     * @param   address
     *          The MIDI channel [CHANNEL_1, CHANNEL_16] and optional Cable
     *          Number [CABLE_1, CABLE_16].
     */
    PBMotorisedFader(OutputBankConfig<BankType::CHANGE_CHANNEL> config,
                    uint8_t faderIndex, MIDIChannelCable address, MIDIAddress touchAddress)
        : MIDIFilteredMotorisedFader{{config, address}, config, faderIndex, {config, touchAddress}, {}, {}} {}
};

} // namespace Bankable

END_CS_NAMESPACE

AH_DIAGNOSTIC_POP()
