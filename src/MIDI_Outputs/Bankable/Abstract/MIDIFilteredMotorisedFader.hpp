#include <stdint.h>
#include <algorithm>
#pragma once

#include <AH/Hardware/FilteredMotorisedFader.hpp>
#include <Banks/BankableAddresses.hpp>
#include <Def/Def.hpp>
#include <MIDI_Outputs/Abstract/MIDIOutputElement.hpp>

BEGIN_CS_NAMESPACE

namespace Bankable {

/**
 * @brief   A class for potentiometers and faders that send MIDI events
 *          and can be added to a Bank.
 *
 * The analog input is filtered and hysteresis is applied.
 *
 * @see     FilteredAnalog
 */
template <class BankAddress, class Sender, class CCSender>
class MIDIFilteredMotorisedFader : public MIDIOutputElement {
  protected:
    /**
     * @brief   Construct a new MIDIFilteredAnalog.
     *
     * @param   bankAddress
     *          The bankable MIDI address to send to.
     * @param   faderIndex
     *          The analog input pin with the wiper of the potentiometer
     *          connected.
     * @param   sender
     *          The MIDI sender to use.
     */
    MIDIFilteredMotorisedFader(BankAddress bankAddress, OutputBankConfig<BankType::CHANGE_CHANNEL> bank, uint8_t faderIndex,
                               BankAddress touchAddress, const Sender &sender, const CCSender &ccSender)
        : address(bankAddress), bank(bank), filteredMotorisedFader(faderIndex), touchAddress(touchAddress), faderIndex(faderIndex), sender(sender), ccSender(ccSender) {}

  public:
    void begin() final override { filteredMotorisedFader.resetToCurrentValue(); }

    void update() final override {

        // Touch
        bool touched = filteredMotorisedFader.getTouched();
        // Serial.print(faderIndex);
        // Serial.print(" ");
        // Serial.println(touched);

        if (touched != prevTouched) {
            if (touched)
                ccSender.sendOn(touchAddress.getActiveAddress().getAddress() + bank.bank.getOffset());
                
            // touched ? ccSender.sendOn(touchAddress.getActiveAddress().getAddress() + bank.bank.getOffset())
            //         : ccSender.sendOff(touchAddress.getActiveAddress().getAddress() + bank.bank.getOffset());
            prevTouched = touched;

            if(!touched) {

                // We have just come off the fader being touched, let's grab the latest value and update the setpoint
//                MotorisedFaderInterfaceMaster.sendDAWFaderToSlave(faderIndex, filteredMotorisedFader.getValue());
            }
        }

        if (prevTouched && filteredMotorisedFader.update())
            forcedUpdate();
    }

    /// Send the value of the analog input over MIDI, even if the value didn't
    /// change.
    void forcedUpdate() {

        // Send Fader Position

       Serial.print("Fad #");
      Serial.print(faderIndex);
       Serial.print(" = ");
       Serial.print(filteredMotorisedFader.getValue());
        sender.send(filteredMotorisedFader.getValue(), address.getActiveAddress());
//        MotorisedFaderInterfaceMaster.sendDAWFaderToSlave(faderIndex, filteredMotorisedFader.getValue() >> 4);
    }

    /**
     * @brief   Specify a mapping function that is applied to the raw
     *          analog value before sending.
     *
     * @param   fn
     *          A function pointer to the mapping function. This function
     *          should take the filtered analog value of @f$ 16 - 
     *          \mathrm{ANALOG\_FILTER\_SHIFT\_FACTOR} @f$ bits as a parameter, 
     *          and should return a value in the same range.
     * 
     * @see     filteredMotorisedFader::map     
     */
    void map(MappingFunction fn) { filteredMotorisedFader.map(fn); }

    /// Invert the analog value.
    void invert() { filteredMotorisedFader.invert(); }

    /**
     * @brief   Get the raw value of the analog input (this is the value 
     *          without applying the filter or the mapping function first).
     */
    uint16_t getRawValue() const { return filteredMotorisedFader.getRawValue(); }

    /**
     * @brief   Get the value of the analog input (this is the value after first
     *          applying the mapping function).
     */
    uint16_t getValue() const { return filteredMotorisedFader.getValue(); }

  protected:
    BankAddress address;
    OutputBankConfig<BankType::CHANGE_CHANNEL> bank;
    AH::FilteredMotorisedFader<Sender::precision()> filteredMotorisedFader;
    BankAddress touchAddress;
    bool prevTouched = false;
    int faderIndex;

  public:
    Sender sender;
    CCSender ccSender;
};

} // namespace Bankable

END_CS_NAMESPACE




//template<uint8_t Idx>
//void sendMIDIMessages() {
//
//
//}
//
//void updateMIDISetpoint(ChannelMessage msg) {
//    auto type = msg.getMessageType();
//    auto channel = msg.getChannel().getRaw();
//    if (type == MIDIMessageType::PITCH_BEND && channel < Config::num_faders)
//        references[channel].setMasterSetpoint(msg.getData14bit() >> 4);
//}