/* Definitions for the HTC SPI keyboard driver found in
 * drivers/input/keyboard/htc-spi-kbd.c
 */


#define HTC_SSP_KBD_MAX_KEYS 64

struct htcathena_ssp_keyboard_button {
	/* Configuration parameters */
	int id;
	int keycode;		/* input event code (KEY_*, SW_*) */
	//char *desc;
};

struct htcathena_ssp_keyboard_platform_data {
	struct htcathena_ssp_keyboard_button keys[HTC_SSP_KBD_MAX_KEYS];
};
