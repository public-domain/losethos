Cd("::/LT/Demo/OldSchool/Demo2");;
Cmp("OD1");
Cmp("OD2");
Cmp("ODMain");
Load("OD1");
Load("OD2");
Load("ODMain");
#include "ObjDemo.HPZ"

ObjDemo("Success");

#ifndef BinRep
#include "::/LT/Adam/BinRep.APZ"
#endif

PressAKey;
BinRep("OD2");

PressAKey;
BinRep("ODMain");

PressAKey;
HashRep(Fs->hash_table,HTT_ALL,NULL,"-p-r");

