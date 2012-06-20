
1. Execute the file "metafile2eps.exe" in the subfolder "emf2eps".
This will install the metafile2eps converter - administrative rights required!
   This converter (license: GPL) is available 
   from http://wiki.lyx.org/Windows/MetafileToEPSConverter
Do NOT change the default installation folder.
It MUST read "...\Metafile to EPS Converter"
The installer also installs the Windows printer "Metafile to EPS Converter".
Do NOT rename this printer and do NOT change its settings.

2. If you need the pictures as pdf (for compiling the TeX file with pdflatex),
you must have a working "epstopdf.exe" or "epstopdf.bat", 
which is e.g. available in MikTeX.
The folder containing epstopdf must be in the search path.
You can test this:
Open a command prompt, 
navigate to the folder where rtf2latex2e is installed,
and type the command "which epstopdf".
The path where epstopdf is installed should be displayed.
If "which" issues the reply  "which: no epstopdf in (..."
then the graphics will be converted to eps only 
(sufficient for compiling the TeX file with latex to dvi)

