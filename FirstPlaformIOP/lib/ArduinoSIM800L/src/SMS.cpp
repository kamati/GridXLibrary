#include "SMS.h"
#include "Parser.h"

const char SMS_OPERATION_MODE[] PROGMEM = "AT+CMGF=1\r\n";
const char OK_ = "OK";

Result SMS::ReadSMS(int messageIndex, char *message, int length)
{
    int i = 0;
    char gprsBuffer[144]; // Buffer size for the SMS message
    char cmd[16];
    char *p, *s;

    sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", DEFAULT_TIMEOUT);
    delay(1000);
    sprintf(cmd, "AT+CMGR=%d\r\n", messageIndex);
    serialSIM800.write(cmd);
    cleanBuffer(gprsBuffer, 144);
    readBuffer(gprsBuffer, 144, DEFAULT_TIMEOUT);

    if (NULL != (s = strstr(gprsBuffer, "+CMGR")))
    {
        if (NULL != (s = strstr(gprsBuffer, "REC")))
        { // Search the beginning of the SMS message
            p = s - 1;
            while ((i < length - 1))
            {
                message[i++] = *(p++);
            }
            message[i] = '\0';
        }
    }
    return 0;
}

int SMS::sendSMS(const char *number, char *data)
{
    char cmd[32];
    if (0 != sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", DEFAULT_TIMEOUT))
    { // Set message mode to ASCII
        ERROR("ERROR:CMGF");
        return -1;
    }
    delay(500);
    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"%s\"\r\n", number);
    if (0 != sendCmdAndWaitForResp(cmd, ">", DEFAULT_TIMEOUT))
    {
        ERROR("ERROR:CMGS");
        return -1;
    }
    delay(1000);
    serialSIM800.write(data);
    delay(500);
    sendEndMark();
    ERROR("Message Send");
    return 0;
}

Result SMS::callUp(char *number)
{
    char cmd[24];
    if (0 != sendCmdAndWaitForResp("AT+COLP=1\r\n", "OK", 5))
    {
        ERROR("COLP");
        return -1;
    }
    delay(1000);
    sprintf(cmd, "\r\nATD%s;\r\n", number);
    serialSIM800.write(cmd);
    return 0;
}

Result SMS ::answer(void)
{
    serialSIM800.write("ATA\r\n");
    return 0;
}

int SMS::init(void)
{
#if 0
    for(int i = 0; i < 2; i++){
        sendCmd("AT\r\n");
        delay(100);
    }
    sendCmd("AT+CFUN=1\r\n");
    if(0 != checkSIMStatus()) {
       ERROR("ERROR:checkSIMStatus");
        return -1;
    }
    return 0;

#endif
    if (sendCmdAndWaitForResp("AT\r\n", "OK\r\n", DEFAULT_TIMEOUT * 3))
    {
        return -1;
    }
    if (sendCmdAndWaitForResp("AT+CFUN=1\r\n", "OK\r\n", DEFAULT_TIMEOUT * 3))
    {
        return -1;
    }
    if (checkSIMStatus())
    {
        return -1;
    }
    return 0;
}

int SMS::checkSIMStatus(void)
{
    char gprsBuffer[32];
    int count = 0;
    cleanBuffer(gprsBuffer, 32);
    while (count < 3)
    {
        sendCmd("AT+CPIN?\r\n");
        readBuffer(gprsBuffer, 32, DEFAULT_TIMEOUT);
        if ((NULL != strstr(gprsBuffer, "+CPIN: READY")))
        {
            break;
        }
        count++;
        delay(300);
    }
    if (count == 3)
    {
        return -1;
    }
    return 0;
}
