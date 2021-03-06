#define MAX_TABLE_SIZE 0x10000

I64 output_found,passes,table_size;

U8 *gate_type_table =NULL,
   *displayed_design=NULL,
   *added_this_pass =NULL;

U16 *input1_table=NULL,
    *input2_table=NULL,
    *input3_table=NULL;

#define CONNECT_WIDTH	16
#define GATE_WIDTH	37

$ID,6$
$PI,"",1$



$PI,"",2$



$PI,"",3$



$PI,"",4$



$PI,"",5$



$PI,"",6$



$PI,"",7$



$PI,"",8$




$PI,"",9$



$PI,"",10$

$ID,-6$

U8 *gate_type_list="NULL\0OUTPUT\0INPUT\0"
     "NOT\0AND\0OR\0NAND\0NOR\0XOR\0AND3\0OR3\0NAND3\0NOR3\0";

#define GT_NULL   0  //Specifies that table entry has not been filled-in
#define GT_OUTPUT 1  //Specifies the table entry is a desired output
#define GT_INPUT  2  //Specifies that table entry comes from an input signal

#define GT_FIRST_REAL_GATE 3
#define GT_NOT		3
#define GT_AND		4
#define GT_OR		5
#define GT_NAND		6
#define GT_NOR		7
#define GT_XOR		8
#define GT_AND3		9
#define GT_OR3		10
#define GT_NAND3		11
#define GT_NOR3		12
#define GT_NUM_ENTRIES	13

#define MAX_SELECTED_GATES 128

GrSprite *imgs[GT_NUM_ENTRIES]={NULL,NULL,NULL,
$IB,"<NOT>",1$,$IB,"<AND>",2$,$IB,"<OR>",3$,$IB,"<NAND>",4$,$IB,"<NOR>",5$,$IB,"<XOR>",6$,$IB,"<AND3>",7$,$IB,"<OR3>",8$,$IB,"<NAND3>",9$,$IB,"<NOR3>",10$};

I64 num_inputs_entered,num_outputs_entered;
I64 num_selected_gates,
    selected_gates[MAX_SELECTED_GATES];

U0 PmtGates()
{
  I64 i;
  U8 *st;

  "\nEnter the available gate types in the order you prefer them to be used.\n"
  "Your choices are:\n";
  for (i=GT_FIRST_REAL_GATE;i<GT_NUM_ENTRIES;i++)
    "%z ",i,gate_type_list;
  NewLine;

  num_selected_gates=0;
  while (num_selected_gates<GT_NUM_ENTRIES) {
    "%d",num_selected_gates;
    st=PmtStr(" Gate: ");
    if (!*st) {
      Free(st);
      return;
    }
    i=ListEntryMatch(st,gate_type_list,LEF_IGNORE_CASE);
    Free(st);
    if (i<GT_FIRST_REAL_GATE)
      "Invalid response\n";
    else
      selected_gates[num_selected_gates++]=i;
  }
}


