
#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

/* ---------------------------- Hardware Config ---------------------------- */

// USB pins
#define USB_CFG_IOPORTNAME   B
#define USB_CFG_DMINUS_BIT   3
#define USB_CFG_DPLUS_BIT    2

// USB interrupts
#define USB_INTR_CFG            MCUCR
#define USB_INTR_CFG_SET        ((1 << ISC00) | (1 << ISC01))
#define USB_INTR_ENABLE         GIMSK
#define USB_INTR_ENABLE_BIT     INT0
#define USB_INTR_PENDING        GIFR
#define USB_INTR_PENDING_BIT    INTF0
#define USB_INTR_VECTOR         EXT_INT0_vect

// 20MHz master clock
#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)

/* --------------------------- Functional Range ---------------------------- */

// Enable interrupt in for sending data.
#define USB_CFG_HAVE_INTRIN_ENDPOINT 1

// Set poll speed to 10ms
#define USB_CFG_INTR_POLL_INTERVAL 10

// USB is not self-powered
#define USB_CFG_IS_SELF_POWERED 0

// About 100mA of power consumption
#define USB_CFG_MAX_BUS_POWER 100

/* -------------------------- Device Description --------------------------- */

// Shared USB vendor-defined PID for name-based discrimination,
// from Van Ooijen Technische Informatica
#define USB_CFG_VENDOR_ID 0xC0, 0x16
#define USB_CFG_DEVICE_ID 0xDC, 0x05

// "orca.pet" manufacturer
#define USB_CFG_VENDOR_NAME     'o', 'r', 'c', 'a', '.', 'p', 'e', 't'
#define USB_CFG_VENDOR_NAME_LEN 8

// "PFS154 prog v1" device
#define USB_CFG_DEVICE_NAME     'P', 'F', 'S', '1', '5', '4', ' ', 'p', 'r', 'o', 'g', ' ', 'v', '1'
#define USB_CFG_DEVICE_NAME_LEN 14

// Version 1.0
#define USB_CFG_DEVICE_VERSION 0x00, 0x01

// Vendor-specific device class
#define USB_CFG_DEVICE_CLASS    0xFF
#define USB_CFG_DEVICE_SUBCLASS 0

// Unused interface descriptor
#define USB_CFG_INTERFACE_CLASS    0
#define USB_CFG_INTERFACE_SUBCLASS 0
#define USB_CFG_INTERFACE_PROTOCOL 0

#endif /* __usbconfig_h_included__ */
