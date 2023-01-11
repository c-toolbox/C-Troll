@echo off


echo ### Removing old folders
if exist bin-old rmdir /S /Q bin-old
if exist build-deploy rmdir /S /Q build-deploy
if exist C-Troll.zip del C-Troll.zip

echo ### Moving existing folders out of the way
if exist bin move bin bin-old


echo ### Run CMake into the 'build-deploy' folder
mkdir build-deploy
cd build-deploy
cmake ..


echo ### Build C-Troll in RelWithDebInfo mode
cmake --build . --config RelWithDebInfo --target C-Troll Editor Starter Tray --parallel -- /p:CL_MPcount=16
cd ..


echo ### Remove files we don't need
del bin\RelWithDebInfo\*.pdb
del bin\RelWithDebInfo\Qt6Svg.dll
del bin\RelWithDebInfo\UnitText.exe
rmdir /S /Q bin\RelWithDebInfo\iconengines
rmdir /S /Q bin\RelWithDebInfo\imageformats
rmdir /S /Q bin\RelWithDebInfo\networkinformation


echo ### Copy example folder
robocopy example bin\RelWithDebInfo\example /E


echo ### Create zip file
cd bin\RelWithDebInfo
"C:\Program Files\7-Zip\7z.exe" a ..\..\C-Troll.zip ..\..\CREDITS ..\..\LICENSE ..\..\README.md *
cd ..\..


echo ### Clean up
rmdir /S /Q bin
if exist bin-old move bin-old bin
if exist build-deploy rmdir /S /Q build-deploy
