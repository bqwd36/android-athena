/*
 *  linux/drivers/video/htcathena_vsfb.c
 *
 * Based on vsfb by Ian Molton
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Frame buffer code for Simple platforms
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>

#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach-types.h>
#include <asm/uaccess.h>

#include <linux/htcathena_vsfb.h>



//static struct platform_device *atiw2284_device;

static struct fb_ops atiw2284_ops = {
	.owner		= THIS_MODULE,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
	.fb_setcolreg	= ati2884_setcolreg,
	.fb_pan_display = ati2884_pan_display,
};
/*
struct fb_var_screeninfo atiw2884_var = {
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

struct fb_fix_screeninfo atiw2884_fix = {
	.id		= "vsfb",
	.smem_start	= ATI_FB_SRAM_PADDR,
	.smem_len	= W2284_YRES_VIRTUAL*W2284_XRES_VIRTUAL*(W2284_BITS_PER_PIXEL/8),
	.type		= FB_TYPE_PACKED_PIXELS,
	.visual		= FB_VISUAL_TRUECOLOR,
	.line_length	= W2284_XRES*(W2284_BITS_PER_PIXEL/8),
	.accel		= FB_ACCEL_NONE,
	.ypanstep	= 1,
};
*/
static int ati2884_setcolreg(u_int regno, u_int red, u_int green, u_int blue, u_int trans, struct fb_info *info)
{
	if (regno > 16)
		return 1;

	((u32 *)(info->pseudo_palette))[regno] = (red & 0xf800) | ((green & 0xfc00) >> 5) | ((blue & 0xf800) >> 11);

	return 0;
}

static int ati2884_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	if (var->xoffset + var->xres > info->var.xres_virtual || var->yoffset + var->yres > info->var.yres_virtual)
		return -EINVAL;

	if (var->yoffset == 0) {
		var->yoffset = 640;
	} else {
		var->yoffset = 0;
	}

	return 0;
}

static int atiw2284_probe(struct platform_device *pdev) {
	struct atiw2284_fb *fb;
	struct vsfb_deviceinfo *vsfb;

	vsfb = pdev->dev.platform_data;
	if(!vsfb)
		return -ENODEV;

	fb = kzalloc(sizeof(*fb), GFP_KERNEL);
	if(fb == NULL) {
		return -ENOMEM;
	}

	//platform_set_drvdata(pdev, fb);

	//fb->info.fix			= atiw2884_fix;
	//fb->info.var			= atiw2884_var;
	memcpy (&fb->info.fix, vsfb->fix, sizeof(struct fb_fix_screeninfo));
	memcpy (&fb->info.var, vsfb->var, sizeof(struct fb_var_screeninfo));

	fb->info.fbops			= &atiw2284_ops;
	fb->info.flags			= FBINFO_FLAG_DEFAULT;
	fb->info.pseudo_palette		= fb->colreg;
	fb->reg_base 			= ioremap_nocache((unsigned long)(ATI_CNRTL_REGS_PADDR), (size_t)(0x20000));

	fb_alloc_cmap(&fb->info.cmap, 16, 0);  //FIXME - needs to work for all depths

	/* Try to grab our phys memory space... */
	if (!(request_mem_region(fb->info.fix.smem_start, fb->info.fix.smem_len, fb->info.fix.id))){
		return -ENOMEM;
	}

	/* Try to map this so we can access it */
	fb->info.screen_base = ioremap(fb->info.fix.smem_start, fb->info.fix.smem_len);
	if (!fb->info.screen_base) {
		release_mem_region(fb->info.fix.smem_start, fb->info.fix.smem_len);
		return -EIO;
	}

	fb->flip_offset = fb->info.screen_base;

	printk(KERN_INFO "ATI W2884: mmCHIP_ID: 0x%08x\n", fb_readl(fb->reg_base+mmCHIP_ID));
	printk(KERN_INFO "ATI W2884: Framebuffer at 0x%lx, mapped to 0x%p, size %dk\n", fb->info.fix.smem_start, fb->info.screen_base, fb->info.fix.smem_len/1024);

	if (register_framebuffer(&fb->info) < 0){
		iounmap(fb->info.screen_base);
		release_mem_region(fb->info.fix.smem_start, fb->info.fix.smem_len);
		return -EINVAL;
	}

	return 0;
}

struct platform_driver atiw2284_driver = {
	.driver = {
		.name = "ATI W2884",
	},
	.probe = atiw2284_probe,
//	.remove = vsfb_remove,
};

static int __init atiw2284_init(void)
{
//	int ret = 0;

//	ret = platform_driver_register (&atiw2284_driver);
	platform_driver_register(&atiw2284_driver);

/*
	if (!ret) {
		atiw2284_device = platform_device_alloc("ATI W2884", 0);

		if (atiw2284_device)
			ret = platform_device_add(atiw2284_device);
		else
			ret = -ENOMEM;

		if (ret) {
			platform_device_put(atiw2284_device);
			platform_driver_unregister(&atiw2284_driver);
		}
	}
*/
	return 0;
}

#if 0
static void __exit tmio_mmc_exit(void)
{
         return platform_driver_unregister (&tmio_mmc_driver);
}
#endif

module_init(atiw2284_init);

MODULE_AUTHOR("David Roth");
MODULE_DESCRIPTION("Very Simple framebuffer driver for the HTC Athena");
MODULE_LICENSE("GPL");
