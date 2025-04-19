#include "EncoderHandler.h"

// constructor
EncoderHandler::EncoderHandler(MCP23017Driver * driver){}

// Enable driver, set the interrupts, and initialize the one-shot timer
void EncoderHandler::begin(long int uSec){
    MCP.begin();

	struct itimerval timer1;
	_timer1 = timer1;
	_uSec = uSec;

	//link the timer handler
	signal(SIGALRM,timerHandler);
}

// Handle the interrupt and reset the MCP23017
void EncoderHandler::triggerTimer(long int uSec){
	_timer1 . it_interval . tv_usec = 0;
	_timer1 . it_interval . tv_sec = 0;
	_timer1 . it_value . tv_usec = uSec;
	_timer1 . it_value . tv_sec = 0;

	setitimer(ITIMER_REAL, &_timer1, NULL);
}

// process the event
void EncoderHandler::processEvent(){
	uint8_t int_flag_a = MCP.getInterruptFlag(0);
	uint8_t int_flag_b = MCP.getInterruptFlag(1);

	uint8_t gpa_read = MCP.readPort(0);
	uint8_t gpb_read = MCP.readPort(1);

	uint8_t result = 0;

	if(int_flag_a==0)
	{
		switch(int_flag_b){
			case MCP23017Driver::ENC_PUSH[0]:
				if(gpb_read&MCP23017Driver::ENC_PUSH[0])
				{
					std::cout << "[EncHandler.cpp] button 1 pressed\n";
					//uiHandler::Update(EncoderHandler::ENC_1,EncoderHandler::ENC_PUSH);
				}
				break;
			case MCP23017Driver::ENC_PUSH[1]:
				if(gpb_read&MCP23017Driver::ENC_PUSH[1])
				{
					std::cout << "[EncHandler.cpp] button 2 pressed\n";
					//uiHandler::Update(EncoderHandler::ENC_2,EncoderHandler::ENC_PUSH);
				}
				break;
			case MCP23017Driver::ENC_PUSH[2]:
				if(gpb_read&MCP23017Driver::ENC_PUSH[2])
				{
					std::cout << "[EncHandler.cpp] button 3 pressed\n";
					//uiHandler::Update(EncoderHandler::ENC_3,EncoderHandler::ENC_PUSH);
				}
				break;
			default:
				break;
		}
	} else
	{
		switch(int_flag_a){
			case MCP23017Driver::ENC_A[0]:
				result = EncoderHandler::encoderRight(MCP23017Driver::ENC_NUM[0],gpa_read&MCP23017Driver::ENC_A[0],gpa_read&MCP23017Driver::ENC_B[0]);
				if (result == EncoderHandler::ENC_LEFT )
				{
					std::cout << "[EncHandler.cpp] encoder 1 turned left\n";
					//uiHandler::Update(EncoderHandler::ENC_1,EncoderHandler::ENC_LEFT);
				} else if (result == EncoderHandler::ENC_RIGHT)
				{
					std::cout << "[EncHandler.cpp] encoder 1 turned right\n";
					//uiHandler::Update(EncoderHandler::ENC_1,EncoderHandler::ENC_RIGHT);
				}
				break;
			case MCP23017Driver::ENC_A[1]:
				result = EncoderHandler::encoderRight(MCP23017Driver::ENC_NUM[1],gpa_read&MCP23017Driver::ENC_A[1],gpa_read&MCP23017Driver::ENC_B[1]);
				if (result == EncoderHandler::ENC_LEFT )
				{
					std::cout << "[EncHandler.cpp] encoder 2 turned left\n";
					//uiHandler::Update(EncoderHandler::ENC_2,EncoderHandler::ENC_LEFT);
				} else if (result == EncoderHandler::ENC_RIGHT)
				{
					std::cout << "[EncHandler.cpp] encoder 2 turned right\n";
					//uiHandler::Update(EncoderHandler::ENC_2,EncoderHandler::ENC_RIGHT);
				} else
				break;
			case MCP23017Driver::ENC_A[2]:
				result = EncoderHandler::encoderRight(MCP23017Driver::ENC_NUM[2],gpa_read&MCP23017Driver::ENC_A[2],gpa_read&MCP23017Driver::ENC_B[2]);
				if (result == EncoderHandler::ENC_LEFT )
				{
					std::cout << "[EncHandler.cpp] encoder 3 turned left\n";
					//uiHandler::Update(EncoderHandler::ENC_3,EncoderHandler::ENC_LEFT);
				} else if (result == EncoderHandler::ENC_RIGHT)
				{
					std::cout << "[EncHandler.cpp] encoder 3 turned right\n";
					//uiHandler::Update(EncoderHandler::ENC_3,EncoderHandler::ENC_RIGHT);
				} else
				break;
			default:
				break;
		}
	}
}

//figure out the encoder action
uint8_t EncoderHandler::encoderRight(uint8_t enc_num, bool enc_a, bool enc_b){
	if (enc_a & enc_b){return EncoderHandler::ENC_LEFT;}
	else if(enc_a){return EncoderHandler::ENC_RIGHT;}

	return EncoderHandler::ENC_ERROR;
}

// Handle the interrupt and reset the MCP23017
void EncoderHandler::hasEvent(gpiod_line_event& e){
    if(e.event_type==GPIOD_LINE_EVENT_FALLING_EDGE)
	{	
		this->processEvent();
		MCP.enableAllInterrupt(false);
		this->triggerTimer(_uSec);
	}
	else if(e.event_type==GPIOD_LINE_EVENT_RISING_EDGE)
	{
		this->triggerTimer(_uSec);
	}
}

// Figure out what caused the interrupt, and figure out what source and action that creates
void timerHandler(int signum){
	
	//re-enable the driver interrupt
	MCP.enableAllInterrupt(true);
}
