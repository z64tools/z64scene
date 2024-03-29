#include <n64.h>

unsigned char sGfxRCP[] = {
	// 0x00
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x30, 0xB3, 0xFF, 0x5F, 0xFE, 0xFE, 0x38,
	0xEF, 0x18, 0x2C, 0xA0, 0xC8, 0x10, 0x4B, 0x50,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x21, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x01
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x78, 0x38,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x20, 0x48,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x02
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x79, 0x3C,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x03
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x78, 0x38,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x04
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x32, 0x7E, 0x64, 0xFF, 0xFF, 0xF7, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x49, 0xD8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x05
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xFE, 0x23, 0xFF, 0xFF, 0xF7, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x49, 0xD8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x06
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF2, 0x38,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x30, 0x48,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x07
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x41, 0xC8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x08
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x79, 0x3C,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x20, 0x48,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x09
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x18, 0x04, 0xFF, 0x13, 0xFF, 0xFF,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x10, 0x41, 0xC8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x0A
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7E, 0x24, 0xFF, 0xFF, 0xF3, 0xF9,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x20, 0x48,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x0B
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7E, 0x03, 0xFF, 0x0F, 0xF3, 0xFF,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x20, 0x48,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x0C
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xFE, 0x23, 0xFF, 0xFF, 0xF7, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x20, 0x48,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x0D
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x42, 0x40,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x0E
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF2, 0x79,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x30, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x0F
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF2, 0x38,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x30, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x10
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x03, 0xFF, 0x0F, 0xFF, 0xFF,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x10, 0x49, 0xD8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x11
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7E, 0x24, 0xFF, 0xFF, 0xF3, 0xF9,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x30, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x12
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7F, 0xFF, 0xFF, 0xFF, 0xF2, 0x38,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x30, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x13
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x52, 0x58,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x14
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x4B, 0x51,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x15
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x18, 0x24, 0xFF, 0x33, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x49, 0xD8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x16
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x18, 0x04, 0xFF, 0x13, 0xFF, 0xFF,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x10, 0x49, 0xD8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x17
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7E, 0x24, 0xFF, 0xFF, 0xF3, 0xF9,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x18
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7E, 0x04, 0xFF, 0x13, 0xF3, 0xFF,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x19
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7E, 0x03, 0xFF, 0x0F, 0xF3, 0xFF,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x1A
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xFE, 0x23, 0xFF, 0xFF, 0xF7, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x1B
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xFE, 0x23, 0xFF, 0xFF, 0xF7, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x49, 0xD8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x1C
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xFE, 0x23, 0xFF, 0xFF, 0xF3, 0xF9,
	0xEF, 0x00, 0xAC, 0x30, 0x00, 0x55, 0x30, 0x49,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x1D
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x18, 0xE0, 0xFF, 0xFF, 0xFF, 0xF8,
	0xEF, 0x18, 0x2C, 0x10, 0x0C, 0x19, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x1E
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF6, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x78, 0x58,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x1F
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x30, 0xB2, 0x61, 0x44, 0x66, 0x49, 0x24,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x26, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x20
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x30, 0xB2, 0x61, 0x44, 0x66, 0x49, 0x24,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x21
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xE2, 0xA1, 0xF0, 0xA6, 0x78, 0x00,
	0xEF, 0x18, 0x2C, 0x10, 0x0C, 0x19, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x22
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0x7C,
	0xEF, 0x00, 0x2C, 0x30, 0x0F, 0x0A, 0x40, 0x01,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x23
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF2, 0x79,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x4D, 0xD8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x24
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x79, 0x3C,
	0xEF, 0x30, 0x2C, 0x30, 0x00, 0x00, 0x00, 0x01,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x25
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x32, 0x7F, 0xFF, 0xFF, 0xFF, 0xF6, 0x38,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x11, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x26
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x79, 0x3C,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x49, 0xD8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x27
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x00, 0x2C, 0x30, 0x00, 0x50, 0x42, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x28
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x25, 0x27, 0xFF, 0x1F, 0xFC, 0x92, 0x38,
	0xEF, 0x18, 0x2C, 0x10, 0x0F, 0x0A, 0x40, 0x00,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x02, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x29
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x18, 0x24, 0xFF, 0x33, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x4A, 0x50,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x2A
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7E, 0x24, 0xFF, 0xFF, 0xF3, 0xF9,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x42, 0x40,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x2B
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xFE, 0x23, 0xFF, 0xFF, 0xF3, 0xF9,
	0xEF, 0x00, 0xAC, 0x30, 0x00, 0x50, 0x42, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x2C
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x97, 0xFF, 0xFF, 0xFF, 0xFE, 0x38,
	0xEF, 0x18, 0x2C, 0x10, 0xC8, 0x10, 0x4F, 0x50,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x21, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x2D
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF2, 0xF9,
	0xEF, 0x00, 0x2C, 0x30, 0x00, 0x50, 0x42, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x2E
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF2, 0xF9,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x42, 0x40,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x2F
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xFE, 0x23, 0xFF, 0xFF, 0xF7, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x4A, 0x50,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x30
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF6, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x41, 0xC8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x31
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x32, 0x7E, 0x64, 0xFF, 0xFF, 0xF7, 0xFB,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x41, 0xC8,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x32
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x00, 0x2C, 0x30, 0x0F, 0x0A, 0x40, 0x01,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x33
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x42, 0x40,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x34
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x00, 0x2C, 0x30, 0x00, 0x50, 0x42, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x35
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x26, 0x2A, 0x60, 0x15, 0x0C, 0x93, 0x7F,
	0xEF, 0x10, 0x2C, 0x30, 0x0F, 0x0A, 0x40, 0x01,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x36
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x26, 0x2A, 0x60, 0x15, 0x0C, 0x93, 0x7F,
	0xEF, 0x10, 0x2C, 0x30, 0x0C, 0x18, 0x42, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x37
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x26, 0x2A, 0x60, 0x15, 0x0C, 0x93, 0x7F,
	0xEF, 0x10, 0x2C, 0x30, 0x0C, 0x18, 0x42, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x38
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x00, 0x0C, 0x30, 0x00, 0x50, 0x42, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x39
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF6, 0xFB,
	0xEF, 0x00, 0x2C, 0x30, 0x00, 0x50, 0x43, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x3A
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x32, 0x38, 0x64, 0xFF, 0x73, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x43, 0x40,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x3B
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0xE2, 0xA1, 0xF0, 0xA6, 0x78, 0x00,
	0xEF, 0x18, 0x2C, 0x10, 0x0C, 0x19, 0x20, 0x48,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x04, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x3C
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x30, 0xB2, 0x61, 0x55, 0x66, 0xFF, 0x7F,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x4B, 0x50,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x3D
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x30, 0xB2, 0x61, 0x55, 0x66, 0xFF, 0x7F,
	0xEF, 0x08, 0x2C, 0xA0, 0x00, 0x50, 0x4B, 0x50,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x3E
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x12, 0x7E, 0x03, 0xFF, 0x0F, 0xF3, 0xFF,
	0xEF, 0x18, 0x2C, 0xD0, 0xC8, 0x11, 0x20, 0x78,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x23, 0x04, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x3F
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x00, 0x2C, 0x30, 0x00, 0x50, 0x43, 0x61,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x40
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF2, 0x38,
	0xEF, 0x10, 0x2C, 0x30, 0x0C, 0x18, 0x42, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x41
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x50, 0x43, 0x41,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x42
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF2, 0x79,
	0xEF, 0x08, 0x2C, 0x10, 0x0F, 0x0A, 0x32, 0x31,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x43
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x0F, 0x0A, 0x32, 0x31,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x44
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF2, 0x79,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x32, 0x71,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x45
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0x11, 0x96, 0x23, 0xFF, 0x2F, 0xFF, 0xFF,
	0xEF, 0x08, 0x2C, 0x10, 0x00, 0x55, 0x32, 0x71,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0x46
	0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD7, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF2, 0x79,
	0xEF, 0x08, 0x2C, 0x10, 0x0F, 0x0A, 0x32, 0x31,
	0xD9, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x05,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

Gfx* gSetupDL = (Gfx*)sGfxRCP;
