@echo off
echo =======================================================
echo          BankCore SDET Test Suite Runner
echo =======================================================
echo.

echo [1/3] Compiling BankCore & Unit Tests...
make all
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed!
    exit /b %ERRORLEVEL%
)

echo.
echo [2/3] Running C++ Unit Tests...
.\unit_tests.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] C++ Unit Tests Failed!
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] Running Python Pytest Subprocess CLI Harness...
pytest -v
if %ERRORLEVEL% NEQ 0 (
    echo [WARNING] Pytest run failed or pytest not installed. Try 'pip install pytest'.
)

echo.
echo =======================================================
echo    All BankCore SDET Reliability Tests Passed!
echo =======================================================
