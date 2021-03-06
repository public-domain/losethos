/*

Graphics can be placed into src code
files.The actual data is stored in the
file after the text area.<CTRL-R> was used
to generate the graphic.A picture $$PI$$
entry was created by <CTRL-R>.Press <CTRL-T> to
toggle to plain text mode and see
how it is encoded.You can place any text
you want in the tag quotes.The text doesn't matter,
but the num does matter.The num identifies
the picture and can't be changed.

You make use of a picture in a program by entering
"insert binary", $$IB$$ cmds by hand after pressing
<CTRL-T> or using <CTRL-R> and
selecting "Pointer to GrSprite Data".
They behave like string constants when compiled.
That is, they are ptrs to data.

GrSprite1.CPZ uses the persistent graphic DC, gr_dc2,
while the other demo's use gr_dc which must
be redrawn each update of the screen (by the
window manager task).This demo is simpler
because you do not need a window update routine.

*/


$IV,0$$PI,"<1>",1$




U0 GrSprite1()
{
  GrDC *dc=GrDCAlias;
  I64 i;
  "$$CL$$";
  for (i=0;i<GR_WIDTH>>2;i++) {
    GrDCFill;
    GrSpritePlot3(dc,i,GR_HEIGHT>>1,0,$IB,"<1>",1$);
    Sleep(3);
  }
  "Image size:%d\n",$IS,"<1>",1$;
  "Image type:%d\n",$IT,"<1>",1$;
  GrDCDel(dc);
  PressAKey;
  GrDCFill;
}

GrSprite1;
           �                 Welcome To Losethos H      H   ����H      X   ����H      8   ����8   ����H   ����H   ����X   ����X   ����8   ����B      9   ����N   ����L   ���� 