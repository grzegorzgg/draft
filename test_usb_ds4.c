//gcc -o test_usb_ds4 test_usb_ds4.c -lusb && sudo ./test_usb_ds4
// http://www.psdevwiki.com/ps4/DS4-USB

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <usb.h>
 
//Define
#define USB_VENDOR 0x054c
#define USB_PRODUCT 0x05c4
#define TIMEOUT (3*1000)
#define EP6	0x05
 
/* Init USB */
struct usb_bus *USB_init()
{
	usb_init();
	usb_find_busses();
	usb_find_devices();
	return(usb_get_busses());
}
 
/* Find USB device  */
struct usb_device *USB_find(struct usb_bus *busses, struct usb_device *dev)
{
	struct usb_bus *bus;
	for(bus=busses; bus; bus=bus->next){
		for(dev=bus->devices; dev; dev=dev->next) {
			if( (dev->descriptor.idVendor==USB_VENDOR) && (dev->descriptor.idProduct==USB_PRODUCT) ){
				return( dev );
			}
		}
	}
	return( NULL );
}
/* USB Open */
struct usb_dev_handle *USB_open(struct usb_device *dev)
{
	struct usb_dev_handle *udev = NULL;
 
	udev=usb_open(dev);
	if( (udev=usb_open(dev))==NULL ){
		fprintf(stderr,"usb_open Error.(%s)\n",usb_strerror());
		exit(1);
	}
 
	if( usb_set_configuration(udev,dev->config->bConfigurationValue)<0 ){
		if( usb_detach_kernel_driver_np(udev,dev->config->interface->altsetting->bInterfaceNumber)<0 ){
			fprintf(stderr,"usb_set_configuration Error.\n");
			fprintf(stderr,"usb_detach_kernel_driver_np Error.(%s)\n",usb_strerror());
 		}
	}
 
	if( usb_claim_interface(udev,dev->config->interface->altsetting->bInterfaceNumber)<0 ){
		if( usb_detach_kernel_driver_np(udev,dev->config->interface->altsetting->bInterfaceNumber)<0 ){
			fprintf(stderr,"usb_claim_interface Error.\n");
			fprintf(stderr,"usb_detach_kernel_driver_np Error.(%s)\n",usb_strerror());
		}
	}
 
	if( usb_claim_interface(udev,dev->config->interface->altsetting->bInterfaceNumber)<0 ){
		fprintf(stderr,"usb_claim_interface Error.(%s)\n",usb_strerror());
	}
 
	return(udev);
}
 
/* USB Close */
void USB_close(struct usb_dev_handle *dh)
{
	if(usb_release_interface(dh, 0)){
		fprintf(stderr,"usb_release_interface() failed. (%s)\n",usb_strerror());
		usb_close(dh);
	}
	if( usb_close(dh)<0 ){
		fprintf(stderr,"usb_close Error.(%s)\n",usb_strerror());
	}
}
 
/* USB altinterface */
void USB_altinterface(struct usb_dev_handle *dh,int tyep)
{
	if(usb_set_altinterface(dh,tyep)<0)
	{
		fprintf(stderr,"Failed to set altinterface %d: %s\n", 1,usb_strerror());
		USB_close(dh);
	}
}


enum ds4pad {
	DS4_L1X,
	DS4_L1Y,
	DS4_R1X,
	DS4_R1Y,

	DS4_L2,
	DS4_R2,
	
	DS4_BATTERYLEVEL,

	DS4_CROSS,
	DS4_CIRCLE,
	DS4_SQUARE,
	DS4_TRIANGLE,


};

typedef struct {
	uint8_t L1_x;
	uint8_t L1_y;
	uint8_t R1_x;
	uint8_t R1_y;

	uint8_t L2_analog;
	uint8_t R2_analog;

	uint8_t batteryLevel;

	char cross;
	char circle;
	char square;
	char triangle;

	char SHARE;
	char OPTIONS;
	
	char L1;
	char R1;

	char L2;
	char R2;

	char L3;
	char R3;

	char PS;
	char PAD;

} ds4pad;

