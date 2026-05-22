@echo off
echo =======================================================
echo  KHOI CHAY DUONG TRUYEN ONLINE (TCP TUNNEL) MIEN PHI
echo =======================================================
echo.
echo He thong dang ket noi toi Pinggy.io...
echo Sau khi ket noi thanh cong, ban se thay mot bang thong tin co dong chu:
echo.
echo   "tcp://a.pinggy.io:XXXXX"  (voi XXXXX la so cong cua ban)
echo.
echo Hay dung dia chi do (vi du: a.pinggy.io:12345) de nhap vao game de choi online!
echo (Chu y: Neu he thong co hoi xac nhan y/n, hay go 'y' va Enter).
echo.
ssh -o StrictHostKeyChecking=no -p 443 -R 0:127.0.0.1:5000 tcp@a.pinggy.io
pause
