@echo off

set hex_file=%1
set /a traceBegin=%2
set output=%3
echo %traceBegin%
..\..\..\tool\srecord\srec_info  %hex_file% -Intel >info.txt

for /f "tokens=2 delims=:" %%a in (info.txt) do (
    set take=%%a
    )
set take=%take:~3,8%
echo %take%
set /a appBegin=0x%take%
echo %appBegin%


for /f %%i in ('find/c /v "%traceBegin%" ^< info.txt') do set/a "sn=%%i-1"
::echo %sn%
for /f "skip=%sn% tokens=2 delims=-" %%a in (info.txt)do (
    setlocal enabledelayedexpansion
        set take=%%a
        goto save
)

:save
    set appEnd=!take!
    set appEnd=%appEnd:~1,9%
    set /a appEnd=0x%appEnd%+1
    echo %appEnd%

for /f "tokens=2 delims=-" %%a in (info.txt) do (
    setlocal enabledelayedexpansion
    set  take=%%a
    )
set traceEnd=%take:~1,9%
set /a traceEnd=0x%traceEnd%+1
echo %traceEnd%
set /a appSize=%appEnd%-%appBegin%
echo %appSize%
set /a traceSize=%traceEnd%-0x%traceBegin%
echo %traceSize%
..\..\..\tool\srecord\srec_cat %hex_file% -Intel -fill 0xff %appBegin% %appEnd%  -split 0x10000000 %appBegin% %appSize% -Output %output% -Binary
..\..\..\tool\srecord\srec_cat %hex_file% -Intel -fill 0xff 0x%traceBegin% %traceEnd% -split 0x10000000 0x%traceBegin% %traceSize% -Output bin\app.trace -Binary
