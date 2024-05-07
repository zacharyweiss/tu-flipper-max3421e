
#define P_MISO &gpio_ext_pa6
#define P_MOSI &gpio_ext_pa7
#define P_CLK &gpio_ext_pb3
#define P_CS &gpio_ext_pa4
#define P_INT &gpio_ext_pc3
#define SPI_TIMEOUT 1000

void board_init();
void board_deinit();
