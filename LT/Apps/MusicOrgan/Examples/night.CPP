//9 has words
//This is "modified" because I name this one, not God.
//God called this a dirge.  I freaked
//out not wanting a dirge so I sped it
//up.  Then, I got to thinking someone
//has to write dirges.
U0 Song()
{
  Fs->task_end_cb=&SndTaskEndCB;
  music_tempo=2.0;
  while (TRUE) {
    "$$FG,GREEN$$God said this was a dirge.\n$$FG$$";
    Play("FqDqDeACqEqEqDqDq",
      "Gone \0to \0sleep \0for \0the \0fi\0nal \0time.\n\0");
    Play("FqDqDeACqEqEqDqDq",
      "Gone \0to \0sleep \0for \0the \0fi\0nal \0time.\n\0");
    Play("DqGqFeGFqFqEqEqBq",
      "\0Ash \0to... \0and \0\0dust \0to \0dust.\n\0");
    Play("DqGqFeGFqFqEqEqBq",
      "\0Ash \0to... \0and \0\0dust \0to \0dust.\n\0");
  }
}

Song;
