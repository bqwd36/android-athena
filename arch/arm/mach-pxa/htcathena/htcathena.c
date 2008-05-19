/*
 * linux/arch/arm/mach-pxa/htcathena/htcathena.c
 *
 *  Support for the HTC Athena.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fb.h>
#include <linux/platform_device.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/input_pda.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <asm/arch/hardware.h>
#include <asm/arch/pxafb.h>
#include <asm/arch/pxa-regs.h>
#include <asm/arch/serial.h>
#include <asm/arch/mmc.h>
#include <asm/arch/irda.h>
#include <asm/arch/ohci.h>

#include <asm/arch/htcathena-gpio.h>
#include <asm/arch/htcathena-asic.h>

#include <asm/arch-pxa/gpio.h>
#include <asm/gpio.h>

#include <linux/mfd/htc-egpio.h>
#include <linux/delay.h>
#include <linux/gpiodev2.h>
#include <linux/mfd/w228x_base.h> 
#include <linux/ad7877.h> // struct ad7877_data
#include <linux/touchscreen-adc.h> // struct tsadc
#include <linux/adc_battery.h> // struct battery_info
#include <linux/pda_power.h> // struct pda_power_pdata
#include <linux/htcathena_vsfb.h>

//#include "htc_bt.h"
//#include "htc_gps.h"

#include "../generic.h"

#if 0

/****************************************************************
 * Phone
 ****************************************************************/

struct phone_funcs {
        void (*configure) (int state);
        void (*suspend) (struct platform_device *dev, pm_message_t state);
        void (*resume) (struct platform_device *dev);
};

static struct phone_funcs phone_funcs;

static void phone_configure (int state)
{
	if (phone_funcs.configure)
		phone_funcs.configure (state);
}

static void phone_suspend (struct platform_device *dev, pm_message_t state)
{
	if (phone_funcs.suspend)
		phone_funcs.suspend (dev, state);
}

static void phone_resume (struct platform_device *dev)
{
	if (phone_funcs.resume)
		phone_funcs.resume (dev);
}

static struct platform_pxa_serial_funcs pxa_phone_funcs = {
	.configure = phone_configure,
	.suspend   = phone_suspend,
	.resume    = phone_resume,
};


static struct platform_device htcathena_phone = {
	.name = "htcathena_phone",
	.dev  = {
		.platform_data = &phone_funcs,
		},
	.id   = -1,
};
#endif

/****************************************************************
 * Bluetooth
 ****************************************************************/

struct htc_bt_funcs {
        void (*configure) (int state);
        void (*suspend) (struct platform_device *dev, pm_message_t state);
        void (*resume) (struct platform_device *dev);
};

static struct htc_bt_funcs bt_funcs;

static void bt_configure (int state)
{
	if (bt_funcs.configure)
		bt_funcs.configure (state);
}

static void bt_suspend (struct platform_device *dev, pm_message_t state)
{
	if (bt_funcs.suspend)
		bt_funcs.suspend (dev, state);
}

static void bt_resume (struct platform_device *dev)
{
	if (bt_funcs.resume)
		bt_funcs.resume (dev);
}

static struct platform_pxa_serial_funcs athena_bt_funcs = {
	.configure = bt_configure,
	.suspend   = bt_suspend,
	.resume    = bt_resume,
};


static struct platform_device htcathena_bt = {
	.name = "htcathena_bt",
	.dev  = {
		.platform_data = &athena_bt_funcs,
		},
	.id   = -1,
};

/****************************************************************
 * GPS
 ****************************************************************/

struct gps_funcs {
        void (*configure) (int state);
        void (*suspend) (struct platform_device *dev, pm_message_t state);
        void (*resume) (struct platform_device *dev);
};

static struct gps_funcs gps_funcs;

static void gps_configure (int state)
{
	if (gps_funcs.configure)
		gps_funcs.configure (state);
}

static void gps_suspend (struct platform_device *dev, pm_message_t state)
{
	if (gps_funcs.suspend)
		gps_funcs.suspend (dev, state);
}

static void gps_resume (struct platform_device *dev)
{
	if (gps_funcs.resume)
		gps_funcs.resume (dev);
}

static struct platform_pxa_serial_funcs athena_gps_funcs = {
	.configure = gps_configure,
	.suspend   = gps_suspend,
	.resume    = gps_resume,
};


