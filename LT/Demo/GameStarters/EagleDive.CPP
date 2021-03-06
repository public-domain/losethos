//This uses $LK,"fixed-point","FI:::/LT/Demo/Lectures/FixedPoint.CPZ"$.

//This program can be static compiled to a .BIZ or JIT.
//If static compiling, override the finename extention warning.
#ifstatic
#include "::/LT/OSMain/StaticAdam.HPZ"
I64 best_score=0; //Doesn't work yet for statics
#else
AcctRegSetDftEntry("LT/EagleDive","I64 best_score=0;\n");
AcctRegExecuteBranch("LT/EagleDive");
#endif


//Keep these power of two so shift is used instead of multiply
//to index arrays.
#define MAP_WIDTH		1024
#define MAP_HEIGHT		1024

#define MAP_SCALE		150
#define DISPLAY_SCALE		100
#define CTRLS_SCALE		0.05

//I think I did these so the heads-up showed intelligable numbers.
//Scaling is a mess.
#define COORDINATE_SCALE	256
#define COORDINATE_BITS		8

#define WATER_ELEVATION		15
#define ROCK_ELEVATION		45
#define SNOW_ELEVATION		55

//Too big makes off-screen draws take place.
#define MAX_PANEL_SIZE		16

class Panel
{
  Panel *next;
  I32 n,   //num polygon sides
     num; //panel number
  P3I32 *points;
  I64 color,cnt;
};

class Fish
{
  Fish *next,*last;
  I64 x,y,z;
} fish_root;





$PI,"<1>",1$





I64 critical_section_flag;
U0 CriticalSectionStart()
{
  while (LBts(&critical_section_flag,0))
    Yield;
}
U0 CriticalSectionEnd()
{
  LBtr(&critical_section_flag,0);
}

//These are $BK,1$NOT$BK,0$ deallocated after the program ends,
//but I did them this way for your benefit
//so 2D array indexing is not done by hand.
//You can type "EagleDive;" at the command line
//to rerun without new allocations.
//Normally, just spawn a popup task,
//run it and kill it using an icon.
#ifstatic
#exe {OptOn(OPTf_GLBLS_ON_DATA_HEAP);};
#endif
I16 elevations[MAP_HEIGHT][MAP_WIDTH];
I32 normals[MAP_HEIGHT][MAP_WIDTH*2];
Panel *panels[MAP_HEIGHT][MAP_WIDTH*2];
#ifstatic
#exe {OptOff(OPTf_GLBLS_ON_DATA_HEAP);};
#endif

I64 num_panels;
Panel *panel_root;
U8 *panels_processed_bitmap;

F64 game_t0,v_grid_size,span_factor,�,�1,�2,speed;

I64 x,y,z,game_score;

BoolI8 game_over;

class MPCtrl {
  I64 mp_cnt;
  U64 do_update;
  F64 update_done_t[MP_MAX_PROCESSORS];
  U64 done_flags;
  BoolI64 app_done;
} mp;

U0 FSTransform(GrDC *dc,I64 *x,I64 *y,I64 *z)
{
  I64 zz;
//I accidentally made the Z coordinate sign backward
//from the fill-poly depth buf scheme, so it is negated at the end.
//Negative Z coordinates are the ones we want plotted.
  GrRot(dc->r,x,y,z);

//We don't want divide by zero or make too monstrous foreground objects.
//The foreground object boundary is kinda crappy looking when
//you point toward the ground.
  *z=-*z;
  zz=*z+50;
  if (zz>0) {
//The divide causes "foreshortening" which makes the distance go to a vanishing point.
    *x = *x * DISPLAY_SCALE/ zz;
    *y = *y * DISPLAY_SCALE/ zz;
  }
  *x+=dc->x;
  *y =dc->y-*y;
}



