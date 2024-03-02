@echo off

set INCLUDE_DIRS=.\include
set LIB_DIR=.\lib
set LIBS=user32.lib shell32.lib gdi32.lib kernel32.lib


if "%1"=="vector_gen" (
  echo Building vector_gen.c...
  call shell cl tool\vector_gen.c /I!INCLUDE_DIRS!
  echo.

  call shell vector_gen.exe
  call shell move vector.h .\include\vector.h
) else if "%1"=="matrix_gen" (
  echo Building matrix_gen.c...
  call shell cl tool\matrix_gen.c /I!INCLUDE_DIRS!
  echo.

  call shell matrix_gen.exe
  rem call shell move vector.h .\include\vector.h
) else (
  echo Building clock_engine.lib...
  call shell cl /c src\clock.c src\clock_math.c /I!INCLUDE_DIRS!
  call shell lib !LIB_DIR!\glfw3_mt.lib clock.obj clock_math.obj /out:.\lib\clock.lib

  call shell cl src\main.c src\gl.c /I!INCLUDE_DIRS! /link !LIB_DIR!\clock.lib !LIBS!
)

echo.
echo Deleting:
for /f %%i in ('dir /b .\') do (
  if %%~xi== .obj (
    if exist %%i (
      call shell del /f %%i
    )
  )
)
