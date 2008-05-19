#ifndef _W228X_BASE_H
#define _W228X_BASE_H

#include <asm/types.h>

struct atiw_mmc_hwconfig {
	void (*setpower)(struct platform_device *dev, int state);
	int (*mmc_get_ro)(struct platform_device *pdev);
	unsigned int address_shift;
};

extern int  asic3_irq_base(struct device *dev);
extern void asic3_write_register(struct device *dev, unsigned int reg, u32 value);
extern u32  asic3_read_register(struct device *dev, unsigned int reg);
extern int  w228x_register_mmc(struct device *dev);
extern int  w228x_unregister_mmc(struct device *dev);

struct w228x_platform_data
{
	// Must be first member
//	struct gpiodev_ops gpiodev_ops;

	/* Standard MFD properties */
	int irq_base;
	int gpio_base;
	struct platform_device **child_devs;
	int num_child_devs;

#if 0
	struct {
		u32 dir;
		u32 init;
		u32 sleep_mask;
		u32 sleep_out;
		u32 batt_fault_out;
		u32 sleep_conf;
		u32 alt_function;
	} gpio_a, gpio_b, gpio_c, gpio_d;
#endif
	unsigned int bus_shift;

	struct atiw_mmc_hwconfig *atiw_mmc_hwconfig;
};

#endif /* _W228X_BASE_H */

