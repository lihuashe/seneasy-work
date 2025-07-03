/**
 * @file shell_cmd.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-30
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "shell_cmd.h"
#include "nr_micro_shell.h"
#include <string.h>
#include <ctype.h>

/*============================================================================*
 *                              Local Variables
 *============================================================================*/


/*============================================================================*
 *                              Global Variables
 *============================================================================*/



/*============================================================================*
 *                              Local Functions
 *============================================================================*/



/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/**
 * @brief ls command
 */
void shell_ls_cmd(char argc, char *argv)
{
	unsigned int i = 0;
	if (argc > 1)
	{
		if (!strcmp("cmd", &argv[argv[1]]))
		{

			for (i = 0; nr_shell.static_cmd[i].fp != NULL; i++)
			{
				shell_printf("%s",nr_shell.static_cmd[i].cmd);
				shell_printf("\r\n");
			}
		}
		else if (!strcmp("-v", &argv[argv[1]]))
		{
			shell_printf("ls version 1.0.\r\n");
		}
		else if (!strcmp("-h", &argv[argv[1]]))
		{
			shell_printf("useage: ls [options]\r\n");
			shell_printf("options: \r\n");
			shell_printf("\t -h \t: show help\r\n");
			shell_printf("\t -v \t: show version\r\n");
			shell_printf("\t cmd \t: show all commands\r\n");
		}
	}
	else
	{
		shell_printf("ls need more arguments!\r\n");
	}
}

/**
 * @brief test command
 */
void shell_test_cmd(char argc, char *argv)
{
	unsigned int i;
	shell_printf("test command:\r\n");
	for (i = 0; i < argc; i++)
	{
		shell_printf("paras %d: %s\r\n", i, &(argv[argv[i]]));
	}
}

#ifdef NR_SHELL_USING_EXPORT_CMD
NR_SHELL_CMD_EXPORT(ls, shell_ls_cmd);
NR_SHELL_CMD_EXPORT(test, shell_test_cmd);
#else
const static_cmd_st static_cmd[] =
	{
		{"ls", shell_ls_cmd},
		{"test", shell_test_cmd},
		{"\0", NULL}};
#endif
