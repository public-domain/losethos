//See $LK,"::/LT/Demo/GameStarters/Stadium/StadiumGen.CPZ","FI:::/LT/Demo/GameStarters/Stadium/StadiumGen.CPZ"$.

U0 ViewScreenCapture(U8 *filename)
{
  GrDC *dc=GrDCAlias(gr_dc2,Fs),
       *b=LGRRead(filename);
  GrDCBlot(dc,0,0,b);
  GrDCDel(b);
  GrDCDel(dc);
}


U0 Main()
{
  "Capture screen...\n";
  PressAKey;
  LGRCaptureScreen("HOME/SnapShot.LGZ");

  "View captured screen...\n";
  PressAKey;
  ViewScreenCapture("HOME/SnapShot.LGZ");

  PressAKey;
  GrDCFill;
}

Main;
