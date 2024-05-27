#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include "tusb_config.h"
#include "tusb.h"
#include "host/usbh_pvt.h"
#include "glue.h"

#if CFG_TUH_ENABLED && CFG_TUH_MAX3421
static void max3421_init(void);
#endif

const usbh_class_driver_t* usbh_app_driver_get_cb(uint8_t* driver_count) {
    *driver_count = 0;
    return NULL;
}

void board_init() {
#if CFG_TUH_ENABLED
#if CFG_TUH_MAX3421
    max3421_init();
#endif
#endif
}

void board_deinit() {
    furi_hal_spi_bus_handle_deinit(SPI_HANDLE);
    furi_hal_gpio_remove_int_callback(P_INT);
    furi_hal_gpio_init(P_INT, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

//--------------------------------------------------------------------+
// API: SPI transfer with MAX3421E, must be implemented by application
//--------------------------------------------------------------------+
#if CFG_TUH_ENABLED && defined(CFG_TUH_MAX3421) && CFG_TUH_MAX3421

void max3421_int_handler(void* ctx) {
    UNUSED(ctx);
    // if(!(gpio == MAX3421_INTR_PIN && event_mask & GPIO_IRQ_EDGE_FALL)) return;
    tuh_int_handler(BOARD_TUH_RHPORT, true);
}

static void max3421_init(void) {
    // SPI init
    // (incl CS pin)
    furi_hal_spi_bus_handle_init(SPI_HANDLE);

    // Interrupt pin
    furi_hal_gpio_init(P_INT, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);
    // "add_int_callback" does "enable_int_callback" automatically.
    furi_hal_gpio_add_int_callback(P_INT, max3421_int_handler, NULL);
}

//// API to enable/disable MAX3421 INTR pin interrupt
void tuh_max3421_int_api(uint8_t rhport, bool enabled) {
    (void)rhport;

    if(enabled) {
        furi_hal_gpio_enable_int_callback(P_INT);
    } else {
        furi_hal_gpio_disable_int_callback(P_INT);
    }
}

// API to control MAX3421 SPI CS
void tuh_max3421_spi_cs_api(uint8_t rhport, bool active) {
    (void)rhport;

    // should I be acquiring/releasing here?
    // or manually writing to CS?
    if(active) {
        furi_hal_spi_acquire(SPI_HANDLE);
    } else {
        furi_hal_spi_release(SPI_HANDLE);
    }
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

    if(tx_buf == NULL) {
        return furi_hal_spi_bus_rx(SPI_HANDLE, rx_buf, xfer_bytes, SPI_TIMEOUT);
    } else if(rx_buf == NULL) {
        return furi_hal_spi_bus_tx(SPI_HANDLE, tx_buf, xfer_bytes, SPI_TIMEOUT);
    } else {
        return furi_hal_spi_bus_trx(SPI_HANDLE, tx_buf, rx_buf, xfer_bytes, SPI_TIMEOUT);
    }
}

#endif
