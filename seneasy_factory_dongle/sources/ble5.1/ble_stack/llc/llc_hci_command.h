#ifndef LLC_HCI_COMMAND_H
#define LLC_HCI_COMMAND_H

typedef struct
{
    uint8_t* pu8ParametersBuffer;
    uint16_t u16ParametersBufferSize;
    uint16_t u16OCF;
    uint8_t u8OGF;
    uint8_t u8ParameterTotalLength;
    bool bMemUsed;
    uint32_t u32Reserved;
} __attribute__((aligned(4)))
stLlcHciCommand_t;


bool rom_llc_hci_command_mem_init(stLlcHciCommand_t* pstLlcHciCommandMemBuffer, uint16_t u16ParametersBufferSize, uint8_t u8LlcHciCommandNum);

uint16_t rom_llc_hci_command_get_parameters_size(void);

uint8_t rom_llc_hci_command_get_num(void);

stLlcHciCommand_t* rom_llc_hci_command_alloc(void);

void rom_llc_hci_command_free(stLlcHciCommand_t* pstLlcHciCommand);


#endif
