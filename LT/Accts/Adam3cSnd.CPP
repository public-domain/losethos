//Change anything you want in this
//acct file or others.

Cd("/LT/Adam/Snd");;
#include "SndFile"
#include "HDAudio"
//Uncomment this after running $LK,"::/LT/Adam/Snd/HDCfg.CPZ","FI:::/LT/Adam/Snd/HDCfg.CPZ"$.
//#include "HOME/HDAudioCfg.APZ"
LBtr(&sys_semas[SYS_SEMA_SND],0);

Cd("HOME");;
