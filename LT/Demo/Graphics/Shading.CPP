//"ls" is light source.

U0 Lighting(GrDC *dc,P3I32 *ls,P3I32 *poly,I64 color)
{ //color is a color from 0-7
  P3I32 v1,v2;
  I64 *r=dc->r,i,vn_x,vn_y,vn_z;
  F64 d;

  v1.x=poly[0].x-poly[1].x;
  v1.y=poly[0].y-poly[1].y;
  v1.z=poly[0].z-poly[1].z;

  v2.x=poly[2].x-poly[1].x;
  v2.y=poly[2].y-poly[1].y;
  v2.z=poly[2].z-poly[1].z;

//V1 and V2 are vects along two sides
//of the polygon joined at point[1].

  vn_x=v1.y*v2.z-v1.z*v2.y;
  vn_y=v1.z*v2.x-v1.x*v2.z;
  vn_z=v1.x*v2.y-v1.y*v2.x;

  if (d=Sqrt(SqrI64(vn_x)+SqrI64(vn_y)+SqrI64(vn_z)))
    d=1<<16/d;
  vn_x*=d;
  vn_y*=d;
  vn_z*=d;
//Vn is the cross product of V1 and V3
//which means it is perpendicular.It
//is the normal vect to the surface.
//It has been scaled to length 65536.
  GrRot(r,&vn_x,&vn_y,&vn_z);
  i=(vn_x*ls->x+vn_y*ls->y+vn_z*ls->z)>>16;
//The dot product of the light source
//vect and the surface normal
//gives an illumination number.


//LoseThos will generate a random U16
//and compare to dither_probability and
//will pick from two colors.
//Probability dithering does not work with pen_width>1 at this time.
  if (i<0) {
    dc->color=ROPF_PROBABILITY_DITHER+BLACK<<16+color;
    dc->dither_probability=-i;
  } else {
    dc->color=ROPF_PROBABILITY_DITHER+(color^8)<<16+color;
    dc->dither_probability=i;
  }
}

#define RINGS	8
#define FACES	32
#define SLOP	0.03 //Gaps appear without this.

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  Ctrl *c=CtrlFindUnique(task,CTRLT_VIEWING_ANGLES);
  ViewAnglesState *s=c->state;
  F64 tt=0.5*(Sin(�*2*(tP(task)%10.0)/10.0)+2.0),
     �,�2,�,�2,radius,d;
  I64 i,j,cx=task->win_pixel_width/2,cy=task->win_pixel_height/2;
  P3I32 poly[3],ls;

  dc->flags|=DCF_TRANSFORMATION;
  GrDCAllocDepthBuf(dc);

  GrIdentEqu(dc->r);
  GrRotZEqu(dc->r,s->az);
  GrRotYEqu(dc->r,s->ay);
  GrRotXEqu(dc->r,s->ax+�);
  GrScaleMatEqu(dc->r,tt);
  GrSetRotMat(dc,dc->r);

  ls.x=-(ipx-task->win_pixel_left-task->win_scroll_x-cx);
  ls.y=-(ipy-task->win_pixel_top-task->win_scroll_y-cy);
  ls.z=GR_WIDTH/8;
  d=1<<16/P3I32Norm(&ls);
  ls.x*=d;
  ls.y*=d;
  ls.z*=d;


  dc->x=cx;
  dc->y=cy;
  dc->z=MaxI64(cx,cy);
  radius =MinI64(cx,cy)/2;


  for (i=0;i<RINGS;i++) {
    � =    i*�/2/RINGS;
    �2=(i+1)*�/2/RINGS+SLOP;
    for (j=0;j<FACES;j++) {
      � =	 j*2*�/FACES;
      �2=(j+1)*2*�/FACES+SLOP;

      //Upper half
      poly[0].x=radius*Cos(�)*Cos(�);
      poly[0].y=radius*Cos(�)*Sin(�);
      poly[0].z=radius*Sin(�);
      poly[1].x=radius*Cos(�)*Cos(�2);
      poly[1].y=radius*Cos(�)*Sin(�2);
      poly[1].z=radius*Sin(�);
      poly[2].x=radius*Cos(�2)*Cos(�+2*�/FACES/2);
      poly[2].y=radius*Cos(�2)*Sin(�+2*�/FACES/2);
      poly[2].z=radius*Sin(�2);
      Lighting(dc,&ls,poly,BLUE);
      GrFillPoly3(dc,3,poly);

      poly[2].x=radius*Cos(�2)*Cos(� +2*�/FACES/2);
      poly[2].y=radius*Cos(�2)*Sin(� +2*�/FACES/2);
      poly[2].z=radius*Sin(�2);
      poly[1].x=radius*Cos(�2)*Cos(�2+2*�/FACES/2);
      poly[1].y=radius*Cos(�2)*Sin(�2+2*�/FACES/2);
      poly[1].z=radius*Sin(�2);
      poly[0].x=radius*Cos(�)*Cos(�2);
      poly[0].y=radius*Cos(�)*Sin(�2);
      poly[0].z=radius*Sin(�);
      Lighting(dc,&ls,poly,BLUE);
      GrFillPoly3(dc,3,poly);

      //Lower half
      poly[2].x=radius*Cos(�)*Cos(�);
      poly[2].y=radius*Cos(�)*Sin(�);
      poly[2].z=-radius*Sin(�);
      poly[1].x=radius*Cos(�)*Cos(�2);
      poly[1].y=radius*Cos(�)*Sin(�2);
      poly[1].z=-radius*Sin(�);
      poly[0].x=radius*Cos(�2)*Cos(�+2*�/FACES/2);
      poly[0].y=radius*Cos(�2)*Sin(�+2*�/FACES/2);
      poly[0].z=-radius*Sin(�2);
      Lighting(dc,&ls,poly,RED);
      GrFillPoly3(dc,3,poly);

      poly[0].x=radius*Cos(�2)*Cos(� +2*�/FACES/2);
      poly[0].y=radius*Cos(�2)*Sin(� +2*�/FACES/2);
      poly[0].z=-radius*Sin(�2);
      poly[1].x=radius*Cos(�2)*Cos(�2+2*�/FACES/2);
      poly[1].y=radius*Cos(�2)*Sin(�2+2*�/FACES/2);
      poly[1].z=-radius*Sin(�2);
      poly[2].x=radius*Cos(�)*Cos(�2);
      poly[2].y=radius*Cos(�)*Sin(�2);
      poly[2].z=-radius*Sin(�);
      Lighting(dc,&ls,poly,RED);
      GrFillPoly3(dc,3,poly);
    }
  }
}

//See $LK,"::/LT/Demo/Graphics/GrSprite2.CPZ","FI:::/LT/Demo/Graphics/GrSprite2.CPZ"$.
//for a GrSprite example.

//See $LK,"GrSpriteMesh","MN:GrSpriteMesh"$() for a fancy example.

U0 Main()
{
  Ctrl *c=ViewAnglesNew;
  ViewAnglesState *s=c->state;
  s->sx=2*VIEWANGLES_SNAP;
  s->sy=7*VIEWANGLES_SNAP;
  s->sz=6*VIEWANGLES_SNAP;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  LtfClear;
  WinMax;
  WinBorder;
  WordStat;
  Fs->draw_it=&DrawIt;
  try {
    "\n\nMove mouse to change light source.\n\n";
    PressAKey;
  } catch
    CatchAll;
  SettingsPop;
  LtfClear;
  ViewAnglesDel;
}

Main;
