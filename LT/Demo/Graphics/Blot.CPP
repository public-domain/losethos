GrDC *b=GrDCAlias;
GrDC *img1=GrDCNew(DCT_COLOR8,64,64);
GrDC *img2=GrDCNew(DCT_COLOR8,512,512);

img1->color=COLOR_MONO;
img1->pen_width=3;
GrCircle(img1,30,30,20);
GrFloodFill3(img1,30,30,0,TRUE);
GrLine3(img1,60,0,0,0,60,0);

GrDCFill(img2);
img2->color=ROP_EQU+RED;
img2->brush=img1;
GrCircle(img2,256-30,256-30,128,67);
img2->color=BLUE;
img2->pen_width=6;
img2->brush=NULL;
GrLine3(img2,128,128,0,256+128,256+128,0);
GrLine3(img2,256+128,128,0,128,256+128,0);

img2->color=YELLOW;
GrRect(img2,256-20,256-20,40,40);

I64 i;
for (i=0;i<256;i+=4) {
  WinSync;
  GrDCFill;
  GrDCBlot(b,i-256,i-256,img2);
  Sleep(10);
}
U8 *tempg=GrDC2Sprite(img1);
PutSprite(tempg);
Free(tempg);
NewLine(9);

GrDCDel(img1);
GrDCDel(img2);
GrDCDel(b);

PressAKey;
GrDCFill;
