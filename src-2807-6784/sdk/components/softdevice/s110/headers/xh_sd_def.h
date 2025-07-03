
#ifndef XH_SD_DEF_H__
#define XH_SD_DEF_H__

#include <stdint.h>

#define XH_PPI_RESTRICTED              0                        /**< 1 if PPI peripheral is restricted, 0 otherwise. */
#define XH_PPI_ALL_APP_CHANNELS_MASK   ((uint32_t)0x00003FFFuL) /**< All PPI channels available to the application. */
#define XH_PPI_PROG_APP_CHANNELS_MASK  ((uint32_t)0x00003FFFuL) /**< Programmable PPI channels available to the application. */
#define XH_PPI_ALL_APP_GROUPS_MASK     ((uint32_t)0x00000003uL) /**< All PPI groups available to the application. */

#endif /* XH_SD_DEF_H__ */
