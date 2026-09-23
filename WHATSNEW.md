FLoRa-1.3.2 (2026-09-23)
------------------------

- LoRaMac: Fixed re-entrant FSM invocation that triggered INET 4.6's `ASSERT(!fsm.busy)` in debug builds (e.g. `LoRaNetworkTest` aborted at event #66). Radio-touching FSM entry actions are now deferred with `FSMA_Delay_Action` and executed via `fsm.executeDelayedActions()` after the `FSMA_Switch` scope has been closed, as in INET's `CsmaCaMac`. Release builds previously ran a nested state transition instead of aborting, which could drop uplinks at the MAC; results of scenarios with downlinks (e.g. ADR) change slightly.


FLoRa-1.3.1 (2026-09-16)
------------------------

- NetworkServerApp: Fixed ADR SNIR units by converting SNIR to dB (math::fraction2dB) before use in ADR. frame->getSNIR() returns a linear fraction, so comparing it against the dB SNR thresholds systematically inflated the ADR margin.


FLoRa-1.3.0 (2026-05-22)
------------------------

- Updated for INET-4.6.0.
- NetworkServerApp: Made per-node cOutVector names unique by including MAC address


FLoRa-1.2.0 (2026-04-16)
------------------------

- Updated for INET-4.5.4.


FLoRa 1.1.0 (2022-06-09)
------------------------

- Updated for INET-4.4.


FLoRa 1.0.0 (2021-04-10)
------------------------

Requires:
  - OMNeT++ 6.0 preview 10 or later
  - INET 4.3.1