U8 sys_temp_filename[136];
TaskCmdStruct sys_macro_root;
KeyDevStruct sys_put_key_root;
TaskStruct *sys_macro_util_task=NULL;

MPCmdCtrl *mp_ctrl=NULL;
MPCrashStruct *mp_crash=NULL;
U8 mp_apic_ids[MP_MAX_PROCESSORS];
TaskStruct *adam_task;
I64 sys_num_spawned_users=0;

LTCacheBlk *sys_ltdc_base=NULL,*sys_ltdc_ctrl=NULL;
LTCacheBlk **sys_ltdc_hash_table=NULL;
U64 sys_ltdc_size;
TaskStruct *sys_ltdc_write_out_task=NULL;
 
U64 sys_raw_dc_col=0,sys_raw_dc_flags=0;
U8 *sys_dc_screen_image;
BoolI8 text_mode;
I64 text_rows,text_cols,gr_width,gr_height;
U8 *gr_font;

TaskStruct *sys_winmgr_task=NULL,
	   *sys_task_being_screen_updated=NULL;
U64 sys_rnd_seed;
Ltf *sys_clipboard_ltf;
U8 *sys_dft_iso_name;
U64 sys_dsk_reads,sys_dsk_writes;
LTBlkDev *local_blkdevs;
LTPrt *local_prts,*dft_prt;
U8 dft_drv;
BoolI8 boot_drv_pmt=FALSE;
I64 num_blkdevs;
LTPrt *drv_to_prt[32];
SysAcctStruct sys_accts,*sys_acct=NULL;
U8 *sys_acct_name=NULL;
U8 *sys_home_dir=NULL;
U8 *reverse_bits_table,*set_bits_table;
U0 (**fp_ctrl_alt_let_cbs)(U64 sc);
U64 ctrl_alt_let_in_irq_flags;
U8 **ctrl_alt_let_descs;
LTDate local_time_offset=0;
BoolI8 time_stamp_freq_calibrated=FALSE;
U64 HPET_freq=0,HPET_kHz_freq=0,
   time_stamp_freq=2676302000,
   time_stamp_kHz_freq=2676302,
   time_stamp_freq_initial=2676302000,
   mouse_evt_time=0,kbd_evt_time=0,mouse_pkt_size=3;
U64 sys_cur_scan_code=0;

U8 key_desc[128];
BoolI8 kbd_irqs_working;
U0 *key_handler;
U64 sys_num_installed_compilers=0;

PCIDevStruct sys_pci_dev_root;
MemRange sys_mem32_dev_root;
U8 *sys_mem64_dev_ptr=MAPPED_MEM_SPACE;

F64 snd_freq=0;
I64 snd_dev=SD_PC_SPEAKER;
BoolI8 snd_record=FALSE;
F64 snd_vol=0.1,snd_reverb_intensity=0.2,snd_reverb_delay=0.3;

F64 *pow10_I64;

HeapCtrl *sys_uncached_heap;


I64 ipx,ipy,ipz,ip_throttle;
I64 ipx_presnap,ipy_presnap,ipz_presnap;
I64 ipx_offset,ipy_offset,ipz_offset;
F64 ipx_scale,ipy_scale,ipz_scale;
F64 ip_speed;
I64 ip_dev;
I64 iptx,ipty;
BoolI8 ip_on,  //Show or hide cursor
     ip_lb_last,ip_cb_last,ip_rb_last,
     ip_lb,ip_cb,ip_rb;
BoolI8 ip_has_wheel;
LTDate ip_last_move_time;
SnapStruct ip_snap_last;

F64 mxx_scale,myy_scale,mzz_scale;

//These are the coordinates before
//they get scaled.
I64 mxx_prescale,myy_prescale,mzz_prescale;
I64 mxx,myy,mzz;
F64 mouse_speed;
I64 old_mxx,old_myy,old_mzz;
BoolI8 mouse_buttons[5];

BoolI8	mouse_has_wheel,
	mouse_has_ext_buttons,
	mouse_evt,
	mouse_installed,
	mouse_install_attempt_in_progress,
	kbdmouse_rst,
	mouse_irqs_working;
U64Fifo	*scan_code_fifo;
U8Fifo	*kbd_fifo,*kbd2_fifo,
	*mouse_fifo,*mouse2_fifo;
U64	sys_key_cnt,
	sys_last_down_scan_code,
	mouse_install_attempts,
	mouse_last_evt_time;

//BitTest, $LK,"Bt","MN:Bt"$(), with a merged scan code.
//(Left and right shift merged, for example.)
U32	key_down_bitmap[8];
//BitTest, $LK,"Bt","MN:Bt"$(), with an unmerged scan code.
U32	key_down_bitmap2[8];

I64 sys_num_usb=0;
LTUSB sys_usb_devs[MAX_USB];

U0 (*fp_snd)(F64 freq,U64 waveform,F64 amp)=NULL;
U0 (*fp_snd_record)(F64 freq,U64 waveform,F64 amp)=NULL;
U0 (*fp_snd_fill_buf)(SND_OUT_CONTAINER *buf,I64 buf_num)=NULL;
U0 (*fp_snd_copy_buf)(SND_IN_CONTAINER *buf,I64 buf_num)=NULL;
U8  *(*fp_mgets)(U64 flags=0)=NULL;
U0 (*fp_prs_stream_blk)(LexStruct *lx)=NULL;
U0 (*fp_update_ctrls)(TaskStruct *task)=NULL;
Ltf *(*fp_ltf_cur)(TaskStruct *task=NULL)=NULL;
U0 (*fp_set_std_palette)()=NULL;

U64 sys_dbg_fault_num=-1;
U0 *sys_dbg_fault_rip=0;

PSectCacheEntry *psect_cache;
I64 psect_cache_index;
//These are for $LK,"SPrintF","MN:SPrintFJoin"$()
U8 *date_fmt_map,*time_fmt_map;
