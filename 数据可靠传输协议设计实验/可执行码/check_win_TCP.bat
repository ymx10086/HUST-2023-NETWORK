@echo off
:: appname 程序名
:: inputname 输入文件名
:: outputname 输出文件名
:: resultname 程序控制台输出重定向文件名

set appname="TCP.exe"
set inputname="I:\Desktop\RDT\Windows VS2017\input.txt"
set outputname="I:\Desktop\RDT\Windows VS2017\output.txt"
set resultname="I:\Desktop\RDT\Windows VS2017\result.txt"

for /l %%i in (1,1,10) do (
    echo Test %appname% %%i:
    %appname% > %resultname% 2>&1
    fc /N %inputname% %outputname%
)
pause