//x must be global.
//Might as well make y global.

F64 x,y,y_last;

U0 Cartesian()
{
  U8 *st;
  U64 ch=0,h,v;
  BoolI8 first;
  GrDC *dc=GrDCAlias;
  do {
    LtfClear;
    "Enter algebraic equation to graph.\n"
    "Example: y=2*x\n"
    "y=";
    if (st=MGetS) {
      if (*st) {
	h=Fs->win_pixel_width/2;
	v=Fs->win_pixel_height/2;
	dc->color=RED;
	GrLine(dc,h,0,h,2*v+1);
	GrLine(dc,0,v,2*h+1,v);
	first=TRUE;
	dc->color=BLACK;
	for (x=-h;x<=h;x++) {
	  y=-ExePrintF("ToF64(%s);",st)(F64);
	  if (!first)
	    GrLine(dc,x-1+h,y_last+v,x+h,y+v);
	  y_last=y;
	  first=FALSE;
	}
	ch=GetChar;
      } else
	ch=CH_SHIFT_ESC;
      Free(st);
    } else
       ch=CH_SHIFT_ESC;
    GrDCFill;
  } while (ch!=CH_SHIFT_ESC && ch!=CH_ESC);
  GrDCDel(dc);
}

Cartesian;

/*$WW,1$
For better performance, compile the expression entered by the user one time with $LK,"CmpExpression","MN:CmpExpression"$() and use $LK,"Call","MN:Call"$(). See $LK,"::/LT/Demo/OldSchool/CompileDemo.CPZ","FI:::/LT/Demo/OldSchool/CompileDemo.CPZ"$.
*/
