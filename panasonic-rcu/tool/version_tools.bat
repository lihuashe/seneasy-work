@echo off
setlocal enabledelayedexpansion

cd ..
REM 1. 检查Git仓库是否存在
if not exist ".git" (
    >&2 echo Error: Not a git repository
    exit /b 1
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
        >&2 echo Error: No tag contains substring "!search_str!"
        exit /b 3
    )
) else (
    REM 未提供参数时使用最新标签
    echo Warning: Args is null, using last tag as version

    for /f "delims=" %%a in ('git describe --tags --abbrev^=0 2^>nul') do set "version=%%a"
    if not defined version (
        echo Warning: No defined tag, using V1.0.0 as version
        set "version=V1.0.0"
        set "exit_code=2"
    )
)

:process_version
REM 3. 清理版本字符串（移除v前缀和-dirty后缀示例）
REM set "version=!version:v=!"
REM set "version=!version:-dirty=!"

REM 4. 生成版本头文件
(
    echo #ifndef VERSION_H_
    echo #define VERSION_H_
    echo;
    echo // Auto-generated version information
    echo #define FW_VERSION_STR "!version!"
    echo;
    echo #endif /* VERSION_H_ */
) > version.h

REM 5. 返回状态码
exit /b %exit_code%