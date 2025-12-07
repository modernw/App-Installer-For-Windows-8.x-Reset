@echo on
cd /d "%~dp0"
uninstall.backup.exe /U:Uninstall\uninstall.xml
exit /b