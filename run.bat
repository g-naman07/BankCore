@echo off
title AlgoBank Deployment Manager
echo ===================================================
echo   AlgoBank: High-Performance C++ TCP & Web App
echo ===================================================
echo.

:: Step 1: Install Node.js dependencies
echo [1/4] Checking Node.js dependencies...
if not exist "node_modules\" (
    echo Node modules not found. Running npm install...
    call npm install
    if %ERRORLEVEL% neq 0 (
        echo Error installing node modules. Please ensure Node.js is installed and run 'npm install' manually.
        pause
        exit /b 1
    )
) else (
    echo Node dependencies already satisfied.
)
echo.

:: Step 2: Build C++ binaries
echo [2/4] Verifying C++ Server Executables...
if not exist "server.exe" (
    echo server.exe not found! Attempting compilation with g++...
    where g++ >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        mingw32-make all
    ) else (
        echo g++ was not detected on PATH. Please compile using your IDE or install MinGW.
        pause
        exit /b 1
    )
) else (
    echo C++ Executables detected. Ready to spin.
)
echo.

:: Step 3: Launch C++ Server in a separate window
echo [3/4] Launching C++ Socket Server (Port 9090)...
start "AlgoBank C++ Core Server" cmd /k "server.exe"
timeout /t 2 >nul

:: Step 4: Launch Node.js API Gateway & Open Web Browser
echo [4/4] Spinning API Gateway and Launching Dashboard...
start "AlgoBank API Gateway" cmd /c "node gateway.js"
timeout /t 2 >nul

echo opening browser to client console...
start http://localhost:3000

echo.
echo ===================================================
echo   SUCCESS: AlgoBank Web Application is now running!
echo   -------------------------------------------------
echo   - C++ Server Socket : Port 9090
echo   - Web App Console   : http://localhost:3000
echo ===================================================
echo.
pause
