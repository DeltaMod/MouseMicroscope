##TIme for more shit, yeah!
import usb
import usb.core
import usb.util
import sys

VENDOR_ID = 0x04D9 #Tecknet mouse VID
PRODUCT_ID = 0xA118 #Tecknet mouse PID

dev = usb.core.find(idVendor=VENDOR_ID,
                       idProduct=PRODUCT_ID)

if dev is None:
    print('No Device, plug some shit in please')
    
dev.set_configuration()



for bRequest in range(255):
    try:
        ret.dev_ctrl_transfer(0xC0,bRequest, 0,0,1)
        print(bRequest)
        print(ret)
    except:
        #failed to get data for this request
        pass