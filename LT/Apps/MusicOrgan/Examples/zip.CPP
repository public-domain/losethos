//8 has graphics


$PI,"<1>",1$






$PI,"<2>",2$




F64 t0=tM;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  F64 dt=(tM-t0)*music_tempo;
  U8 *inter;
  if (dt>=16.0) {
    inter=GrSpriteInterpolate($IB,"<1>",1$,$IB,"<2>",2$,Abs((4.0*tM%2-1)%1));
    GrSpritePlot3(dc,20,20,0,inter);
    Free(inter);
  }
}

U0 Song()
{
  TaskStruct *task;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;
  Fs->task_end_cb=&SndTaskEndCB;

  music_tempo=2.5;
  try {
    task=PopUpViewStr(
    "$LK-A,"BF:Proverbs,10:19"$\n"
    "$LK-A,"BF:Proverbs,15:2"$\n"
    "$LK-A,"BF:Proverbs,17:28"$\n"
	"$LK-A,"BF:Proverbs,29:11"$\n"
	);
    TaskWaitIdle(task);
    task->win_right=Fs->win_right;
    task->win_left=Fs->win_left;
    task->win_top=Fs->win_bottom+3;
    task->win_bottom=task->win_top+12;
    TaskDerivedValsUpdate(task);

    while (!ScanKey) {
      t0=tM;
      Play("3qGE2eG3BqGetDBDeDE2qGetG3BF",
	  "Wise\0men\0use\0few\0words.\n\0\0\0\0\0\0\0\0\0\0");
      Play("qGE2eG3BqGetDBDeDE2qGetG3BF",
	  "Wise\0men\0use\0few\0words.\n\0\0\0\0\0\0\0\0\0\0");
      Play("eEAsFEFEqFAetDBEAA2G3qCA",
	  "Fools\0\0kinda\0\0\0\0\0\0talk\0\0\0all\0day\0long.\n\0\0\0");
      Play("eEAsFEFEqFAetDBEAA2G3qCA",
	  "Fools\0\0kinda\0\0\0\0\0\0talk\0\0\0all\0day\0long.\n\0\0\0");
    }
  } catch
    CatchAll;
  SettingsPop;
}

Song;
           A         ����                                 	   ����	   ����   ����   ����!      !                                                                                                                                                                     
   ����                 ����             
   ����
   ����   ����   ����   ����   ����   ����	   ����   ����                                                                                          A         ����!   $                            	   ����	   ����   ����   ����!      !                                                                                                                                                                     
   ����                 ����             
   ����
   ����   ����   ����   ����   ����   ����	   ����   ����                         !      !      !      !                                     