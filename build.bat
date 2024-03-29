@echo off

rem for clean.bat
set CLEAN_QUIET=1

set INCLUDE_DIRS=.\include
set LIB_DIR=.\lib
set LIBS=user32.lib shell32.lib gdi32.lib kernel32.lib
set CLOCK_FILES=clock_core clock_math clock_vector clock_matrix clock_texture clock_sprite clock_rect clock_resource_manager clock_render_target clock_color clock_font clock_timer clock_ui
set CLOCK_SRCS=
set CLOCK_OBJS=
set COMMON_CFLAGS=/Zi /W1

for %%i in (!CLOCK_FILES!) do (
  set CLOCK_SRCS=!CLOCK_SRCS! src\clock\%%i.c
  set CLOCK_OBJS=!CLOCK_OBJS! %%i.obj
)

rem remove preceding space
set CLOCK_SRCS=!CLOCK_SRCS:~1!

set arg=%1
set config=%2

if "!config!" == "" (
  set config=Debug
)

if "!config!" NEQ "Debug" (
if "!config!" NEQ "Release" (
  echo ERROR: Invalid config '!config!'...
  exit /b 1
))

echo Build configured for !config!...
echo.


if "!config!" == "Release" (
  set COMMON_CFLAGS=/O2
)

rem RPG --------------------------------------------------
set RPG_SRC_PATH=src\rpg
set RPG_FILES=rpg player enemy tile common stage debug_box
set RPG_SRCS=

for %%i in (!RPG_FILES!) do (
  set RPG_SRCS=!RPG_SRC_PATH!\%%i.c !RPG_SRCS!
)

rem RPG --------------------------------------------------

if not defined arg (
  echo ERROR: No subcommand provided...
  exit /b 1
)

if "!arg!"=="vector_gen" (
  echo Building vector_gen.c...
  call shell cl !COMMON_CFLAGS! tool\vector_gen.c /I!INCLUDE_DIRS!
  echo.

  call shell vector_gen.exe
  call shell move clock_vector.h .\include\clock\
  call shell move clock_vector.c .\src\clock\
) else if "!arg!"=="examples" (
  echo Building examples...
  call shell build rotating_quad
  call shell build drawing_sprite
  call shell build text
  call shell build ui

) else if "!arg!"=="rotating_quad" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\rotating_quad.c /I!INCLUDE_DIRS! /link !LIB_DIR!\clock.lib !LIBS!
) else if "!arg!"=="drawing_sprite" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\drawing_sprite.c /I!INCLUDE_DIRS! /link !LIB_DIR!\clock.lib !LIBS!
) else if "!arg!"=="text" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\text.c /I!INCLUDE_DIRS! /link !LIB_DIR!\clock.lib !LIBS!
) else if "!arg!"=="ui" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\ui.c /I!INCLUDE_DIRS! /link !LIB_DIR!\clock.lib !LIBS!
) else if "!arg!"=="clean" (
  call shell clean exe
  call shell clean ilk
  call shell clean pdb
  call shell clean obj
) else if "!arg!"=="clock" (
  echo Building clock_engine.lib...
  call shell cl !COMMON_CFLAGS! /c !CLOCK_SRCS! src\gl\gl.c /I!INCLUDE_DIRS!
  call shell lib !LIB_DIR!\glfw3_mt.lib !CLOCK_OBJS! gl.obj /out:.\lib\clock.lib
) else if "!arg!"=="main" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! src\main.c /I!INCLUDE_DIRS! /link clock.lib !LIBS! /LIBPATH:!LIB_DIR!
) else if "!arg!"=="rpg" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! !RPG_SRCS! /Fe:rpg /DDEBUG /I!INCLUDE_DIRS! /link clock.lib !LIBS! /LIBPATH:!LIB_DIR!
) else (
  echo ERROR: Unknown subcommand '!arg!'...
  exit /b 1
)

echo.
for /f %%i in ('dir /b .\') do (
  if %%~xi== .obj (
    if exist %%i (
      call shell del /f %%i
    )
  )
)