U0 CalcNormals()
{
/*The idea is that there are two triangles
forming every grid square and we find the normal
vect with a curl.

 i  j  k
 0  1  dz2
 1  0  dz1

Normal:
dz1*i + dz2*j - k

  i   j   k
 -1   0  -dz3
  0  -1  -dz4

Normal:
-dz3*i - dz4*j + k

 
Basically, we want to combine grid
locations with the same normals.  These
are not unit vects for one thing.

What I came-up with is dz1/dz2 and dz3/dz4.

It works pretty well.It combines
more than it should, but that's good
because it would  result in too many
panels.

*/
  I64 i,j,k,dz1,dz2;
  for (j=0;j<MAP_HEIGHT-1;j++) {
    k=0;
    for (i=0;i<MAP_WIDTH-1;i++) {
      dz1=elevations[j][i+1]-elevations[j][i];
      dz2=elevations[j+1][i]-elevations[j][i];
      if (dz2)
	normals[j][k++]=dz1<<16/dz2;
      else {
	if (!dz1)
	  normals[j][k++]=0;
	else if (dz1>0)
	  normals[j][k++]=MAX_I32;
	else
	  normals[j][k++]=MIN_I32;
      }

      dz1=elevations[j+1][i+1]-elevations[j][i+1];
      dz2=elevations[j+1][i+1]-elevations[j+1][i];
      if (dz2)
	normals[j][k++]=dz1<<16/dz2;
      else {
	if (!dz1)
	  normals[j][k++]=0;
	else if (dz1>0)
	  normals[j][k++]=MAX_I32;
	else
	  normals[j][k++]=MIN_I32;
      }
    }
    normals[j][k++]=0;
    normals[j][k++]=0;
  }
  k=0;
  for (i=0;i<MAP_WIDTH-1;i++) {
    normals[j][k++]=0;
    normals[j][k++]=0;
  }
}

BoolI64 TestSameSlope(I64 x,I64 y,I64 w,I64 h)
{
  I64 k1,k2,xx,yy,s;
  if (!(0<=x && x+w<MAP_WIDTH && 0<=y && y+h<MAP_HEIGHT))
    return FALSE;
    //Works well averaging the two slopes of two triangles.
    //If it is too picky, we get too many panels.
  s=normals[y][x*2]+normals[y][x*2+1];
  for (k2=0;k2<h;k2++)
    for (k1=0;k1<w;k1++) {
      xx=(x+k1)*2;
      yy=y+k2;
      if (normals[yy][xx]+normals[yy][xx+1]!=s)
       return FALSE;
    }
  return TRUE;
}

