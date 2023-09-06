@ECHO OFF
REM This scripts builds the Visual Studio 2022 compilation configuration files (vsprojx)

REM Asserts Visual Studio is present
where /q devenv.exe || ECHO Could not find the devenv.exe program. Check that it is in the path. && EXIT /B 1
REM Asserts CMake is present
where /q cmake.exe || ECHO Could not find the cmake.exe program. Check that it is in the path. && EXIT /B 1

REM Asserts the dependencies are present. We find them through environment variables.
REM Those *_DIR environment variables are hints to help CMake finding the
REM libraries and include dirs.
REM Due to *.bat limitations, we must use one letter for the loop variable
SETLOCAL enabledelayedexpansion
FOR %%i IN (FLTK_DIR Microsoft.GSL_DIR input_lite_DIR SCRIPTS_DIR) DO (
  IF NOT DEFINED %%i (ECHO need to set the %%i environment variable && EXIT /B 1)
  FOR /F %%j IN ('ECHO %%i') DO (
    IF NOT EXIST !%%j!\NUL (ECHO %%i refers to a non existing directory '!%%j!' && EXIT /B 1)
  )
)
ENDLOCAL

REM builds the Visual Studio 2022 compilation configuration files
IF EXIST build (RD /Q /S build)
MD build
PUSHD build
ECHO Generate the Visual Studio 2022 compilation configuration files in the 'build' directory...
REM See https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2017%202022.html
ECHO:
cmake -G "Visual Studio 17 2022" -A x64 -T host=x64 -DFETCHCONTENT_BASE_DIR=..\..\cmake_cache %~dp0
POPD

ECHO:
ECHO Generate the 'format.bat' script in in the 'build' directory
ECHO ^pushd %CD% ^&^& ^format ^&^& ^popd > build\format.bat

IF ERRORLEVEL 1 (ECHO Error building Visual Studio project file. && EXIT /B 1)
ENDLOCAL
@ECHO ON
