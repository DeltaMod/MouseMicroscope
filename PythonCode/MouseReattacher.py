if dev is not None:# release the device
    usb.util.release_interface(dev, interface)
# reattach the device to the OS kernel
    dev.attach_kernel_driver(interface)

