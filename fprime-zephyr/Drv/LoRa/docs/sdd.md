# Zephyr::LoRa

Wrapper for the [Zephyr LoRa driver](https://docs.zephyrproject.org/latest/connectivity/lora_lorawan/index.html). This will integrate into the communication stack.

### Typical Usage

This is used as a radio in the F Prime communication stack transmitting via the LoRa radio. The `run` port must be
connected to a rate group (the reference deployment uses 200 Hz); it emits the recovery SUCCESS owed after a deferred
transmit, so an unconnected `run` port stalls the uplink chain after the first deferral.

### Receive-Aware Transmit

The LoRa modem is half-duplex. Before transmitting, `dataIn` reads the SX127x `RegModemStat` register (via the
loramac-node HAL, available when `CONFIG_HAS_SEMTECH_SX1276`/`SX1272` is set) and, when a packet reception is in
progress, returns the buffer through `dataReturnOut` and emits `Fw::Success::FAILURE` without transmitting, so the
in-flight receive is not aborted. Per the Communication Adapter Protocol the component then owes one recovery
`SUCCESS`, which `run` emits once the modem no longer reports a reception or after `DEFERRED_TX_RECOVERY_TICKS`
ticks, whichever comes first. Exactly one `SUCCESS` is emitted per deferral, and it is armed only after the
`FAILURE` has been delivered so it can never overtake it. No events are emitted on the send path; deferrals are
counted in the `TransmitsDeferred` channel. On radios without modem status support the check compiles to false and
transmits pre-empt receives as before.

The recovery `SUCCESS` is emitted on the rate-group thread. An upstream `Svc.ComRetry` should therefore be
configured with `recover_on_sender_thread = true` so the resend (and the blocking `lora_send`) is pulled back onto
the sending thread rather than executed on the rate group.

## Requirements

| Name | Description | Validation |
|---------|---|---|
| LORA-01 | The LoRa component shall interface with the Zephyr Lora driver | INSPECTION |
| LORA-02 | The LoRa component shall provide DATA_RATE and CODING_RATE as paramaters | Unit-Test |
| LORA-03 | The LoRa component shall provide FREQUENCY, BANDWITH, TRANSMIT_POWER, and PREAMBLE as configurable parameters | Unit-Test |
| LORA-04 | The LoRa component shall telemeter BytesSent and BytesReceived channels | Unit-Test |
| LORA-05 | The LoRa component shall support the Svc.Com interface | Unit-Test |
| LORA-06 | The LoRa component shall have a continuous wave command | Unit-Test |
| LORA-07 | The LoRa component shall wrap the Zephyr LoRa driver | Unit-Test |
| LORA-08 | The LoRa component shall configure the Zephyr LoRa driver for tranmit only when sending data | Unit-Test |
| LORA-09 | The LoRa component shall not abort a packet reception in progress in order to transmit; it shall return the buffer and emit `FAILURE` instead | Hardware Test |
| LORA-10 | The LoRa component shall emit exactly one recovery `SUCCESS`, from the `run` port, after each deferred transmit once the reception completes or `DEFERRED_TX_RECOVERY_TICKS` elapse | Hardware Test |
| LORA-11 | The LoRa component shall not emit events on the send path for deferrals; it shall telemeter the `TransmitsDeferred` count | Inspection |


## Port Interfaces

| Name | Description |
|---|---|
| Svc.Com | Interface to plug the radio into the communication stack |
| run (Svc.Sched) | Rate-group tick; must be connected. Emits the recovery SUCCESS owed for a deferred transmit |


## Configuration

| Name | Description |
|------|---|
| FREQUENCY   | Frequency of the radio transmission / receive |
| BANDWIDTH   | Number of parity bits sent             |
| TX_POWER    | Transmission power of the raio |
| PREAMBLE    | Preamble length in bytes |
| DEFERRED_TX_RECOVERY_TICKS | Maximum `run` ticks a deferred transmit waits for the receive to end before the recovery SUCCESS is forced (units: ticks of the connected rate group) |

Projects that override `zephyr-config/LoRaCfg.hpp` must define `DEFERRED_TX_RECOVERY_TICKS`. Receive detection
requires the loramac-node module (`ZEPHYR_LORAMAC_NODE_MODULE_DIR`) and an SX127x radio.

## Command

| Name | Description |
|------|---|
| CONTINUOUS_WAVE | Send continuous wave for a supplied duration |
| TRANSMIT | Enable or disable transmission |

## Parameters

| Name | Description |
|------|---|
| DATA_RATE   | Spreading factor / data rate for radio |
| CODING_RATE | Number of parity bits sent             |
| BANDWIDTH_TX | Transmit bandwidth |
| BANDWIDTH_RX | Receive bandwidth |

## Telemetry

| Name | Description |
|---|---|
| LastRssi | RSSI value of last receive |
| LastSnr  | SNR value of last receive  |
| BytesSent | Total payload bytes transmitted |
| BytesReceived | Total payload bytes received |
| TransmitsDeferred | Count of transmits deferred because a receive was in progress |

## Events

| Name | Description |
|---|---|
| ConfigurationFailed | Failed to configure the LoRa radio |
| SendFailed          | Failed to send data out LoRa radio |
| AllocationFailed    | Failed to allocate buffer for received data|
