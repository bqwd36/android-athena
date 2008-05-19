/*
 * Driver interface to ATI W228x
 *
 * Copyright 2001 Compaq Computer Corporation.
 * Copyright 2004-2005 Phil Blundell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, OR
 * (at your option) ANY LATER VERSION.
 *
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/irq.h>

#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <linux/mfd/w228x_base.h>
#include "soc-core.h"


struct w228x_data {
	void *mapping;
	unsigned int bus_shift;
	int irq_base;
	int irq_nr;

	u16 irq_bothedge[4];
	struct device *dev;

	struct platform_device *mmc_dev;
//	struct platform_device *i2s_dev;
};

static void w228x_release(struct device *dev)
{
	struct platform_device *sdev = to_platform_device(dev);

	kfree(sdev->resource);
	kfree(sdev);
}

int w228x_register_mmc(struct device *dev)
{
	struct platform_device *sdev = kzalloc(sizeof(*sdev), GFP_KERNEL);
	struct atiw_mmc_hwconfig *mmc_config = kmalloc(sizeof(*mmc_config),
						       GFP_KERNEL);
	struct platform_device *pdev = to_platform_device(dev);
	struct w228x_data *asic = dev->driver_data;
	struct w228x_platform_data *w228x_pdata = dev->platform_data;
	struct resource *res;
	int rc;

	if (sdev == NULL || mmc_config == NULL)
		return -ENOMEM;

	if (w228x_pdata->atiw_mmc_hwconfig) {
		memcpy(mmc_config, w228x_pdata->atiw_mmc_hwconfig,
		       sizeof(*mmc_config));
	} else {
		memset(mmc_config, 0, sizeof(*mmc_config));
	}
	mmc_config->address_shift = asic->bus_shift;

	sdev->id = -1;
	sdev->name = "atiw_mmc";
	sdev->dev.parent = dev;
	sdev->num_resources = 2;
	sdev->dev.platform_data = mmc_config;
	sdev->dev.release = w228x_release;

	res = kzalloc(sdev->num_resources * sizeof(struct resource),
		      GFP_KERNEL);
	if (res == NULL) {
		kfree(sdev);
		kfree(mmc_config);
		return -ENOMEM;
	}
	sdev->resource = res;

	res[0].start = pdev->resource[2].start;
	res[0].end   = pdev->resource[2].end;
	res[0].flags = IORESOURCE_MEM;
	res[1].start = res[1].end = pdev->resource[3].start;
	res[1].flags = IORESOURCE_IRQ;

	printk("W228X: registering %s @ %x\n",sdev->name, pdev->resource[2].start);
	rc = platform_device_register(sdev);
	if (rc) {
		printk(KERN_ERR "w228x_base: "
		       "Could not register atiw_mmc device\n");
		kfree(res);
		kfree(sdev);
		return rc;
	}

	asic->mmc_dev = sdev;

	return 0;
}
EXPORT_SYMBOL_GPL(w228x_register_mmc);

int w228x_unregister_mmc(struct device *dev)
{
	struct w228x_data *asic = dev->driver_data;
	platform_device_unregister(asic->mmc_dev);
	asic->mmc_dev = 0;

	return 0;
}
EXPORT_SYMBOL_GPL(w228x_unregister_mmc);

static int w228x_remove(struct platform_device *pdev)
{
	struct w228x_platform_data *pdata = pdev->dev.platform_data;
	struct w228x_data *asic = platform_get_drvdata(pdev);
	int i;

	if (asic->mmc_dev)
		w228x_unregister_mmc(&pdev->dev);

	iounmap(asic->mapping);

	kfree(asic);

	return 0;
}

static int w228x_probe(struct platform_device *pdev)
{
	struct w228x_platform_data *pdata = pdev->dev.platform_data;
	struct w228x_data *asic;
	struct device *dev = &pdev->dev;
	unsigned long clksel;
	int i, rc;

	asic = kzalloc(sizeof(struct w228x_data), GFP_KERNEL);
	if (!asic)
		return -ENOMEM;

	platform_set_drvdata(pdev, asic);
	asic->dev = &pdev->dev;

	printk("W228X: remapping=%x\n",pdev->resource[0].start);
	asic->mapping = ioremap(pdev->resource[0].start, 0x20000);	// TODO.
	if (!asic->mapping) {
		printk(KERN_ERR "w228x: couldn't ioremap ATIW228x\n");
		kfree (asic);
		return -ENOMEM;
	}

	if (pdata && pdata->bus_shift)
		asic->bus_shift = pdata->bus_shift;
	else
		asic->bus_shift = 2;

	printk("W228X: num_resources=%d\n",pdev->num_resources);
	if (pdev->num_resources > 2) {
		int rc;
		rc = w228x_register_mmc(dev);
		if (rc) {
			w228x_remove(pdev);
			return rc;
		}
	}

	return 0;
}


static void w228x_shutdown(struct platform_device *pdev)
{
}

static int w228x_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct w228x_data *asic = platform_get_drvdata(pdev);

	return 0;
}

static int w228x_resume(struct platform_device *pdev)
{
	struct w228x_data *asic = platform_get_drvdata(pdev);

	return 0;
}

static struct platform_driver w228x_device_driver = {
	.driver		= {
		.name	= "w228x",
	},
	.probe		= w228x_probe,
	.remove		= w228x_remove,
	.suspend	= w228x_suspend,
	.resume		= w228x_resume,
	.shutdown	= w228x_shutdown,
};

static int __init w228x_base_init(void)
{
	int retval = 0;
	retval = platform_driver_register(&w228x_device_driver);
//	printk("W228X: base_init+register ret=%d\n",retval);
	return retval;
}

static void __exit w228x_base_exit(void)
{
	platform_driver_unregister(&w228x_device_driver);
}

#ifdef MODULE
module_init(w228x_base_init);
#else	/* start early for dependencies */
subsys_initcall(w228x_base_init);
#endif
module_exit(w228x_base_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phil Blundell <pb@handhelds.org>");
MODULE_DESCRIPTION("Core driver for ATI W228x");
MODULE_SUPPORTED_DEVICE("w228x");
