/*
If you are sure a fun won't be called
before import is resolved, you can use
"import".  Otherwise, use a fun pointer
var and check it before calling.
*/
import U0 MemberListDel(ClassStruct *tempc);
import I64 ExePutS(U8 *buf,U8 *filename=NULL,BoolI8 just_load=FALSE);
import I64 ExeFile(U8 *name,BoolI8 just_load=FALSE);
import SysAcctStruct *AcctFind(U8 *name);
import U0 DisplaySpecialKeys(I64 sc);
import BoolI64 LtfUnlock(Ltf *l);
import Ltf *EdJoin(U8 *filename,BoolI8 bail);

extern U0 throw(...);
extern U8 *FileNameAbs(U8 *name);
extern U8 *DftExt(U8 *name,U8 *extension);
extern BoolI64 Cd(U8 *name,BoolI8 make_dirs=FALSE);
extern BoolI64 MkDir(U8 *filename);
extern U0 Msg(U64 code,U64 p1,U64 p2,U64 flags=0);
extern BoolI64 BlkDevLock(LTBlkDev *bdev);
extern BoolI64 PrtLock(LTPrt *p);
extern BoolI64 BlkDevUnlock(LTBlkDev *bdev,BoolI8 rst=FALSE);
extern BoolI64 PrtUnlock(LTPrt *p,BoolI8 rst=FALSE);
extern BoolI64 RBlks(LTPrt *p,U8 *buf, U64 blk, U64 cnt);
extern BoolI64 WBlks(LTPrt *p,U8 *buf, U64 blk, U64 cnt,BoolI8 write_cache=FALSE);
extern U0 DskCacheInvalidate(LTPrt *p);
extern BoolI64 Drv(U8 drv_let);
extern U0 BlkDevsRelease();
extern U0 PrtsRelease();
extern BoolI64 IsRaw();
extern U0 D(U8 *buf,U64 cnt=0x80,BoolI8 show_offset=TRUE);
extern U0 Dm(U8 *buf,U64 cnt=0x80);
extern U0 MPInt(U64 num,U64 cpu_num=1);
extern U0 XTalk(TaskStruct *task,U8 *fmt,...);
extern BoolI64 TaskValidate(TaskStruct *task);

extern U8 *FileExtRem(U8 *src,U8 *dst=NULL);
extern BoolI64 IsDir(U8 *dir_name);
extern BoolI64 FileFind(U8 *filename,LTDirEntry *rde=NULL,
      BoolI8 dir_only=FALSE,BoolI8 file_only=FALSE,BoolI8 scan_parents=FALSE);
extern BoolI64 FilesFindMatch(U8 *files_find_mask,U8 *filename,U64 fuf_flags=0);

extern BoolI64 ISOPrtInit(LTPrt *p,U64 blk);
extern BoolI64 ISO3PrtInit(LTPrt *p,U64 blk);
extern BoolI64 ISO1FileFind(LTPrt *p,U64 cur_dir_cluster,U8 *name,LTDirEntry *result,
		     BoolI8 dir_only,BoolI8 file_only);
extern LTDirEntry *ISO1FilesFind(U8 *files_find_mask,U64 fuf_flags,LTDirEntry *parent=NULL);
extern BoolI64 ISO3FileFind(LTPrt *p,U64 cur_dir_cluster,U8 *name,LTDirEntry *result,
		     BoolI8 dir_only,BoolI8 file_only);
extern LTDirEntry *ISO3FilesFind(U8 *files_find_mask,U64 fuf_flags,LTDirEntry *parent=NULL);

extern U0 LTFSFreeFreeList(LTPrt *p);
extern U0 LTFSPrtInit(LTPrt *p);
extern U0 FAT32PrtInit(LTPrt *p);
extern BoolI64 LTFSDrvValidate(U8 drv_let);
extern U0 LTFSFmt(U8 drv_let,BoolI8 quick=TRUE);
extern BoolI64 LTFSFileFind(LTPrt *p,I64 cur_dir_cluster,U8 *name,LTDirEntry *result,
		     BoolI8 dir_only,BoolI8 file_only);
extern BoolI64 FAT32FileFind(LTPrt *p,I64 cur_dir_cluster,U8 *name,LTDirEntry *result,
		     BoolI8 dir_only,BoolI8 file_only);
extern U0 *FileRead(U8 *filename,I64 *size=NULL,I64 *attr=NULL,BoolI8 raw=FALSE);
extern U64 LTFSAllocClusters(LTPrt *p,I64 cnt);
extern U64 FAT32AllocClusters(LTPrt *p,I64 c,I64 cnt);
extern U64 FAT32AllocContiguousClusters(LTPrt *p,I64 cnt);
extern BoolI64 LTFSDirEntryNew(LTPrt *p,U8 *cur_dir,LTDirEntry *de,BoolI8 free_old_chain);
extern BoolI64 FAT32DirEntryNew(LTPrt *p,U8 *cur_dir,LTDirEntry *de,BoolI8 free_old_chain);
extern LTDirEntry *LTFSFilesFind (U8 *files_find_mask,U64 fuf_flags,LTDirEntry *parent=NULL);
extern LTDirEntry *FAT32FilesFind(U8 *files_find_mask,U64 fuf_flags,LTDirEntry *parent=NULL,I64 *_dir_size=NULL);
extern U64 FileWrite(U8 *filename,U8 *fbuf,
   I64 size,LTDate date=0,I64 attr=0,BoolI8 raw=FALSE);

extern U0 Exit();
extern U0 AdamLog(U8 *fmt,...);
extern U0 Dbg(U8 *msg=NULL,U64 num=0,BoolI8 fault=FALSE);
extern U8 *Caller(U64 num=1);
extern BoolI64 BootCdProbeAll(LTBlkDev *bdev);
extern U64 PCIReadU16(I64 bus,I64 dev,I64 fun,I64 rg);
extern U64 PCIReadU32(I64 bus,I64 dev,I64 fun,I64 rg);
extern TaskStruct *SpawnUser(U8 *st=NULL);
extern U0 TaskRstAwaitingMsg(TaskStruct *task=NULL);
extern TaskCmdStruct *QueueMsg(TaskStruct *_servant,TaskStruct *master,
		     I64 code,U64 p1,U64 p2,U64 flags);
extern U0 TaskWaitIdle(TaskStruct *task);
extern U0 TaskPause(TaskStruct *task,BoolI8 state);
extern U0 UserTaskInit();
extern U64 ServantCmdsHandler(TaskStruct *task=NULL);
extern U0 PrintF(U8 *fmt,...);
extern U0 PutS(U8 *st);
extern U0 PutChar(U64 ch);
extern U0 PutDefine(U8 *name);
extern U0 NewLine(I64 cnt=1);
extern U64 GetChar(U64 *scancode=NULL,BoolI8 echo=TRUE,BoolI8 raw_cursor=FALSE);
extern BoolI64 IsInDbg();
extern U64 FlushMsgs(TaskStruct *task=NULL);
extern U0 SetMouseIP(I64 x,I64 y,I64 z,I64 l,I64 r,I64 c);
extern U8 *MSPrintF(U8 *fmt,...);
extern U8 *TextFileRead(U8 *filename,I64 *attr=NULL);
extern U0 SPrintFPSect(U8 *buf,U64 add,U64 field_len);
extern U64 GetS(U8 *buf,U64 size,BoolI8 allow_ext=TRUE);
extern U0 UserTaskCont();

#exe {
  KeepPrivate(ON);
  Externs2Imports(ON);
};
#include "/LT/Compiler/Compiler.HPZ"
#exe {
  Externs2Imports(OFF);
  KeepPrivate(OFF);
};
