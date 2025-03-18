#include "Packet.h"
#include <HashUtil.h>

void Packet::toBytes(uint8_t *buffer) const
{
    uint8_t data[9];
    uint32_t hash;

    // Копируем toAddr в буфер (с учетом порядка байтов)
    data[0] = (to >> 24) & 0xFF;
    data[1] = (to >> 16) & 0xFF;
    data[2] = (to >> 8) & 0xFF;
    data[3] = to & 0xFF;

    // Копируем fromAddr в буфер (с учетом порядка байтов)
    data[4] = (from >> 24) & 0xFF;
    data[5] = (from >> 16) & 0xFF;
    data[6] = (from >> 8) & 0xFF;
    data[7] = from & 0xFF;

    // Копируем type в буфер (приводим к uint8_t, так как enum class основан на uint8_t)
    data[8] = static_cast<uint8_t>(type);

    //Hash data only
    hash = HashUtil::compute(data);

    // Копируем hash в буфер
    buffer[0] = (hash >> 24) & 0xFF;
    buffer[1] = (hash >> 16) & 0xFF;
    buffer[2] = (hash >> 8) & 0xFF;
    buffer[3] = hash & 0xFF;

    // Копируем 8 байт из src в dest (начиная с 4-го байта)
    memcpy(&buffer[4], data, 9); // Копируем с 4-го байта массива data 9 байт
}

void Packet::fromBytes(const uint8_t *buffer)
{
    hash = (uint32_t(buffer[0]) << 24) |
           (uint32_t(buffer[1]) << 16) |
           (uint32_t(buffer[2]) << 8) |
           uint32_t(buffer[3]);

    to = (uint32_t(buffer[4]) << 24) |
             (uint32_t(buffer[5]) << 16) |
             (uint32_t(buffer[6]) << 8) |
             uint32_t(buffer[7]);

    from = (uint32_t(buffer[8]) << 24) |
               (uint32_t(buffer[9]) << 16) |
               (uint32_t(buffer[10]) << 8) |
               uint32_t(buffer[11]);
               
    // Восстановление type
    type = static_cast<PacketType>(buffer[12]); // Преобразуем байт в тип PacketType
}
