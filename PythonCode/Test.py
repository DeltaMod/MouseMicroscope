#This is just a test lol
import usb
import usb.core
print('Wankers, lol')
VENDOR_ID = 0x04D9 #Tecknet mouse VID
PRODUCT_ID = 0xA118 #Tecknet mouse PIDn
interface = 0
dev = usb.core.find(idVendor=VENDOR_ID,
                       idProduct=PRODUCT_ID)
if dev is not None:
    print('Mouse is here, get on it fatso')
    
if dev is None:
    print('Can`t see shit mylord, better pour some more beer down your gullet')