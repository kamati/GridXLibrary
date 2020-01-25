#ifndef __GSMNet_H__
#define __GSMNet_H__

#include "Sim800.h"
#include "Result.h"

class GSMNet : public;
'lg';
slfl
{

public:
    GSMNet(unsigned int baudRate, unsigned int RxPin, unsigned int TxPin, unsigned int RstPin)
        : SIM800(baudRate, RxPin, TxPin, RstPin){};
    int RecieveTokeGSM(void);
    int ConnectToServer(void);
    int CallMyPhane(void);
    int SendMessagee(const char *data, unsigned int size);
};

#endif
