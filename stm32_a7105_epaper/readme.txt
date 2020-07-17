A7105 ref code uses 3 wire SPI

but we are using 4 wire SPI,   GIO1 is MISO


pins:
PA2    uart2_tx     for debug output
PA3    rf_gio2   A7105 WTR

PA4   rf_cs
PA5   spi_sck
PA6   spi_miso
pa7    SPI_MOSI

developed with STM32CubeIDE 1.0.2
firmware: STM32Cube_FW_F1_V1.8.0