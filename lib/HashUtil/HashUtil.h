#ifndef HASHUTIL_H
#define HASHUTIL_H

#include <Arduino.h>
#include <xxhash.h>

class HashUtil
{
public:
    // Метод для вычисления хеша
    static uint32_t compute(const uint8_t *data)
    {
        // Вычисляем хеш XX32
        return XXH32(data, sizeof(data), 0);
    }

    // Метод для сравнения готового хеша с вычисленным
    static bool compare(uint32_t hash, const uint8_t *data)
    {
        return hash == compute(data);
    }
};

#endif // HASHUTIL_H