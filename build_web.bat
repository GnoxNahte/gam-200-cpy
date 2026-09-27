@echo off
REM =============================================================================
REM build_web.bat – Build GLPIFrameworkIntro-Modern for WebGL (Emscripten/Windows)
REM
REM Prerequisites:
REM   1. Install Emscripten SDK: https://emscripten.org/docs/getting_started/
REM   2. Activate emsdk in this shell before running, OR edit EMSDK_PATH below.
REM   3. Run from the project root.
REM
REM Output: web\index.html (+ index.js, index.wasm, index.data)
REM Serve:  cd web && python -m http.server 8080
REM
REM Windowing backend (choose one):
REM   GLFW  – default, recommended  (-DUSE_GLFW  -s USE_GLFW=3)
REM   SDL2  – legacy fallback        (no -DUSE_GLFW, -s USE_SDL=2)
REM =============================================================================

REM SET EMSDK_PATH=C:\emsdk
REM CALL "%EMSDK_PATH%\emsdk_env.bat"

setlocal enabledelayedexpansion

echo Configuring build for Web (Emscripten)...
call emcmake cmake -S . -B build_web
if %ERRORLEVEL% NEQ 0 goto :error

echo.
echo Building project...
call cmake --build build_web
if %ERRORLEVEL% NEQ 0 goto :error

echo.
echo Deploying to web directory...
if not exist web mkdir web

REM Copy generated build output (index.html, index.js, index.wasm, index.data)
copy /Y build_web\index.* web\
if %ERRORLEVEL% NEQ 0 goto :error

echo.
echo Build succeeded.
echo Run:  cd web ^&^& python -m http.server 8080
echo Open: http://localhost:8080/
pause

:error
echo.
echo Build FAILED.
pause
exit /b 1
