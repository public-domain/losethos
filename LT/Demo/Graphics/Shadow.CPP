F64 �,�,�,s;
GrDC *dc0,*dc1;









		 $PI,"<1>",1$













U0 GrBlackLighting(GrDC *dc,P3I32 *p1,P3I32 *p2,P3I32 *p3,ColorROPU32 color)
{
  nounusedwarn p1,p2,p3,color;
  dc->color=0;
}

U0 DrawShadow(TaskStruct *task)
{
  I64 r[4][4],*r2;

  GrIdentEqu(dc1->r);
  GrRotZEqu(dc1->r,�);
  GrRotXEqu(dc1->r,�);
  GrRotZEqu(dc1->r,�);
  GrScaleMatEqu(dc1->r,s);
  GrSetTranslation(dc1->r,200,200,5000);
  dc1->flags|=DCF_TRANSFORMATION;
  GrSetRotMat(dc1,dc1->r);

  MemSet(r,0,sizeof(r));

  r[0][0]=gr_scale;
  r[1][1]=gr_scale/2; //Pick a plane for the shadow, to suit your taste.
  r[1][2]=gr_scale/2; //It doesn't have to have a norm of 1.0.
  r[2][2]=gr_scale;
  r[3][3]=gr_scale;

  GrSetTranslation(r,0,-2200,1000);
  r2=GrMulMat(r,dc1->r,task);
  Free(dc1->r);
  GrSetRotMat(dc1,r2);

  dc1->lighting=&GrBlackLighting;
  GrSpritePlot3(dc1,0,0,0,$IB,"<1>",1$);
}

U0 DrawIt(TaskStruct *task,GrDC *_dc)
{
  nounusedwarn _dc;
  GrDCDepthBufRst(dc0);
  GrIdentEqu(dc0->r);
  GrRotZEqu(dc0->r,�);
  GrRotXEqu(dc0->r,�);
  GrRotZEqu(dc0->r,�);
  GrScaleMatEqu(dc0->r,s);
  GrSetTranslation(dc0->r,200,200,5000);
  dc0->flags|=DCF_TRANSFORMATION;
  GrSetRotMat(dc0,dc0->r);
  if (mp_cnt>1)
    MPJob(&DrawShadow,task);
  else
    DrawShadow(task);
  GrSpritePlot3(dc0,0,0,0,$IB,"<1>",1$);
}

U0 Shadow()
{
  dc0=GrDCAlias(gr_dc,Fs);
  dc1=GrDCAlias(gr_dc,Fs);
  GrDCAllocDepthBuf(dc0);
  dc1->depth_buf=dc0->depth_buf;

  �=0;
  �=0;
  �=0;
  s=1.5;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  WinMax;
  Fs->draw_it=&DrawIt;
  Preempt(OFF);

  while (!ScanChar) {
    Sleep(20);
    �+=2*�/70;
    �  +=2*�/90;
    � +=2*�/110;
  }
  SettingsPop;
  GrDCDel(dc0);
  dc1->depth_buf=NULL;
  GrDCDel(dc1);
}

Shadow;
           B           d           K   ����       ����    ��������    ����        K       ����       ��������    ����������������d           K                    ����K       ����        K       ����       ��������    ��������   ����                                                                                                                                
        
      	        	   
        
                                                                                    