U0 MPDoPanels(TaskStruct *task)
{
  I64 i,j,k,l,k1,k2,w,h,threshold,lo,hi;
  BoolI8 cont;
  Panel *tempp,*start_ptr=NULL,*end_ptr=NULL;
  P3I32 *poly;
  Fish *tempf;
  lo=Gs->num*(MAP_HEIGHT-1)/mp.mp_cnt;
  hi=(Gs->num+1)*(MAP_HEIGHT-1)/mp.mp_cnt;
  for (threshold=8;threshold>=1;threshold--)
    for (j=lo;j<hi;j++) {
      k=0;
      for (i=0;i<MAP_WIDTH-1;i++) {
	if (!panels[j][k]) {
	  w=1;
	  h=1;
	  do {
	    cont=FALSE;
	    if (w<MAX_PANEL_SIZE && TestSameSlope(i,j,w+1,h)) {
	      w++;
	      cont=TRUE;
	    }
	    if (h<MAX_PANEL_SIZE && TestSameSlope(i,j,w,h+1)) {
	      h++;
	      cont=TRUE;
	    }
	  } while (cont);
	  if (w>=threshold || h>=threshold) {
	    tempp=CAlloc(sizeof(Panel),task);
	    lock {tempp->num=num_panels++;}
	    l=elevations[j][i];
	    if (l<=WATER_ELEVATION*MAP_SCALE &&
		elevations[j][i+w-1]<=WATER_ELEVATION*MAP_SCALE &&
		elevations[j+h-1][i]<=WATER_ELEVATION*MAP_SCALE &&
		elevations[j+h-1][i+w-1]<=WATER_ELEVATION*MAP_SCALE) {
	      tempp->color=BLUE;
	      if (Rand<0.05) {
		tempf=MAlloc(sizeof(Fish),task);
		tempf->x=(i+w/2)*MAP_SCALE;
		tempf->y=(j+h/2)*MAP_SCALE;
		tempf->z=l;
		CriticalSectionStart;
		InsQue(tempf,fish_root.last);
		CriticalSectionEnd;
	      }
	    } else {
	      if (RandI16&1) {
		if (l<ROCK_ELEVATION*MAP_SCALE)
		  tempp->color=LTGREEN;
		else if (l<SNOW_ELEVATION*MAP_SCALE) {
		  if (!(RandU16&3))
		    tempp->color=LTGRAY;
		  else
		    tempp->color=LTGREEN;
		} else {
		  if (!(RandU16&3))
		    tempp->color=WHITE;
		  else
		    tempp->color=LTGRAY;
		}
	      } else {
		if (l<ROCK_ELEVATION*MAP_SCALE)
		  tempp->color=GREEN+LTGREEN<<16+ROPF_DITHER;
		else if (l<SNOW_ELEVATION*MAP_SCALE) {
		  if (!(RandU16&3))
		    tempp->color=DKGRAY+LTGRAY<<16+ROPF_DITHER;
		  else
		    tempp->color=GREEN+LTGREEN<<16+ROPF_DITHER;
		} else {
		  if (!(RandU16&3))
		    tempp->color=LTGRAY+WHITE<<16+ROPF_DITHER;
		  else
		    tempp->color=DKGRAY+LTGRAY<<16+ROPF_DITHER;
		}
	      }
	    }
	    tempp->n=4;
	    poly=tempp->points=MAlloc(sizeof(P3I32)*tempp->n,task);
	    poly[0].x=MAP_SCALE*i;
	    poly[0].y=MAP_SCALE*j;
	    poly[0].z=elevations[j][i];
	    poly[1].x=MAP_SCALE*(i+w);
	    poly[1].y=MAP_SCALE*j;
	    poly[1].z=elevations[j][i+w];
	    poly[2].x=MAP_SCALE*(i+w);
	    poly[2].y=MAP_SCALE*(j+h);
	    poly[2].z=elevations[j+h][i+w];
	    poly[3].x=MAP_SCALE*i;
	    poly[3].y=MAP_SCALE*(j+h);
	    poly[3].z=elevations[j+h][i];
	    tempp->next=start_ptr;
	    start_ptr=tempp;
	    if (!end_ptr)
	      end_ptr=tempp;
	    for (k2=0;k2<h;k2++)
	      for (k1=0;k1<w;k1++) {
		panels[j+k2][(i+k1)*2]=tempp;
		panels[j+k2][(i+k1)*2+1]=tempp;
	      }
	  }
	}
	k+=2;
      }
    }
  if (end_ptr) {
    CriticalSectionStart;
    if (end_ptr)
      end_ptr->next=panel_root;
    panel_root=start_ptr;
    CriticalSectionEnd;
  }
  LBts(&mp.done_flags,Gs->num);
}


U0 InitMap()
{
//We make a topographic data structure "elevations[][]"
//and convert it to panels.  The conversion to panels leaves
//gaps the way I did it, but it looks good, except for sky leaking through
//There is no wrong way to pick colors if it looks good, unless
//you wanted to do lighting.

//"panels[][]" holds the flat and sloped panels for each spot, while
//"hstep_panels[][]" holds side panels in cases of step-like formations.
//They require a separate array because a single "elevation[][]" spot can
//have a panel on top and a panel on the side like stairs.

  I64 i,j,l,k1,k2,x,y,xx,yy;
  MemSet(elevations,0,sizeof(elevations));
  MemSet(panels,0,sizeof(panels));
  for (i=0;i<MAP_WIDTH*MAP_HEIGHT/128;i++) {
    x=RandU32%MAP_WIDTH;
    y=RandU32%MAP_HEIGHT;
    j=1<<(RandU32%6);
    l=0;
    while (j--) {
      if (!l && RandU16<MAX_U16/4)
	l=RandU16%(j+1);
      if (l) {
	for (k1=-j;k1<=j;k1++)
	  for (k2=-j;k2<=j;k2++) {
	    xx=x+k2; yy=y+k1;
	    if (0<=xx<MAP_WIDTH &&
		0<=yy<MAP_HEIGHT)
	      elevations[yy][xx]+=MAP_SCALE/2;
	  }
	  l--;
      }
    }
  }

  xx=x/(MAP_SCALE*COORDINATE_SCALE);
  yy=y/(MAP_SCALE*COORDINATE_SCALE);
  z+=elevations[yy][xx]*COORDINATE_SCALE;

  for (j=0;j<MAP_HEIGHT;j++)
    for (i=0;i<MAP_WIDTH;i++)
      if (elevations[j][i]<WATER_ELEVATION*MAP_SCALE)
	elevations[j][i]=WATER_ELEVATION*MAP_SCALE;


  CalcNormals;

  panel_root=NULL;
  num_panels=0;

  mp.done_flags=0;
  for (i=1;i<mp.mp_cnt;i++)
    MPSpawn(&MPDoPanels,Fs,"Do Panels",1<<i);
  MPDoPanels(Fs);
  while (mp.done_flags!=1<<mp.mp_cnt-1)
    Yield;
  panels_processed_bitmap=MAlloc((num_panels+7)>>3);
}


