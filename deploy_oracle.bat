@echo off
REM ── Deploy server.js lên Oracle Cloud VM ─────────────────────────────────
REM Chỉnh 2 dòng dưới theo thông tin Oracle VM của bạn:

set ORACLE_IP=YOUR_ORACLE_VM_IP
set KEY_FILE=C:\Users\buima\.ssh\oracle_key.key

echo [1/3] Uploading server.js...
scp -i "%KEY_FILE%" -o StrictHostKeyChecking=no ^
    server.js package.json ^
    ubuntu@%ORACLE_IP%:~/

echo [2/3] Restarting server on Oracle VM...
ssh -i "%KEY_FILE%" -o StrictHostKeyChecking=no ubuntu@%ORACLE_IP% ^
    "pm2 restart block-puzzle 2>/dev/null || pm2 start server.js --name block-puzzle && pm2 save"

echo [3/3] Done!
echo Server running at %ORACLE_IP%:5000
pause
