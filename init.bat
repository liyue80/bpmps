@echo off
echo Importing and initializing database
mysql\bin\mysql.exe -u root -h localhost test<dump.sql
echo Initialize done!
pause