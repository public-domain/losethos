//8 has words
U0 Song()
{
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  music_tempo= 2.480;
  music_stacatto_factor= 0.902;
  try {
  while (!ScanKey) {
	Play("W13etAAA2eGF3A2GqF3etAAA2eGF3A2GqF",
		"Je\0\0sus\0\0made\0fur\0ni\0ture.\n\0Je\0\0sus\0\0made\0fur\0ni\0ture.\n\0");
	Play("etGGG3eAB2G3A2qGetGGG3eAB2G3A2qG",
		"God\0\0\0made\0\0all\0Na\0ture.\n\0God\0\0\0made\0\0all\0Na\0ture.\n\0");
	Play("G!3etGDC2qG3eCDetDA!!CC!A2G3sBABA2eG3F");
	Play("2qG!3etGDC2qG3eCDetDA!!CC!A2G3sBABA2eG3F");
  }
  } catch
    CatchAll;
  Snd(0);
}

Song;
