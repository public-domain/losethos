//7 has graphics


$PI,"",1$










$PI,"",2$















F64 t0=tM;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  F64 y,dt=(tM-t0)*music_tempo;
  if (dt>=24.0)
    GrSpritePlot3(dc,0,0,0,$IB,"<2>",2$);
  else {
    y=Sin(2*�*(tM%1.0-0.5)/1.0);
    GrSpritePlot3(dc,0,5.0*y,0,$IB,"<1>",1$);
    GrSpritePlot3(dc,0,-5.0*y,0,$IB,"<2>",2$);
  }
}

U0 Song()
{
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;
  Fs->task_end_cb=&SndTaskEndCB;

  TaskStruct *task=PopUpViewStr(
      "$LK-A,"BF:Mark,4:37"$\n"
      );
  TaskWaitIdle(task);
  task->win_right=Fs->win_right;
  task->win_left=Fs->win_left;
  task->win_top=Fs->win_bottom+3;
  task->win_bottom=task->win_top+12;
  TaskDerivedValsUpdate(task);
  music_tempo=2.5;
  try {
    while (!ScanKey) {
      t0=tM;
      LtfClear;
      "$$FG,WHITE$$$$BG,CYAN$$\n";
      Play("3qEsGEGEqDsCCCCqBsDEDEqFG",
	  "Lord,\0there's\0\0a\0\0storm\0u\0\0\0\0pon\0the\0\0\0\0sea.\n\0\0");
      Play("EsGEGEqDsCCCCqBsDEDEqFG",
	  "Lord,\0there's\0\0a\0\0storm\0u\0\0\0\0pon\0the\0\0\0\0sea.\n\0\0");
      Play("GEC2G3etB2G3FGGEDBBeBE",
	  "Re\0lax\0fel\0las.\n\0\0\0\0\0\0\0\0\0\0\0\0");
      Play("qGEC2G3etB2G3FGGEDBBeBE",
	  "(Sea\0be\0came\0glass.)\n\0\0\0\0\0\0\0\0\0\0\0\0");
    }
  } catch
    CatchAll;
  SettingsPop;
}

Song;
           �         �   i   %   �M��y�&I�4I�$I�$I�!I�$)�$I&)�"I�$I�$I�$I�<ϓ4I��y�6I�$i�&��y   i   #   �۶m^]ם���aذmða[6l۶m۶m۶m۶m۶m۶mۼ��   �      $M�$I�$I�$I�$I�$I�$I�$I�$M��y��y�<�&y�$I�$I��H�$I�$I�$J   .     $O��I�<I�$I�&I�$I�$I�$I�$I�$                   ����,      (      (   �   5   �   5   |  K   9   *   8   ;   8   ;   �   J   �   8   �   I   2  D   2  J   7   ;      7      7   ����=      6      J   b   @   a   J   ����`   �  `   d   B      @   5   @   '   1   U   7   u   F   �   =    