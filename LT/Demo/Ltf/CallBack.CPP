U8 *BootTimeCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  LTDate i;
//This routine is called by the window
//manager things would get corrupted
//if the window manager used it's own
//heap, so we use the owning task's heap.
  U8 *st=MAlloc(64,mem_task);
  nounusedwarn l,ll;

  i=BootTime;

  //Doesn't have to be fixed width!!
  SPrintF(st,"%d=%h11T",i,i);
  return st;
}

U8 *CurTimeCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  LTDate i;
  U8 *st=MAlloc(64,mem_task);
  nounusedwarn l,ll;
  i=GetCurTimeLTDate;
  SPrintF(st,"%h0D %h0T",i,i);
  return st;
}

U0 DoIt()
{
/* The main window is kept at Fs->last_ltf.
A second framework is kept at Fs->next_ltf.
Normally, the next_ltf is used for updating
the border, primarily.

See $LK,"UseTermLtf","MN:UseTermLtf"$().
See $LK,"LtfUpdateTaskLtfs","MN:LtfUpdateTaskLtfs"$().

Note: the next_ltf is not to be confused
with the aux_str, a member of the $LK,"LtfEntry","MN:LtfEntry"$.

*/
  Ltf *aux=LtfAux, //Fetches Fs->next_ltf basically.
      *cur=LtfCur; //Fetches Fs->last_ltf basically.
  LtfEntry *ll;

// We don't want an update between
// creating the LtfEntry and defining the
// callback, so we turn-off preemption.

  BoolI8 old_preempt=Preempt(OFF);

  LtfBottom(aux);  //Insert at the bottom
  LtfPutS(aux,"$$FG,RED$$$$CM+BY+LX,5,-3$$");

// The $LK,"LtfPutS","MN:LtfPutS"$() routine returns the addr of the last entry.
  ll=LtfPutS(aux,"$$TX+TC,\" \"$$");
// The TC flag is "has tag callback".

// Flags are explained here:
//	$LK,"::/LT/Doc/CtrlL.TXZ","FI:::/LT/Doc/CtrlL.TXZ"$
//	$LK,"::/LT/Doc/Dollar.TXZ","FI:::/LT/Doc/Dollar.TXZ"$
//	$LK,"Dollar Flags","MN:LTFLf_AUX_STR"$
//	$LK,"ST_LTF_FLAGS","FF:::/LT/Adam/Ltf/Ltf.APZ,ST_LTF_FLAGS"$

  ll->tag_cb=&BootTimeCB;
  LtfPutS(aux,"$$FG$$");

//$BK,1$WARNING:$BK,0$ If you use the last_ltf you
//run the risk of the user pressing
//<CTRL-T> or using the clipboard, both
//of which will crash.	So, you might want
//to use the next_ltf.

  LtfPutS(cur,"$$FG,LTRED$$");
  ll=LtfPutS(cur,"$$TX+TC,\" \"$$");
  ll->tag_cb=&CurTimeCB;
  LtfPutS(cur,"$$FG$$");

  Preempt(old_preempt);

//Send carriage return, new line, so
//that the timer string is not part
//of the next cmd on the cmd line.
  NewLine;
}



U0 UndoIt()
{
//Clear-out entries without a +H hold flag.
  LtfBottom(Fs->next_ltf);
  LtfPutS(Fs->next_ltf,"$$CL$$");
}


DoIt;

// See $LK,"::/LT/Demo/Ltf/CallBack2.CPZ","FI:::/LT/Demo/Ltf/CallBack2.CPZ"$ for another way.
