#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include "tusb_config.h"
#include "tusb.h"

#define P_MISO &gpio_ext_pa6
#define P_MOSI &gpio_ext_pa7
#define P_CLK &gpio_ext_pb3
#define P_CS &gpio_ext_pa4
#define P_INT &gpio_ext_pc3
#define SPI_TIMEOUT 1000

#if CFG_TUH_ENABLED && CFG_TUH_MAX3421
static void max3421_init(void);
#endif

const LL_SPI_InitTypeDef spi_preset_custom = {
    //hspi5.Instance = SPI5;
    //hspi5.Init.Mode = SPI_MODE_MASTER;
    .Mode = LL_SPI_MODE_MASTER,
    //hspi5.Init.Direction = SPI_DIRECTION_2LINES;
    .TransferDirection = LL_SPI_FULL_DUPLEX,
    //hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
    .DataWidth = LL_SPI_DATAWIDTH_8BIT,
    //hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
    .ClockPolarity = LL_SPI_POLARITY_LOW,
    //hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
    .ClockPhase = LL_SPI_PHASE_1EDGE,
    //hspi5.Init.NSS = SPI_NSS_HARD_OUTPUT;
    .NSS = LL_SPI_NSS_HARD_OUTPUT,
    //hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    .BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV128,
    //hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    .BitOrder = LL_SPI_MSB_FIRST,
    //hspi5.Init.TIMode = SPI_TIMODE_DISABLED;
    //hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    .CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE,
    //hspi5.Init.CRCPolynomial = 10;
    .CRCPoly = 10,
};

// copied from: "targets/f7/furi_hal/furi_hal_spi_config.c"
inline static void furi_hal_spi_bus_external_handle_event_callback(
    FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event,
    const LL_SPI_InitTypeDef* preset) {
    if(event == FuriHalSpiBusHandleEventInit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
    } else if(event == FuriHalSpiBusHandleEventDeinit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(event == FuriHalSpiBusHandleEventActivate) {
        LL_SPI_Init(handle->bus->spi, (LL_SPI_InitTypeDef*)preset);
        LL_SPI_SetRxFIFOThreshold(handle->bus->spi, LL_SPI_RX_FIFO_TH_QUARTER);
        LL_SPI_Enable(handle->bus->spi);

        furi_hal_gpio_init_ex(
            handle->miso,
            GpioModeAltFunctionPushPull,
            GpioPullDown,
            GpioSpeedVeryHigh,
            GpioAltFn5SPI1);
        furi_hal_gpio_init_ex(
            handle->mosi,
            GpioModeAltFunctionPushPull,
            GpioPullDown,
            GpioSpeedVeryHigh,
            GpioAltFn5SPI1);
        furi_hal_gpio_init_ex(
            handle->sck,
            GpioModeAltFunctionPushPull,
            GpioPullDown,
            GpioSpeedVeryHigh,
            GpioAltFn5SPI1);

        furi_hal_gpio_write(handle->cs, false);
    } else if(event == FuriHalSpiBusHandleEventDeactivate) {
        furi_hal_gpio_write(handle->cs, true);

        furi_hal_gpio_init(handle->miso, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->mosi, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

        LL_SPI_Disable(handle->bus->spi);
    }
}

inline static void furi_hal_spi_bus_r_handle_event_callback(
    FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event,
    const LL_SPI_InitTypeDef* preset) {
    if(event == FuriHalSpiBusHandleEventInit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    } else if(event == FuriHalSpiBusHandleEventDeinit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(event == FuriHalSpiBusHandleEventActivate) {
        LL_SPI_Init(handle->bus->spi, (LL_SPI_InitTypeDef*)preset);
        LL_SPI_SetRxFIFOThreshold(handle->bus->spi, LL_SPI_RX_FIFO_TH_QUARTER);
        LL_SPI_Enable(handle->bus->spi);

        furi_hal_gpio_init_ex(
            handle->miso,
            GpioModeAltFunctionPushPull,
            GpioPullNo,
            GpioSpeedVeryHigh,
            GpioAltFn5SPI1);
        furi_hal_gpio_init_ex(
            handle->mosi,
            GpioModeAltFunctionPushPull,
            GpioPullNo,
            GpioSpeedVeryHigh,
            GpioAltFn5SPI1);
        furi_hal_gpio_init_ex(
            handle->sck,
            GpioModeAltFunctionPushPull,
            GpioPullNo,
            GpioSpeedVeryHigh,
            GpioAltFn5SPI1);

        furi_hal_gpio_write(handle->cs, false);
    } else if(event == FuriHalSpiBusHandleEventDeactivate) {
        furi_hal_gpio_write(handle->cs, true);

        furi_hal_gpio_init(handle->miso, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->mosi, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

        LL_SPI_Disable(handle->bus->spi);
    }
}

static void spi_bus_handle_custom_event_callback(
    FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event) {
    furi_hal_spi_bus_r_handle_event_callback(handle, event, &spi_preset_custom);
}

FuriHalSpiBusHandle spi_bus_handle_custom = {
    .bus = &furi_hal_spi_bus_r,
    .callback = spi_bus_handle_custom_event_callback,
    .miso = P_MISO,
    .mosi = P_MOSI,
    .sck = P_CLK,
    .cs = P_CS,
};

#define SPI_HANDLE &spi_bus_handle_custom

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
    // GPIO pin (C3) is pull-up to VCC.  Add switch to ground for change in value.
    // I use 220ohm resistor from switch to C3 (but optional if you are SURE
    // the pin is in input/interrupt mode).
    //
    // GpioModeInterruptRiseFall means callback invoked when going from VCC (from our
    // pull-up resistor) to GND.
    //
    // NOTE: You can use GpioModeInterruptRise for invoking on a GND->VCC and
    // GpioModeInterruptRiseFall for invoking on both transitions.
    furi_hal_gpio_init(P_INT, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);
}

//// API to enable/disable MAX3421 INTR pin interrupt
void tuh_max3421_int_api(uint8_t rhport, bool enabled) {
    (void)rhport;

    if(enabled) {
        // NOTE: "add_int_callback" does "enable_int_callback" automatically.
        // For the 3rd parameter, you can pass any object that you want to be passed
        // to your callback method.
        furi_hal_gpio_add_int_callback(P_INT, max3421_int_handler, NULL);
    } else {
        furi_hal_gpio_remove_int_callback(P_INT);
    }
}

// API to control MAX3421 SPI CS
void tuh_max3421_spi_cs_api(uint8_t rhport, bool active) {
    (void)rhport;

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
    // Does CS api get called surrounding the XFER?
    // Or do I manually need to add acquire/release here too?

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