GrDC *main_dc;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  main_dc->flags|=DCF_NO_TRANSPARENTS;
  GrDCBlot(dc,0,0,main_dc);
}


/*$PI,"",2$





















Cores render strips.  The strips are wider toward the horizon by span_factor
and overlap +/- 15%. The cores check the panel map array
and render the panel for each square and mark-it done.

The depth buf is not locked in the graphic routines
so we get some glitches.

*/

U0 MPDrawIt(TaskStruct *task,GrDC *dc)
{
//This is identical to the previous routine but is run by core#1
  I64 i,j,ww,hh,*s2w,x1,y1,z1, xx,yy,
      xh,yh,zh, yh2,xh2, reg x1w, reg y1w, x1h,y1h,
      x3,y3,z3,
      cx=task->win_pixel_width>>1,
      cy=task->win_pixel_height>>1;
  F64 h_grid_size,t0=tNP;
  Panel reg *tempp;

  xx=x/(MAP_SCALE*COORDINATE_SCALE);
  yy=y/(MAP_SCALE*COORDINATE_SCALE);

  //World to screen coordinates
  GrIdentEqu(dc->r);
  GrRotZEqu(dc->r,�2);
  GrRotXEqu(dc->r,�);
  GrRotZEqu(dc->r,�1);
  GrSetRotMat(dc,dc->r);

  //Screen to world coordinates

//This gives us the vects for stepping through the grid in
//the direction the plane is facing. we step horizontally and vertically
//and use the reciprocal slope principle	y=mx+b and y=(-1/m)x+b are perpendicular.

  s2w=GrIdent;
  GrRotZEqu(s2w,-�1);
  GrRotXEqu(s2w,-�);
  GrRotZEqu(s2w,-�2);
  xh=0;
  yh=0;
  zh=-256;
  GrRot(s2w,&xh,&yh,&zh);

//The layer for core#1 is not cleared automatically
//it is persistent.  I have carefully syncronized to the update
//cycle initiated by core#0 to prevent flicker.

  dc->flags|=DCF_TRANSFORMATION;
  dc->transform=&FSTransform;
  dc->x=cx;
  dc->y=cy;

//dc->x and the translation part of dc->r are identical in effect,
//but we use both to show-off.	We could add offsets together and use one or the other.

  x1=-x>>COORDINATE_BITS;
  y1=-y>>COORDINATE_BITS;
  z1=-z>>COORDINATE_BITS;
  GrRot(dc->r,&x1,&y1,&z1);
  GrSetTranslation(dc->r,x1,y1,z1);

  //This is a refinement.
  if (Abs(�*180/�)>90) {
    x3=0;
    y3=-cy;
    z3=0;
    GrRot(s2w,&x3,&y3,&z3);
    xx+=x3;
    yy+=y3;
  }

  h_grid_size=4.0*v_grid_size*(Gs->num+span_factor)/span_factor;
  if (Gs->num&1) { //alternate left-right,right-left
    yh2=-yh;
    xh2=-xh;
  } else {
    yh2=yh;
    xh2=xh;
  }

  ww=h_grid_size;
  hh=1.30*v_grid_size; //Cores overlap 15% on each side of strip.
  //Calc starting point.
  x1h=xx<<8+0.5*yh2*h_grid_size+xh*v_grid_size*(Gs->num+1.15);
  y1h=yy<<8-0.5*xh2*h_grid_size+yh*v_grid_size*(Gs->num+1.15);

  xh=-xh; //Back to front to help with depth.
  yh=-yh;

  //Take half steps to cover whole grid.
  xh>>=1;  yh>>=1;
  xh2>>=1; yh2>>=1;
  ww<<=1;
  hh<<=1;

  for (j=0;j<hh;j++) {
    x1w=x1h;
    y1w=y1h;
    for (i=0;i<ww;i++) {
      x1=x1w>>8; y1=y1w>>8;
      if (0<=x1<MAP_WIDTH && 0<=y1<MAP_HEIGHT) {
	  if ((tempp=panels[y1][x1*2]) && !LBts(panels_processed_bitmap,tempp->num)) {
	    if (tempp->cnt>8*(1.1-Gs->idle_factor)) {
	      dc->color=tempp->color;
	      tempp->cnt=GrFillPoly3(dc,tempp->n,tempp->points);
	    } else
	      tempp->cnt++;
	  }
      }
      x1w-=yh2;
      y1w+=xh2;
    }
    x1h+=xh;
    y1h+=yh;
  }
  Free(s2w);
  mp.update_done_t[Gs->num]=tNP-t0;
}

