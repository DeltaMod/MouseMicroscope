import usb 
import usb.core
import usb.util
##List of addresses
Product_ID                      = 0x00
Revision_ID                     = 0x01
Motion                          = 0x02
Delta_X_L                       = 0x03
Delta_X_H                       = 0x04
Delta_Y_L                       = 0x05
Delta_L_H                       = 0x06
SQUAL                           = 0x07
Pixel_Sum                       = 0x08
Mximum_Pixel                    = 0x09
Minimum_Pixel                   = 0x0a
Shutter_Upper                   = 0x0b
Shutter_Lower                   = 0x0c
Frame_Period_Lower              = 0x0d
Frame_Period_Upper              = 0x0e
Configuration_I                 = 0x0f
Configuration_II                = 0x10
Frame_Capture                   = 0x12
SROM_Enable                     = 0x13
Run_Downshift                   = 0x14
Rest1_Rate                      = 0x15
Rest1_Downshift                 = 0x16
Rest2_Rate                      = 0x17
Rest2_Downshift                 = 0x18
Rest3_Rate                      = 0x19
Frame_Period_Max_Bound_Lower    = 0x1a
Frame_Period_Max_Bound_Upper    = 0x1b
Frame_Period_Max_Bound_Lower    = 0x1c
Frame_Period_Max_Bound_Upper    = 0x1d
Shutter_Max_Bound_Lower         = 0x1e
Shutter_Max_Bound_Upper         = 0x1f
LASER_CTRL0                     = 0x20
#Reserved                       = 0x21-0x23
Observation                     = 0x24
Data_Out_Lower                  = 0x25
Data_Out_Upper                  = 0x26
#Reserved                       = 0x27-0x29
SROM_ID                         = 0x2a
Lift_Detection_Thr              = 0x2e
Configuration_V                 = 0x2f
#Reserved                       = 0x30-0x38
Configuration_VI                = 0x39
Power_Up_Reset                  = 0x3a
Shutdown                        = 0x3b
#Reserved                       = 0x3c-0x3e
Inverse_Product_ID              = 0x3f
#Reserved                       = 0x40-0x41
Snap_Angle                      = 0x42
#Reserved                       = 0x43-0x4f
Motion_Burst                    = 0x50
SROM_Load_Burst                 = 0x62
Pixel_Burst                     = 0x64



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
response = dev.ctrl_transfer(bmRequestType = 0x40, #Write - Frame Capture: 0x93  --originally was 0x40 for PIX_GRAB
                                     bRequest = 0x01,
                                     wValue = 0x0000,
                                     wIndex = 0x12, #Pixel_Burst: 0x64 Frame_Capture: 0x12 --Originally was PIX_GRAB
                                     data_or_wLength = None
                                     )

# Read all the pixels (0-899 in this chip! Because arrays start at zero... for some reason)
pixList = []
for i in range(900):
    response = dev.ctrl_transfer(bmRequestType = 0xC0, #Read - Frame Capture: 0xc5  --Originally was 0xC0 for PIX_GRAB
                                         bRequest = 0x01,
                                         wValue = 0x0000,
                                         wIndex = 0x64, #Pixel_Burst: 0x64 Frame_Capture: 0x12 --Originally was PIX_GRAB
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

