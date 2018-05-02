#!/usr/bin/python
import sys
import usb.core
import usb.util
# decimal vendor and product values
#VENDOR_ID = 0x04D9 #Tecknet mouse VID
#PRODUCT_ID = 0xA118 #Tecknet mouse PIDn
VENDOR_ID = 0x1D57 #Zelotes VID
PRODUCT_ID = 0xAD17 #Zelotes PID
interface = 0
dev = usb.core.find(idVendor=VENDOR_ID,
                       idProduct=PRODUCT_ID)
if dev is not None:
    print('Mouse Found, Attempting code...')
# or, uncomment the next line to search instead by the hexidecimal equivalent
#dev = usb.core.find(idVendor=0x45e, idProduct=0x77d)
# first endpoint
endpoint = dev[0][(0,0)][0]
# if the OS kernel already claimed the device, which is most likely true
# thanks to http://stackoverflow.com/questions/8218683/pyusb-cannot-set-configuration
if dev.is_kernel_driver_active(interface) is True:
  # tell the kernel to detach
  dev.detach_kernel_driver(interface)
  # claim the device
  usb.util.claim_interface(dev, interface)
collected = 0
attempts = 50
while collected < attempts :
    try:
        data = dev.read(endpoint.bEndpointAddress,endpoint.wMaxPacketSize)
        collected += 1
        print(data)
    except usb.core.USBError as e:
        data = None
        if e.args == ('Operation timed out',):
            continue
# release the device
usb.util.release_interface(dev, interface)
# reattach the device to the OS kernel
dev.attach_kernel_driver(interface)