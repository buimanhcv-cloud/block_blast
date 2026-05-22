@echo off
title Deploy Block Blast Server to Railway
echo ========================================
echo  Deploy Matchmaking Server to Railway.app
echo ========================================
echo.

echo [1/4] Installing Railway CLI globally...
call npm install -g @railway/cli
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to install Railway CLI.
    echo Please make sure Node.js/npm is installed and accessible.
    pause & exit /b 1
)

echo.
echo [2/4] Logging in to Railway...
echo --------------------------------------------------
echo A browser window will open. Please log in or sign up
echo on the Railway website to authorize this CLI tool.
echo --------------------------------------------------
call railway login
if %ERRORLEVEL% NEQ 0 (
    echo Error: Login failed or cancelled.
    pause & exit /b 1
)

echo.
echo [3/4] Linking or creating a new Railway project...
echo --------------------------------------------------
echo Follow the prompts to create a new project or link
echo to an existing one.
echo --------------------------------------------------
call railway init
if %ERRORLEVEL% NEQ 0 (
    echo Warning: Project initialization was skipped or returned an error.
)

echo.
echo [4/4] Deploying files to Railway...
echo --------------------------------------------------
echo Uploading project files and starting deployment.
echo --------------------------------------------------
call railway up
if %ERRORLEVEL% NEQ 0 (
    echo Error: Deployment failed.
    pause & exit /b 1
)

echo.
echo ==================================================
echo               DEPLOYMENT SUCCESSFUL!
echo ==================================================
echo.
echo Ba buoc tiep theo de choi Online:
echo.
echo 1. Truy cap trang dashboard: https://railway.app
echo 2. Click vao Service vua tao -> Vao tab "Settings" -> cuon xuong muc "TCP Proxy" -> click "Add TCP Proxy".
echo 3. Railway se cap cho ban mot dia chi TCP Cong khai dang:
echo    Host: xxxxx.proxy.rlwy.net
echo    Port: yyyyy
echo.
echo Dung dia chi do de nhap vao phan Host/Port trong game de choi online 24/7!
echo ==================================================
pause