static struct platform_device htcathena_gps = {
	.name = "htcathena_gps",
	.dev  = {
		.platform_data = &athena_gps_funcs,
		},
	.id   = -1,
};

/****************************************************************
 * GPIO Keys
 ****************************************************************/

static struct gpio_keys_button htcathena_button_table[] = {
	{KEY_POWER,      GPIO_NR_HTCATHENA_KEY_POWER,       1, "Power button"},
	{KEY_VOLUMEUP,   GPIO_NR_HTCATHENA_KEY_VOL_UP,      0, "Volume slider (up)"},
	{KEY_VOLUMEDOWN, GPIO_NR_HTCATHENA_KEY_VOL_DOWN,    0, "Volume slider (down)"},
	{KEY_CAMERA,     GPIO_NR_HTCATHENA_KEY_CAMERA,      0, "Camera button"},
	{KEY_RECORD,     GPIO_NR_HTCATHENA_KEY_RECORD,      0, "Record button"},
	{KEY_WWW,        GPIO_NR_HTCATHENA_KEY_WWW,         0, "WWW button"},
	{KEY_SEND,       GPIO_NR_HTCATHENA_KEY_SEND,        0, "Send button"},
	{KEY_END,        GPIO_NR_HTCATHENA_KEY_END,         0, "End button"},
	{KEY_RIGHT,      GPIO_NR_HTCATHENA_KEY_RIGHT,       1, "Right button"},
	{KEY_UP,         GPIO_NR_HTCATHENA_KEY_UP,          1, "Up button"},
	{KEY_LEFT,       GPIO_NR_HTCATHENA_KEY_LEFT,        1, "Left button"},
	{KEY_DOWN,       GPIO_NR_HTCATHENA_KEY_DOWN,        1, "Down button"},
	{KEY_KPENTER,    GPIO_NR_HTCATHENA_KEY_ENTER,       1, "Action button"},
	{229,    3,       0, "Windows Button"},
#if 0
	{KEY_F8,         GPIO37_HTCATHENA_KEY_PHONE_HANGUP, 0, "Phone hangup button"},
	{KEY_F10,        GPIO38_HTCATHENA_KEY_CONTACTS,     0, "Contacts button"},
	{KEY_F9,         GPIO90_HTCATHENA_KEY_CALENDAR,     0, "Calendar button"},
	{KEY_PHONE,      GPIO102_HTCATHENA_KEY_PHONE_LIFT,  0, "Phone lift button"},
	{KEY_F13,        GPIO99_HTCATHENA_HEADPHONE_IN,     0, "Headphone switch"},
#endif
};

static struct gpio_keys_platform_data htcathena_gpio_keys_data = {
	.buttons  = htcathena_button_table,
	.nbuttons = ARRAY_SIZE(htcathena_button_table),
};

static struct platform_device htcathena_gpio_keys = {
	.name = "gpio-keys",
	.dev  = {
		.platform_data = &htcathena_gpio_keys_data,
		},
	.id   = -1,
};

/****************************************************************
 * EGPIOs
 ****************************************************************/

static struct resource egpio_cpld2_resources[] = {
       [0] = {
              .start = HTCATHENA_EGPIO_CPLD2_BASE_0, // HACK ALERT!
              .end   = HTCATHENA_EGPIO_CPLD2_BASE_0 + 0x2*2, /* 1 reg  */
              .flags = IORESOURCE_MEM,
       },
};

struct htc_egpio_platform_data egpio_cpld2_data = {
	.bus_shift   = 0,
	.gpio_base   = 2*GPIO_BASE_INCREMENT,
	.nrRegs      = 1,
};

struct platform_device htcathena_cpld2 = {
       .name = "htc-egpio",
       .id   =  1,
       .dev  = {
              .platform_data = &egpio_cpld2_data,
       },
       .resource      = egpio_cpld2_resources,
       .num_resources = ARRAY_SIZE(egpio_cpld2_resources),
};

/*
 * HTC EGPIO on the Xilinx CPLD1
 *
 * 8 16-bit aligned 8-bit registers
 *
 */

static struct resource egpio_cpld1_resources[] = {
	[0] = {
		.start	= HTCATHENA_CPLD1_BASE,
		.end	= HTCATHENA_CPLD1_BASE + 0x20,
		.flags	= IORESOURCE_MEM,
	},
//	[1] = {
//		.start	= gpio_to_irq(GPIO_NR_HTCATHENA_CPLD1_EXT_INT),
//		.end	= gpio_to_irq(GPIO_NR_HTCATHENA_CPLD1_EXT_INT),
//		.flags	= IORESOURCE_IRQ,
//	},
};

