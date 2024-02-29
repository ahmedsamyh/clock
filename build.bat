@echo off

set INCLUDE_DIRS=.\include
set LIB_DIR=.\lib
set LIBS=user32.lib shell32.lib gdi32.lib kernel32.lib

echo.
echo Building clock_engine.lib...
call shell cl /c src\clock.c /I!INCLUDE_DIRS!
call shell lib !LIB_DIR!\glfw3_mt.lib clock.obj /out:.\lib\clock.lib

call shell cl src\main.c src\commonlib.c src\gl.c /I!INCLUDE_DIRS! /link !LIB_DIR!\clock.lib !LIBS!

echo.
echo Deleting:
for /f %%i in ('dir /b .\') do (
  if %%~xi== .obj (
    if exist %%i (
      call shell del /f %%i
    )
  )
)