U0 Init()
{
  I64 i;

  do {
    table_size=PmtI64("\nTable size in hex (3 input=0x100,4=0x10000): ",0);
    if (table_size>MAX_TABLE_SIZE)
      "Too large\n";
    else if (table_size<1) {
      "No table specified, aborting.\n";
      ThrowBreak;
    }
  } while (table_size>MAX_TABLE_SIZE);

  gate_type_table =CAlloc(table_size*sizeof(U8));
  displayed_design=MAlloc((table_size+7)/8);
  added_this_pass =MAlloc((table_size+7)/8);
  input1_table	  =MAlloc(table_size*sizeof(U16));
  input2_table	  =MAlloc(table_size*sizeof(U16));
  input3_table	  =MAlloc(table_size*sizeof(U16));

  "\nEnter the hex truth table column values of inputs.\n";
  if (table_size<=0x100)
    "For example, enter A=0xF0, B=0xCC and C=0xAA.\n";
  else
    "For example, enter A=0xFF00, B=0xF0F0, C=0xCCCC and D=0xAAAA.\n";
  num_inputs_entered=0;
  while (TRUE) {
    "Input %C: ",'A'+num_inputs_entered;
    i=PmtI64("",-1);
    if (i<0) break;
    if (i>table_size)
      "Too large\n";
    else {
      if (gate_type_table[i])
	"Duplicate\n";
      else {
	gate_type_table[i]=GT_INPUT;
	input1_table[i]=num_inputs_entered++;
      }
    }
  }
  if (!num_inputs_entered) {
    "No inputs specified, aborting.\n";
    ThrowBreak;
  }

  "\nEnter the hex truth table columns values of the outputs.\n";
  num_outputs_entered=0;
  while (TRUE) {
    "Output %C: ",'A'+num_outputs_entered;
    i=PmtI64("",-1);
    if (i<0) break;
    if (i>table_size)
      "Too large\n";
    else {
      if (gate_type_table[i]==GT_INPUT)
	"To produce this output, connect to input %C\n",
	    'A'+input1_table[i];
      else if (gate_type_table[i]==GT_OUTPUT)
	"Duplicate\n";
      else {
	gate_type_table[i]=GT_OUTPUT;
	input1_table[i]=num_outputs_entered++;
      }
    }
  }

  if (!num_outputs_entered) {
    "No output specified, aborting.\n";
    ThrowBreak;
  }
}

U0 DrawDesign(GrDC *dc,I64 &y,U64 output,I64 depth,I64 *_x_out,I64 *_y_out)
{
  I64 type=gate_type_table[output],
      xx=(passes-depth)*(GATE_WIDTH+CONNECT_WIDTH),yy=y,
      x1,y1,x2,y2,x3,y3;
  if (_x_out) *_x_out=xx;
  if (_y_out) *_y_out=yy;
  if (Bt(displayed_design,output) && type!=GT_INPUT) {
    dc->color=GREEN;
    GrPutS(dc,xx-FONT_WIDTH*3,y-4,"Dup");
    y+=10;
  } else
    switch (type) {
      case GT_INPUT:
	dc->color=GREEN;
	GrPrintF(dc,xx-FONT_WIDTH-4,y-4,"%C",'A'+input1_table[output]);
	y+=10;
	break;
      case GT_NOT:
	if (!Bt(displayed_design,output)) {
	  y+=16;
	  DrawDesign(dc,y,input1_table[output],depth+1,&x1,&y1);
	  yy=y1;

	  dc->color=BLUE;
	  GrSpritePlot3(dc,xx,yy,0,
	      imgs[type]);

	  dc->color=RED;
	  GrLine(dc,xx-GATE_WIDTH,yy,x1,y1);
	  if (_y_out) *_y_out=yy;
	}
	break;
      case GT_AND:
      case GT_OR:
      case GT_NAND:
      case GT_NOR:
      case GT_XOR:
	if (!Bt(displayed_design,output)) {
	  y+=24;
	  DrawDesign(dc,y,input1_table[output],depth+1,&x1,&y1);
	  DrawDesign(dc,y,input2_table[output],depth+1,&x2,&y2);
	  yy=(y1+y2)/2;

	  dc->color=BLUE;
	  GrSpritePlot3(dc,xx,yy,0,imgs[type]);

	  dc->color=RED;
	  GrLine(dc,xx-GATE_WIDTH,yy-4,x1,y1);
	  GrLine(dc,xx-GATE_WIDTH,yy+4,x2,y2);
	  if (_y_out) *_y_out=yy;
	}
	break;
      case GT_AND3:
      case GT_OR3:
      case GT_NAND3:
      case GT_NOR3:
	if (!Bt(displayed_design,output)) {
	  y+=32;
	  DrawDesign(dc,y,input1_table[output],depth+1,&x1,&y1);
	  DrawDesign(dc,y,input2_table[output],depth+1,&x2,&y2);
	  DrawDesign(dc,y,input3_table[output],depth+1,&x3,&y3);
	  yy=(y1+y2+y3)/3;

	  dc->color=BLUE;
	  GrSpritePlot3(dc,xx,yy,0,imgs[type]);

	  dc->color=RED;
	  GrLine(dc,xx-GATE_WIDTH,yy-8,x1,y1);
	  GrLine(dc,xx-GATE_WIDTH,yy  ,x2,y2);
	  GrLine(dc,xx-GATE_WIDTH,yy+8,x3,y3);
	  if (_y_out) *_y_out=yy;
	}
	break;
    }
  dc->color=BLACK;
  GrPrintF(dc,xx,yy+3,"%04X",output);
  Bts(displayed_design,output);
}

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  I64 y=0;
  MemSet(displayed_design,0,(table_size+7)/8*sizeof(BoolI8));
  DrawDesign(dc,y,output_found,0,NULL,NULL);
}

