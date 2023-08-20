@ECHO OFF
REM This scripts does the following
REM - ensures that the dependencies are found
REM - builds the Visual Studio 2022 compilation configuration files (vsprojx)

REM Asserts Visual Studio is present
where /q devenv.exe || ECHO Could not find the devenv.exe program. Check that it is in the path. && EXIT /B 1
REM See https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2017%202022.html
SET CMAKE_GENERATOR="Visual Studio 17 2022" -A x64 -T host=x64
SET BUILD_DIR=build

REM Asserts the dependencies are present. We find them through environment variables.
REM the *_DIR environment variables are hints to help CMake finding the
REM libraries and include dirs.
REM Due to *.bat limitations, must use one letter for the loop variable
SETLOCAL enabledelayedexpansion
FOR %%i IN (FLTK_DIR Microsoft.GSL_DIR SCRIPTS_DIR) DO (
  IF NOT DEFINED %%i ECHO need to set the %%i environment variable && EXIT /B 1
  FOR /F %%j IN ('ECHO %%i') DO (
    IF NOT EXIST !%%j!\NUL ECHO %%i refers to a non existing directory '!%%j!' && EXIT /B 1
  )
)
ENDLOCAL

REM builds the Visual Studio 2022 compilation configuration files
IF EXIST %BUILD_DIR% (
  ECHO The '%BUILD_DIR%' directory exists, I remove it
  RD /Q /S %BUILD_DIR%
)
ECHO Create the '%BUILD_DIR%' directory
MD %BUILD_DIR%
PUSHD %BUILD_DIR%
ECHO Generating the Visual Studio 2022 compilation configuration files inside '%BUILD_DIR%'...
ECHO:
cmake -G %CMAKE_GENERATOR% %~dp0
POPD

ECHO:
ECHO Generate the 'format.bat' script
ECHO ^pushd %CD% ^&^& ^format ^&^& ^popd > %BUILD_DIR%\format.bat


IF ERRORLEVEL 1 ECHO Error building Visual Studio project file. && EXIT /B 1
PUSHD %BUILD_DIR%
ENDLOCAL
@ECHO ON