U0 APEagleDiveTask(TaskStruct *master_task)
{
  GrDC *dc=GrDCAlias(main_dc,master_task);
  if (!IsSysDeterministic)
    Preempt(ON);
  while (!mp.app_done) {
    LBts(&Fs->task_flags,TASKf_IDLE);
    while (!LBtr(&mp.do_update,Gs->num) && !mp.app_done)
      Yield;
    LBtr(&Fs->task_flags,TASKf_IDLE);
    if (!mp.app_done)
      MPDrawIt(master_task,dc);
  }

//We made an alias of this we don't want freed.
  dc->depth_buf=NULL;

  GrDCDel(dc);
  LBts(&mp.done_flags,Gs->num);
}

U0 DrawHorizon(GrDC *dc)
{
  I64 x1,y1,z1,x2,y2,z2,xh,yh,zh,*s2w=GrIdent,
      cx=Fs->win_pixel_width>>1,
      cy=Fs->win_pixel_height>>1;
  P3I32 p[4];
  I32 *old_db=dc->depth_buf;
  dc->depth_buf=NULL;

  GrIdentEqu(dc->r);
  GrRotZEqu(dc->r,�2);
  GrRotXEqu(dc->r,�);
  GrRotZEqu(dc->r,�1);
  GrSetRotMat(dc,dc->r);
  dc->flags&=~DCF_TRANSFORMATION;
  dc->transform=&FSTransform;
  dc->x=cx;
  dc->y=cy;

  GrRotZEqu(s2w,-�1);
  GrRotXEqu(s2w,-�);
  GrRotZEqu(s2w,-�2);
  xh=0;
  yh=0;
  zh=-256;
  GrRot(s2w,&xh,&yh,&zh);
  Free(s2w);

  x1=xh+yh*32; y1=yh-xh*32; z1=0;
  (*dc->transform)(dc,&x1,&y1,&z1);
  x2=xh-yh*32; y2=yh+xh*32; z2=0;
  (*dc->transform)(dc,&x2,&y2,&z2);
  GrDCClipLine(dc,&x1,&y1,&x2,&y2);

  MemSet(p,0,sizeof(p));
  if (x2<x1) {
    SwapU64(&x1,&x2);
    SwapU64(&y1,&y2);
  }
  if (!x1 && x2==dc->width-1) {
    p[0].x=0;
    p[0].y=0;
    p[1].x=dc->width-1;
    p[1].y=0;
    p[2].x=dc->width-1;
    p[2].y=y2;
    p[3].x=0;
    p[3].y=y1;
    if (-�/2<=�1<�/2 ^^ -�/2<=Unwrap(�-�/2)<�/2)
      dc->color=LTCYAN;
    else
      dc->color=LTGREEN;
    GrFillPoly3(dc,4,p);
    p[0].y=dc->height-1;
    p[1].y=dc->height-1;
    if (-�/2<=�1<�/2 ^^ -�/2<=Unwrap(�-�/2)<�/2)
      dc->color=LTGREEN;
    else
      dc->color=LTCYAN;
    GrFillPoly3(dc,4,p);
  } else {
    if (y2<y1) {
      SwapU64(&x1,&x2);
      SwapU64(&y1,&y2);
    }
    if (!y1 && y2==dc->height-1) {
      p[0].x=0;
      p[0].y=0;
      p[1].x=0;
      p[1].y=dc->height-1;
      p[2].x=x2;
      p[2].y=dc->height-1;
      p[3].x=x1;
      p[3].y=0;
      if (x1<x2 ^^ -�/2<=�1<�/2 ^^ -�/2<=Unwrap(�-�/2)<�/2)
	dc->color=LTCYAN;
      else
	dc->color=LTGREEN;
      GrFillPoly3(dc,4,p);
      p[0].x=dc->width-1;
      p[1].x=dc->width-1;
      if (x1<x2 ^^ -�/2<=�1<�/2 ^^ -�/2<=Unwrap(�-�/2)<�/2)
	dc->color=LTGREEN;
      else
	dc->color=LTCYAN;
      GrFillPoly3(dc,4,p);
    }
  }
  dc->depth_buf=old_db;
}