U0 FillNot(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i)) {
      progress1=i;
      j= (~i) & (table_size-1);
      old_type=gate_type_table[j];
      if (old_type<GT_INPUT) {
	gate_type_table[j]=GT_NOT;
	input1_table[j]=i;
	Bts(added_this_pass,j);
	*chged=TRUE;
	if (old_type==GT_OUTPUT) {
	  if (output_found<0) output_found=j;
	  *num_outputs_found=*num_outputs_found+1;
	}
      }
    }
}

U0 FillAnd(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k)) {
	  progress1=i;
	  j= (i & k) & (table_size-1);
	  old_type=gate_type_table[j];
	  if (old_type<GT_INPUT) {
	    gate_type_table[j]=GT_AND;
	    input1_table[j]=i;
	    input2_table[j]=k;
	    Bts(added_this_pass,j);
	    *chged=TRUE;
	    if (old_type==GT_OUTPUT) {
	      if (output_found<0) output_found=j;
	      *num_outputs_found=*num_outputs_found+1;
	    }
	  }
	}
}

U0 FillOr(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k)) {
	  progress1=i;
	  j= (i | k) & (table_size-1);
	  old_type=gate_type_table[j];
	  if (old_type<GT_INPUT) {
	    gate_type_table[j]=GT_OR;
	    input1_table[j]=i;
	    input2_table[j]=k;
	    Bts(added_this_pass,j);
	    *chged=TRUE;
	    if (old_type==GT_OUTPUT) {
	      if (output_found<0) output_found=j;
	      *num_outputs_found=*num_outputs_found+1;
	    }
	  }
	}
}

U0 FillNAnd(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k)) {
	  progress1=i;
	  j= (~ (i & k)) & (table_size-1);
	  old_type=gate_type_table[j];
	  if (old_type<GT_INPUT) {
	    gate_type_table[j]=GT_NAND;
	    input1_table[j]=i;
	    input2_table[j]=k;
	    Bts(added_this_pass,j);
	    *chged=TRUE;
	    if (old_type==GT_OUTPUT) {
	      if (output_found<0) output_found=j;
	      *num_outputs_found=*num_outputs_found+1;
	    }
	  }
	}
}

U0 FillNOr(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k)) {
	  progress1=i;
	  j= (~ (i | k)) & (table_size-1);
	  old_type=gate_type_table[j];
	  if (old_type<GT_INPUT) {
	    gate_type_table[j]=GT_NOR;
	    input1_table[j]=i;
	    input2_table[j]=k;
	    Bts(added_this_pass,j);
	    *chged=TRUE;
	    if (old_type==GT_OUTPUT) {
	      if (output_found<0) output_found=j;
	      *num_outputs_found=*num_outputs_found+1;
	    }
	  }
	}
}

U0 FillXor(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k)) {
	  progress1=i;
	  j= (i ^ k) & (table_size-1);
	  old_type=gate_type_table[j];
	  if (old_type<GT_INPUT) {
	    gate_type_table[j]=GT_XOR;
	    input1_table[j]=i;
	    input2_table[j]=k;
	    Bts(added_this_pass,j);
	    *chged=TRUE;
	    if (old_type==GT_OUTPUT) {
	      if (output_found<0) output_found=j;
	      *num_outputs_found=*num_outputs_found+1;
	    }
	  }
	}
}

