
#ifndef SOURCES_PROJECTS_SOURCE_MODULE_SHELL_SHELL_PORT_H_
#define SOURCES_PROJECTS_SOURCE_MODULE_SHELL_SHELL_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int usb_putchar(char x);
int usb_printf(const char * str /*format*/, ...);

#ifdef __cplusplus
}
#endif

#endif  // SOURCES_PROJECTS_SOURCE_MODULE_SHELL_SHELL_PORT_H_
