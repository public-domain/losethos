//This is executed by Adam at boot.
//See $LK,"Adam1 Start-up","FF:::/LT/OSMain/OSEnd.CPZ,Adam1:2"$
extern U64i SysEcho(U64i val);
SysEcho(0);  //0=echo OFF   1=echo ON
#include "/LT/OSMain/BootDefs.HPZ"
#include "/LT/OSMain/Adam1a.HPZ"
#include "/LT/Compiler/CmpDefs.HPZ"
#include "/LT/OSMain/Adam1b.HPZ"
#include "/LT/OSMain/Adam1c.HPZ"
#include "/LT/Compiler/Compiler.HPZ"
Cd("/LT/Adam");;
#include "Adam2.APZ"
