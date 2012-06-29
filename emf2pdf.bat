@echo off
"%ProgramFiles%\Metafile to EPS Converter\metafile2eps.exe" %1 %~dpn1.eps
if errorlevel 1 exit /b 9
which epstopdf > NUL
if errorlevel 1 exit /b 1
rem epstopdf comes with a TeX installation, e.g. MikTeX
call epstopdf %~dpn1.eps --outfile=%~dpn1.pdf
if errorlevel 1 exit /b 1
rem del %~dpn1.eps
exit /b 0
