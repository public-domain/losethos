//8 no nothing
U0 Song()
{
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  music_tempo= 2.480;
  music_stacatto_factor= 0.902;
  try {
  while (!ScanKey) {
	Play("W13eBEBEBEBEBDBDBEBE");
	Play("BEBEBEBEBDBDBEBE");
	Play("qAAEFeGFCAqFE");
	Play("AAEFeGFCAqFE");
  }
  } catch
    CatchAll;
  Snd(0);
}

Song;
