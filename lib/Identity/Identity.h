#ifndef IDENTITY_H
#define IDENTITY_H

#include <Arduino.h>
#include <WiFi.h>

class Identity
{
public:
    // Метод для вычисления хеша
    static uint32_t get()
    {
        uint32_t chipId = 0;

        for (int i = 0; i < 17; i += 8)
        {
            chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xFF) << i;
        }

        return chipId;
    }
};

#endif // IDENTITY_H