#ifndef CONTROLDEV_RAW_COMMAND_HPP
#define CONTROLDEV_RAW_COMMAND_HPP

#include <base/Time.hpp>
#include <string>
#include <vector>

namespace controldev {
    /** A data structure for raw data values of input devices
     */
    struct RawCommand
    {
        /*
         * Device Type identifier like
         */
        std::string deviceIdentifier;

        /*
         * The timestamp
         */
        base::Time time;

        /**
         * Axis values, scaled betwen 0 and 1.
         * */
        std::vector<double> axisValue;

        /*
         * Button state (either 0 or 1)
         *
         * Three State switches are handles as two
         *
         * Bool vectors are not supported by orogen. This is why it is using bytes
         */
        std::vector<uint8_t> buttonValue;
    };
}

#endif

