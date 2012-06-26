How to install under MS Windows

1. Log in to Windows with administrative rights.

2. Create a subfolder rtf2latex2e in your "Program Files" folder
   (in a 64bit system, "Program Files (x86)")
   or whatever the program files folder is named in your system.
   If you have a previous version of r2f2latex2e,
   you can simply delete all files of the previous version.

3. Unzip the contents of this archive to that rtf2latex2e folder.
   Move the subfolder "test" to a folder where the standard user has write access.

4. Execute the file "metafile2eps.exe" in the subfolder "emf2eps"
   (not necessary if already installed by the LyX installer).
     This will install the metafile2eps converter which is available from
     http://wiki.lyx.org/Windows/MetafileToEPSConverter
     License: GPL
   Do NOT change the default installation folder.
   It MUST read "...\Metafile to EPS Converter"
   The installer also installs the Windows printer "Metafile to EPS Converter".
   Do NOT rename this printer and do NOT change its settings.

5. Edit file emf2pdf.bat and make sure that line 1 
   points to the correct installation folder of metafile2eps.

6. You can now log off as admin and log in as standard user.

7. If you need the pictures as pdf (for compiling the TeX file with pdflatex),
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

