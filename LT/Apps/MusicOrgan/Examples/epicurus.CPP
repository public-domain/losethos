//7 has words
U0 Song()
{
  TaskStruct *task;
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  music_tempo= 2.500;
  music_stacatto_factor= 0.900;
  try {
  task=PopUpViewStr(
    "$$WW+H,1$$You get what you pray for."
    "Most people are fools like King Midas.People have laughable notions of Heaven."
    "You'd be surprised -- I can't imagine things being much better than Earth "
    "because we need a sense of purpose."
    "Don't ask for stuff, let God pick, but He might get annoyed at that."
    "God wants us to be happy.He tries hard.If you do get what you pray for, you damn "
    "well better be happy, lil' sonfabitch!\n\n"
    "$LK,"Ecclesiastes,3:13","BF:Ecclesiastes,3:13"$\n"
    "$LK,"John,16:24","BF:John,16:24"$\n"
    );
  TaskWaitIdle(task);
  task->win_right=Fs->win_right;
  task->win_left=Fs->win_left;
  task->win_top=Fs->win_bottom+3;
  task->win_bottom=task->win_top+12;
  TaskDerivedValsUpdate(task);
  while (!ScanKey) {
	Play("W13qBeCDqCAetAACeECsF2G3F2GqG",
		"I'm\0on\0a\0mis\0sion\0from\0\0\0my\0\0God.\n\0\0\0\0\0");
	Play("3BeCDqCAetAACeECsF2G3F2GqG",
		"I'm\0on\0a\0mis\0sion\0from\0\0\0my\0\0God.\n\0\0\0\0\0");
	Play("3etEDDDGABG2GeG3E2G3GqF2G3etGCD",
		"He\0\0\0told\0\0\0me\0\0\0\0to\0\0be\0hap\0py.\n\0\0\0\0");
	Play("EDDDGABG2GeG3E2G3GqF2G3etGCD",
		"He\0\0\0said\0\0\0to\0\0\0\0make\0\0it\0snap\0py.\n\0\0\0\0");
  }
  } catch
    CatchAll;
  Snd(0);
}

Song;
