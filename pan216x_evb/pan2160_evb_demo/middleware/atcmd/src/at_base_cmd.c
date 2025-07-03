#include <at.h>
#include <stdlib.h>
#include <string.h>

#ifdef AT_USING_SERVER

#define AT_ECHO_MODE_CLOSE             0
#define AT_ECHO_MODE_OPEN              1

extern at_server_t at_get_server(void);

static at_result_t at_exec(void)
{
    return AT_RESULT_OK;
}
//ָ, ָ=<value>, Է, ѯ, ÷,ִз) 
AT_CMD_EXPORT("AT", PN_NULL, PN_NULL, PN_NULL, PN_NULL, at_exec);

static at_result_t atz_exec(void)
{
    at_server_printfln("OK");
    at_port_factory_reset();
    
    return AT_RESULT_NULL;
}
AT_CMD_EXPORT("AT+Z", PN_NULL, PN_NULL, PN_NULL, PN_NULL, atz_exec);

static at_result_t at_rst_exec(void)
{
    at_server_printfln("OK");
    at_port_reset();

    return AT_RESULT_NULL;
}
AT_CMD_EXPORT("AT+RESET", PN_NULL, PN_NULL, PN_NULL, PN_NULL, at_rst_exec);

//static at_result_t ate_query(void)
//{
//    at_server_printfln("+E:%d", at_get_server()->echo_mode);

//    return AT_RESULT_OK;
//}
//static at_result_t ate_setup(const char *args)
//{
//    int echo_mode, argc;
//    const char *req_expr = "=%d";

//    argc = at_req_parse_args(args, req_expr, &echo_mode);
//    if (argc != 1)
//    {
//        return AT_RESULT_PARSE_FAILE;
//    }

//    if(echo_mode == AT_ECHO_MODE_CLOSE || echo_mode == AT_ECHO_MODE_OPEN)
//    {
//        at_get_server()->echo_mode = echo_mode;
//    }
//    else
//    {
//        return AT_RESULT_FAILE;
//    }

//    return AT_RESULT_OK;
//}
//AT_CMD_EXPORT("AT+E", "=<value>", PN_NULL, ate_query, ate_setup, PN_NULL);

static at_result_t at_show_cmd_exec(void)
{
    extern void rt_at_server_print_all_cmd(void);

    rt_at_server_print_all_cmd();

    return AT_RESULT_OK;
}
AT_CMD_EXPORT("AT&L", PN_NULL, PN_NULL, PN_NULL, PN_NULL, at_show_cmd_exec);

#endif /* AT_USING_SERVER */