struct htc_egpio_pinInfo egpio_cpld1_pins[] = {
#if 0
	/* Output pins that default on */
	{
		.gpio           = EGPIO_MAGICIAN_GSM_RESET,
		.type           = HTC_EGPIO_TYPE_OUTPUT,
		.output_initial = 1,
	},
	{
		.gpio           = EGPIO_MAGICIAN_IN_SEL1,
		.type           = HTC_EGPIO_TYPE_OUTPUT,
		.output_initial = 1,
	},
#endif
	/* Input pins with associated IRQ */
	{
		.type           = HTC_EGPIO_TYPE_INPUT,
		.gpio           = EGPIO1_HTCATHENA_USB_DETECT,
		.input_irq      = EGPIO1_HTCATHENA_USB_DETECT,
			/* 'pin' corresponding to IRQ_MAGICIAN_EP_IRQ */
	},
};

struct htc_egpio_platform_data egpio_cpld1_data = {
	.bus_shift   = 0,
	.irq_base    = IRQ_BOARD_START,		/* 16 IRQs, we only have 4 though */
	.gpio_base   = 1*GPIO_BASE_INCREMENT,
	.nrRegs      = 9,
//	.ackRegister = 4,
//	.pins        = egpio_cpld1_pins,
//	.nr_pins     = ARRAY_SIZE(egpio_cpld1_pins),
};

struct platform_device htcathena_cpld1 = {
	.name = "htc-egpio",
	.id   = 0,
	.dev  = {
		.platform_data = &egpio_cpld1_data,
	},
	.resource      = egpio_cpld1_resources,
	.num_resources = ARRAY_SIZE(egpio_cpld1_resources),
};

/****************************************************************
 * SSP Ports
 ****************************************************************/

struct platform_device ssp_device = {
	.name		= "htc_athena-ssp",
	.id		= -1,
};

/****************************************************************
 * Touchscreen
 ****************************************************************/

static struct ad7877_platform_data ad7877_data = {
	.dav_irq = IRQ_GPIO(GPIO_NR_HTCATHENA_TS_DAV),
};

static struct platform_device ad7877 = {
	.name = "ad7877",
	.id = -1,
	.dev    = {
		.platform_data = &ad7877_data,
	},
};

static struct tsadc_platform_data tsadc = {
	.pressure_factor = 100000, // XXX - adjust value
	.max_sense = 4096,
	.min_pressure = 1,   // XXX - don't know real value.
	.pen_gpio = GPIO_NR_HTCATHENA_TS_PENDOWN,

	.x_pin = "ad7877:x",
	.y_pin = "ad7877:y",
	.z1_pin = "ad7877:z1",
	.z2_pin = "ad7877:z2",
	.num_xy_samples = 1,
	.num_z_samples = 1,
};

static struct resource htcathena_pen_irq = {
	.start	= IRQ_GPIO(GPIO_NR_HTCATHENA_TS_PENDOWN),
	.end 	= IRQ_GPIO(GPIO_NR_HTCATHENA_TS_PENDOWN),
	.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_LOWEDGE,
};

static struct platform_device htcathena_ts = {
	.name = "ts-adc",
	.id = -1,
	.resource = &htcathena_pen_irq,
	.num_resources = 1,
	.dev    = {
		.platform_data = &tsadc,
	},
};

/****************************************************************
 * Graphic/Video
 ****************************************************************/

struct fb_var_screeninfo htcathena_ati2284_var = {
	.yres		= W2284_YRES, //Height
	.xres		= W2284_XRES, //Width
	.yres_virtual	= W2284_YRES_VIRTUAL,
	.xres_virtual	= W2284_XRES_VIRTUAL,
	.bits_per_pixel	= W2284_BITS_PER_PIXEL,
	.red		= { 11, 5, 0 },
	.green		= {  5, 6, 0 },
	.blue		= {  0, 5, 0 },
	.activate	= FB_ACTIVATE_NOW,
	.height		= -1,
	.width		= -1,
	.vmode		= FB_VMODE_NONINTERLACED,
};

