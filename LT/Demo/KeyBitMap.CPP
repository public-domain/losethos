//I recently added bitmaps based on scan codes
//and haven't made use of them, yet.


$PI,"<1>",1$




I64 x=0;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  GrSpritePlot3(dc,x%100,100,0,$IB,"<1>",1$);
}


U0 KeyDownBitMapDemo()
{
  //This conversion routine is slow.
  I64 i_sc=A2ScanCode('i');

  "Press and hold $$FG,GREEN$$'i'$$FG$$.\n";
  while (!Bt(key_down_bitmap,i_sc))
    Yield;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;

//$LK,"key_down_bitmap","MN:key_down_bitmap"$ is indexed with the
//merged scan code.(left shift and
//right shift combined for example.)
//$LK,"key_down_bitmap2","MN:key_down_bitmap2"$ is indexed with the
//distinct scan code.See $LK,"Char","HI:Char"$ and $LK,"Scan Codes","MN:SC_INSERT"$.

  while (Bt(key_down_bitmap,i_sc)) {
    x++;
    Sleep(10);
  }
  SettingsPop;
  FlushMsgs;
}

KeyDownBitMapDemo;
           '         ����5          ����3       