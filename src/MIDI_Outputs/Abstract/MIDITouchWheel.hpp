#pragma once

#include <Def/Def.hpp>
#include <Def/TypeTraits.hpp>
#include <MIDI_Outputs/Abstract/MIDIOutputElement.hpp>
#include <MIDI_Constants/MCU.hpp>
#include <mpr121.h>

#define TOUCH_PLAY_PAUSE  1
#define TOUCH_STOP        9
#define TOUCH_FORWARD     12
#define TOUCH_REWIND      4
#define TOUCH_MENU        20
#define PRESS_COUNT_THRESHOLD       5

AH_DIAGNOSTIC_WERROR()

BEGIN_CS_NAMESPACE

/**
 * @brief   An abstract class for a capacitve touch wheel (classic iPod style) 
 * 
 */
template <class RelativeSender, class DigitalSender>
class MIDITouchWheel : public MIDIOutputElement {
  public:
    /**
     * @brief   Construct a new MIDITouchWheel.
     *
     * @todo    Documentation
     */
    MIDITouchWheel(MIDIAddress address, const RelativeSender &relativeCCSender, const DigitalSender &digitalCCSender)
        : address(address), relativeCCSender(relativeCCSender), digitalCCSender(digitalCCSender) {}

    void begin() override {
        // any init that needs to happen here
        // Wire.begin();
        CapaTouch.begin();
    }

    void update() override {

        // read the current value
        int touchValue = CapaTouch.wheelKey();
        // Serial.println(touchValue);

        // are we touching the wheel?  
        if (touchValue > 0) {

            // Yes! we are touching!  Ok check if this is a first press, and if so just record things for now
            if (isFirstTouch) {
                firstTouchValue = touchValue;   // record the value
                startPressTime = millis();      // record the time
                pressCount++;                   // increment count
                isFirstTouch = false;
            }

            // Not a first touch value:
            else {

                // Is it the same value as what was first pressed?
                if (touchValue == firstTouchValue) {
                    pressCount++;

                    // If above threshold for first time, send it.  If allowed to resend again on same button hold, send it.
                    if (pressCount >= PRESS_COUNT_THRESHOLD && pressHoldAllowed) {
                        sendMidi(touchValue);
                        pressCount = 0;
                    }
                }

                // ... ok now let's check if the value has changed
                else if (touchValue != previousTouch) {

                    // a scroll has happened, so no more button presses
                    pressHoldAllowed = false;

                    if (isScrollingAllowed) {
                        
                         // filter out any accidental center button presses
                        if (previousTouch != TOUCH_MENU && touchValue != TOUCH_MENU) {
                            int delta = touchValue - previousTouch;

                            // account for positive and negative rollover when scrolling past origin
                            if (delta >= -15 && delta <= -10)
                                delta += 16;
                            else if (delta >= 10 && delta <= 15)
                                delta -= 16;

                            // fire off a relative CC value    
                            relativeCCSender.send(delta*2, address);
                        }
                    }
                }
                // hello daddy ha ha ha
            }

            previousTouch = touchValue;
        }

        // ... not touching wheel now 
        else {
            isFirstTouch = true;
            isScrolling = false;
            isFWDPressed = false;
            pressHoldAllowed = true;
            isScrollingAllowed = true;
            pressCount = 0;
        }
    }

    void sendMidi(uint8_t touchValue) {

        switch (touchValue) {

            case TOUCH_PLAY_PAUSE:
            case 16:
                if (isPlaying) {
                    digitalCCSender.sendOn(MCU::STOP);
                    digitalCCSender.sendOff(MCU::STOP);
                }
                else {
                    digitalCCSender.sendOn(MCU::SHIFT);
                    digitalCCSender.sendOn(MCU::PLAY);
                    digitalCCSender.sendOff(MCU::PLAY);
                    digitalCCSender.sendOff(MCU::SHIFT);
                }
                isPlaying = !isPlaying;
                pressHoldAllowed = false;
                isScrollingAllowed = false;
            break;

            case TOUCH_STOP:
            case 10:
                // Send it twice so Ableton moves to beginning of track
                digitalCCSender.sendOn(MCU::STOP);
                digitalCCSender.sendOff(MCU::STOP);
                // digitalCCSender.sendOn(MCU::STOP);
                // digitalCCSender.sendOff(MCU::STOP);
                isPlaying = false;
                pressHoldAllowed = false;
                isScrollingAllowed = false;
            break;

            case TOUCH_FORWARD:
            case 13:
                digitalCCSender.sendOn(MCU::FAST_FWD);
                digitalCCSender.sendOff(MCU::FAST_FWD);
                isFWDPressed = true;
                isScrollingAllowed = false;
            break;

            case TOUCH_REWIND:
            case 5:
            case 6:
                digitalCCSender.sendOn(MCU::REWIND);
                digitalCCSender.sendOff(MCU::REWIND);
                isFWDPressed = true;
                isScrollingAllowed = false;
            break;

            case TOUCH_MENU:
                digitalCCSender.sendOn(MCU::PLAY);
                pressHoldAllowed = false;
                isScrollingAllowed = false;
            break;
        }
    }

    /// Get the MIDI address.
    MIDIAddress getAddress() const { return this->address; }

    /// Set the MIDI address.
    void setAddress(MIDIAddress address) { this->address = address; }

  private:
    MIDIAddress address;
    uint8_t previousTouch = -1;
    bool isFirstTouch = true;
    bool isScrolling = false;
    bool isPlaying = false;
    bool isFWDPressed = false;
    unsigned long startPressTime;
    uint8_t pressCount = 0;
    uint8_t firstTouchValue;
    bool pressHoldAllowed = true;
    bool isScrollingAllowed = true;

  public:
    RelativeSender relativeCCSender;
    DigitalSender digitalCCSender;
};


END_CS_NAMESPACE

AH_DIAGNOSTIC_POP()






        //     // ... this is a sustained button press, likely means we are scrolling
        //     else {
                
        //         // First check if the value has changed, another indicator we are scrolling
        //         if (previousTouch != touchValue) {
        //             isScrolling = true;

        //             // filter out any accidental center button presses
        //             if (previousTouch != TOUCH_MENU && touchValue != TOUCH_MENU) {
        //                 int delta = touchValue - previousTouch;

        //                 // account for positive and negative rollover when scrolling past origin
        //                 if (delta >= -15 && delta <= -10)
        //                     delta += 16;
        //                 else if (delta >= 10 && delta <= 15)
        //                     delta -= 16;

        //                 // fire off a relative CC value    
        //                 relativeCCSender.send(delta*2, address);
        //             }

        //             previousTouch = touchValue;
        //         }
        //         // This is a sustained press on the same button
        //         else {
        //             if (isFWDPressed && !isScrolling) {
        //                 if (touchValue == TOUCH_FORWARD || touchValue == 13) {
        //                     digitalCCSender.sendOn(MCU::FAST_FWD);
        //                     digitalCCSender.sendOff(MCU::FAST_FWD);
        //                 }
        //                 else if (touchValue == TOUCH_REWIND || touchValue == 5 || touchValue == 6) {
        //                     digitalCCSender.sendOn(MCU::REWIND);
        //                     digitalCCSender.sendOff(MCU::REWIND);
        //                 }
        //             }
        //         }
        //     }
        // }