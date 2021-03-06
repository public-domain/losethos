U0 PromptDemo()
{
  F64 d;
  I64 i;
  U8 *name;
  LTDate datetime;

  "\n\nNow prompting from the cmd line.\n\n"
  "$$FG,GREEN$$<ENTER>$$FG$$ keeps default.\n\n";

  i=PmtI64("Enter int expression(%d):",123);
  "Int=%d\n",i;

  d=PmtF64("Enter float expression(%12g):",1.2e3);
  "Float=%e\n",d;

  name=PmtStr("Name (%s):","John");
  "Name:%s\n",name;
  Free(name);

  datetime=PmtLTDate("Date (%D):",GetCurTimeLTDate);
  "Date=%D\n",datetime;
}

PromptDemo;