U0 FillAnd3(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k,l;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k))
	  for (l=0;l<table_size;l++)
	    if (gate_type_table[l]>GT_OUTPUT && !Bt(added_this_pass,l)) {
	      progress1=i;
	      j= (i & k & l) & (table_size-1);
	      old_type=gate_type_table[j];
	      if (old_type<GT_INPUT) {
		gate_type_table[j]=GT_AND3;
		input1_table[j]=i;
		input2_table[j]=k;
		input3_table[j]=l;
		Bts(added_this_pass,j);
		*chged=TRUE;
		if (old_type==GT_OUTPUT) {
		  if (output_found<0) output_found=j;
		  *num_outputs_found=*num_outputs_found+1;
		}
	      }
	    }
}

U0 FillOr3(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k,l;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k))
	  for (l=0;l<table_size;l++)
	    if (gate_type_table[l]>GT_OUTPUT && !Bt(added_this_pass,l)) {
	      progress1=i;
	      j= (i | k | l) & (table_size-1);
	      old_type=gate_type_table[j];
	      if (old_type<GT_INPUT) {
		gate_type_table[j]=GT_OR3;
		input1_table[j]=i;
		input2_table[j]=k;
		input3_table[j]=l;
		Bts(added_this_pass,j);
		*chged=TRUE;
		if (old_type==GT_OUTPUT) {
		  if (output_found<0) output_found=j;
		  *num_outputs_found=*num_outputs_found+1;
		}
	      }
	    }
}

U0 FillNAnd3(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k,l;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k))
	  for (l=0;l<table_size;l++)
	    if (gate_type_table[l]>GT_OUTPUT && !Bt(added_this_pass,l)) {
	      progress1=i;
	      j= (~(i & k & l)) & (table_size-1);
	      old_type=gate_type_table[j];
	      if (old_type<GT_INPUT) {
		gate_type_table[j]=GT_NAND3;
		input1_table[j]=i;
		input2_table[j]=k;
		input3_table[j]=l;
		Bts(added_this_pass,j);
		*chged=TRUE;
		if (old_type==GT_OUTPUT) {
		  if (output_found<0) output_found=j;
		  *num_outputs_found=*num_outputs_found+1;
		}
	      }
	    }
}

U0 FillNOr3(BoolI8 *chged,I64 *num_outputs_found)
{
  U64 i,j,k,l;
  U8 old_type;
  for (i=0;i<table_size;i++)
    if (gate_type_table[i]>GT_OUTPUT && !Bt(added_this_pass,i))
      for (k=0;k<table_size;k++)
	if (gate_type_table[k]>GT_OUTPUT && !Bt(added_this_pass,k))
	  for (l=0;l<table_size;l++)
	    if (gate_type_table[l]>GT_OUTPUT && !Bt(added_this_pass,l)) {
	      progress1=i;
	      j= (~(i | k | l)) & (table_size-1);
	      old_type=gate_type_table[j];
	      if (old_type<GT_INPUT) {
		gate_type_table[j]=GT_NOR3;
		input1_table[j]=i;
		input2_table[j]=k;
		input3_table[j]=l;
		Bts(added_this_pass,j);
		*chged=TRUE;
		if (old_type==GT_OUTPUT) {
		  if (output_found<0) output_found=j;
		  *num_outputs_found=*num_outputs_found+1;
		}
	      }
	    }
}


