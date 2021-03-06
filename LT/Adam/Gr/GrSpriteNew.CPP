#help_index "Graphics/GrSprite"
#help_file "::/LT/Doc/GrSprite"

I64 gr_sprite_base_size[GRST_NUM_TYPES]=
{
offset(GrSprite.x1)-offset(GrSprite.start), //GRST_END
0,
1+offset(GrSprite.color8)-offset(GrSprite.start), //GRST_COLOR8
0,
offset(GrSprite.x2)-offset(GrSprite.start), //GRST_POINT
offset(GrSprite.linespeed)-offset(GrSprite.start), //GRST_LINE
offset(GrSprite.y1)-offset(GrSprite.start), //GRST_POLYLINE
offset(GrSprite.y1)-offset(GrSprite.start), //GRST_POLYPOINT
offset(GrSprite.y1)-offset(GrSprite.start), //GRST_WIDTH
offset(GrSprite.x2)-offset(GrSprite.start), //GRST_SHIFT
offset(GrSprite.y1)-offset(GrSprite.start), //GRST_BSPLINE2
offset(GrSprite.y1)-offset(GrSprite.start), //GRST_BSPLINE3
offset(GrSprite.y1)-offset(GrSprite.start), //GRST_BSPLINE2_CLOSED
offset(GrSprite.y1)-offset(GrSprite.start), //GRST_BSPLINE3_CLOSED
offset(GrSprite.y1)-offset(GrSprite.start), //GRST_SPEEDLINE
offset(GrSprite.sides)-offset(GrSprite.start), //GRST_SPEEDLINE_POINT
0,
offset(GrSprite.linespeed)-offset(GrSprite.start), //GRST_RECT
offset(GrSprite.y2)-offset(GrSprite.start), //GRST_CIRCLE
offset(GrSprite.x2)-offset(GrSprite.start), //GRST_FLOOD_FILL_NOT
offset(GrSprite.x2)-offset(GrSprite.start), //GRST_FLOOD_FILL
offset(GrSprite.linespeed)-offset(GrSprite.start), //GRST_BITMAP8
0,
offset(GrSprite.st)-offset(GrSprite.start), //GRST_TEXT
offset(GrSprite.sides)-offset(GrSprite.start), //GRST_ELLIPSE
sizeof(GrSprite)-offset(GrSprite.start), //GRST_REG_POLY
offset(GrSprite.linespeed)-offset(GrSprite.start), //GRST_PLANAR_SYMMETRY
2+offset(GrSprite.dither_color8)-offset(GrSprite.start), //GRST_DITHER_COLOR8
offset(GrSprite.sides)-offset(GrSprite.start), //GRST_ROTATED_RECT
offset(GrSprite.x2)-offset(GrSprite.start), //GRST_MESH
offset(GrSprite.shiftable_triangle_cnt)+sizeof(I32)-offset(GrSprite.start), //GRST_SHIFTABLE_MESH
offset(GrSprite.x1)-offset(GrSprite.start), //GRST_TRANSFORM_ON
offset(GrSprite.x1)-offset(GrSprite.start), //GRST_TRANSFORM_OFF
offset(GrSprite.linespeed)-offset(GrSprite.start), //GRST_ARROW
offset(GrSprite.st)-offset(GrSprite.start), //GRST_TEXT_BOX
offset(GrSprite.st)-offset(GrSprite.start), //GRST_TEXT_DIAMOND
};

