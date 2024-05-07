#include <furi.h>
#include <furi_hal.h>

#if CFG_TUH_ENABLED && CFG_TUH_MAX3421
static void max3421_init(void);
#endif

void board_init(void) {
#if CFG_TUH_ENABLED
#if CFG_TUH_MAX3421
    max3421_init();
#endif
#endif
}

//--------------------------------------------------------------------+
// API: SPI transfer with MAX3421E, must be implemented by application
//--------------------------------------------------------------------+
#if CFG_TUH_ENABLED && defined(CFG_TUH_MAX3421) && CFG_TUH_MAX3421

void max3421_int_handler(uint gpio, uint32_t event_mask) {
    if(!(gpio == MAX3421_INTR_PIN && event_mask & GPIO_IRQ_EDGE_FALL)) return;
    tuh_int_handler(BOARD_TUH_RHPORT, true);
}

static void max3421_init(void) {
    // CS pin

    // Interrupt pin

    // SPI init
}

//// API to enable/disable MAX3421 INTR pin interrupt
void tuh_max3421_int_api(uint8_t rhport, bool enabled) {
    (void)rhport;
    irq_set_enabled(IO_IRQ_BANK0, enabled);
}

// API to control MAX3421 SPI CS
void tuh_max3421_spi_cs_api(uint8_t rhport, bool active) {
    (void)rhport;
    gpio_put(MAX3421_CS_PIN, !active);
}

// API to transfer data with MAX3421 SPI
// Either tx_buf or rx_buf can be NULL, which means transfer is write or read only
bool tuh_max3421_spi_xfer_api(
    uint8_t rhport,
    uint8_t const* tx_buf,
    uint8_t* rx_buf,
    size_t xfer_bytes) {
    (void)rhport;

    if(tx_buf == NULL && rx_buf == NULL) {
        return false;
    }

    int ret;

    if(tx_buf == NULL) {
        ret = spi_read_blocking(MAX3421_SPI, 0, rx_buf, xfer_bytes);
    } else if(rx_buf == NULL) {
        ret = spi_write_blocking(MAX3421_SPI, tx_buf, xfer_bytes);
    } else {
        ret = spi_write_read_blocking(spi0, tx_buf, rx_buf, xfer_bytes);
    }

    return ret == (int)xfer_bytes;
}

#endif
