#help_index "Utils;Cmd Line (Typically);Ltf/Html"

//See $LK,"::/LT/Doc/Acknowledgements.TXZ","FI:::/LT/Doc/Acknowledgements.TXZ"$ (12).

U0 HtmlPutS(Ltf *l,I64 c,I64 *_old_c,U8 *st,I64 *_col,U8 *style_bitmap=NULL)
{
  U8 *ch;
  if (c!=*_old_c) {
    if (*_old_c!=-1)
      LtfPutS(l,"</span>");
    LtfPrintF(l,"<span class=c%02X>",c);
    *_old_c=c;
    if (style_bitmap)
      LBts(style_bitmap,c);
  }
  while (ch=*st++) {
    switch (ch) {
      case '&':
	LtfPutS(l,"&amp;");
	*_col=*_col+1;
	break;
      case '<':
	LtfPutS(l,"&lt;");
	*_col=*_col+1;
	break;
      case '>':
	LtfPutS(l,"&gt;");
	*_col=*_col+1;
	break;
      case '"':
	LtfPutS(l,"&quot;");
	*_col=*_col+1;
	break;
      case CH_TAB:
	do {
	  LtfPutKey(l,CH_SPACE,0);
	  *_col=*_col+1;
	} while (*_col&7);
	break;
      case CH_SHIFT_SPACE:
	LtfPutS(l,"");
	*_col=*_col+1;
	break;
      case '�':
	LtfPutS(l,"pi");
	*_col=*_col+1;
	break;
      case '�':
	LtfPutS(l,"theta");
	*_col=*_col+1;
	break;
      case '�':
	LtfPutS(l,"phi");
	*_col=*_col+1;
	break;
      case '�':
	LtfPutS(l,"omega");
	*_col=*_col+1;
	break;
      case '�':
	LtfPutS(l,"inf");
	*_col=*_col+1;
	break;
      default:
	LtfPutKey(l,ch,0);
	*_col=*_col+1;
    }
  }
}

