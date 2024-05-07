Attempting to get the max3421e working on the F0, using tinyusb and `furi`.

From application root:
```
mkdir lib
cd lib
git submodule add https://github.com/zacharyweiss/tu-flipper-max3421e
cd ..
git submodule update --init --recursive
```

`application.fam`
```
fap_private_libs=[
    Lib(
        name="tu-flipper-max3421e",
        # fap_include_paths=["include"],
        sources=[
            # replicates tinyusb/src/tinyusb.mk
            "tinyusb/src/tusb.c",
            "tinyusb/src/common/tusb_fifo.c",
            "tinyusb/src/device/usbd.c",
            "tinyusb/src/device/usbd_control.c",
            "tinyusb/src/typec/usbc.c",
            "tinyusb/src/class/audio/audio_device.c",
            "tinyusb/src/class/cdc/cdc_device.c",
            "tinyusb/src/class/dfu/dfu_device.c",
            "tinyusb/src/class/dfu/dfu_rt_device.c",
            "tinyusb/src/class/hid/hid_device.c",
            "tinyusb/src/class/midi/midi_device.c",
            "tinyusb/src/class/msc/msc_device.c",
            "tinyusb/src/class/net/ecm_rndis_device.c",
            "tinyusb/src/class/net/ncm_device.c",
            "tinyusb/src/class/usbtmc/usbtmc_device.c",
            "tinyusb/src/class/video/video_device.c",
            "tinyusb/src/class/vendor/vendor_device.c",
            "tinyusb/src/host/usbh.c",
            "tinyusb/src/host/hub.c",
            "tinyusb/src/class/cdc/cdc_host.c",
            "tinyusb/src/class/hid/hid_host.c",
            "tinyusb/src/class/msc/msc_host.c",
            "tinyusb/src/class/vendor/vendor_host.c",
            "tinyusb/src/typec/usbc.c",
            # "tinyusb/src/common/*",
            # "tinyusb/src/host/*",
            # adds max3421e as host controller
            "tinyusb/src/portable/analog/max3421/hcd_max3421.c",
            # replicates "family" / "board" -level code, but impl in furi
            "glue.c",
        ],
        cdefines=[
            "CFG_TUH_MAX3421", 
            "CFG_TUH_ENABLED", 
            "MAX3421_HOST"
        ],
        # cflags=[],
        # cincludes=[],
    ),
],
```