I64 gr_sprite_queued_base_size[GRST_NUM_TYPES]=
{
offset(GrSprite.x1),//GRST_END
0,
1+offset(GrSprite.color8), //GRST_COLOR8
0,
offset(GrSprite.x2), //GRST_POINT
offset(GrSprite.linespeed), //GRST_LINE
offset(GrSprite.y1), //GRST_POLYLINE
offset(GrSprite.y1), //GRST_POLYPOINT
offset(GrSprite.y1), //GRST_WIDTH
offset(GrSprite.x2), //GRST_SHIFT
offset(GrSprite.y1), //GRST_BSPLINE2
offset(GrSprite.y1), //GRST_BSPLINE3
offset(GrSprite.y1), //GRST_BSPLINE2_CLOSED
offset(GrSprite.y1), //GRST_BSPLINE3_CLOSED
offset(GrSprite.y1), //GRST_SPEEDLINE
offset(GrSprite.sides), //GRST_SPEEDLINE_POINT
0,
offset(GrSprite.linespeed), //GRST_RECT
offset(GrSprite.y2), //GRST_CIRCLE
offset(GrSprite.x2), //GRST_FLOOD_FILL_NOT
offset(GrSprite.x2), //GRST_FLOOD_FILL
offset(GrSprite.linespeed), //GRST_BITMAP8
0,
offset(GrSprite.st), //GRST_TEXT
offset(GrSprite.sides), //GRST_ELLIPSE
sizeof(GrSprite), //GRST_REG_POLY
offset(GrSprite.linespeed), //GRST_PLANAR_SYMMETRY
2+offset(GrSprite.dither_color8), //GRST_DITHER_COLOR8
offset(GrSprite.sides), //GRST_ROTATED_RECT
offset(GrSprite.x2), //GRST_MESH
offset(GrSprite.shiftable_triangle_cnt)+sizeof(I32), //GRST_SHIFTABLE_MESH
offset(GrSprite.x1), //GRST_TRANSFORM_ON
offset(GrSprite.x1), //GRST_TRANSFORM_OFF
offset(GrSprite.linespeed), //GRST_ARROW
offset(GrSprite.st), //GRST_TEXT_BOX
offset(GrSprite.st), //GRST_TEXT_DIAMOND
};


U64 GrSpriteElemSize(GrSprite *tempg)
{
  U64 i=gr_sprite_base_size[tempg->type];
  switch (tempg->type) {
    case GRST_POLYLINE:
      i+=tempg->num<<1*sizeof(I32);
      break;
    case GRST_TEXT:
    case GRST_TEXT_BOX:
    case GRST_TEXT_DIAMOND:
      i+=StrLen(tempg->st)+1;
      break;
    case GRST_SPEEDLINE:
      i+=tempg->num*(2*sizeof(I32)+sizeof(F64));
      break;
    case GRST_BITMAP8:
      i+=((tempg->width+7)&~7)*tempg->height;
      break;
    case GRST_POLYPOINT:
      i+=sizeof(I32)<<1+(tempg->num*3+7)>>3;
      break;
    case GRST_BSPLINE2:
    case GRST_BSPLINE3:
    case GRST_BSPLINE2_CLOSED:
    case GRST_BSPLINE3_CLOSED:
      i+=tempg->num*sizeof(P3I32);
      break;
    case GRST_MESH:
      i+=tempg->vertex_cnt*sizeof(P3I32)+tempg->triangle_cnt*sizeof(GrMeshTri);
      break;
    case GRST_SHIFTABLE_MESH:
      i+=tempg->shiftable_vertex_cnt*sizeof(P3I32)+tempg->shiftable_triangle_cnt*sizeof(GrMeshTri);
      break;
  }
  return i;
}

public I64 GrSpriteSize(U8 *elems)
{
  GrSprite *tempg=elems-offset(GrSprite.start),*tempg1=tempg;
  while (tempg->type)
    tempg(U8 *)+=GrSpriteElemSize(tempg);
  return tempg(U8 *)-tempg1(U8 *)+gr_sprite_base_size[GRST_END];
}

I64 GrSpriteTypeMask(U8 *elems)
{
  I64 result=0;
  GrSprite *tempg=elems-offset(GrSprite.start);
  while (tempg->type) {
    Bts(&result,tempg->type);
    tempg(U8 *)+=GrSpriteElemSize(tempg);
  }
  return result;
}

