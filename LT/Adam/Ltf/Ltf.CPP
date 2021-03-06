#help_index "Ltf"
#help_file "::/LT/Doc/Ltf"

//See $LK,"::/LT/Doc/Ltf.TXZ","FI:::/LT/Doc/Ltf.TXZ"$

extern I64 EdLeftClickLink(Ltf *l,LtfEntry *cl);
extern I64 TermRightClickLink(Ltf *l,LtfEntry *cl);
extern BoolI64 Ed(U8 *filename);
extern U0 LtfPutSPartial(Ltf *l,U8 *st);
extern U0 LtfFormBackward(Ltf *l,BoolI8 giveup=FALSE);
extern U0 EdUndoFilter(Ltf *l);

DefineListLoad("ST_LTF_CMDS","TX\0CR\0SR\0CU\0TB\0CL\0PB\0DA\0PL\0LM\0RM\0HD\0FO\0ID\0"
	"FG\0BG\0FD\0BD\0F2\0FL\0FM\0FA\0FH\0FE\0FU\0FB\0FY\0FR\0FC\0FQ\0"
	"WW\0UL\0IV\0BK\0SX\0SY\0CM\0AN\0LK\0BT\0"
	"CB\0MA\0MU\0HX\0HS\0HE\0TR\0PI\0IB\0IT\0IS\0BP\0SO\0HL\0LP\0CC\0HI\0HV\0HC\0ER\0");
DefineListLoad("ST_LTF_FLAGS","A\0L\0"
	"LC\0LE\0LM\0LA\0RC\0RE\0RM\0RA\0"
	"TC\0BI\0X\0Q\0LX\0CX\0RX\0TY\0CY\0BY\0SX\0SY\0SCX\0SCY\0"
	"HL\0LP\0CC\0WW\0BK\0IV\0SEL\0UL\0"
	"B\0S\0BD\0C\0CA\0RD\0P\0T\0"
	"Z\0H\0TR\0SK\0AL\0PU\0PRY\0MRX\0"
	"FST\0SIF\0WD\0HT\0HS\0FS\0NC\0DD\0UD\0U8\0U16\0U32\0U64\0D\0WR\0HTML\0"
	);
DefineListLoad("ST_LINK_TYPES","FI\0FA\0FF\0FL\0MN\0PI\0PF\0PL\0BF\0DF\0HI\0AD\0");

#include "LtfNew"
#include "LtfHighlight"
#include "LtfRecalc"
#include "LtfPlain"
#include "LtfFile"
#include "LtfPopUp"
#include "LtfCtrlL"
#include "LtfMacro"
#include "LtfIDE"
#include "LtfEd"
#include "LtfCodeTools"
#include "LtfBible"
#include "LtfTree"
#include "LtfMain"
#help_index ""

$WW,1$/*$AN,"","Ltf"$
LoseThos Ltf's can have "cursor movement" cmds which can move the cursor up the screen and layer on existing text.It can also have call-back funs which supply live, changing text.For these reasons, you can't assume you know where the visible portion of the document is and must process an extensive portion of the document each time it is placed on the screen, becoming CPU intensive on big documents.The l->recalc_start member attempts to limit what is recalculated.This is tricky, howeve, and optimizations create minor glitches.Similarly, an optimization to only refresh the entire document 1 in 10 times it is drawn to the screen creates minor glitches.Since the Ltf handles the cmd line, it must also be fast when adding text output from programs.l->recalc_start comes into play here.

*/