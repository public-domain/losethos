


	    $PI,"",1$



U0 Logo()
{
  GrDC *dc=GrDCAlias;
  dc->flags|=DCF_TRANSFORMATION;
  LtfClear;
  while (!ScanChar) {
    dc->x=Fs->win_pixel_width>>1;
    dc->y=Fs->win_pixel_height>>1;

//$LK,"Unwrap","MN:Unwrap"$() converts an angle to an
//equivalent angle in a range starting
//from [arg2,arg2+�*2).
    GrIdentEqu(dc->r);
    GrRotYEqu(dc->r,Abs(Unwrap(tP,-�/2)-�/2)-�/2);
    GrRotXEqu(dc->r,30.0*�/180);
    GrSetRotMat(dc,dc->r);

    GrSpritePlot3(dc,0,0,0,$IB,"<1>",1$);
//See also $LK,"GrSpritePlotRotX3b","MN:GrSpritePlotRotX3b"$(),
//$LK,"GrSpritePlotRotY3b","MN:GrSpritePlotRotY3b"$(),$LK,"GrSpritePlotRotZ3b","MN:GrSpritePlotRotZ3b"$().

    WinSync;
    GrDCFill(dc);
  }
  GrDCDel(dc);
}

Logo;
           *        ������������   ����   <      ������������   ����   ����   ����   ����������������������������������������������������    ����    ����    ����    ����   ����   ����   ������������   ����   ����   ����    ����    ������������������������<   ����    ����          ����                                    ����      $      $            $      $      $   ����$   ����   ����(   ����4   ����(   ����(       (       4       4       4      4      (      <   ����<      <      <       