@ECHO OFF
REM no SETLOCAL to stay in the build dir so that "test" works
where /q devenv.exe || ECHO Could not find the devenv.exe program. Check that it is in the path. && EXIT /B 1
REM see https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2017%202022.html
SET CMAKE_GENERATOR="Visual Studio 17 2022" -A x64 -T host=x64
SET BUILD_DIR=build

SETLOCAL
IF NOT DEFINED COMPILER ECHO need to set the COMPILER environment variable && EXIT /B 1

REM the *_DIR environment variables are hints to help CMake finding the libraries and include dirs
REM one letter for the loop variable
SETLOCAL enabledelayedexpansion
FOR %%i IN (FLTK_DIR Microsoft.GSL_DIR SCRIPTS_DIR) DO (
  IF NOT DEFINED %%i ECHO need to set the %%i environment variable && EXIT /B 1
  FOR /F %%j IN ('ECHO %%i') DO (
    IF NOT EXIST !%%j!\NUL ECHO %%i refers to a non existing directory '!%%j!' && EXIT /B 1
  )
)
ENDLOCAL

IF EXIST %BUILD_DIR% (RD /Q /S %BUILD_DIR%)
ECHO MD %BUILD_DIR%
MD %BUILD_DIR%
PUSHD %BUILD_DIR%
ECHO cmake -G %CMAKE_GENERATOR% %~dp0
cmake -G %CMAKE_GENERATOR% %~dp0
POPD
ECHO ^pushd %CD% ^&^& ^format ^&^& ^popd > %BUILD_DIR%\format.bat


IF ERRORLEVEL 1 ECHO Error building Visual Studio project file. && EXIT /B 1
PUSHD %BUILD_DIR%
ENDLOCAL
@ECHO ON