public Ltf *Ltf2Html(Ltf *l_in,U8 *html_header=NULL,
  U8 *body_header=NULL,U8 *body_footer=NULL,BoolI8 line_anchors=TRUE)
{
  LtfEntry *cl,*style,*cl2;
  LtfSettingsStruct *s;
  I64 i,y,old_y=MIN_I64,cur_u8_attr,col,old_c=-1;
  U32 *hl,*src;
  U8 st[2],*style_bitmap=CAlloc(256/8);
  BGREntry p[16];
  GetVGAPalette4(p);
  Ltf *l_out=LtfNew;
  BoolI8 unlock_l=LtfLock(l_in),no_backward;

  for (i=0xF0;i<=0xFF;i++)
    LBts(style_bitmap,i);

  LtfRecalc(l_in);

  st[0]=0; st[1]=0;
  l_out->flags|=LTFF_PLAIN_TEXT|LTFF_NO_CURSOR;
  if (!html_header) html_header="";
  if (!body_header) body_header="";
  if (!body_footer) body_footer="";
  LtfPrintF(l_out,
    "<!DOCTYPE HTML>\n"
    "<html>\n"
    "<head>\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=US-ASCII\">\n"
    "%s"
    "<meta name=\"generator\" content=\"LoseThos V%4.2f\">\n"
    "<style type=\"text/css\">\n",html_header,os_version);
  style=l_out->cur_entry->last;
  LtfPutS(l_out,
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<pre style=\"font-family:courier;font-size:10pt\">\n");
  LtfPutS(l_out,body_header);

  cl=l_in->root.next;
  s=LtfSettingsLast(cl);
  col=cl->x;
  y=cl->y;
  cur_u8_attr=cl->type.u8[1];
  while (cl!=l_in) {
    if (!(cl->flags&(LTFLF_SKIP|LTFLF_HARD_SKIP))) {
      if (line_anchors && y!=old_y) {
	LtfPrintF(l_out,"<a name=\"l%d\"></a>",y+1);
	old_y=y;
      }
      while (y<cl->y) {
	HtmlPutS(l_out,cur_u8_attr,&old_c,"\n",&col,style_bitmap);
	y++;
	if (line_anchors && y!=old_y) {
	  LtfPrintF(l_out,"<a name=\"l%d\"></a>",y+1);
	  old_y=y;
	}
	col=0;
      }

      no_backward=TRUE;
      cl2=cl->next;
      while (cl2!=l_in && cl2->y==cl->y) {
	if (cl2->x<cl->x) {
	  no_backward=FALSE;
	  break;
	}
	cl2=cl2->next;
      }
      if (no_backward)
	while (col<cl->x)
	  HtmlPutS(l_out,cur_u8_attr,&old_c,"",&col,style_bitmap);

      if (cl->flags&LTFLF_HTML_LINK) {
	if (old_c!=-1) {
	  LtfPutS(l_out,"</span>");
	  old_c=-1;
	}
	LtfPrintF(l_out,"<a href=\"%s\">",cl->html_link);
      }
      if (cl->settings)
	s=cl->settings;
      cur_u8_attr=cl->type.u8[1];
      switch (cl->type_u8) {
	case LTFT_TEXT:
	  if (cl->flags&LTFLF_HIGHLIGHT) {
	    src=hl=LtfHighlight(l_in,FALSE,cl,&s,cl->tag,StrLen(cl->tag),cur_u8_attr<<8);
	    while (*src) {
	      st[0]=*src&0xFF;
	      HtmlPutS(l_out,*src>>8,&old_c,st,&col,style_bitmap);
	      src++;
	    }
	    Free(hl);
	  } else
	    HtmlPutS(l_out,cur_u8_attr,&old_c,cl->tag,&col,style_bitmap);
	  break;
	case LTFT_TAB:
	  HtmlPutS(l_out,cur_u8_attr,&old_c,"\t",&col,style_bitmap);
	  break;
	case LTFT_HTML_IMG:
	  if (old_c!=-1) {
	    LtfPutS(l_out,"</span>");
	    old_c=-1;
	  }
	  LtfPrintF(l_out,"<img src=\"%s\" width=\"640\" height=\"480\" alt=\"\">\n",cl->tag);
	  break;
	case LTFT_HTML_VIDEO:
	  if (old_c!=-1) {
	    LtfPutS(l_out,"</span>");
	    old_c=-1;
	  }
	  LtfPrintF(l_out,
		"<!--begin exclude-->\n"
		"<object classid=\"clsid:22D6F312-B0F6-11D0-94AB-0080C74C7E95\""
		"codebase=\"http://activex.microsoft.com/activex/controls/mplayer/en/nsmp2inf.cab\""
		"width=\"640\" height=\"512\">\n"
		"<param name=\"FileName\" value=\"%s\">\n"
		"<param name=\"AutoStart\" value=\"false\">\n"
		"<param name=\"ShowTracker\" value=\"true\">\n"
		"<param name=\"ShowControls\" value=\"true\">\n"
		"<param name=\"ShowGotoBar\" value=\"false\">\n"
		"<param name=\"ShowDisplay\" value=\"false\">\n"
		"<param name=\"ShowStatusBar\" value=\"false\">\n"
		"<param name=\"AutoSize\" value=\"false\">\n"
		"<embed src=\"%s\" AutoStart=\"false\" ShowTracker=\"true\" ShowControls=\"true\""
		"ShowGotoBar=\"false\" ShowDisplay=\"false\" ShowStatusBar=\"false\""
		"AutoSize=\"false\" pluginspage=\"http://www.microsoft.com/windows/windowsmedia/download/\""
		"width=\"640\" height=\"512\"></embed></object>\n"
		"<!--end exclude-->\n",cl->tag,cl->tag);
	  break;
	case LTFT_HTML_CODE:
	  if (old_c!=-1) {
	    LtfPutS(l_out,"</span>");
	    old_c=-1;
	  }
	  LtfPutS(l_out,cl->tag);
	  break;
	case LTFT_PICTURE:
	  HtmlPutS(l_out,cur_u8_attr,&old_c,cl->tag,&col,style_bitmap);
	  HtmlPutS(l_out,s->comment_attr&15+old_c&0xF0,&old_c,"/* Graphics Not Rendered in HTML */",&col,style_bitmap);
	  break;
	default:
	  if (Bt(ltf_tag_types,cl->type_u8))
	    HtmlPutS(l_out,cur_u8_attr,&old_c,cl->tag,&col,style_bitmap);
      }
      if (cl->flags&LTFLF_HTML_LINK) {
	if (old_c!=-1) {
	  LtfPutS(l_out,"</span>");
	  old_c=-1;
	}
	LtfPutS(l_out,"</a>");
      }
    }
    cl=cl->next;
  }
  while (y<cl->y) {
    HtmlPutS(l_out,cur_u8_attr,&old_c,"\n",&col,style_bitmap);
    y++;
    col=0;
  }
  if (old_c!=-1) {
    LtfPutS(l_out,"</span>");
    old_c=-1;
  }
  LtfPutS(l_out,body_footer);
  LtfPutS(l_out,"</pre></body>\n</html>\n");

  l_out->cur_entry=style->next;
  for (i=0;i<256;i++)
    if (Bt(style_bitmap,i))
      LtfPrintF(l_out,".c%02X{color:#%02x%02x%02x;background-color:#%02x%02x%02x;}\n",i,p[i&15].r,p[i&15].g,p[i&15].b,p[i/16].r,p[i/16].g,p[i/16].b);
  l_out->cur_entry=&l_out->root;
  LtfRecalc(l_out);

  if (unlock_l)
    LtfUnlock(l_in);
  return l_out;
}

public U0 ToHtml(U8 *_in_name,U8 *_out_name=NULL,U8 *html_header=NULL,
  U8 *body_header=NULL,U8 *body_footer=NULL,I64 width=80,BoolI8 line_anchors=TRUE)
{
  U8 *in_name,*out_name;
  Ltf *l_in,*l_out;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  SetWinHorz(0,width-1); //This sets doc width for word wrap

  in_name=DftExt(_in_name,"CPZ"),
  if (_out_name)
    out_name=DftExt(_out_name,"html");
  else
    out_name=ChgExt(_in_name,"html");

  l_in=LtfRead(in_name);
  l_out=Ltf2Html(l_in,html_header,body_header,body_footer,line_anchors);
  StrCpy(&l_out->filename.name,out_name);

  SettingsPop;

  LtfWrite(l_out);
  LtfDel(l_in);
  LtfDel(l_out);
  Free(in_name);
  Free(out_name);
}

#help_index ""
