//8 has graphics

$PI,"<heart>",1$











U0 Song()
{
  TaskStruct *task;
  Fs->task_end_cb=&SndTaskEndCB;
  music_tempo=2.5;
  try {
  task=PopUpViewStr(
    "$$WW+H,1$$$LK-A,"BF:Matthew,22:35"$\n\n"
    "How many people hear \"Love God with all heart "
    "mind and soul; and neighbor as self,\" but walk "
    "away thinking \"Love neighbor with all heart "
    "mind and soul?\"\n"
    );
  TaskWaitIdle(task);
  task->win_right=Fs->win_right;
  task->win_left=Fs->win_left;
  task->win_top=Fs->win_bottom+3;
  task->win_bottom=task->win_top+12;
  TaskDerivedValsUpdate(task);
  while (!ScanKey) {
	Play("3qFeA2G3qBeGBqEetDCBqEeFF",
	"First, \0lo\0ve \0God \0with \0 \0ALL \0$$FG,RED$$heart$$FG$$, \0mind \0and \0soul.\n\0 \0 \0");
	PutSprite($IB,"<heart>",1$);
	"$$CM,0,6$$";
	Play("qFeA2G3qBeGBqEetDCBqEeFF",
	"First, \0lo\0ve \0God \0with \0 \0ALL \0$$FG,RED$$heart$$FG$$, \0mind \0and \0soul.\n\0 \0 \0");
	PutSprite($IB,"<heart>",1$);
	"$$CM,0,6$$";
	Play("GBsADADqE2G3eCDqG2sG3B2G3BetBDB",
	"What's \0 \0 \0 \0 \0 \0left?\0 \0I \0don't\0know.\n\0 \0 \0 \0 \0 \0 \0 \0");
	Play("eGBsADADqE2G3eCDqG2sG3B2G3BetBDB",
	"What's \0 \0 \0 \0 \0 \0left?\0 \0I \0don't\0know.\n\0 \0 \0 \0 \0 \0 \0 \0");
  }
  } catch
    CatchAll;
  Snd(0);
}

Song;
           �                  +      +   &      &      $      $                                                                                                                              (   #         &            #   $      "                                                                                 !         #                                             (   #         $   !                                             "               "      "               $   #      "      !       