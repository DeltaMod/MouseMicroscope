import sys
import usb
import usb.core
import usb.util
import usb.backend.libusb1
backend = usb.backend.libusb1.get_backend(find_library=lambda x: "/usr/lib/libusb-1.0.so")
dev     = usb.core.find(..., backend=backend)
#backend = usb.backend.libusb1.get_backend(find_library=lambda x: C:"\PythonCode\libusb-1.0.dll")
#dev = usb.core.find(backend=backend)
import matplotlib.pyplot as plt
import numpy as np

#VENDOR_ID = 0x1D57 #Zelotes VID
#PRODUCT_ID = 0xAD17 #Zelotes PID

VENDOR_ID = 0x03F0 #HP mouse VID
PRODUCT_ID = 0x094A #HP mouse PID
# find the USB device
device = usb.core.find(idVendor=VENDOR_ID,
                       idProduct=PRODUCT_ID)

# use the first/default configuration
device.set_configuration()


# In order to read the pixel bytes, reset PIX_GRAB by sending a write command
response = device.ctrl_transfer(bmRequestType = 0x40, #Write
                                     bRequest = 0x01,
                                     wValue = 0x0000,
                                     wIndex = 0x0D, #PIX_GRAB register value
                                     data_or_wLength = None
                                     )

# Read all the pixels (360 in this chip)
pixList = []
for i in range(361):

    response = device.ctrl_transfer(bmRequestType = 0xC0, #Read
                                         bRequest = 0x01,
                                         wValue = 0x0000,
                                         wIndex = 0x0D, #PIX_GRAB register value
                                         data_or_wLength = 1
                                         )
    pixList.append(response)

pixelArray = np.asarray(pixList)
pixelArray = pixelArray.reshape((19,19))

plt.imshow(pixelArray)
plt.show()
