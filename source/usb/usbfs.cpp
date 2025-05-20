#include <cstdio>
#include "usbfs.h"
#include <cstring>


static u32 g_usbDeviceCount = 0;
static UsbHsFsDevice *g_usbDevices = NULL;


#ifdef USB_USE_CALLBACK_SYSTEM
static Mutex g_usbDeviceMutex = 0;
static bool g_updated = false;
#endif

#ifdef USB_USE_EVENT_SYSTEM
static UEvent *g_statusChangeEvent, g_exitEvent;
static thrd_t g_thread = {0};



int usbThread(void *arg) {
	sleep(1);
	USBMounter * mymounter = (USBMounter *)arg;
	(void) arg;
    Result rc = 0;
    int idx = 0;
	u32 listed_device_count = 0;

    Waiter status_change_event_waiter = waiterForUEvent(g_statusChangeEvent);
    Waiter exit_event_waiter = waiterForUEvent(&g_exitEvent);

	while (true) {
        rc = waitMulti(&idx, -1, status_change_event_waiter,exit_event_waiter);
		if (R_FAILED(rc)) continue;
        if (g_usbDevices) {
            free(g_usbDevices);
            g_usbDevices = nullptr;
        }
		
		g_usbDeviceCount = usbHsFsGetMountedDeviceCount();
		g_usbDevices = (UsbHsFsDevice *) calloc(g_usbDeviceCount, sizeof(UsbHsFsDevice));
		mymounter->mounted_devs.clear();
		
		if (idx == 1)
        {
            break;
        }

		if (!g_usbDevices)
        {
            continue;
        }

		if (!(listed_device_count = usbHsFsListMountedDevices(g_usbDevices, g_usbDeviceCount)))
        {
            continue;
        }
		
		for(u32 i = 0; i < listed_device_count; i++)
        {
            UsbHsFsDevice *device = &(g_usbDevices[i]);
            usb_devices tmpdev;
            printf("Device #%u:\n" \
                        "\t- USB interface ID: %d.\n" \
                        "\t- Logical Unit Number: %u.\n" \
                        "\t- Filesystem index: %u.\n" \
                        "\t- Write protected: %s.\n" \
                        "\t- Vendor ID: 0x%04X.\n" \
                        "\t- Product ID: 0x%04X.\n" \
                        "\t- Manufacturer: \"%s\".\n" \
                        "\t- Product Name: \"%s\".\n" \
                        "\t- Serial Number: \"%s\".\n" \
                        "\t- Logical Unit Capacity: 0x%lX bytes.\n" \
                        "\t- Mount name: \"%s\".\n" \
                        "\t- Filesystem type: %s.\n" \
                        "\t- Mount flags: 0x%08X.\n", \
                        i + 1, \
                        device->usb_if_id, \
                        device->lun, \
                        device->fs_idx, \
                        device->write_protect ? "yes" : "no", \
                        device->vid, \
                        device->pid, \
                        device->manufacturer, \
                        device->product_name, \
                        device->serial_number, \
                        device->capacity, \
                        device->name, \
                        LIBUSBHSFS_FS_TYPE_STR(device->fs_type), \
                        device->flags);
            
            tmpdev.mount_point = device->name;
			tmpdev.name = device->product_name;
			tmpdev.fstype = LIBUSBHSFS_FS_TYPE_STR(device->fs_type);
			tmpdev.capacity = device->capacity;
			mymounter->mounted_devs.push_back(tmpdev);
           
        }
	
    }
	
	for(u32 i = 0; i < listed_device_count; i++)
        {
            UsbHsFsDevice *device = &(g_usbDevices[i]);
            usbHsFsUnmountDevice(device, false);
        }
        
	
	return 0;
}

#endif

#ifdef USB_USE_CALLBACK_SYSTEM

