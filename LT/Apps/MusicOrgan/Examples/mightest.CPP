//8 has words
U0 Song()
{
  Fs->task_end_cb=&SndTaskEndCB;
  music_tempo=2.5;
  while (TRUE) {
    Play("qBq2G3sGAGAetCBBqAq2G3sBBBBeFE",
      "I \0am \0feeling \0\0\0\0\0\0brave.\n\0");
    Play("qBq2G3sGAGAetCBBqAq2G3sBBBBeFE",
      "I \0am \0feeling \0\0\0\0\0\0brave.\n\0");
    Play("qCsDEDEq2G3qDqBqAqGq2G3",
      "Test \0me \0\0\0\0Lord. \0See \0if \0I'm \0wor\0thy.\n\0");
    Play("qCsDEDEq2G3qDqBqAqGq2G3",
      "Test \0me \0\0\0\0Lord. \0See \0if \0I'm \0wor\0thy.\n\0");
  }
}

Song;
