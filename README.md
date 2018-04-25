# MouseMicroscope
Using a python 3.6 anaconda distribution on ubuntu 16.04, with the direct assistance of libusb1 and pyusb, we indend to make a microscope out of a mouse sensor. 

Ubuntu is a must, but there are several things that you need to do before you get cracking!

`============================================================================================
-------------------- Allowing ubuntu to take control of your USB device --------------------
============================================================================================`

There are a few modifications that you first need to make to your linux system before doing anything past this!

1. Connect your USB device and open the terminal with ctrl+alt+t, then type in lsusb to get a list of your usb devices. 
-You should be able to identify your device by name, and then it should have two hex numbers in the format "1a1a:2b2b" (obviously, these numbers can be anything, and you CAN convert them to decimal if you really want to. These represent idVendor:idProduct, and you are going to need them for the next step:

2. Now you need to make a udev rule (which is a text document), that needs to be placed in the following directory: `computer/etc/udev/rules.d`. You can call the pretty much whatever, as long as you end it with .rules. For this example, we will call it "usbhax.rules", and we will fill it with the following information.
`"ACTION=="add", SUBSYSTEMS=="usb", SUBSYSTEM=="usb", ATTRS{idVendor}=="1a1a", ATTRS{idProduct}=="2b2b", MODE="0666", GROUP="plugdev"`

3. Unless you are not a superuser (command: `su -`), you must add the user you intend to run your code from into the group plugdev. You can do this by simply using: `sudo adduser <username> plugdev` from a superuser account. If you are a superuser, then you don't need to add yourself.

4. Now you can unplug your device, restart ubuntu, and when you plug it back in, it should now allow you to use it with the anaconda distribution!
