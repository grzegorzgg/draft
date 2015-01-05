//gcc -o test_usb_ds4 test_usb_ds4.c -lusb && sudo ./test_usb_ds4

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <usb.h>
 
//Define
#define USB_VENDOR 0x054c
#define USB_PRODUCT 0x05c4
#define TIMEOUT (5*1000)
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
	// USB_altinterface(dh,1);
	if (usb_claim_interface(dh, 0)) {
		printf("\n\n%s\n\n", usb_strerror());
		printf("---------\n");
	} else {
		printf("2222\n");
	}

	if (usb_control_msg(dh, 0x40, 0, 0, 0, NULL, 0, 3000) != 0)
    	puts("błąd resetu układu");


    char * aaa[32];

    memset(aaa, 0, 32);

    aaa[0] = 05;
    aaa[1] = 255;
    aaa[4] = 100;


	int t;
	for (t=0; t<2; t++) {
		printf("Wysłano: %d\n", usb_bulk_write(dh, 0x02, aaa, 3, 3000));
		// UWAGA: wysyłana porcja danych nie może być zbyt duża !!!
		sleep(1);
	}

	printf("xxxxxxxxx99xxxxxxxxx\n");
	return -1;

    puts("Czekam na dane wejściowe");
  	
  	int i, j, k;
  	for (j=0; j<100; j++) {
    	// read data
	    static unsigned char buf[64];
	    i = usb_bulk_read(dh, 0x04, buf, 64, 5);
	    
	    // UWAGA: bufor mniejszy od 64 znakówe nie ma sensu ze względu na sposób transmisji ...
	    if (i==64) {
	      printf("j = %d : ", j);
	      for (k=0; k<i; k++)
	        printf("%X ", buf[k]);
	      puts("");
	    } else {
	      j--;
	    }
	}

	printf("xxxxxxxxx22xxxxxxxxx\n");

	return 0;

	// printf("%i\n", sizeof(buf));
	// printf("%d\n", ret);
	return 0;

	ret = usb_bulk_read(dh,EP6, buf, sizeof(buf), TIMEOUT);
	if(ret<0){
		// fprintf(stderr,"usb_bulk_read() failed? buf(%d) usb(%d)\n", sizeof(buf), ret);
		fprintf(stderr,"usb_bulk_read error.(%s)\n",usb_strerror());
		// USB_close(dh);
		printf("---\n");
		return 1;
	}
	printf("usb_bulk_read() finished\n");
	fwrite(buf, 1, sizeof(buf), stdout);
 	return 0;

	// USB_close(dh);
	// printf("USB End\n");
	// return 0;

}
