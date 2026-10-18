# edusar

Educational AUTOSAR tools and implementations.

The [communication stack](PduR/docs/README.md) provides COM, PduR, and CanIf,
a JSON configuration generator, and a host-based CAN loopback demo.

```sh
make -C PduR/build demo
make -C PduR/build ui    # open http://127.0.0.1:8000
make -C PduR/build test
```

Requires a C99 compiler, GNU Make, and Python 3. This implements a small
educational profile rather than a complete AUTOSAR stack.