I64 FillGateTable()
{
  I64 current_gate,num_outputs_found=0;
  BoolI8 chged=TRUE;
  passes=1;
  output_found=-1;
  ProgressBarsRst;
  progress1_max=table_size;
  NewLine;
  while (num_outputs_found<num_outputs_entered && chged) {
    "Pass : %d\n",passes++;
    chged=FALSE;
    MemSet(added_this_pass,0,(table_size+7)/8);
    for (current_gate=0;current_gate<num_selected_gates &&
      num_outputs_found<num_outputs_entered;current_gate++) {
      switch (selected_gates[current_gate]) {
	case GT_NOT:   FillNot	(&chged,&num_outputs_found); break;
	case GT_AND:   FillAnd	(&chged,&num_outputs_found); break;
	case GT_OR:    FillOr	(&chged,&num_outputs_found); break;
	case GT_NAND:  FillNAnd (&chged,&num_outputs_found); break;
	case GT_NOR:   FillNOr	(&chged,&num_outputs_found); break;
	case GT_XOR:   FillXor	(&chged,&num_outputs_found); break;
	case GT_AND3:  FillAnd3 (&chged,&num_outputs_found); break;
	case GT_OR3:   FillOr3	(&chged,&num_outputs_found); break;
	case GT_NAND3: FillNAnd3(&chged,&num_outputs_found); break;
	case GT_NOR3:  FillNOr3 (&chged,&num_outputs_found); break;
      }
    }
    }
  ProgressBarsRst;
  return num_outputs_found;
}

U0 CleanUp()
{
  Free(gate_type_table);
  Free(displayed_design);
  Free(added_this_pass);
  Free(input1_table);
  Free(input2_table);
  Free(input3_table);
}