U0 BSPEagleDive()
{
  GrDC *dc=GrDCAlias(main_dc,Fs);
  F64 last_t;
  I64 i,xx,yy,height,bkcolor,
      cx=Fs->win_pixel_width>>1,
      cy=Fs->win_pixel_height>>1;
  Fish *tempf;

  MemSet(panels_processed_bitmap,0,(num_panels+7)>>3);
  MemSet(mp.update_done_t,0,sizeof(F64)*MP_MAX_PROCESSORS);

  xx=x/(MAP_SCALE*COORDINATE_SCALE);
  yy=y/(MAP_SCALE*COORDINATE_SCALE);
  height=z/COORDINATE_SCALE-elevations[yy][xx];
  if (height<0) {
    if (elevations[yy][xx]>WATER_ELEVATION*MAP_SCALE) {
      dc->color=bkcolor=BLACK;
      if (!game_over) {
	GrRect(dc,0,0,dc->width,dc->height);
	music_mute=TRUE;
	Beep;
	game_over=TRUE;
	music_mute=FALSE;
      }
    } else
      bkcolor=BLUE;
  } else
    bkcolor=LTCYAN;

  if (height>=0 && !(-�/4<=Unwrap(�-�)<�/4)) {
    DrawHorizon(dc);
    GrDCDepthBufRst(dc);

  //Signal all cores to start update
    mp.do_update=1<<mp.mp_cnt-1-1;

    //Wait for all cores to start
    while (mp.do_update)
      Yield;

    MPDrawIt(Fs,dc);

    xx=x/COORDINATE_SCALE;
    yy=y/COORDINATE_SCALE;
    tempf=fish_root.next;
    while (tempf!=&fish_root) {
      if (SqrI64(tempf->x-xx)+SqrI64(tempf->y-yy)<MAP_SCALE*MAP_SCALE*20*20)
	GrSpritePlot3(dc,tempf->x,tempf->y,tempf->z,$IB,"<1>",1$);
      tempf=tempf->next;
    }

    //Wait for all cores to complete.
    LBts(&Fs->task_flags,TASKf_IDLE);
    for (i=1;i<mp.mp_cnt;i++)
      while (!mp.update_done_t[i])
	Yield;
    LBtr(&Fs->task_flags,TASKf_IDLE);

    last_t=mp.update_done_t[0];
    for (i=1;i<mp.mp_cnt;i++)
      last_t=Max(last_t,mp.update_done_t[i]);

      //Adjust size of updated grid.
    v_grid_size  *=0.90+0.10*(0.65/Fs->win_max_refresh)/last_t;
    v_grid_size=Limit(v_grid_size,20,200);
    //Regulate how much grid expands in the distance.
    if (mp.mp_cnt>1)
      span_factor*=0.90+0.10*mp.update_done_t[mp.mp_cnt-1]/mp.update_done_t[0];
    span_factor=Limit(span_factor,.30,3.0);

  } else {
    dc->color=bkcolor;
    GrRect(dc,0,0,dc->width,dc->height);
  }
  dc->pen_width=2;
  if (bkcolor==BLUE)
    dc->color=WHITE;
  else
    dc->color=BLACK;
  dc->flags&=~DCF_TRANSFORMATION;
  GrLine3(dc,cx+5,cy,0,cx-5,cy,0);
  GrLine3(dc,cx,cy+5,0,cx,cy-5,0);
  GrPrintF(dc,0,0,"�1:%5.1f �:%5.1f �2:%5.1f Box:%5.1f",
      �1*180/�,�*180/�,�2*180/�,v_grid_size);
  GrPrintF(dc,0,FONT_HEIGHT,"x:%5.1f y:%5.1f z:%5.1f height:%3d score:%d high:%d",
      ToF64(x)/COORDINATE_SCALE,ToF64(y)/COORDINATE_SCALE,ToF64(z)/COORDINATE_SCALE,
      height,game_score,best_score);

  if (game_over && Blink(,tP)) {
    dc->color=RED;
    GrPutS(dc,(Fs->win_pixel_width-9*FONT_WIDTH)/2,
	(Fs->win_pixel_height-FONT_HEIGHT)/2,"Game Over");
  } else if (tP-game_t0<5.0 && Blink(,tP)) {
    dc->color=WHITE;
    GrPutS(dc,(Fs->win_pixel_width-10*FONT_WIDTH)/2,
	(Fs->win_pixel_height-FONT_HEIGHT)/2,"Catch Fish");
  }
//We made an alias of this we don't want freed.
  dc->depth_buf=NULL;
  GrDCDel(dc);
  WinSync;
}

