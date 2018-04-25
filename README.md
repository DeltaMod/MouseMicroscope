# MouseMicroscope
Using a python 3.6 anaconda distribution on ubuntu 16.04, with the direct assistance of libusb1 and pyusb, we indend to make a microscope out of a mouse sensor. 

Ubuntu is a must, but there are several things that you need to do before you get cracking!

=================================================================================

**--------------------------------------- Allowing ubuntu to take control of your USB device ---------------------------------------**

=================================================================================

There are a few modifications that you first need to make to your linux system before doing anything past this!

1. Connect your USB device and open the terminal with ctrl+alt+t, then type in lsusb to get a list of your usb devices. 
  - You should be able to identify your device by name, and then it should have two hex numbers in the format "1a1a:2b2b". These represent the idVendor:idProduct indices respectively, and you are going to need them for the next step. 

  - These ID numbers can be anything of the same format as shown, and you CAN convert them to decimal if you really want to. It's still more convenient to use the hex format, since it requires no conversion. 
    
   - *NOTE: If you want to use these in python scripts, then you must preface them with 0x (so 0x1a1a and 0x2b2b), or else it will not recognise that they are hexadecimal. If you converted the numbers to decimal, then this is not required.* 

2. Now you need to make a udev rule, which is a text document containing exceptions (or rules, if you may) of which devices a non root user may access completely, so that you don't run into the `[Errno 13] Access denied (insufficient permissions)` error code when trying to access your device.

 - This is done by creating a new document, anywhere you like, and naming it something like `"50-usbhax.rules"`. There are two important things, the number (and dash) and the ending .rules. The number 50 denotes the order of priority in the udev read list (don't quote me on this), and as long as you follow that with a dash, and then any name (like `50-MyVariable.rules`) and end it with .rules, then it should work.

 - Inside the document, you should copy and paste the following:  
    
  - `ACTION=="add", SUBSYSTEMS=="usb", SUBSYSTEM=="usb", ATTRS{idVendor}=="1a1a", ATTRS{idProduct}=="2b2b", MODE="0666", GROUP="plugdev"`

 - *Make sure that you replace the 1a1a and 2b2b with your own identified idVendor:idProduct indices.*

 - Finally, after saving your document, you must open the terminal again, using ctrl+alt+t, and type in `sudo nautilus` *[Disclaimer: I know many advice against using `sudo nautilus`, but you're only going to be moving one file to one folder that isn't in your home folder (so it should be fine). Plus, this is a noobuser guide so that ANYONE can try this. ]* which will open up a file manager that allows you to move your files into folders owned by root, which is what you need to do!. You need to move your `"50-usbhax.rules"`, or whatever your called it, file into the following directory:
    
 - `computer/etc/udev/rules.d`
  
3. Now, unless you are not a superuser (command: `su -`), you must add the user you intend to run your code from into the group plugdev. You can do this by simply using: `sudo adduser <username> plugdev` from a superuser account (that is, writing `su -` into the terminal before adding the user). If you are a superuser, then you don't need to add yourself because the new group is not considered root, and so you have access to it.

4. Now you can unplug your device, restart ubuntu, and when you plug it back in, it should now allow you to use it with the anaconda distribution!
