/*
This uses LtfPutS/LtfPrintF because updates
to within a tree while a tree is collapsed get
messed-up if "live"(like using ordinary PrintF/PutS.)
The insert point is confused by hiding of
data in collapsed trees.  Therefore, the window main
Ltf is taken off-line by disabling preemption so there
are no screen updates until all data has been inserted.

There is a special Ltf function which is also
available to take an Ltf off-line.  See $LK,"LtfInsertSafe","MN:LtfInsertSafe"$.
For this example, however, I wanted to use LtfPrintF
and disabled Preemption myself.
*/

#include "/LT/Demo/Shared/TreeDemoJoin"

U0 TreeDemo()
{
  U64 fuf_flags=0;
  Ltf *l=LtfNew;
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),"+r");
  LtfPutS(l,"$$TR-C+AL,\"\"$$\n");
  LtfPutS(l,"$$ID,+2$$");
  TreeSub(l,FilesFind("/LT/*",fuf_flags));
  LtfPutS(l,"$$ID,-2$$");
  LtfInsertLtf(LtfCur,l);
  LtfDel(l);
}
 
TreeDemo;
