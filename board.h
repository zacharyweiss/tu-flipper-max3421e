#ifndef BOARD_H_
#define BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
// USB Host MAX3421E
//--------------------------------------------------------------------

#define MAX3421_SPI PICO_DEFAULT_SPI_INSTANCE
#define MAX3421_SCK_PIN PICO_DEFAULT_SPI_SCK_PIN
#define MAX3421_MOSI_PIN PICO_DEFAULT_SPI_TX_PIN
#define MAX3421_MISO_PIN PICO_DEFAULT_SPI_RX_PIN
#define MAX3421_CS_PIN 10
#define MAX3421_INTR_PIN 9

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