static void usbMscPopulateFunc(const UsbHsFsDevice *devices, u32 device_count, void *user_data){
	USBMounter *ctx = (USBMounter *)user_data;
	
	mutexLock(&g_usbDeviceMutex);
	
	if (g_usbDevices)
    {
        free(g_usbDevices);
        g_usbDevices = NULL;
    }

    g_usbDeviceCount = 0;

    if (devices && device_count)
    {
        /* Allocate mounted devices buffer. */
        g_usbDevices = (UsbHsFsDevice *)calloc(device_count, sizeof(UsbHsFsDevice));
        if (!g_usbDevices)
        {
            printf("Failed to allocate memory for mounted USB Mass Storage devices buffer!\n\n");
            consoleUpdate(NULL);
        } else {
            /* Copy input data. */
            memcpy(g_usbDevices, devices, device_count * sizeof(UsbHsFsDevice));
            g_usbDeviceCount = device_count;
        }
    }

    g_updated = true;
	
	mutexUnlock(&g_usbDeviceMutex);
}


void USBMounter::usbMscTestDevices(void)
{
    mutexLock(&g_usbDeviceMutex);

    if (!g_updated || !g_usbDevices || !g_usbDeviceCount)
    {
        mutexUnlock(&g_usbDeviceMutex);
        return;
    }

    g_updated = false;
	mounted_devs.clear();
    /* Print info from mounted devices. */
    for(u32 i = 0; i < g_usbDeviceCount; i++)
    {
        UsbHsFsDevice *device = &(g_usbDevices[i]);
		usb_devices tmpdev;
        printf("Device #%u:\n" \
                    "\t- USB interface ID: %d.\n" \
                    "\t- Logical Unit Number: %u.\n" \
                    "\t- Filesystem index: %u.\n" \
                    "\t- Write protected: %s.\n" \
                    "\t- Vendor ID: 0x%04X.\n" \
                    "\t- Product ID: 0x%04X.\n" \
                    "\t- Manufacturer: \"%s\".\n" \
                    "\t- Product Name: \"%s\".\n" \
                    "\t- Serial Number: \"%s\".\n" \
                    "\t- Logical Unit Capacity: 0x%lX bytes.\n" \
                    "\t- Mount name: \"%s\".\n" \
                    "\t- Filesystem type: %s.\n" \
                    "\t- Mount flags: 0x%08X.\n" \
                    "\t- Filesystem tests:\n", \
                    i + 1, \
                    device->usb_if_id, \
                    device->lun, \
                    device->fs_idx, \
                    device->write_protect ? "yes" : "no", \
                    device->vid, \
                    device->pid, \
                    device->manufacturer, \
                    device->product_name, \
                    device->serial_number, \
                    device->capacity, \
                    device->name, \
                    LIBUSBHSFS_FS_TYPE_STR(device->fs_type), \
                    device->flags);
		tmpdev.mount_point = device->name;
		tmpdev.name = device->product_name;
		tmpdev.fstype = LIBUSBHSFS_FS_TYPE_STR(device->fs_type);
		tmpdev.capacity = device->capacity;
		mounted_devs.push_back(tmpdev);
		
       

        
    }

 
    mutexUnlock(&g_usbDeviceMutex);
	
	
}


#endif

USBMounter::~USBMounter(){
#ifdef USB_USE_EVENT_SYSTEM
	ueventSignal(&g_exitEvent);
	thrd_join(g_thread, NULL);
#endif

#ifdef USB_USE_CALLBACK_SYSTEM
    
	for(u32 i = 0; i < g_usbDeviceCount; i++)
    {
        UsbHsFsDevice *device = &(g_usbDevices[i]);
        usbHsFsUnmountDevice(device, false);
    }
#endif

	usbHsFsExit();
	if (g_usbDevices) free(g_usbDevices);
}

USBMounter::USBMounter(){
	Result rc;
    
	
	rc = usbHsFsInitialize(0);
	if (R_FAILED(rc))
    {
        
    }
	
#ifdef USB_USE_EVENT_SYSTEM
	g_statusChangeEvent = usbHsFsGetStatusChangeUserEvent();
	usbHsFsSetFileSystemMountFlags(UsbHsFsMountFlags_ShowHiddenFiles | UsbHsFsMountFlags_ReadOnly);
    ueventCreate(&g_exitEvent, true);
	thrd_create(&g_thread, usbThread, (void *)this);
#endif

#ifdef USB_USE_CALLBACK_SYSTEM	
	usbHsFsSetPopulateCallback(&usbMscPopulateFunc, this);
#endif

}

std::string USBMounter::getBasePath(){
	return basepath;
}

void USBMounter::setBasePath(std::string _basepath){
	basepath = _basepath;
}