U0 DigitalLogic()
{
  gate_type_table =NULL;
  displayed_design=NULL;
  added_this_pass =NULL;
  input1_table	  =NULL;
  input2_table	  =NULL;
  input3_table	  =NULL;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  WinMax;
  WordStat;
  WinBorder(ON);
  LtfClear;
  PmtGates;
  try {
    Init;
    if (FillGateTable) {
      LtfCursor;
      LtfClear;
      Fs->draw_it=&DrawIt;
      GetChar;
      LtfClear;
      WinSync(TRUE);
      LtfBottom;
    }
  } catch
    CatchAll;
  SettingsPop;
  CleanUp;
}
           �     %      ����������������������               ��������   �������������� ��������������  ������   �������������� ��������������� ������   �������������� ���������������� �����                  ����������������� ����   �������������� ����������������� ����   �������������� ����������������� ����   �������������� ����������������� ����   �������������� �����������������        �������������� ����������������� ����   �������������� ����������������� ����   �������������� ����������������� ����                  ����������������� ����   �������������� ���������������� �����   �������������� ��������������� ������   �������������� ��������������  ������   ��������������               ��������              �     %      �������������������������� ������������������   ������������������  �����������������   ������������������ � ����������������   ������������������ �� ���������������   ������������������ ��� ��������������   ������������������ ���� ����    �����   ������������������ ����� �� ���� ����   ������������������ ������ � ���� ����                      �������  ����        ������������������ ������ � ���� ����   ������������������ ����� ��� �  �����   ������������������ ���� ����� �������   ������������������ ��� ��������������   ������������������ �� ���������������   ������������������ � ����������������   ������������������  �����������������   ������������������ ������������������              �     %      ����������������               ��������������   �������� ��������������  ������������   �������� ��������������� ������������   �������� ���������������� �����������            ����������������� ����������   �������� ����������������� �    �����   �������� �����������������  ���� ����   �������� �����������������  ���� ����   �������� �����������������  ����        �������� �����������������  ���� ����   �������� ����������������� � �  �����   �������� ����������������� �� �������            ����������������� ����������   �������� ���������������� �����������   �������� ��������������� ������������   �������� ��������������  ������������   ��������               ��������������              �     %      ��������������������              �����������   ������������ ������������  ����������   ������������� �������������  ��������   �������������� �������������  �������   �������������� ��������������  ������                  ���������������  �����   �������������� ���������������� �����   �������������� ����������������� ����   �������������� ����������������� ����   ��������������� ����������������        �������������� ����������������� ����   �������������� ���������������� �����   �������������� ���������������  �����                  ��������������  ������   �������������� �������������� �������   ������������� �������������  ��������   ������������ ������������  ����������   ������������              �����������              �     %      ��������������              �����������������   ������ ������������  ����������������   ������� �������������  ��������������   �������� �������������  �������������   �������� ��������������  ������������            ���������������  �����������   �������� ���������������� ��    �����   �������� �����������������  ���� ����   �������� �����������������  ���� ����   ��������� ����������������  ����        �������� �����������������  ���� ����   �������� ���������������� �� �  �����   �������� ���������������  ��� �������            ��������������  ������������   �������� �������������� �������������   ������� �������������  ��������������   ������ ������������  ����������������   ������              �����������������              �     %      ����������������� ��              �����������   ��������� �� ������������  ����������   ���������� �� �������������  ��������   ����������� �� �������������  �������   ����������� �� ��������������  ������                  ���������������  �����   ����������� �� ���������������� �����   ����������� �� ����������������� ����   ����������� �� ����������������� ����   ������������ �� ����������������        ����������� �� ����������������� ����   ����������� �� ���������������� �����   ����������� �� ���������������  �����                  ��������������  ������   ����������� �� �������������� �������   ���������� �� �������������  ��������   ��������� �� ������������  ����������   ��������� ��              �����������              �     &      ����������������                    ����������  �������� ������������������ ����������  �������� �������������������  ��������           ��������������������� �������  �������� ���������������������  ������  �������� ���������������������� ������  �������� ����������������������� �����  �������� ����������������������� �����  �������� ����������������������� �����  �������� ����������������������� �����  �������� ����������������������� �����           �����������������������     �  �������� ����������������������� �����  �������� ����������������������� �����  �������� ����������������������� �����  �������� ����������������������� �����  �������� ����������������������  �����  �������� ���������������������� ������  �������� ��������������������� �������           ��������������������  �������  �������� �������������������  ��������  �������� ������������������ ����������  ��������                    ����������             �     %      �������������                   �������������   �����  ���������������  �������������   ������ ����������������   �����������   ������� �����������������   ���������            ������������������  ��������   ��������  ������������������ ��������   ��������� ������������������  �������   ��������� �������������������  ������   ��������� ��������������������  �����   ��������� ��������������������� �����   ��������� ���������������������  ����   ��������� ���������������������� ����             ����������������������        ��������� ���������������������� ����   ��������� ���������������������  ����   ��������� ��������������������  �����   ��������� �������������������  ������   ��������� ������������������  �������   ��������� ������������������ ��������   ��������  �����������������  ��������            ����������������   ���������   ������ ����������������   �����������   �����  ���������������  �������������   �����                   �������������       	       �     %      ����������                    ���������������   �� ������������������ ���������������   �� �������������������  �������������      ��������������������� ������������   �� ���������������������  �����������   �� ���������������������� �����������   �� ����������������������� ����������   �� ����������������������� ����������   �� ����������������������� �    �����   �� �����������������������  ���� ����   �� �����������������������  ���� ����      �����������������������  ����        �� �����������������������  ���� ����   �� ����������������������� � �  �����   �� ����������������������� �� �������   �� ����������������������� ����������   �� ����������������������  ����������   �� ���������������������� �����������   �� ��������������������� ������������      ��������������������  ������������   �� �������������������  �������������   �� ������������������ ���������������   ��                    ���������������       
       �     %      ��������                  �������������������    ���������������  �������������������    ����������������   �����������������   � �����������������   ���������������      ������������������  ��������������   ��  ������������������ ��������������   ��� ������������������  �������������   ��� �������������������  ������������   ��� ��������������������  �����������   ��� ��������������������� ��    �����   ��� ���������������������   ���� ����   ��� ����������������������  ���� ����       ����������������������  ����        ��� ����������������������  ���� ����   ��� ���������������������  � �  �����   ��� ��������������������  ��� �������   ��� �������������������  ������������   ��� ������������������  �������������   ��� ������������������ ��������������   ��  �����������������  ��������������      ����������������   ���������������   � ���������������   �����������������    ���������������  �������������������                     �������������������    