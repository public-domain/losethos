/* This demonstrates "interpolation"
which is generating frames between
frames.  For interpolation to work,
your GrSprite must not be bitmaps and
must be identical, except shifted
around.  Cut and paste and use
"shift points."
*/





$PI,"<1>",1$









$PI,"<2>",2$




F64 t0;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  U8 *inter;

  if (tP(task)-t0<1.0) {
    inter=GrSpriteInterpolate($IB,"<1>",1$,$IB,"<2>",2$,(tP(task)-t0)%1.0);
    GrSpritePlot3(dc,100,100,0,inter);
    Free(inter);
  } else
    GrSpritePlot3(dc,100,100,0,$IB,"<1>",1$);
}

U0 Golf()
{
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;

  LtfClear;
  do t0=tP;
  while (PressAKey);
  SettingsPop;
}

Golf;
           �         6      >   ����>   ����D      >   ����F   ����>   ����:   ����B   ����@   ����@   ����D   ����D   ����D       8   ����@   ����   B   ����B                 �         6      >   ����>   ����B      >   ����F   ����8   ����:   ����B   ����@   ����@   ����D   ����D   ����8   ����8   ����@   ����   6   ����(       