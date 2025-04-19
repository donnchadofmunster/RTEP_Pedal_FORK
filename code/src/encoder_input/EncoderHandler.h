#ifndef ENCINPUTHANDLER_H
#define ENCINPUTHANDLER_H

#include <iostream>
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <gpioevent.h>

#include "MCP23017Driver.h"

static MCP23017Driver MCP;

/**
 * @class Encoder Input Handler
 * @brief Set-up to go from an encoder interrupt to a change in the Pedal
 * 
 * NB  - Could wither be done with polling->iterrupt on change or fully with interrupts.
 * It is based on the MCP23017 I/O expander that has interrupt pins on change.
 */
class EncoderHandler : public GPIOPin::GPIOEventCallbackInterface  {
    public:
        /**
        * @brief Constructor, uses the driver's pointet.
        * @param driver right now this does nothing other than counter a warning that the driver isn't used
        */
        EncoderHandler(MCP23017Driver * driver);

        /**
        * @brief Begins the Handler for the encoder driver and functions.
        * @param uSec sets the microseconds time of the one-shot timer
        */
        void begin(long int uSec);

        /**
        * @brief triggers or disables the timer based on the uSec parameter.
        * @param uSec sets the microseconds time of the one-shot timer
        */
        void triggerTimer(long int uSec);

        /**
        * @brief Event handler for when the interrupt triggers.
        * @param e the source of the event (rising, falling, or other)
        */
        virtual void hasEvent(gpiod_line_event& e) override;

        /**
        * @brief Interpret the new encoder action based on the previous state
        * @param enc_num selects which encoder did the action
        */
        static uint8_t encoderRight(uint8_t enc_num, bool enc_a, bool enc_b); 

        static void processEvent();

        //source values to send to UI handler Update()
        static constexpr uint8_t ENC_1 = 0;
        static constexpr uint8_t ENC_2 = 1;
        static constexpr uint8_t ENC_3 = 2;

        //action values to send to UI handler Update()
        static constexpr uint8_t ENC_LEFT = 0;
        static constexpr uint8_t ENC_RIGHT = 1;
        static constexpr uint8_t ENC_PUSH = 2;

        //to make sure the interpreter spits a known error value
        static constexpr uint8_t ENC_ERROR = 4;

    private:
        itimerval _timer1;
        long int _uSec;

};

/**
* @brief Begins the Handler for the encoder driver and functions.
* @param signum the signal source, which is expected to be SIGALRM (see Linux Doc.)
*/
void timerHandler(int signum);

#endif // ENCINPUTHANDLER_H