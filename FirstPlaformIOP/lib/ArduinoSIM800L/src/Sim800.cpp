/*
 * sim800.cpp
 * A library for SeeedStudio seeeduino GPRS shield 
 *
 * Copyright (c) 2013 seeed technology inc.
 * Author        :   lawliet zou
 * Create Time   :   Dec 2013
 * Change Log    :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "sim800.h"

void SIM800::preInit(void)
{

    pinMode(SIM800_POWER_STATUS, INPUT);
    delay(10);
    if (LOW == LOW)
    {
        if (sendATTest() != 0)
        {
            delay(800);
            digitalWrite(resetPin, HIGH);
            delay(200);
            digitalWrite(resetPin, LOW);
            delay(2000);
            digitalWrite(resetPin, HIGH);
            delay(3000);
            Serial.println("Power check");
        }
        while (sendATTest() != 0)
            ;
        Serial.println("Init O.K!");
    }
    else
    {
        Serial.println("Power check failed!");
    }
}

int SIM800::checkReadable(void)
{
    return serialSIM800.available();
}

int SIM800::readBuffer(char *buffer, int count, unsigned int timeOut)
{
    int i = 0;
    unsigned long timerStart, timerEnd;
    timerStart = millis();
    while (1)
    {
        while (serialSIM800.available())
        {
            char c = serialSIM800.read();
            if (c == '\r' || c == '\n')
                c = '$';
            buffer[i++] = c;
            if (i > count - 1)
                break;
        }
        if (i > count - 1)
            break;
        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeOut)
        {
            break;
        }
    }
    delay(500);
    while (serialSIM800.available())
    { // display the other thing..
        serialSIM800.read();
    }
    return 0;
}

void SIM800::cleanBuffer(char *buffer, int count)
{
    for (int i = 0; i < count; i++)
    {
        buffer[i] = '\0';
    }
}

void SIM800::sendCmd(const char *cmd)
{
    serialSIM800.write(cmd);
}

int SIM800::sendATTest(void)
{
    int ret = sendCmdAndWaitForResp("AT\r\n", "OK", DEFAULT_TIMEOUT);
    return ret;
}

int SIM800::waitForResp(const char *resp, unsigned int timeout)
{
    int len = strlen(resp);
    int sum = 0;
    unsigned long timerStart, timerEnd;
    timerStart = millis();

    while (1)
    {
        if (serialSIM800.available())
        {
            char c = serialSIM800.read();
            sum = (c == resp[sum]) ? sum + 1 : 0;
            if (sum == len)
                break;
        }
        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout)
        {
            return -1;
        }
    }

    while (serialSIM800.available())
    {
        serialSIM800.read();
    }

    return 0;
}

void SIM800::sendEndMark(void)
{
    serialSIM800.println((char)26);
}

int SIM800::sendCmdAndWaitForResp(const char *cmd, const char *resp, unsigned timeout)
{
    sendCmd(cmd);
    return waitForResp(resp, timeout);
}

void SIM800::serialDebug(void)
{
    while (1)
    {
        if (serialSIM800.available())
        {
            Serial.write(serialSIM800.read());
        }
        if (Serial.available())
        {
            serialSIM800.write(Serial.read());
        }
    }
}

String SIM800::searchNetwork(unsigned int timeout)
{
    //Function to search for available cellular networks|Get network registration|Send attched network to Nextion display.
    String sendCOPScommand, readCOPScommad, networkOperator, sendNetOperator;
    int startCOPS = -1, copsFirstcomma, copsSecondcomma, copsNL;
    unsigned long timerStart, timerEnd;

    sendCmd("AT+COPS?\r\n");
    timerStart = millis();

    while (1)
    {
        if (serialSIM800.available())
        {
            readCOPScommad = serialSIM800.readString();
            if (readCOPScommad.indexOf("+COPS:") != -1)
            {
                copsFirstcomma = readCOPScommad.indexOf(",", startCOPS + 1);
                copsSecondcomma = readCOPScommad.indexOf(",", copsFirstcomma + 2);
                copsNL = readCOPScommad.indexOf("\n", copsSecondcomma + 1);
                networkOperator = readCOPScommad.substring(copsSecondcomma + 2, copsNL - 2);
                return networkOperator;
            }

            return "Searching for network";
        }
        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout)
        {
            return "No netowrk found";
        }
    }
}
