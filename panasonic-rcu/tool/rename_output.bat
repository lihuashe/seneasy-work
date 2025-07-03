@echo off
setlocal enabledelayedexpansion

cd ..
set "hex_path=.\MDK-ARM\Objects"
set "output_dir=.\MDK-ARM\Output"
set "hex_pattern=%hex_path%\*.hex"

if not exist ".git" (
    >&2 echo Error: Not a git repository
    exit /b 1
)

:: 检查是否找到HEX文件
if not exist %hex_pattern% (
    echo Error: HEX file not found
    exit /b 1
)

:: 检查输出目录是否存在，不存在则创建
if not exist "%output_dir%" (
    mkdir "%output_dir%"
)


set "version="
set "exit_code=0"

REM 2. 处理传入参数
if not "%~1"=="" (
    set "search_str=%~1"
    set "match_found="
    REM 获取所有标签并按版本降序排列
    for /f "delims=" %%a in ('git tag --sort=-version:refname 2^>nul') do (
        set "current_tag=%%a"
        REM 调试输出：显示正在检查的标签
        @REM echo Checking tag: !current_tag!
        @REM echo search_str:   !search_str!
        
        REM 使用 findstr 替代变量替换
        echo !current_tag! | findstr /i /c:"!search_str!" >nul
        if !errorlevel! equ 0 (
            set "version=!current_tag!"
            set "match_found=yes"
            goto :process_version
        )
    )

    REM 未找到匹配标签
    if not defined match_found (
        echo Warning: No match tag, using commit hash 
        for /f "delims=" %%a in ('git rev-parse --short HEAD') do (
            set "version=%%a"
        )
    )
) else (
    REM 未提供参数时使用最新标签
    echo Warning: Args is null, using last tag as version

    for /f "delims=" %%a in ('git describe --tags --abbrev^=0 2^>nul') do set "version=%%a"
    if not defined version (
        @REM 如果没有Tag，使用commit hash
        echo Warning: No defined tag, using commit hash 
        for /f "delims=" %%a in ('git rev-parse --short HEAD') do (
            set "version=%%a"
        )
    )
)
:process_version
:: 找到最新修改的HEX文件
for /f "delims=" %%F in ('dir /b /a-d /od %hex_pattern%') do (
    set "latest_hex=%%F"
)

:: 计算MD5校验值并转换为大写
for %%F in ("%hex_path%\!latest_hex!") do (
    certutil -hashfile "%%F" MD5 | find /v "MD5" | find /v ":" > md5.tmp
    set /p md5_hash=<md5.tmp
    :: 转换为大写
    set "md5_hash=!md5_hash:~0,32!"
    call :ToUpper md5_hash
    del md5.tmp
)

:: 重命名HEX文件（强制覆盖）
set "new_name=!version!-!md5_hash!.hex"
if exist "%hex_path%\!new_name!" (
    del "%hex_path%\!new_name!"
)
ren "%hex_path%\!latest_hex!" "!new_name!"
echo Renamed !latest_hex! to !new_name!

:: 移动文件到输出目录（强制覆盖）
if exist "%output_dir%\!new_name!" (
    del "%output_dir%\!new_name!"
)
move "%hex_path%\!new_name!" "%output_dir%\" >nul
echo Moved !new_name! to %output_dir%

endlocal
goto :eof

:ToUpper
:: 转换为大写的子程序
set %1=!%1:a=A!
set %1=!%1:b=B!
set %1=!%1:c=C!
set %1=!%1:d=D!
set %1=!%1:e=E!
set %1=!%1:f=F!
set %1=!%1:g=G!
set %1=!%1:h=H!
set %1=!%1:i=I!
set %1=!%1:j=J!
set %1=!%1:k=K!
set %1=!%1:l=L!
set %1=!%1:m=M!
set %1=!%1:n=N!
set %1=!%1:o=O!
set %1=!%1:p=P!
set %1=!%1:q=Q!
set %1=!%1:r=R!
set %1=!%1:s=S!
set %1=!%1:t=T!
set %1=!%1:u=U!
set %1=!%1:v=V!
set %1=!%1:w=W!
set %1=!%1:x=X!
set %1=!%1:y=Y!
set %1=!%1:z=Z!
goto :eof