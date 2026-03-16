# Pi_Common
This repository contains the C source code to allow use of the GPIO, I2C and SPI Devices from Ada.

The `src/` directory provides full hardware implementations (e.g., `spi_driver.c` opens `/dev/spidev*` devices, configures hardware via ioctl, performs real I/O). Use these when compiling for a real Raspberry Pi with actual GPIO/I2C/SPI hardware.

## src_stub

This repository also provides no-op stub implementations in `src_stub/` (e.g., `spi_driver_stub.c` has the same function signatures as the real drivers but returns success without touching hardware). Use these when building for simulation/testing on a machine without real hardware, or in Docker containers.

The stubs are useful for:
- **CI/CD pipelines** that need to build the code without hardware
- **Simulator builds** that want realistic Ada elaboration and control flow without hardware side effects
- **Testing** Ada logic in isolation from hardware concerns

**Example:** The [IOT_Clock](https://github.com/David-Haley/Clock) project uses `src_stub` in its Docker simulator. The `iot_clock_sim.gpr` project file links against `src_stub` instead of `src`, allowing the same Ada code to run in a container that has no GPIO/SPI hardware. The clock can be tested and debugged end-to-end without a physical Raspberry Pi.
