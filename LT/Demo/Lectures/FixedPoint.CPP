/*
"Fixed point" means you use integers
that are scaled by a value.A common
example would be using number of pennies
instead of dollars with a float.

Fixed-point used to be much $TX+HTML,"faster","http://en.wikipedia.org/wiki/X87"$,
but modern processors do well with
floats.It also depends on the compiler
and my compiler used to do floats poorly.
It's a little better.

I often use 64-bit ints with upper 32-bits
as integer and lower 32-bits as fraction.

See $LK,"::/LT/Demo/SubIntAccess.CPZ","FI:::/LT/Demo/SubIntAccess.CPZ"$ for how
to access upper or lower 32-bits.

For a complete list of nonstandard compiler
features, see $LK,"::/LT/Doc/Differences.TXZ","FI:::/LT/Doc/Differences.TXZ"$.

*/

U0 Main()
{
  F64 t0,f_sum=0,f_val;
  I64 i ,i_sum=0,i_val;

  i_val= 2.0000002 *0x100000000;
  t0=tP;
  for (i=1000000000;i;i--)
    i_sum+=i_val;
  "IntegerSum:%.9f Time:%7.3fs\n",i_sum/ToF64(0x100000000),tP-t0;

  f_val= 2.0000002;
  t0=tP;
  for (i=1000000000;i;i--)
    f_sum+=f_val;
  "FloatSum:%.9f Time:%7.3fs\n",f_sum,tP-t0;
  NewLine;
}

U0 DoIt2()
{
  U64 i=0x123456789ABCDEF0;
  "i=%X\n",i;

  "i&0xFFFFFFFF=%X\n",i&0xFFFFFFFF;
  "i>>32=%X\n",i>>32;

/*
  Standard integer types are declared
  with a special compiler feature which
  allows a structure to be accessed as a
  whole.That's why the i variable can
  be accessed normally in addition to
  structure member access $LK,"U64","MN:U64"$.The actual
  intrinsic compiler type is U64i.

public U64i union U64
{
  I8i i8[8];
  U8i u8[8];
  I16 i16[4];
  U16 u16[4];
  I32 i32[2];
  U32 u32[2];
};

  It's not quite as great as it seems
  because the compiler decides it cannot
  place i into a register, so there is a
  penalty.

For a complete list of nonstandard compiler
features, see $LK,"::/LT/Doc/Differences.TXZ","FI:::/LT/Doc/Differences.TXZ"$.

*/

  "i.u32[0]=%X\n",i.u32[0];
  "i.u32[1]=%X\n",i.u32[1];
}

CPURep;
Main;
DoIt2;

//See $LK,"::/LT/Demo/Lectures/FixedPointAdvanced.CPZ","FI:::/LT/Demo/Lectures/FixedPointAdvanced.CPZ"$


/*  Program Output$HL,0$$WW+H,1$$FD,1$

8 Cores 2.660GHz
Integer Sum:2000000199.768689408 Time:	0.787s
Float	Sum:2000000225.656126208 Time:	3.918s

i	    =123456789ABCDEF0
i&0xFFFFFFFF=9ABCDEF0
i>>32	    =12345678
i.u32[0]    =9ABCDEF0
i.u32[1]    =12345678

$HL,1$*/