struct fb_fix_screeninfo htcathena_ati2284_fix = {
	.id		= "ATI W2884",
	.smem_start	= ATI_FB_SRAM_PADDR,
	.smem_len	= W2284_YRES_VIRTUAL*W2284_XRES_VIRTUAL*(W2284_BITS_PER_PIXEL/8),
	.type		= FB_TYPE_PACKED_PIXELS,
	.visual		= FB_VISUAL_TRUECOLOR,
	.line_length	= W2284_XRES*(W2284_BITS_PER_PIXEL/8),
	.accel		= FB_ACCEL_NONE,
	.ypanstep	= 1,
};

static struct vsfb_deviceinfo htcathena_atiw2284_deviceinfo = {
	.fix = &htcathena_ati2284_fix,
	.var = &htcathena_ati2284_var,
};

static struct platform_device htcathena_graphics = {
        .name           = "ATI W2884",
        .id             = -1,
        .dev            = {
                .platform_data  = &htcathena_atiw2284_deviceinfo,
        },
};

/****************************************************************
 * Battery monitoring
 ****************************************************************/

static struct battery_adc_platform_data athena_main_batt_params = {
	.battery_info = {
		.name = "main-battery",
		.voltage_max_design = 4200000,
		.voltage_min_design = 3500000,
		.charge_full_design = 1000000,
		.use_for_apm = 1,
	},
	.voltage_pin = "ad7877:aux1",
	.current_pin = "ad7877:aux2",
	.temperature_pin = "ad7877:bat1",
	/* Coefficient is 1.08 */
	.voltage_mult = 1080,
	/* Coefficient is 1.13 */
	.current_mult = 1130,
};

static struct platform_device athena_main_batt = {
	.name = "adc-battery",
	.id = -1,
	.dev = {
		.platform_data = &athena_main_batt_params,
	}
};

/****************************************************************
 * Power management
 ****************************************************************/

static void set_charge(int flags)
{
//	gpio_set_value(EGPIO_NR_HTCAPACHE_USB_PWR
//		       , flags == PDA_POWER_CHARGE_USB);

	// XXX - enable/disable battery charger once charge complete
	// detection available.
}

static int ac_on(void)
{
//	int haspower = !gpio_get_value(EGPIO_NR_HTCAPACHE_PWR_IN_PWR);
//	int hashigh = !gpio_get_value(EGPIO_NR_HTCAPACHE_PWR_IN_HIGHPWR);
//	return haspower && hashigh;
 return 0;
}

static int usb_on(void)
{
//	int haspower = !gpio_get_value(EGPIO_NR_HTCAPACHE_PWR_IN_PWR);
//	int hashigh = !gpio_get_value(EGPIO_NR_HTCAPACHE_PWR_IN_HIGHPWR);
//	return haspower && !hashigh;
 return 0;
}

static struct pda_power_pdata power_pdata = {
	.is_ac_online	= ac_on,
	.is_usb_online	= usb_on,
	.set_charge	= set_charge,
};

static struct resource power_resources[] = {
	[0] = {
		.name	= "ac",
//		.start	= IRQ_EGPIO(EGPIO_NR_HTCAPACHE_PWR_IN_PWR),
//		.end	= IRQ_EGPIO(EGPIO_NR_HTCAPACHE_PWR_IN_PWR),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device power_dev = {
	.name		= "pda-power",
	.id		= -1,
	.resource	= power_resources,
	.num_resources	= ARRAY_SIZE(power_resources),
	.dev = {
		.platform_data	= &power_pdata,
	},
};


static struct platform_device *devices[] __initdata = {
	&htcathena_cpld1,
	&htcathena_cpld2,
	&ad7877,
//	&power_dev,
	&athena_main_batt,
	&htcathena_ts,
//	&htcathena_phone,
//	&htcathena_flash,
	&htcathena_gpio_keys,
	&htcathena_gps,
	&htcathena_bt,
	&ssp_device,
	&htcathena_graphics
};

/****************************************************************
 * USB client controller
 ****************************************************************/

//	.gpio_pullup	= EGPIO1_HTCATHENA_USB_PUEN,

static int athena_ohci_init(struct device *dev)
{
	/* setup Port1 GPIO pin. */
	pxa_gpio_mode( 88 | GPIO_ALT_FN_1_IN);	/* USBHPWR1 */
	pxa_gpio_mode( 89 | GPIO_ALT_FN_2_OUT);	/* USBHPEN1 */

	/* Set the Power Control Polarity Low and Power Sense
	   Polarity Low to active high. */
	UHCHR = (UHCHR | UHCHR_PSPL| UHCHR_SSEP2 | UHCHR_CGR) &
		~(UHCHR_SSEP1 | UHCHR_PCPL | UHCHR_SSEP3 | UHCHR_SSE);

	UHCRHDA &= ~UHCRHDA_NOCP;

	return 0;
}

/****************************************************************
 * USB host controller
 ****************************************************************/

static struct pxaohci_platform_data athena_ohci_platform_data = {
	.port_mode	= PMM_PERPORT_MODE,
	.init		= athena_ohci_init,
	.power_budget	= 500,	// ???
};

/* ATI W2284 */
static void athena_atiw228x_sdio_setpower(struct device *dev, unsigned int vdd)
{
//	struct pxamci_platform_data* p_d = dev->platform_data;

//        if ((1 << vdd) & p_d->ocr_mask)
        if ((1 << vdd) & MMC_VDD_32_33)
		gpio_set_value(EGPIO1_HTCATHENA_SDIO_POWER, 1);
	else
		gpio_set_value(EGPIO1_HTCATHENA_SDIO_POWER, 0);
}

static struct atiw_mmc_hwconfig athena_atiw_mmc_hwconfig = {
        .setpower = athena_atiw228x_sdio_setpower,
};

static struct w228x_platform_data htcathena_w2284_platform_data = {

