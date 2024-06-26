@echo off

rem for clean.bat
set CLEAN_QUIET=1

set INCLUDE_DIRS=.\include
set LIB_DIR=.\lib
set CLOCK_LINKER_FLAGS=/LIBPATH:!LIB_DIR!
set CLOCK_LIBS=glfw3_mt.lib
set IGNORED_WARNINGS_RAW= 4201 4127 4005

set EXEC_LIBS=clock.lib user32.lib shell32.lib gdi32.lib kernel32.lib

set EXEC_LINKER_FLAGS=/LIBPATH:!LIB_DIR!

for %%i in (!IGNORED_WARNINGS_RAW!) do (
  set IGNORED_WARNINGS=!IGNORED_WARNINGS! /wd%%i
)

set COMMON_CFLAGS=/Z7 /W4 /nologo /MTd !IGNORED_WARNINGS! /MP

set CLOCK_FILES=clock_core clock_math clock_vector clock_matrix clock_texture clock_sprite clock_rect clock_resource_manager clock_render_target clock_color clock_font clock_timer clock_ui clock_controller
set CLOCK_SRCS=
set CLOCK_OBJS=
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
  set COMMON_CFLAGS=/W4 /nologo /MT !IGNORED_WARNINGS! /MP /O2
)

rem RPG --------------------------------------------------
set RPG_SRC_PATH=src\rpg
set RPG_FILES=rpg player enemy tile common stage
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
  call shell build sprite
  call shell build text
  call shell build ui
) else if "!arg!"=="xinput" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\xinput.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS! XInput.lib
) else if "!arg!"=="path_drop" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\path_drop.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS!
) else if "!arg!"=="camera" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\camera.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS!
) else if "!arg!"=="bezier" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\bezier.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS!
) else if "!arg!"=="rotating_quad" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\rotating_quad.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS!
) else if "!arg!"=="sprite" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\sprite.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS!
) else if "!arg!"=="text" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\text.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS!
) else if "!arg!"=="ui" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\ui.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS!
) else if "!arg!"=="input" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! examples\input.c /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! !EXEC_LIBS!
) else if "!arg!"=="clean" (
  call shell clean exe
  call shell clean ilk
  call shell clean pdb
  call shell clean obj
) else if "!arg!"=="clock" (
  echo Building clock_engine.lib...
  call shell cl !COMMON_CFLAGS! /c !CLOCK_SRCS! src\gl\gl.c /I!INCLUDE_DIRS!
  call shell lib /out:.\lib\clock.lib /nologo !CLOCK_OBJS! gl.obj !CLOCK_LINKER_FLAGS! !CLOCK_LIBS!
) else if "!arg!"=="rpg" (
  call shell build clock !config!

  call shell cl !COMMON_CFLAGS! !RPG_SRCS! /Fe:rpg /DDEBUG /I!INCLUDE_DIRS! /link !EXEC_LINKER_FLAGS! clock.lib !EXEC_LIBS!
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