#define ANIMATE_MS	10

U0 AnimateTask(U64 dummy=0)
{
  nounusedwarn dummy;
  //This just steadily moves the airplane fwd.
  I64 *s2w,x1,y1,z1;
  F64 t0=tP(Fs->parent_task),mS;
  Fish *tempf,*tempf1;
  while (TRUE) {
    mS=1000*(tP(Fs->parent_task)-t0);
    t0=tP(Fs->parent_task);
    if (!game_over) {
      //Screen to world coordinates
      s2w=GrIdent;
      GrRotZEqu(s2w,-�1);
      GrRotXEqu(s2w,-�);
      GrRotZEqu(s2w,-�2);
      x1=0;y1=0;z1=-speed*mS*COORDINATE_SCALE;
      GrRot(s2w,&x1,&y1,&z1);
      x+=x1;
      y+=y1;
      z+=z1;
      Free(s2w);
      x1=x/COORDINATE_SCALE; y1=y/COORDINATE_SCALE; z1=z/COORDINATE_SCALE;
      if (z1<WATER_ELEVATION*MAP_SCALE) {
	speed=0.5;
	tempf=fish_root.next;
	while (tempf!=&fish_root) {
	  tempf1=tempf->next;
	  if (SqrI64(tempf->x-x1)+SqrI64(tempf->y-y1)<MAP_SCALE*MAP_SCALE*5*5) {
	    game_score++;
	    if (game_score>best_score)
	      best_score=game_score;
	    RemQue(tempf);
	    Free(tempf);
	    music_mute=TRUE;
	    Snd(1000); Sleep(200); Snd(0);
	    music_mute=FALSE;
	  }
	  tempf=tempf1;
	}
      } else {
	if (-�/4<=�<=�/4)
	  speed+=0.0005;
	else if (-3*�/4<=�<=3*�/4)
	  speed+=0.0001;
	else
	  speed-=0.0001;
	speed=Limit(speed+(0.0005-0.0002*Abs(�)/(�/4)),0.1,5.0);
      }
    }
    WinSync;
  }
}

U0 MPEnd()
{
  mp.done_flags=0;
  mp.app_done=TRUE;
  //Wait for all cores to exit
  while (mp.done_flags!=1<<mp.mp_cnt-1-1)
    Yield;
}

U0 TaskEndCB()
{
  MPEnd;
  Exit;
}

U0 SongTask(U64 dummy=0)
{ //Song by Terry A. Davis
  nounusedwarn dummy;
#ifstatic
  Fs->task_end_cb=ExtFind("SndTaskEndCB");//(Can't take addr of an extern function.)
#else
  Fs->task_end_cb=&SndTaskEndCB;
#endif
  MusicSettingsRst;
  music_stacatto_factor=0.2;
  while (TRUE) {
	Play("W13eBDEDBDEDFEEDFEED");
	Play("BDEDBDEDFEEDFEED");
  }
}

U0 Init(I64 _mp_cnt)
{
  I64 i;
  main_dc=GrDCNew(DCT_COLOR8,GR_WIDTH,GR_HEIGHT);
  critical_section_flag=0;
  game_over=FALSE;
  game_score=0;
  MemSet(&mp,0,sizeof(MPCtrl));
  if (_mp_cnt<=mp_cnt)
    mp.mp_cnt=_mp_cnt;
  else
    mp.mp_cnt=mp_cnt;
  fish_root.next=fish_root.last=&fish_root;
  InitMap;
  GrDCAllocDepthBuf(main_dc);

  Fs->win_max_refresh=30.0;

  v_grid_size=64.0;
  span_factor=1.0;
  �	=-90.0*�/180.0;
  �1=  0.0*�/180.0;
  �2=  0.0*�/180.0;
  speed =  2.5;

  x=MAP_WIDTH>>1 *COORDINATE_SCALE*MAP_SCALE;
  y=MAP_HEIGHT>>1*COORDINATE_SCALE*MAP_SCALE;
  z=64		 *COORDINATE_SCALE*MAP_SCALE;

  for (i=1;i<mp.mp_cnt;i++)
    MPSpawn(&APEagleDiveTask,Fs,"AP EagleDive",1<<i);
  Fs->task_end_cb=&TaskEndCB;
  game_t0=tP;
}

