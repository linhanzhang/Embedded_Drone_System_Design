
/*------------------------------------------------------------
 * console I/O
 *------------------------------------------------------------
 */


#ifndef CONSOLE_IO_H
#define CONSOLE_IO_H

void term_initio(void);
void term_exitio(void);
void term_puts(char *s);
void term_putchar(char c);
void term_putchar_int(uint8_t c);
int	term_getchar_nb(void);
int	term_getchar(void);

#endif //CONSOLE_IO_H