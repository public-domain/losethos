//8 no nothing
U0 Song()
{
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  music_tempo= 2.480;
  music_stacatto_factor= 0.902;
  try {
  while (!ScanKey) {
	Play("W12qG3eCB2hGqG3eCB2qG3A");
	Play("CeBA2G3DqEeDCBA2qG3A");
	Play("eBA2h.G3qE!!CAF");
	Play("FD!A!eEDqE!!CAF");
	Play("FD!A!eED");
  }
  } catch
    CatchAll;
  Snd(0);
}

Song;
