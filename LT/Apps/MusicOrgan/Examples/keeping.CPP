//8 no nothing
U0 Song()
{
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  music_tempo= 2.151;
  music_stacatto_factor= 0.780;
  try {
  while (!ScanKey) {
	Play("W13eCsBBeA#BCDCBCsBBeA#BCDCB");
	Play("etDCBeCBCBABetDCBeCBCBAB");
	Play("!A2G!3sBG!B!GFC!FCBFBFetF!GAsFGF!G!qG!D");
	Play("!eA2G!3sBG!B!GFC!FCBFBFetF!GAsFGF!G!qG!D");
  }
  } catch
    CatchAll;
  Snd(0);
}

Song;
