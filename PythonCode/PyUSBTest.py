import usb.core
#VENDOR_ID = 0x1D57 #Zelotes VID
#PRODUCT_ID = 0xAD17 #Zelotes PID
#VENDOR_ID = 0x03F0 #HP mouse VID
#PRODUCT_ID = 0x094A #HP mouse PID
VENDOR_ID = 0x04D9 #Tecknet mouse VID
PRODUCT_ID = 0xA118 #Tecknet mouse PID
dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
if dev is None:
    raise ValueError('Our device is not connected')
    
yourmum = dev

print(yourmum)