#ifndef __SMS_H__
#define __SMS_H__

#include "Sim800.h"
#include "Result.h"

class SMS : public SIM800
{

public:
    SMS(unsigned int baudRate,
        unsigned int rxPin,
        unsigned int txPin,
        unsigned int rstPin) : SIM800(baudRate, rxPin, txPin, rstPin){};

    Result ReadSMS(int messageIndex, char *message, int length);
    int sendSMS(const char *number, char *data);
    Result deleteSMS(int index);
    Result callUp(char *number);
    Result answer(void);
    String getNetwork(void);

    /** initialize GPRS module including SIM card check & signal strength
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int init(void);
    /** check network is OK or not
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int checkSIMStatus(void);
};

#endif