#help_index "WordStat"
#help_file "::/LT/Doc/WordStat"

//See $LK,"::/LT/Doc/WordStat.TXZ","FI:::/LT/Doc/WordStat.TXZ"$

class WsSubStruct
{
  I32 hits,repeats;
};

#define WS_SUBSTRUCTS_PER_STRUCT 8
class WsStruct
{
  WsStruct *next,*last;
  I64 substructs,     //-1 means WsMatrixSide
     entries;
  WsSubStruct subs[WS_SUBSTRUCTS_PER_STRUCT];
};

class WsMatrixSide
{
  WsStruct *next,*last;
  I64 const_minus_one, //-1 used to header in queue
     row_hits,col_hits;
};

#define WS_MAX_MATRIX_WORDS 0x8000
class WsMatrix
{
  I64 n;
  WsMatrixSide side[WS_MAX_MATRIX_WORDS];
};

public I64 ws_num_words,ws_num_matrix_words;
SysHashTable *ws_hash_table=NULL;
public U8 *ws_cur_word=NULL;

#define WSf_ENABLE		0
#define WSf_INIT_IN_PROGRESS	1
U64 ws_flags=0;

public WsMatrix *ws_matrix=NULL;
public WsHashEntry **ws_sorted_words=NULL;
TaskStruct *ws_task=NULL;


#define WS_MAX_FILLINS 10
I64 ws_prtial_len=0,ws_fillin_matches[WS_MAX_FILLINS+1];
I64 ws_num_fillins=0,ws_fillin_scores[WS_MAX_FILLINS+1],ws_fillin_hits[WS_MAX_FILLINS+1];

//**** Dictionary Stuff
#help_index "WordStat/Dictionary"

#define WSD_WORD_FILENAME	"/LT/Adam/WordStat/WsWords.DTZ"
#define WSD_DEF_FILENAME	"/LT/Adam/WordStat/WsDefs.DTA"
#define WSD_DEF_FILENAME_Z	"/LT/Adam/WordStat/WsDefs.DTZ"

#define WSD_H1			0
#define WSD_H1_END		1
#define WSD_DEF			2
#define WSD_DEF_END		3
#define WSD_PRONUNCIATION	4
#define WSD_PRONUNCIATION_END	5
#define WSD_POS			6
#define WSD_POS_END		7
#define WSD_EXTRA		8
#define WSD_EXTRA_END		9
#define WSD_BLK_SIZE		0x4000

#define WSD_END_CHAR	0x00
#define WSD_WORD_CHAR	0x01
#define WSD_DEF_CHAR	0x02
#define WSD_PRONUNCIATION_CHAR	0x03
#define WSD_POS_CHAR	0x04
#define WSD_EXTRA_CHAR	0x05

public U8 *ws_dict_word_list=NULL;
public I64 ws_dict_word_list_size=0,
   ws_dict_num_words=0,ws_structs_size=0;

extern U0 WsdWordsLoad();
extern U0 WsdDictWordsAdd(Ltf *l,U8 *st=NULL);

#define WSD_MAX_FILLINS 10
U8 *wsd_fillins[WSD_MAX_FILLINS];
I64 wsd_num_fillins=0;

// ************************************

#include "WsMain.APZ"
#include "WsDict.APZ"

#help_index ""
