# AGENTS.md

This file provides guidance to AI coding agents working with code in this repository.

## Project

FLoRa (Framework for LoRa) is an OMNeT++ simulation model for end-to-end LoRa/LoRaWAN networks, built on the INET framework. It is compiled as a shared library (`src/libflora`) loaded by `opp_run`, not as a standalone executable. The current version (see `Version`, `WHATSNEW.md`) targets OMNeT++ 6.x and INET 4.6.0. When changing behavior, add an entry to `WHATSNEW.md`.

## Build

Requires an OMNeT++ environment (`opp_makemake`, `opp_run` on `PATH`) and an INET checkout pointed to by `INET_ROOT`. The top-level `Makefile` uses it as `INET_DIR`, as a path *relative to the repo root*.

```sh
make makefiles        # generate src/Makefile (needed once, and again after adding/removing source dirs)
make                  # build src/libflora (MODE=debug for debug build -> libflora_dbg)
make clean / make cleanall
```

`*_m.h` / `*_m.cc` files are generated from the `.msg` files by `opp_msgc` during the build and are gitignored. Edit the `.msg` file, never the generated code.

### Setting up a local test environment (macOS)

If OMNeT++/INET are not installed, a self-contained setup can live in a gitignored folder inside the repo (e.g. `.deps/`, excluded via `.git/info/exclude`), so it can be removed with `rm -rf` without leftovers. opp_env is not suitable for this, because on macOS it requires a system-wide Nix installation.
- Download the OMNeT++ `omnetpp-<ver>-core.tgz` and INET `inet-<ver>-src.tgz` release archives from GitHub.
- macOS lacks `pkg-config` and ships bison 2.3 and GNU make 3.81. OMNeT++ needs pkg-config, bison ≥ 3.0 and a newer GNU make (3.81 fails with "No rule to make target …/lcgrandom.o"). Build pkg-config (`--with-internal-glib`), bison and make from source into a local prefix and put its `bin` first on `PATH`.
- Configure OMNeT++ headless: `./configure WITH_QTENV=no WITH_OSG=no WITH_PYTHON=no WITH_SCAVE_PYTHON_BINDINGS=no PREFER_LLD=no USE_CCACHE_PREFIX=no`. Keep `WITH_NETBUILDER` enabled, otherwise NED files cannot be loaded at runtime.
- Build OMNeT++, INET and FLoRa with `make MODE=release` and `make MODE=debug`. Use the debug build to check correctness: INET's `ASSERT`s are compiled out in release builds, so bugs such as FSM re-entry show up only in debug.
- `INET_ROOT` is used both relative to the repo root (by `make`) and relative to `simulations/` (by `run_flora`). When the two differ, set it per step, or call `opp_run`/`opp_run_dbg` directly with an absolute `-n .:../src:<inet>/src`. There is no `run_flora_dbg`; use `opp_run_dbg -l ../src/flora …`.

## Running simulations

There is no unit test suite. Verify changes by running the example simulations from inside `simulations/`:

```sh
cd simulations
./run -u Cmdenv -f omnetpp.ini                 # single-node config (repeat = 30)
./run -u Cmdenv -f examples/n100-gw1.ini -r 0  # one run of a larger example
./run_example_networks.sh                      # all examples/*.ini in Cmdenv
```

`src/run_flora` (called by `simulations/run`) runs `opp_run -l ../src/flora -n .:../src:$INET_ROOT/src`. It must be run from `simulations/` because the library path is relative. If `INET_ROOT` is unset it falls back to `../../inet4.3`, which is stale, so set `INET_ROOT` explicitly. Results go to `simulations/results/` (gitignored). The example ini files write vectors to `../results/`.

**Keep verification runs short.** All configs set `sim-time-limit = 1d`. That takes a very long real time for the 1000-node examples, especially in debug builds, and even 1h of simulated time is too long for a quick check. When a run only has to show that a change works (e.g. that a crash or assertion is gone), pass a short limit on the command line, e.g. `--sim-time-limit=600s`, just past the point where the problem appears. Run full-length simulations only when complete results are needed.

**Comparing results before/after a change:** build an unmodified copy of `master` (e.g. `git archive master | tar -x -C <dir>`) next to the modified tree. Run both with the same config and `-r`, and diff the `scalar` lines of the `.sca` files. The example ini files hard-code `output-vector-file`/`output-scalar-file`, and `--result-dir` does not override them, so also pass `--output-vector-file=… --output-scalar-file=…` to keep the runs from overwriting each other.

## Architecture

NED packages mirror the directories under `src/` (`flora.LoRa`, `flora.LoRaPhy`, …). The test network `LoRaNetworkTest` lives in `simulations/package.ned`: `loRaNodes[]` → (radio via `LoRaMedium`) → `loRaGW[]` → Ethernet/`gwRouter`/`InternetCloud`/`nsRouter` → `networkServer` (an INET `StandardHost` whose `app[0]` is `NetworkServerApp`).

