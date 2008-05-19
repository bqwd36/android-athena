/* drivers/video/htcathena_vsfb.h
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
*/

#include <linux/fb.h>
#ifdef CONFIG_ANDROID_POWER
#include <linux/android_power.h>
#endif

#define W2284_YRES		640
#define W2284_XRES		480
#define W2284_YRES_VIRTUAL	W2284_YRES*2
#define W2284_XRES_VIRTUAL	W2284_YRES
#define W2284_BITS_PER_PIXEL	16

#define ATI_FB_SRAM_PADDR	0x04800000
#define ATI_CNRTL_REGS_PADDR	0x04000000

#define mmCHIP_ID		0x10000
#define mmGRAPHIC_CTRL		0x10414
#define mmGRAPHIC_PITCH		0x1041c
#define mmGRAPHIC_OFFSET	0x10418
#define mmDST_OFFSET		0x11004

struct vsfb_deviceinfo
{
	struct fb_var_screeninfo *var;
	struct fb_fix_screeninfo *fix;
};

struct graphic_ctrl_s {
	u32 color_depth		: 3;
	u32 portrait_mode	: 2;
	u32 low_power_on	: 1;
	u32 req_freq		: 4;
	u32 en_crtc		: 1;
	u32 en_graphic_req	: 1;
	u32 en_graphic_crtc	: 1;
	u32 total_req_graphic	: 9;
	u32 lcd_pclk_on		: 1;
	u32 lcd_sclk_on		: 1;
	u32 pclk_running	: 1;
	u32 sclk_running	: 1;
	u32			: 6;
} __attribute__((packed));

union graphic_ctrl_u {
	u32 val			: 32;
	struct graphic_ctrl_s	f_w2284;
} __attribute__((packed));

struct atiw2284_fb {
	uint32_t		reg_base;
	char __iomem		*flip_offset;
	int			rotation;
	struct			fb_info info;
	u32			colreg[17];
#ifdef CONFIG_ANDROID_POWER
        android_early_suspend_t	early_suspend;
#endif
};

static int ati2884_setcolreg(u_int regno, u_int red, u_int green, u_int blue, u_int trans, struct fb_info *info);
static int ati2884_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
