import usb1
with usb1.USBContext() as context:
    handle = context.openByVendorIDAndProductID(
        VENDOR_ID,
        PRODUCT_ID,
        skip_on_error=True,
    )
    if handle is None:
        # Device not present, or user is not allowed to access device.
        with handle.claimInterface(INTERFACE):
        # Do stuff with endpoints on claimed interface