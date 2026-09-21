#ifndef LORA_CFG_HPP
#define LORA_CFG_HPP
#include <zephyr/drivers/lora.h>
#include <Fw/FPrimeBasicTypes.hpp>
namespace LoRaConfig {
const U32 FREQUENCY = 915000000;               // 437400000; //!< LoRa frequency in Hz
lora_signal_bandwidth BANDWIDTH = BW_125_KHZ;  //!< LoRa bandwidth
const I8 TX_POWER = 14;                        //!< LoRa transmission power in dBm
const U16 PREAMBLE_LENGTH = 8;                 //!< LoRa preamble length
U8 HEADER[] = {0, 0, 0, 0};                    //!< LoRa header (not used)
const U32 DEFERRED_TX_RECOVERY_TICKS = 400;    //!< Run ticks before a deferred transmit's recovery SUCCESS is forced: 2 s at 200 Hz, > max SF12/BW125 air time
}  // namespace LoRaConfig
#endif  // LORA_CFG_HPP
