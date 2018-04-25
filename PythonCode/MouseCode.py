import usb 
import usb.core
import usb.util

#backend = usb.backend.libusb1.get_backend(find_library=lambda x: C:"\PythonCode\libusb-1.0.dll")
#dev = usb.core.find(backend=backend)
import matplotlib.pyplot as plt
import numpy as np

VENDOR_ID = 0x04D9 #Tecknet mouse VID
PRODUCT_ID = 0xA118 #Tecknet mouse PID

#VENDOR_ID = 0x1D57 #Zelotes VID
#PRODUCT_ID = 0xAD17 #Zelotes PID

#VENDOR_ID = 0x03F0 #HP mouse VID
#PRODUCT_ID = 0x094A #HP mouse PID
# find the USB device
dev = usb.core.find(idVendor=VENDOR_ID,
                       idProduct=PRODUCT_ID)

# use the first/default configuration
interface = 0
endpoint = dev[0][(0,0)][0]
if dev.is_kernel_driver_active(interface) is True:
  # tell the kernel to detach
  dev.detach_kernel_driver(interface)
  # claim the device
  usb.util.claim_interface(dev, interface)
# In order to read the pixel bytes, reset PIX_GRAB by sending a write command
response = dev.ctrl_transfer(bmRequestType = 0x40, #Write
                                     bRequest = 0x01,
                                     wValue = 0x0000,
                                     wIndex = 0x0D, #PIX_GRAB register value
                                     data_or_wLength = None
                                     )

# Read all the pixels (0-899 in this chip! Because arrays start at zero... for some reason)
pixList = []
for i in range(899):
    response = dev.ctrl_transfer(bmRequestType = 0xC0, #Read
                                         bRequest = 0x01,
                                         wValue = 0x0000,
                                         wIndex = 0x0D, #PIX_GRAB register value
                                         data_or_wLength = 1
                                         )
    pixList.append(response)

pixelArray = np.asarray(pixList)
pixelArray = pixelArray.reshape((30,30))

plt.imshow(pixelArray)
plt.show()
# release the device
usb.util.release_interface(dev, interface)
# reattach the device to the OS kernel
dev.attach_kernel_driver(interface)