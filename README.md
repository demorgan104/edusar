# edusar

Explore how an automotive signal moves through a communication stack—from an
application, through **COM**, **PduR**, and **CanIf**, across a simulated CAN bus,
and back to a receiving application.

edusar is an educational, AUTOSAR-inspired C99 project with a configuration
generator, a command-line demo, and an interactive two-ECU visualizer. Everything
runs locally, without CAN hardware or third-party packages.

![Signal journey: VehicleSpeed travels down ECU 1 through COM, PduR and CanIf, across simulated CAN, and up ECU 2 to the receiving application.](PduR/docs/images/signal-journey.svg)

## Try it

You need a **C99 compiler**, **GNU Make**, and **Python 3**. Run these commands
from the repository root.

Start the visual demo:

```sh
make -C PduR/build ui
```

Open **http://127.0.0.1:8000**, enter a VehicleSpeed value from `0` to `65535`, and
select **Send signal**. The UI highlights each layer as the signal travels down
ECU 1, across CAN, and up ECU 2. Pause, step through individual layers, or adjust
the playback pace. The frame inspector shows the encoded bytes, the received
value, and the transmit confirmation count.

Stop the server with **Ctrl+C**. To use a different port:

```sh
make -C PduR/build ui PORT=8080
```

Prefer the terminal? Run:

```sh
make -C PduR/build demo
```

```text
COM sends VehicleSpeed=1234
CAN TX id=0x123 data=D2 04
COM receives VehicleSpeed=1234; confirmations=1
```

## How it works

A **signal** is an application value, such as VehicleSpeed. A **PDU** (protocol
data unit) is the byte buffer that carries it between stack modules. A **CAN
frame** carries that payload with an identifier used by the receiving interface.

| Layer | Sending path · ECU 1 | Receiving path · ECU 2 |
| --- | --- | --- |
| Application | Supplies VehicleSpeed | Reads the received value |
| COM | Packs the signal into bytes | Decodes and stores the signal |
| PduR · PDU Router | Routes the COM PDU to CanIf | Routes the CanIf PDU to COM |
| CanIf · CAN Interface | Maps the PDU to a CAN identifier | Matches the CAN identifier and length |
| Simulated driver / bus | Copies the frame into a buffer | Delivers the buffered frame to CanIf |

For example, `1234` is `0x04D2`. COM packs it in **little-endian** order, so the
low byte comes first: **`D2 04`**. PduR maps COM transmit PDU `10` to CanIf PDU
`20`, and CanIf sends it with CAN identifier **`0x123`**. On reception, CanIf
selects receive PDU `21`, PduR routes it to COM PDU `11`, and COM reconstructs
`1234` for the receiving application.

Each UI transmission calls a local Python endpoint, which runs the compiled C
demo and returns its result as JSON. The animation then replays that completed
result. The C demo runs two **sequential ECU lifecycles**, resetting module state
before receiving the buffered frame. Both use the same generated configuration;
they are not concurrent ECU instances. A transmit confirmation means the
simulated driver completed its synchronous send, rather than a hardware CAN ACK.

## Configure the stack

Edit [PduR/config/demo.json](PduR/config/demo.json) to define signal IDs, COM and
CanIf PDU IDs, and CAN identifiers. Every build runs the Python generator to
produce the configuration tables for all three modules in
[PduR_PBcfg.c](PduR/PduR/src/PduR_PBcfg.c).

```sh
make -C PduR/build generate  # regenerate configuration only
make -C PduR/build          # generate and compile
```

The supported profile has **1–16 unsigned 16-bit signals**, with one signal per
two-byte PDU and standard 11-bit CAN identifiers. The demo, UI labels, and stack
tests use the default IDs; update them when changing those IDs. Edit the JSON
source rather than the generated C tables.

## Find your way around

| Path | Contents |
| --- | --- |
| [PduR/Com](PduR/Com) | Signal packing, decoding, and receive storage |
| [PduR/PduR](PduR/PduR) | PDU routing and generated configuration tables |
| [PduR/CanIf](PduR/CanIf) | CAN identifier mapping and receive filtering |
| [PduR/demo](PduR/demo) | C demo, local Python server, and browser UI |
| [PduR/config](PduR/config) / [PduR/tools](PduR/tools) | JSON configuration and generator |
| [PduR/tests](PduR/tests) | Stack, generator, and UI integration checks |
| [PduR/build](PduR/build) | Makefile and compiler settings |

## Build and check

```sh
make -C PduR/build test       # C stack and Python generator checks
python3 PduR/tests/test_ui.py # HTTP/C bridge checks; run after building the demo
make -C PduR/build clean      # remove build outputs
```

The checks cover signal encoding, routing, confirmations, invalid inputs,
reinitialization, configuration validation, and HTTP integration. The UI check
starts a temporary localhost server. The default C build treats compiler
warnings as errors.

See the [stack documentation](PduR/docs/README.md) for API behavior, configuration
details, and implementation limits.

## Scope

This project is a learning tool, not an AUTOSAR-conformant BSW stack. It supports
immediate signal transmission, one-to-one PDU routing, and simulated classic CAN.
It does not implement ARXML/ECUC, cyclic scheduling, transport protocols, CAN FD,
concurrent execution, or an asynchronous hardware driver.

Licensed under the [GNU General Public License v3.0](LICENSE).
