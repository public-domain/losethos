F64 glbl;

F64 CompileDemo(U8 *st)
{
  F64 result=0;
  U64 type;
  U0 *code;
  LexStruct *lx=LexNew(st,LF_DONT_FREE_BUF);
  Lex(lx);  //Gotta get it started
  "Compile \"%s\"\n",st;
  do {
    if (code=CmpExpression(lx,&type)) {
      if (type!=IT_F64)
	result=ToF64(Call(code));
      else
	result=Call(code)(F64);
      ExpressionDel(code);
    }
    "result=%9.4f\n",result;
    if (lx->token==';')
      Lex(lx);
  } while (lx->token!=TK_EOF);	//end of file?
  LexDel(lx);
  return result;
}

CompileDemo("2+Cos(�)");
CompileDemo("1;2+4");
CompileDemo("glbl=1;glbl*2;glbl/3;Sin(glbl)");

//This is just like you typed-it on
//the cmd line.
ExePutS(
"I64 i;"
"for (i=0;i<10;i++) "
"  \"%d\n\",i;"
);
