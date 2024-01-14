#ifndef NXMP_USBFS_H
#define NXMP_USBFS_H

#include <cstdlib>
#include <string>
#include <vector>
#include <threads.h>
#include <usbhsfs.h>



#define USB_USE_EVENT_SYSTEM
//#define USB_USE_CALLBACK_SYSTEM



struct usb_devices{
	std::string mount_point;
	std::string name;
	std::string fstype;
	size_t capacity;
};


#ifdef USB_USE_EVENT_SYSTEM
int usbThread(void *arg);
#endif

class USBMounter{
public:
	USBMounter();
	~USBMounter();
	std::vector<usb_devices> mounted_devs;
	
	void setBasePath(std::string _basepath);
	std::string getBasePath();
	//UsbHsFsDevice *g_usbDevices;
	//u32 g_usbDeviceCount;

#ifdef USB_USE_CALLBACK_SYSTEM	
	
	void usbMscTestDevices(void);
#endif

private:
	
	std::string basepath = "";
	
};

#endif
