//8 no nothing
U0 Song()
{
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  music_tempo= 2.480;
  music_stacatto_factor= 0.802;
  try {
  while (!ScanKey) {
	Play("W13qDsCCCCeBBCCqDsCCCCeBBCC");
	Play("sBBBBeAAsBA2G3AqAsBBBBeAAsBA2G3AqA");
	Play("AeB2G3GGqE2etG3F!B!eFGqFeEC");
	Play("qAeB2G3GGqE2etG3F!B!eFGqFeEC");
  }
  } catch
    CatchAll;
  Snd(0);
}

Song;
