#help_index "Graphics"
#define GRST_END		0
//Unused
#define GRST_COLOR8		2
//Unused
#define GRST_POINT		4
#define GRST_LINE		5
#define GRST_POLYLINE		6
#define GRST_POLYPOINT		7
#define GRST_WIDTH		8
#define GRST_SHIFT		9
#define GRST_BSPLINE2		10
#define GRST_BSPLINE3		11
#define GRST_BSPLINE2_CLOSED	12
#define GRST_BSPLINE3_CLOSED	13
#define GRST_SPEEDLINE		14
#define GRST_SPEEDLINE_POINT	15
//Unused
#define GRST_RECT		17
#define GRST_CIRCLE		18
#define GRST_FLOOD_FILL_NOT	19
#define GRST_FLOOD_FILL		20
#define GRST_BITMAP8		21
//Unused
#define GRST_TEXT		23
#define GRST_ELLIPSE		24
#define GRST_REG_POLY		25
#define GRST_PLANAR_SYMMETRY	26
#define GRST_DITHER_COLOR8	27
#define GRST_ROTATED_RECT	28
#define GRST_MESH		29
#define GRST_SHIFTABLE_MESH	30
#define GRST_TRANSFORM_ON	31
#define GRST_TRANSFORM_OFF	32
#define GRST_ARROW		33
#define GRST_TEXT_BOX		34
#define GRST_TEXT_DIAMOND	35
#define GRST_NUM_TYPES		36

DefineListLoad("ST_GRSPRITE_TYPES","End\0\0Color 8Bit\0\0Point\0"
	"Line\0PolyLine\0PolyPoint\0Width\0Shift\0"
	"BSpline2\0BSpline3\0BSpline2 Closed\0BSpline3 Closed\0SpeedLine\0"
	"\0\0Rect\0Circle\0Flood Fill Not Color\0Flood Fill\0BitMap 8Bit\0"
	"\0Text\0Ellipse\0Reg Polygon\0Planar Symmetry\0"
	"Dither Color 8Bit\0Rotated Rect\0Mesh\0Shiftable Mesh\0"
	"Transform On\0Transform Off\0Arrow\0Text Box\0Text Diamond\0");

class GrMeshTri
{
  I32 color; //Give one of 0-7 colors.	Colors 8-15 are 0-7 with intensity bit set.
  I32 nums[3]; //Vertex number
};

public class GrSprite
{
//See $LK,"Graphics/GrSprite","HI:Graphics/GrSprite"$
//See $LK,"::/LT/Adam/Gr/GrSpritePlot.APZ","FL:::/LT/Adam/Gr/GrSpritePlot.APZ,1"$ for how GrSprite are stored.
  GrSprite *next,*last;

  U0 start;
  U8 type;
  union {
    U8 color8;
    U16 dither_color8;
    I32 x1;
    I32 num;
    I32 width;
    I32 vertex_cnt; //Mesh
  }
  union {
    I32 y1;
    I32 height;
    I32 triangle_cnt; //Mesh
  }
  union {
    I32 z1;
    I32 x2;
    U8 st[4];
  }
  union {
    I32 y2;
    I32 shiftable_vertex_cnt //Shiftable Mesh
  }
  union {
    F64 linespeed;
    F64 rot_angle;
    I32 shiftable_triangle_cnt //Shiftable Mesh
  }
  I32 sides;
};


extern GrDC *GrDCNew(U64 type,U64 width,U64 height,TaskStruct *task=NULL,BoolI8 null_bitmap=FALSE);
extern GrDC *GrDCAlias(GrDC *dc=NULL,TaskStruct *task=NULL);
extern U0 GrDCDel(GrDC *dc);
extern U0 EdExistingGraphic(Ltf *l,I64 x,I64 y,I64 *import_snap=NULL,F64 *import_ip_scaler=NULL);
extern GrSprite *GrSpriteLtf2Queue(Ltf *l,GrSprite *root,I64 *cur_num);
extern BoolI64 GrDCBlot3(GrDC *dc,I64 x1,I64 y1,I64 z1,GrDC *img);
extern U8 *GrBitMap4ToBitMap8(U8 *dst,U8 *src,I64 src_size,I64 bkcolor);

#include "GrCfg"
#include "GrAsm"
#include "MathGr"
#include "GrScreen"
#include "GrDC"
#include "GrBitMap"
#include "GrPrimatives"
#include "GrComposites"
#include "GrChar"

#define GE_CANCEL	(-1)
#define GE_OKAY_DONE	0
#define GE_OKAY_CONT	1

#include "GrSpriteNew"
#include "GrSpritePlot"
#include "GrSpriteMesh"
#include "GrSpriteBitMap"
#include "GrSpriteVect"
#include "GrSpriteEd"
#include "GrVectMap"
#help_index ""
