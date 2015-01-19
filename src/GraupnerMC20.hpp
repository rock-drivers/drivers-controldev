#ifndef GRAUPNERMC20_HPP
#define GRAUPNERMC20_HPP

#include <iodrivers_base/Driver.hpp>
namespace controldev
{
    struct MC20Output{
        int channel[16];
    };

    class GraupnerMC20 : public iodrivers_base::Driver {
        private:
            uint16_t genCRC(uint8_t const* buffer) const;
            uint16_t CRC(uint16_t crc, uint8_t value) const;
            int extractPacket (uint8_t const *buffer, size_t buffer_size) const;
        public:
            GraupnerMC20();
            bool getReading(MC20Output *out);
    };
}
#endif
