#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include <Identity.h>

// Перечисление для типов пакетов
enum class PacketType : uint8_t
{
    ACK = 0x00,  // Пакет подтверждения
    DATA = 0x01, // Пакет с данными

    // NACK = 0x03,   // Пакет отрицательного подтверждения
    // CONTROL = 0x04 // Пакет управления
};

struct Packet
{
    uint32_t hash;
    uint32_t to;   // Адрес назначения (4 байта)
    uint32_t from; // Адрес отправителя (4 байта)
    PacketType type;   // Тип пакета (1 байт)

    // Конструктор, инициализирующий только type
    Packet(PacketType type) 
        : type(type), from(Identity::get()) {}
    // Конструктор, инициализирующий из байтов
    Packet(const uint8_t *data);

    // Метод для преобразования структуры в массив байтов
    void toBytes(uint8_t *buffer) const;
};

#endif // PACKET_H