U0 CleanUp()
{
  Fish *tempf=fish_root.next,*tempf1;
  Panel *tempp=panel_root,*tempp1;

  MPEnd;

  while (tempf!=&fish_root) {
    tempf1=tempf->next;
    RemQue(tempf);
    Free(tempf);
    tempf=tempf1;
  }
  while (tempp) {
    tempp1=tempp->next;
    Free(tempp->points);
    Free(tempp);
    tempp=tempp1;
  }
  Free(panels_processed_bitmap);
  GrDCDel(main_dc);
}

U0 EagleDive(I64 _mp_cnt=MP_MAX_PROCESSORS)
{
  I64 ch,sc;
  F64 pp,tt1,tt2;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  //We really can't afford to run at 60Hz.	This is how often VGA
  //memory is updated, not related to actual hardware refresh rates.

  WordStat;
  MenuPush(
  "File {"
  "  Abort(,CH_SHIFT_ESC);"
  "  Exit(,CH_ESC);"
  "}"
  "Play {"
  "  Restart(,CH_NEW_LINE);"
  "  Down(,,SC_CURSOR_UP);"
  "  Up(,,SC_CURSOR_DOWN);"
  "  Left(,,SC_CURSOR_LEFT);"
  "  Right(,,SC_CURSOR_RIGHT);"
      "}"
      );
  WinMax;
  WinBorder;
  "Initializing...\n";
  Fs->song_task=Spawn(&SongTask,NULL,"Song",Fs);
  Init(_mp_cnt);
  Fs->animate_task=Spawn(&AnimateTask,NULL,"Animate",Fs);
  LtfCursor;
  LtfClear;
  Fs->draw_it=&DrawIt;
  try //in case <CTRL-ALT-C> is pressed.
    do {
      if (ScanKey(&ch,&sc)) {
	pp=�; tt1=�1; tt2=�2;
	switch (ch) {
	  case 0:
	    switch (sc.u8[0]) {
	      case SC_CURSOR_DOWN:
		�	+=-CTRLS_SCALE*Cos(tt1);
		�2+=-CTRLS_SCALE*Sin(tt1)*Sin(�);
		break;
	      case SC_CURSOR_UP:
		�	-=-CTRLS_SCALE*Cos(tt1);
		�2-=-CTRLS_SCALE*Sin(tt1)*Sin(�);
		break;
	      case SC_CURSOR_RIGHT:
		�1+=CTRLS_SCALE;
		break;
	      case SC_CURSOR_LEFT:
		�1-=CTRLS_SCALE;
		break;
	    }
	    break;
	  case CH_NEW_LINE:
	    Fs->draw_it=NULL;
	    CleanUp;
	    WinSync;
	    Init(_mp_cnt);
	    Fs->draw_it=&DrawIt;
	    break;
	  }
	�1=Unwrap(�1);
	�	=Unwrap(�);
	�2=Unwrap(�2);
      } else
	BSPEagleDive;
    } while (ch!=CH_SHIFT_ESC && ch!=CH_ESC);
  catch
    CatchAll;
  SettingsPop;
  CleanUp;
#ifnstatic
  AcctRegWriteBranch("LT/EagleDive","I64 best_score=%d;\n",best_score);
#endif
  MenuPop;
}

EagleDive;
           .           �               �      �   �   �   �   �   �      �      �   �   �   �   �      x      x   �   h   �   h      X      X   �   H   �   H      8      8   �   (   �   (               �         �      �   "      "      2   �   2   �   B      B      R   �   R   �   b      b      r   �   r   �   �      �      �   �   �   !r   8   P   "   !t   2   R      !v   ,   T      !N   *   p   @   !L   .   l   D   !L   4   h   H   !T      x   &   !X      z       !\      |         !Z   D   t                  L             ����                  %       ����                #    