//$BK,1$USB is not yet supported.$BK,0$
//See $LK,"::/LT/Demo/Lectures/USB.CPZ","FI:::/LT/Demo/Lectures/USB.CPZ"$.
//See $LK,"::/LT/Demo/Lectures/PCIInterrupts.CPZ","FI:::/LT/Demo/Lectures/PCIInterrupts.CPZ"$.

U0 USBInitOne(I64 b,I64 d,I64 f)
{
  LTUSB *u;
  if (sys_num_usb<MAX_USB &&
      PCIReadU16(b,d,f,0)==0x8086) {
    u=&sys_usb_devs[sys_num_usb++];
    u->num=sys_num_usb;
    u->bus=b;
    u->dev=d;
    u->fun=f;
    u->ports=PCIReadU32(b,d,f,0x20)&~0x1F;
  }
}

U0 USBEndAll()
{
  sys_num_usb=0;
}

U0 USBInitAll() //This is only valid for my ICH10 dev
{
  if (sys_num_usb)
    USBEndAll;
  USBInitOne(0,29,0);
  USBInitOne(0,29,1);
  USBInitOne(0,29,2);
  USBInitOne(0,29,3);
  USBInitOne(0,26,0);
  USBInitOne(0,26,1);
  USBInitOne(0,26,2);
}

USBTD *USBAllocTD()
{
  return MAllocAligned(sizeof(USBTD),0x10,sys_uncached_heap);
}

U0 USBFreeTD(USBTD *temptd)
{
  Free(temptd);
}

U32 *USBAllocFrameList(U64 usb_num,U64 size)
{ //aligned to 0x1000
  LTUSB *u;
  if (0<=usb_num<sys_num_usb) {
    u=&sys_usb_devs[usb_num];
    Free(u->frame_list);
    u->frame_list=MAllocAligned(size*sizeof(U32),0x1000,sys_uncached_heap);
    OutU16(u->ports+USBP_CMD,0); //Stop
    OutU16(u->ports+USBP_FRNUM,0);
    OutU32(u->ports+USBP_FRBASEADD,u->frame_list);
    return u->frame_list;
  }
  return NULL;
}