	.bus_shift = 2,

//	.child_devs     = htcuniversal_asic3_devices,
//	.num_child_devs = ARRAY_SIZE(htcuniversal_asic3_devices),

	.atiw_mmc_hwconfig = &athena_atiw_mmc_hwconfig,
};

static struct resource htcathena_w2284_resources[] = {
	[0] = {
		.start	= HTCATHENA_W2284_BASE_PHYS,
		.end	= HTCATHENA_W2284_CTRL_PHYS + 0x10000,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_GPIO(31), // fake
		.end	= IRQ_GPIO(31),
		.flags	= IORESOURCE_IRQ,
	},
	[2] = {
		.start  = HTCATHENA_W2284_MMC_PHYS,
		.end    = HTCATHENA_W2284_MMC_PHYS + 0x4000,
		.flags  = IORESOURCE_MEM,
	},
	[3] = {
		.start  = IRQ_GPIO(GPIO_NR_HTCATHENA_ATI_INT),
		.end  	= IRQ_GPIO(GPIO_NR_HTCATHENA_ATI_INT),
		.flags  = IORESOURCE_IRQ,
	},
};

struct platform_device htcathena_w2284 = {
	.name           = "w228x",
	.id             = 0,
	.num_resources  = ARRAY_SIZE(htcathena_w2284_resources),
	.resource       = htcathena_w2284_resources,
	.dev = { .platform_data = &htcathena_w2284_platform_data, },
};
EXPORT_SYMBOL_GPL(htcathena_w2284);

static void __init htcathena_init(void)
{
	platform_device_register(&htcathena_w2284);

	platform_add_devices( devices, ARRAY_SIZE(devices) );
//	pxa_set_btuart_info(&htcathena_pxa_phone_funcs);

	mdelay(5000);

#if 0
	printk("HTCATHENA: isp1582=%d, board=%d\n",
	 gpio_get_value(EGPIO1_HTCATHENA_ISP1582_ON),0);
//	 EGPIO1_HTCATHENA_ISP1582_ON,EGPIO2_HTCATHENA_BOARDID0);

	printk("HTCATHENA: board_id=%d\n",
	 gpio_get_value(EGPIO2_HTCATHENA_BOARDID0)*1+
	 gpio_get_value(EGPIO2_HTCATHENA_BOARDID1)*2+
	 gpio_get_value(EGPIO2_HTCATHENA_BOARDID1)*4
	);
#endif

//	pxa_set_mci_info(&athena_mci_platform_data);
//	pxa_set_ficp_info(&athena_ficp_platform_data);
	pxa_set_ohci_info(&athena_ohci_platform_data);

}

MACHINE_START(HTCATHENA, "HTC Athena")
	.phys_io	= 0x40000000,
	.io_pg_offst	= io_p2v(0x40000000),
	.boot_params	= 0xa0000100,
	.map_io 	= pxa_map_io,
	.init_irq	= pxa27x_init_irq,
	.timer  	= &pxa_timer,
	.init_machine	= htcathena_init,
MACHINE_END
