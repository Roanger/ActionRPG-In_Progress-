@echo off
@echo on
echo Compiling shaders...
echo Compiling shaders to SPIR-V format...

rem Create the spirv directory if it doesn't exist
if not exist "spirv" mkdir spirv

rem Set the path to the Vulkan SDK
set VULKAN_SDK=C:\VulkanSDK

rem Find the latest version of the SDK
for /f "tokens=*" %%a in ('dir /b /ad "%VULKAN_SDK%"') do (
    set LATEST_SDK=%%a
)

rem Set the path to the glslangValidator compiler
set GLSLANG_VALIDATOR="%VULKAN_SDK%\%LATEST_SDK%\Bin\glslangValidator.exe"

rem Check if glslangValidator exists
if not exist %GLSLANG_VALIDATOR% (
    echo Error: glslangValidator compiler not found at %GLSLANG_VALIDATOR%
    echo Please install the Vulkan SDK and ensure it's in the correct location.
    echo You can download it from: https://vulkan.lunarg.com/sdk/home
    exit /b 1
)

echo Using glslangValidator from: %GLSLANG_VALIDATOR%

rem Compile vertex shader
echo Compiling vertex shader...
%GLSLANG_VALIDATOR% -V -o spirv/vert.spv shaders/shader.vert
if %ERRORLEVEL% neq 0 (
    echo Error: Failed to compile vertex shader.
    exit /b 1
)

rem Compile fragment shader
echo Compiling fragment shader...
%GLSLANG_VALIDATOR% -V -o spirv/frag.spv shaders/shader.frag
if %ERRORLEVEL% neq 0 (
    echo Error: Failed to compile fragment shader.
    exit /b 1
)

echo Shader compilation completed successfully.
echo Compiled shaders are in the spirv directory.
echo Shader compilation completed successfully.