**Uplink/downlink path**
- `LoRaApp/SimpleLoRaApp`: end-device application. It chooses SF/TP/BW/CR/CF, attaches them to each packet as a `LoRaTag` (`LoRa/LoRaTagInfo.msg`), and emits `LoRa_AppPacketSent`. It applies ADR commands received in downlinks and runs the node-side ADR_ACK_CNT logic (`evaluateADRinNode`).
- `LoRa/LoRaMac`: class-A MAC as an `Fsm` (`IDLE → TRANSMIT → WAIT_DELAY_1 → LISTENING_1 → RECEIVING_1 → WAIT_DELAY_2 → …`) that implements the two receive windows after each uplink. It moves the `LoRaTag` parameters into the `LoRaMacFrame` header.
- `LoRa/LoRaRadio` (node) and `LoRa/LoRaGWRadio` (gateway) are INET radio subclasses. The gateway radio can receive several frames at once. The NICs (`LoRaNic`, `LoRaGWNic`) extend INET's `WirelessInterface` and wire in these radios, `LoRaTransmitter`/`LoRaReceiver` and `LoRaMac`/`LoRaGWMac`.
- `LoRa/PacketForwarder`: gateway application that bridges LoRa frames to UDP toward the network server. It stamps RSSI/SNIR from the reception into the `LoRaMacFrame`.
- `LoRa/NetworkServerApp`: deduplicates uplinks received by several gateways, tracks known nodes, and runs server-side ADR (`adrMethod` `"max"`/`"avg"` over the last 20 SNIR samples, `adrDeviceMargin`). It sends ADR downlinks back through the best gateway. SNIR arrives as a linear fraction and must be converted with `math::fraction2dB` before it is compared with the dB thresholds (fixed in 1.3.1).

**PHY (`src/LoRaPhy/`)**: LoRa-specific replacements for INET's radio-medium pieces. They are selected in ini via `**.radio.radioMediumModule = "LoRaMedium"` and `**.LoRaMedium.pathLossType = ...`.
- `LoRaMedium`, `LoRaMediumCache` and `LoRaNeighborCache` specialize INET's `RadioMedium`.
- `LoRaTransmitter`/`LoRaTransmission` compute time-on-air from SF/BW/CR. `LoRaReceiver`/`LoRaReception` decide reception. `LoRaReceiver::isPacketCollided` models inter-SF (non-)orthogonality and the capture effect using the `nonOrthDelta[SF][SF]` threshold table and preamble timing overlap.
- Path-loss models: `LoRaLogNormalShadowing` (default, `sigma`), `LoRaHataOkumura`, `LoRaPathLossOulu`.
- The scalar analog models live in `LoRaAnalogModel` and `LoRa{Transmission,Reception}AnalogModel`.

**Energy**: `LoRaEnergyModules/LoRaEnergyConsumer` is a state-based consumer. It reads per-state and per-TX-power current draw from `simulations/energyConsumptionParameters.xml`.

**Configuration conventions** (see `simulations/omnetpp.ini` and `examples/`): node radio parameters are set per node with `initialLoRaSF/TP/BW/CR`, and positions with `initialX/Y` plus `initFromDisplayString = false`. Gateway-to-server wiring uses `packetForwarder.destAddresses = "networkServer"` and `networkServer.app[0].destAddresses = "loRaGW[0]"`. Cloud latency comes from `cloudDelays.xml`.

## Releasing

Previous releases (e.g. v1.3.1) follow this pattern:
1. Commit "Bump Version to X.Y.Z" on `master`. It sets `Version` to `flora-X.Y.Z` and turns the `FLoRa-X.Y.Z (unreleased)` heading in `WHATSNEW.md` into `FLoRa-X.Y.Z (YYYY-MM-DD)`.
2. Create an annotated tag: `git tag -a vX.Y.Z -m "Version X.Y.Z"`.
3. Build the release asset from the tag: `git archive --format=tar.gz --prefix=flora/ -o flora-X.Y.Z.tgz vX.Y.Z`. The top-level directory is `flora/` (not `flora-X.Y.Z/`), and the archive contains only tracked files. If the tagged commit changes, rebuild it.
4. After pushing `master` and the tag, create a GitHub release titled "FLoRa X.Y.Z" from the tag, and attach `flora-X.Y.Z.tgz`. For the notes, use a "## Bug fixes" / "## Changes" list with the component in bold and the PR number, as in the v1.3.1 notes. Add a `**Full Changelog**: …/compare/vPREV...vX.Y.Z` link.

## Maintaining this file

When a task teaches you something practical about this project, add it to the matching section of this file in the same session, without waiting to be asked. This covers things like environment setup, required tool versions, build pitfalls, how to verify changes efficiently, and the release procedure. Keep entries short and concrete, and tell the user that `AGENTS.md` changed.