public U8 *GrDC2Sprite(GrDC *tempb)
{
  GrSprite *tempg;
  GrDC *tempb1;
  BoolI8 temp_bitmap;
  if (tempb->type!=DCT_COLOR8) {
    tempb1=GrDCNew(DCT_COLOR8,tempb->width,tempb->height,Fs);
    tempb1->color=WHITE;
    GrDCBlot(tempb1,0,0,tempb);
    temp_bitmap=TRUE;
  } else {
    tempb1=tempb;
    temp_bitmap=FALSE;
  }
  tempg=CAlloc(gr_sprite_base_size[GRST_BITMAP8]+
	      tempb1->width_internal*tempb1->height+
	   gr_sprite_base_size[GRST_END])
	(U8 *)-offset(GrSprite.start);
  tempg->type=GRST_BITMAP8;
  tempg->width=tempb1->width;
  tempg->height=tempb1->height;
  tempg->x2=0;
  tempg->y2=0;
  MemCpy(tempg(U8 *)+offset(GrSprite.linespeed),tempb1->body,
    tempb1->width_internal*tempb1->height);
  if (temp_bitmap)
    GrDCDel(tempb1);
  return tempg(U8 *)+offset(GrSprite.start);
}

public U8 *GrSprite2Text(GrSprite *tempg)
{ //Study $LK,"::/LT/Demo/Graphics/GrSprite4.CPZ","FI:::/LT/Demo/Graphics/GrSprite4.CPZ"$.
  U8 buf[256],buf2[256],buf3[136];
  I32 *ptr;
  SPrintF(buf,"%Z",tempg->type,"ST_GRSPRITE_TYPES");
  *buf2=0;
  switch (tempg->type) {
    case GRST_COLOR8:
      SPrintF(buf2,"%s",Color2Text(buf3,tempg->color8));
      break;
    case GRST_POINT:
    case GRST_FLOOD_FILL:
    case GRST_FLOOD_FILL_NOT:
    case GRST_SHIFT:
      SPrintF(buf2,"%d,%d",tempg->x1,tempg->y1);
      break;
    case GRST_LINE:
    case GRST_ARROW:
    case GRST_PLANAR_SYMMETRY:
    case GRST_RECT:
    case GRST_ROTATED_RECT:
      SPrintF(buf2,"(%d,%d)-(%d,%d)",
	  tempg->x1,tempg->y1,tempg->x2,tempg->y2);
      break;
    case GRST_CIRCLE:
      SPrintF(buf2,"%d,%d:%dR",
	  tempg->x1,tempg->y1,tempg->x2);
      break;
    case GRST_WIDTH:
      SPrintF(buf2,"%d",tempg->width);
      break;
    case GRST_TEXT:
    case GRST_TEXT_BOX:
    case GRST_TEXT_DIAMOND:
      SPrintF(buf2,"%d,%d:%16tQ",tempg->x1,tempg->y1,tempg->st);
      break;
    case GRST_DITHER_COLOR8:
      SPrintF(buf2,"%s",Color2Text(buf3,ROPF_DITHER|tempg->dither_color8.u8[0]|tempg->dither_color8.u8[1]<<COLORROP_BITS));
      break;
    case GRST_POLYLINE:
    case GRST_SPEEDLINE:
    case GRST_POLYPOINT:
      ptr=&tempg->y1;
      SPrintF(buf2,"%d%d,%d",tempg->num,ptr[0],ptr[1]);
      break;
    case GRST_ELLIPSE:
    case GRST_REG_POLY:
    case GRST_BITMAP8:
      SPrintF(buf2,"%dW,%dH:%d,%d",
	  tempg->width,tempg->height,tempg->x2,tempg->y2);
      break;
    case GRST_BSPLINE2:
    case GRST_BSPLINE3:
    case GRST_BSPLINE2_CLOSED:
    case GRST_BSPLINE3_CLOSED:
      SPrintF(buf2,"%d",tempg->num);
      break;
    case GRST_MESH:
      SPrintF(buf2,"%dV,%dT",tempg->vertex_cnt,tempg->triangle_cnt);
      break;
    case GRST_SHIFTABLE_MESH:
      SPrintF(buf2,"%dV,%dT",tempg->shiftable_vertex_cnt,tempg->shiftable_triangle_cnt);
      break;
  }
  StrCat(buf,buf2);
  return StrNew(buf);
}