ds4pad pad1;


void rx_handler(char *data)
{


	pad1.L1_x = data[1];
	pad1.L1_y = data[2];

	pad1.R1_x = data[3];
	pad1.R1_y = data[4];
	
	pad1.L2_analog = data[8];
	pad1.R2_analog = data[9];

	pad1.batteryLevel = data[12];

	pad1.square = ( data[5] & 16 ) != 0;
	pad1.cross = ( data[5] & 32 ) != 0;
	pad1.circle = ( data[5] & 64 ) != 0;
	pad1.triangle = ( data[5] & 128 ) != 0;

	pad1.SHARE = ( data[6] & 16 ) != 0;
	pad1.OPTIONS = ( data[6] & 32 ) != 0;
	pad1.L3 = ( data[6] & 64 ) != 0;
	pad1.R3 = ( data[6] & 128 ) != 0;

	pad1.L1 = (data[6] & 1) != 0;
	pad1.R1 = (data[6] & 2) != 0;
	pad1.L2 = (data[6] & 4) != 0;
	pad1.R2 = (data[6] & 8) != 0;

	pad1.PS = (data[7] & 1) != 0;
	pad1.PAD = (data[7] & 2) != 0;

}



main()
{

	

	struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *dh;
	unsigned char buf[64];
	int ret;
	/* Initialize */
 
	printf("starting!\n");
	bus=USB_init();
	dev=USB_find(bus, dev);
	if( dev==NULL ) { 
		printf("device not found\n");
		exit(1);
	}
	printf("Initialize OK\n");

	// /*-------------*/
	// /* Device Open */
	// /*-------------*/
	
	usb_dev_handle *usbdev; 
  	usbdev = USB_open(dev);

	dh=USB_open(dev);
	if( dh==NULL ){ exit(2); }
	printf("Device Open OK\n");
	
 
	/*
		Put command here
	*/
	printf("Start Read data from EP6\n");
	if (usb_claim_interface(dh, 0)) {
		printf("\n\n%s\n\n", usb_strerror());
		printf("---------\n");
	}

	if (usb_control_msg(dh, 0x40, 0, 0, 0, NULL, 0, 3000) != 0)
    	puts("błąd resetu układu");

    
    char data[32];
    memset(data, 0, 32);

    data[0] = 05;
    data[1] = 255;

    data[4] = 0;		// RIGHT
    data[5] = 0;		// LEFT

    data[6] = 25;	// RED
    data[7] = 30;	// GREEN
    data[8] = 20;	// BLUE

	printf("Wysłano: %d\n", usb_bulk_write(dh, 0x03, data, 32, 3000));
	

    puts("Czekam na dane wejściowe");
  	

    int i;
    while (1) {
    	static unsigned char buf[64];
	    i = usb_bulk_read(dh, 0x04, buf, 64, TIMEOUT);

	    if (i==64) {
	    	rx_handler(buf); 
	    	// printf("%i\n", pad1.R1_x);
	    }
    }


 //  	int i, j, k;
 //  	for (j=0; j<15; j++) {
 //    	// read data
	//     static unsigned char buf[64];
	//     i = usb_bulk_read(dh, 0x04, buf, 64, TIMEOUT);
	    
	//     // UWAGA: bufor mniejszy od 64 znakówe nie ma sensu ze względu na sposób transmisji ...
	//     if (i==64) {
	// 		// printf("j = %d : ", j);

	//     	rx_handler(buf);

	// 		for (k=0; k<i; k++) {
	// 			// printf("%X ", buf[k]);
	// 		}
			

	// 		puts(" ");

	//     } else {
	// 		j--;
	//     }
	// }

	USB_close(dh);
	printf("USB End\n");
	return 0;
}
