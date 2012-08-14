#ifndef LCD_LIB_H
#define LCD_LIB_H

void lcd_setLine(unsigned char line);
void lcd_setDisplay(unsigned char config);
void lcd_setEntry(unsigned char config);
void lcd_sendData(unsigned char i);
void lcd_waitUntilReady(void);
void lcd_write(char *stringIn);
void lcd_init(void);

#endif
