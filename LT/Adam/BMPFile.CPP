#help_index "Graphics/Windows BMP Files"

//This provides support for Window's BMP files.

class BMPFile
{
  U16 type;
  U32 file_size;
  U32 reserved;
  U32 data_offset;
 
  U32 header_size;
  U32 width;
  U32 height;
  U16 planes;
  U16 bit_cnt;
  U32 compression;
  U32 image_size;
  U32 x_pixels_per_meter;
  U32 y_pixels_per_meter;
  U32 colors_used;
  U32 important_colors;

  U0 end;

  BGREntry palette[16];
};


BMPFile *BMPFileNew(U64 width,U64 height,U64 bits)
{
  BMPFile *r;
  U64 size;
  if (bits==24)
    size=offset(BMPFile.end)+
	 width*height*sizeof(RGBEntry);
  else if (bits==4)
    size=offset(BMPFile.end)+
	 width*height>>1+16*sizeof(BGREntry);
  else {
    "Format Not Supported.\n";
    return NULL;
  }
  r=CAlloc(size);
  r->type=0x4D42;
  r->planes=1;
  r->file_size=size;
  r->data_offset=offset(BMPFile.end);
  r->header_size=offset(BMPFile.end)-
		 offset(BMPFile.header_size);
  r->width=width;
  r->height=height;
  if (bits==24) {
    r->bit_cnt=32;
    r->image_size=width*height<<2;
  } else {
    r->bit_cnt=4;
    r->image_size=width*height>>1;
    r->data_offset+=16*sizeof(BGREntry);
    GetVGAPalette4(r->palette);
  }
  return r;
}


public BMPFile *BMPFile4To(GrDC *dc)
{
//To Windows 4 bit BMP
  U8 *src;
  BMPFile *r=BMPFileNew(dc->width,dc->height,4);
  I64 x,y,w=dc->width>>1;
  U8 *bgr;
  bgr=r(U8 *)+r->data_offset;
  for (y=dc->height-1;y>=0;y--) {
    src=y*dc->width_internal+dc->body;
    for (x=0;x<w;x++) {
      *bgr|=(*src++&15)<<4;
      *bgr|=*src++&15;
      bgr++;
    }
  }
  return r;
}

public BMPFile *BMPFile24To(GrDC *dc)
{
//To Windows 24 bit BMP
  U8 *src;
  BMPFile *r=BMPFileNew(dc->width,dc->height,24);
  I64 i,x,y;
  BGREntry *bgr;

  bgr=r(U8 *)+r->data_offset;
  for (y=dc->height-1;y>=0;y--) {
    src=y*dc->width_internal+dc->body;
    for (x=0;x<dc->width;x++) {
      i=*src++;
      if (i&BLUE)  bgr->b=0x7F;
      if (i&GREEN) bgr->g=0x7F;
      if (i&RED)   bgr->r=0x7F;
      if (i&8) {
	if (bgr->b) bgr->b=0xFF;
	if (bgr->g) bgr->g=0xFF;
	if (bgr->r) bgr->r=0xFF;
      }
      bgr(U8 *)+=4;
    }
  }
  return r;
}

public U64 BMPFileWrite(U8 *filename,GrDC *dc,U64 bits=4)
{ //Window's BMP Files
  U64 size;
  BMPFile *r;
  if (bits==4)
    r=BMPFile4To(dc);
  else if (bits==24)
    r=BMPFile24To(dc);
  else {
    "Format Not Supported.\n";
    return 0;
  }
  size=r->file_size;
  FileWrite(filename,r,r->file_size);
  Free(r);
  return size;
}

U8 *BMPFilePaletteNew(BMPFile *r)
{
  I64 i,j,best,score,best_score;
  U8 *result=CAlloc(16*sizeof(U8));
  BGREntry palette[16];
  GetVGAPalette4(palette);
  for (i=0;i<16;i++) {
    best=i;
    best_score=MAX_I64;
    for (j=0;j<16;j++) {
      score=SqrI64(r->palette[i].r-palette[j].r)+
	  SqrI64(r->palette[i].g-palette[j].g)+
	  SqrI64(r->palette[i].b-palette[j].b);
      if (score<best_score) {
	best=j;
	best_score=score;
      }
    }
    result[i]=best;
  }
  return result;
}

U8 ms_paint_palette[16]={0,4,2,6,1,5,3,8,7,12,10,14,9,13,11,15};

public GrDC *BMPFileRead(U8 *filename,BoolI8 use_ms_paint_palette=FALSE)
{ //Window's BMP Files
  I64 i,j;
  U8 *palette_map,*ptr1;
  BMPFile *r;
  GrDC *result=NULL;
  if (ptr1=FileRead(filename)) {
    r=ptr1;
    if (0<r->width<MAX_I32 && 0<r->height<MAX_I32) {
      result=GrDCNew(DCT_COLOR8,r->width,r->height);
      ptr1+=r->data_offset;
      if (r->bit_cnt==4) {
	if (use_ms_paint_palette)
	  palette_map=ms_paint_palette;
	else
	  palette_map=BMPFilePaletteNew(r);
	for (i=r->height-1;i>=0;i--)
	  for (j=0;j<(r->width+7)&-8;) {
	    result->color=palette_map[*ptr1&15];
	    GrPlot(result,j+1,i);
	    result->color=palette_map[*ptr1>>4];
	    GrPlot(result,j,i);
	    ptr1++;
	    j+=2;
	  }
	if (!use_ms_paint_palette)
	  Free(palette_map);
      } else {
	"Format Not Supported.\n";
	GrDCDel(result);
	result=NULL;
      }
    } else
      "Invalid BMP File\n";
    Free(r);
  }
  return result;
}

#help_index "Graphics/Windows BMP Files;Graphics/Screen"

public U64 BMPFileCaptureScreen(U8 *filename,U64 bits=4)
{
  GrDC *dc=GrDCCaptureScreen;
  U64 size=BMPFileWrite(filename,dc,bits);
  GrDCDel(dc);
  return size;
}

#help_index ""
