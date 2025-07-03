SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;D:\soft ware\TortoiseSVN\bin;D:\soft ware\mingw64\bin;D:\qt\qt5.12\5.12.12\mingw73_32\bin;D:\qt\qt5.12\5.12.12\mingw73_64\bin;D:\qt\qt5.12\Tools\mingw730_32\bin;D:\qt\qt5.12\Tools\mingw730_64\bin;D;C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Bin\;C:\Program Files\IVI Foundation\VISA\Win64\Bin\;C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Bin;D:\soft ware\python\Scripts\;D:\soft ware\python\;C:\Users\E001544\AppData\Local\Microsoft\WindowsApps;D:\soft ware\Microsoft VS Code\Microsoft VS Code\bin;C:\Program Files\JetBrains\PyCharm Community Edition 2022.1.3\bin;;C:\MinGW\bin;
SET ARMCC6_ASMOPT=--diag_suppress=9931
SET ARMCC6_CCOPT=--diag_suppress=9931
SET ARMCC6_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=ARMCM33_DSP_FP
SET CPU_VENDOR=ARM
SET UV2_TARGET=flash_algorithm
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_pmu.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_crg.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_efuse.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_gpio.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_flash.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_uart.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_wdt.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_sys_ctrl.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\app_debug.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\utility.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via ".\objects\utility_asm._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\flash_prog.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\flashdev.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via ".\Objects\Algorithm.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" ".\Objects\Algorithm.axf" --i32combined --output ".\Objects\Algorithm.hex"
cmd.exe /c copy "Objects\Algorithm.axf" "C:\Keil_v5\ARM\Flash\MXD267x_FLASH_V1.4.FLM"
cmd.exe /c copy "Objects\Algorithm.axf" "..\..\MXD267x_FLASH_V1.4.FLM"
