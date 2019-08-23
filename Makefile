
INC = /C/'Program Files (x86)'/'Windows Kits'/10/Include/10.0.17763.0/um \
/C/'Program Files (x86)'/'Windows Kits'/10/Include/10.0.17763.0/shared \
/C/'Program Files (x86)'/'Microsoft Visual Studio'/2019/Community/VC/Tools/MSVC/14.21.27702/include \
/C/'Program Files (x86)'/'Windows Kits'/10/Include/10.0.17763.0/ucrt


LIB = /c/'Program Files (x86)'/'Windows Kits'/10/Lib/10.0.17763.0/um/x64

all: fingerDealing.h
	@echo $(INC)
	gcc -o fingercmd.exe  fingerDealing.cpp fingerPrintCMD.cpp -L . -l WinBio.lib -l stdc++ -idirafter /C/'Program Files (x86)'/'Windows Kits'/10/Include/10.0.17763.0/um -idirafter /C/'Program Files (x86)'/'Windows Kits'/10/Include/10.0.17763.0/shared -idirafter /C/'Program Files (x86)'/'Microsoft Visual Studio'/2019/Community/VC/Tools/MSVC/14.21.27702/include -idirafter /C/'Program Files (x86)'/'Windows Kits'/10/Include/10.0.17763.0/ucrt


