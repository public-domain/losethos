I64 mp_n;

I64 MPSum(I64 my_mp_cnt)
{ //We could use the formula n*(n+1)/2
  I64 lo=mp_n*Gs->num/my_mp_cnt,
     hi=mp_n*(Gs->num+1)/my_mp_cnt,
     result=0,i;
  for (i=lo;i<hi;i++)
    result+=i;
  return result;
}

I64 Sum(I64 n,I64 my_mp_cnt)
{
  MPCmdStruct *tempm[MP_MAX_PROCESSORS];
  I64 result=0,i;
  mp_n=n+1;
  for (i=1;i<my_mp_cnt;i++)
    tempm[i]=MPJob(&MPSum,my_mp_cnt,0,1<<i);
  result=MPSum(my_mp_cnt);
  for (i=1;i<my_mp_cnt;i++)
    result+=MPJobResult(tempm[i]);
  return result;
}


#define SAMPLE_SIZE 100

F64 Test(I64 n,I64 my_mp_cnt)
{
  U64 i,val=0;
  F64 start,end;
  start=tNP;
  for (i=0;i<SAMPLE_SIZE;i++)
    val+=Sum(n,my_mp_cnt);
  end=tNP;
  "Val:%,d\n",val/SAMPLE_SIZE;
  "$$FG,RED$$N:%12,d Time:%10.8f$$FG$$\n",n,(end-start)/SAMPLE_SIZE;
  return end-start;
}

#define NUM_TESTS	7
#define MAX_PERCENT	200

U0 Main()
{
  I64 i,n,
    h=Fs->win_pixel_width,
    v=Fs->win_pixel_height;
  F64 t1,t2,results[NUM_TESTS];
  GrDC *dc=GrDCAlias;

  for (i=0,n=10;i<NUM_TESTS;i++,n*=10) {
    t1=Test(n,1);
    t2=Test(n,mp_cnt);
    results[i]=t2*100.0/t1;
    "$$FG,GREEN$$%8.4f%%$$FG$$\n\n",results[i];
  }
  PressAKey;

  LtfClear;

  dc->color=BLUE;
  for (i=0;i<MAX_PERCENT;i+=MAX_PERCENT/10) {
    GrPrintF(dc,0,v-ToF64(i)/MAX_PERCENT*v-FONT_HEIGHT,"%3d%%",i);
    GrLine(dc,0,v-ToF64(i)/MAX_PERCENT*v,h,v-ToF64(i)/MAX_PERCENT*v);
  }

  for (i=0;i<NUM_TESTS-1;i++) {
    dc->color=RED;
    dc->pen_width=2;
    GrLine3(dc,i*h/(NUM_TESTS-1),v-results[i	]/MAX_PERCENT*v,0,
		 (i+1)*h/(NUM_TESTS-1),v-results[i+1]/MAX_PERCENT*v,0);
    dc->color=GREEN;
    GrPrintF(dc,i*h/(NUM_TESTS-1),v-FONT_HEIGHT,"10e%d",i+1);
    GrLine(dc,i*h/(NUM_TESTS-1),0,
		i*h/(NUM_TESTS-1),v);
  }
  PressAKey;

  GrDCFill;
  GrDCDel(dc);
}

Main;
