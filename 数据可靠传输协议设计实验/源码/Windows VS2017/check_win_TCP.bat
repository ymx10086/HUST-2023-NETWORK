@echo off
:: appname ������
:: inputname �����ļ���
:: outputname ����ļ���
:: resultname �������̨����ض����ļ���

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