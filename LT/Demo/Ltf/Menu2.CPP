


$PI,"<1>",1$




U0 Menu2()
{
  I64 i;
  LtfCur->flags|=LTFF_FORM; //Cursor navigation will skip empty locations.
  do {
    LtfClear;
    NewLine(1);

//Picture with flags for left-expression and an exit.
//See $LK,"ltf_dft_flags","MN:ltf_dft_flags"$.	Note the %d in the string for the picture number.
    PutSprite($IB,"<1>",1$,"$$PI+LE+X,\"50 Hz\",50,%d$$");

    NewLine(3);
    "$$MU,\"100 Hz\",100$$\n";
    "$$MU,\"200 Hz\",200$$\n";
    "$$MU,\"CANCEL\",LTFM_CANCEL$$\n";
    i=LtfMenuDo(LtfCur);
    if (i>0) Snd(i);
  } while (i>=0);
  Snd(0);
  LtfClear;
  NewLine(1);
  LtfCur->flags&=~LTFF_FORM;
}

Menu2;           �                  �����       ���       �       �              �       �       ��       ����       �����       ������       �������       ���������       ���������       �����������       �������������       �������������       ���������������       ����������������        