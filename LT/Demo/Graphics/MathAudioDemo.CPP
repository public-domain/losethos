U0 FunDraw1(GrDC *dc,F64 (*f)(F64 t))
{
  I64 i,w=Fs->win_pixel_width,h=Fs->win_pixel_height;
  F64 scale=h/5.0;
  for (i=-w/2;i<=w/2;i++)
    GrLine(dc,w/2+i,h/2-scale*f(i/scale*2*�),w/2+i+1,h/2-scale*f((i+1)/scale*2*�));
}

U0 FunDraw2(GrDC *dc,F64 (*f)(F64 t,F64 p),F64 T)
{
  I64 i,w=Fs->win_pixel_width,h=Fs->win_pixel_height;
  F64 scale=h/5.0;
  for (i=-w/2;i<=w/2;i++)
    GrLine(dc,w/2+i,h/2-scale*f(i/scale,T),w/2+i+1,h/2-scale*f((i+1)/scale,T));
}

U0 Init(GrDC *dc)
{
  I64 i,w=Fs->win_pixel_width,h=Fs->win_pixel_height;
  F64 scale=h/5.0;
  LtfClear;
  GrDCFill(dc);
  dc->color=BLACK;
  GrLine(dc,0,h/2,w-1,h/2);
  GrLine(dc,w/2,0,w/2,h-1);
  for (i=-2;i<=2;i++) {
    dc->color=BLACK;
    GrLine(dc,w/2-3,h/2-scale*i,w/2+3,h/2-scale*i);
    if (i) {
      dc->color=DKGRAY;
      GrPrintF(dc,w/2+5,h/2-scale*i-FONT_HEIGHT/2,"%5.1f",ToF64(i));
    }
  }
  for (i=-4;i<=4;i++) {
    dc->color=BLACK;
    GrLine(dc,w/2+scale*i,h/2-3,w/2+scale*i,h/2+3);
    if (i) {
      dc->color=DKGRAY;
      GrPrintF(dc,w/2+scale*i-7*FONT_WIDTH/2,h/2+5+FONT_HEIGHT,"%5.1f*T",ToF64(i));
    }
  }
}

U0 SndMathDemo()
{
  GrDC *dc=GrDCAlias;

  Init(dc);
  dc->color=RED;
  FunDraw1(dc,&Sin);
  "$$FG,RED$$Sin$$FG$$\n";
  PressAKey;
  dc->color=BLUE;
  FunDraw2(dc,&Saw,1.0);
  "$$FG,BLUE$$Saw$$FG$$\n";
  PressAKey;

  Init(dc);
  dc->color=RED;
  FunDraw1(dc,&Cos);
  "$$FG,RED$$Cos$$FG$$\n";
  PressAKey;
  dc->color=BLUE;
  FunDraw2(dc,&Caw,1.0);
  "$$FG,BLUE$$Caw$$FG$$\n";
  PressAKey;
  dc->color=GREEN;
  FunDraw2(dc,&FullCaw,1.0);
  "$$FG,GREEN$$FullCaw$$FG$$\n";
  PressAKey;

  Init(dc);
  dc->color=RED;
  FunDraw1(dc,&Sin);
  "$$FG,RED$$Sin$$FG$$\n";
  PressAKey;
  dc->color=BLUE;
  FunDraw2(dc,&Tri,1.0);
  "$$FG,BLUE$$Tri$$FG$$\n";
  PressAKey;
  dc->color=GREEN;
  FunDraw2(dc,&FullTri,1.0);
  "$$FG,GREEN$$FullTri$$FG$$\n";
  PressAKey;

  GrDCFill(dc);
  GrDCDel(dc);
}

SndMathDemo;
