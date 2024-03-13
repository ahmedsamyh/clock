@echo off

set INCLUDE_DIRS=.\include
set LIB_DIR=.\lib
set LIBS=user32.lib shell32.lib gdi32.lib kernel32.lib
set CLOCK_FILES=clock_core clock_math clock_vector clock_matrix clock_texture clock_sprite clock_rect clock_resource_manager
set CLOCK_SRCS=
set CLOCK_OBJS=

for %%i in (!CLOCK_FILES!) do (
  set CLOCK_SRCS=!CLOCK_SRCS! src\clock\%%i.c
  set CLOCK_OBJS=!CLOCK_OBJS! %%i.obj
)

rem remove preceding space
set CLOCK_SRCS=!CLOCK_SRCS:~1!

if "%1"=="vector_gen" (
  echo Building vector_gen.c...
  call shell cl tool\vector_gen.c /I!INCLUDE_DIRS!
  echo.

  call shell vector_gen.exe
  call shell move clock_vector.h .\include\clock\
  call shell move clock_vector.c .\src\clock\
) else if "%1"=="matrix_gen" (
  echo Building matrix_gen.c...
  call shell cl tool\matrix_gen.c /I!INCLUDE_DIRS!
  echo.

  call shell matrix_gen.exe
) else if "%1"=="rotating_quad" (
  echo Building clock_engine.lib...
  call shell cl /c !CLOCK_SRCS! src\gl\gl.c /I!INCLUDE_DIRS!
  call shell lib !LIB_DIR!\glfw3_mt.lib !CLOCK_OBJS! gl.obj /out:.\lib\clock.lib

  call shell cl examples\rotating_quad.c /I!INCLUDE_DIRS! /link !LIB_DIR!\clock.lib !LIBS!
) else if "%1"=="drawing_sprite" (
  echo Building clock_engine.lib...
  call shell cl /c !CLOCK_SRCS! src\gl\gl.c /I!INCLUDE_DIRS!
  call shell lib !LIB_DIR!\glfw3_mt.lib !CLOCK_OBJS! gl.obj /out:.\lib\clock.lib

  call shell cl examples\drawing_sprite.c /I!INCLUDE_DIRS! /link !LIB_DIR!\clock.lib !LIBS!
) else if "%1"=="clean" (
  for /f %%i in ('dir /b .') do (
    if "%%~xi"==".exe" (
      call shell del /f %%i
    ) else if "%%~xi"==".obj" (
      call shell del /f %%i
    )
  )
) else (
  echo Building clock_engine.lib...
  call shell cl /c !CLOCK_SRCS! src\gl\gl.c /I!INCLUDE_DIRS!
  call shell lib !LIB_DIR!\glfw3_mt.lib !CLOCK_OBJS! gl.obj /out:.\lib\clock.lib

  call shell cl src\main.c /I!INCLUDE_DIRS! /link clock.lib !LIBS! /LIBPATH:!LIB_DIR!
)

echo.
for /f %%i in ('dir /b .\') do (
  if %%~xi== .obj (
    if exist %%i (
      call shell del /f %%i
    )
  )
)
