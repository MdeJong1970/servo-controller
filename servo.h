// servo: device driver for controlling servos through a PCA9685
//
// Mechanismen:
//  * Dateischnittstelle: open(), close(), write()
//  * GPIO's
//  * dynamisch oder statisch vergebene Major-Number
//  * Generierung eines /sys-FS-Eintrages inkl. UEVENT für udevd bzw. mdev
//

#define MAX_CONTROLLER		4
#define SERVO_PER_CONTROLLER	16

typedef struct{
	unsigned char i2c_bus[128];
	unsigned int slave_address;
	unsigned int size;
	unsigned char name[128];
}eeprom_info_t;

typedef struct{
	unsigned char i2c_bus[128];
	unsigned int slave_address;
	unsigned int prescaler;
	unsigned int mode2;
	unsigned int update-mode;
	unsigned int servo_oe_gpio;
}controller_info_t;

typedef struct{
	unsigned int controller;
	unsigned int period;
	unsigned int min_pulse;
	unsigned int zero_pulse;
	unsigned int max_pulse;
} servo_info_t;

typedef struct{
	eeprom_info_t eeprom_info;
	controller_info_t controller_info[MAX_CONTROLLER];
	servo_info_t servo_info[MAX_CONTROLLER*SERVO_PER_CONTROLLER];
} servo_controller_info_t;

