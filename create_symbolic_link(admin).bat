@echo off

setlocal enabledelayedexpansion

:: 获取批处理文件所在的完整路径
set "SCRIPT_DIR=%~dp0"
echo "Script directory: !SCRIPT_DIR!"

:: 定义源目录
set "SOURCE_DIRS=assets shaders Configs"
set "TARGET_PARENT_DIRS=bin\FNAF_GL_DEMO\Release bin\FNAF_GL_DEMO\Debug bin\FNAF_VR_DEMO\Release bin\FNAF_VR_DEMO\Debug"

:: 循环遍历所有源目录和目标父目录
for %%S in (%SOURCE_DIRS%) do (
    for %%P in (%TARGET_PARENT_DIRS%) do (
        set "TARGET_DIR=!SCRIPT_DIR!%%P\%%S"

        echo Checking "!TARGET_DIR!"...

        :: 检查并创建目标目录的父目录
        if not exist "!SCRIPT_DIR!%%P\" (
            echo Parent directory "%%P\" does not exist. Creating...
            mkdir "!SCRIPT_DIR!%%P\"
        )

        :: 检查目标目录是否存在
        if exist "!TARGET_DIR!" (
            echo Directory "!TARGET_DIR!" already exists. Deleting it...
            rmdir /s /q "!TARGET_DIR!"
        )

        :: 创建软链接
        echo Creating symbolic link to "!TARGET_DIR!"...
        mklink /D "!TARGET_DIR!" "!SCRIPT_DIR!%%S"
    )
)

endlocal