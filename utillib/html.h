#ifndef _HTML_H
#define _HTML_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "element.h"

#define ALIGN_NONE   -1
#define ALIGN_LEFT    0
#define ALIGN_MIDDLE  1
#define ALIGN_RIGHT   2
#define ALIGN_TOP     3
#define ALIGN_BOTTOM  4
extern SFString _aligns[];

//-------------------------------------------------------------------------
class CTable : public CElement
{
public:
	 CTable(CExportContext& ctx, const SFString& attribStr)
		: CElement(ctx, "table", attribStr, TRUE)
		{
		}
	~CTable()
		{
		}
};

//-------------------------------------------------------------------------
class CRow : public CElement
{
public:
	 CRow(CExportContext& ctx, const SFString& bgcolor=nullString, const SFString& background=nullString, SFInt32 align=ALIGN_NONE, SFInt32 valign=ALIGN_NONE)
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			if (align>=ALIGN_LEFT && align<=ALIGN_RIGHT)
				ADDATTRIB("align", _aligns[align]);
			if ((valign>=ALIGN_TOP && valign<=ALIGN_BOTTOM) || valign==ALIGN_MIDDLE)
				ADDATTRIB("valign", _aligns[valign]);
			ADDATTRIB("background", background);

			if (bgcolor.Contains("cw_css_"))
			{
				SFString col = bgcolor;
				col.Replace("class=", EMPTY);
				ADDATTRIB("class", col);
			} else if (bgcolor.Contains("style="))
			{
				SFString col = bgcolor;
				col.Replace("style=", EMPTY);
				ADDATTRIB("style", col);
			} else
			{
				ADDATTRIB("bgcolor", bgcolor);
			}
			InitElement("tr", -1, n, attribs);
		}

	 ~CRow()
		{
		}
};

//-------------------------------------------------------------------------
class CColumn : public CElement
{
public:
	 CColumn(CExportContext& ctx, const SFString& attribStr)
		: CElement(ctx, "td", attribStr, TRUE)
		{
		}

	 CColumn(CExportContext& ctx, SFInt32 align, SFInt32 nowrap, const SFString& valign, SFInt32 pctWidth=-1, SFInt32 cs=0, SFInt32 rs=0, SFInt32 nl=0, const SFString& background=nullString) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;

			if (align>=ALIGN_LEFT && align<=ALIGN_RIGHT && align!=ALIGN_LEFT) // the default is ALIGN_LEFT
				ADDATTRIB("align", _aligns[align]);
			ADDATTRIB("background", background);
			if (rs!=1)
				{ ADDATTRIB("rowspan", asString(rs, 0)); }
			if (cs!=1)
				{ ADDATTRIB("colspan", asString(cs, 0)); }
			ADDATTRIBSINGLE("nowrap", nowrap);
			ADDATTRIB("valign", valign);
			if (pctWidth!=-1)
				{ ADDATTRIB("width", asString(pctWidth, -1)+"%"); }
			InitElement("td", 1, n, attribs);
			if (nl)
				m_ctx << "\n";
		}
	 CColumn(CExportContext& ctx, SFInt32 align, const SFString& bgcolor, SFInt32 nowrap=0, const SFString& valign=nullString, SFInt32 pctWidth=-1, SFInt32 cs=0, SFInt32 rs=0, SFInt32 nl=0, const SFString& background=nullString) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;

			if (align>=ALIGN_LEFT && align<=ALIGN_RIGHT && align!=ALIGN_LEFT) // the default is ALIGN_LEFT
				ADDATTRIB("align", _aligns[align]);
			ADDATTRIB("background", background);
			if (bgcolor.Contains("cw_css_"))
			{
				SFString col = bgcolor;
				col.Replace("class=", EMPTY);
				ADDATTRIB("class", col);
			} else
			{
				if (!(bgcolor % "none"))
					ADDATTRIB("bgcolor", bgcolor);
			}
			if (rs!=1)
				{ ADDATTRIB("rowspan", asString(rs, 0)); }
			if (cs!=1)
				{ ADDATTRIB("colspan", asString(cs, 0)); }
			ADDATTRIBSINGLE("nowrap", nowrap);
			ADDATTRIB("valign", valign);
			if (pctWidth!=-1)
				{ ADDATTRIB("width", asString(pctWidth, -1)+"%"); }
			InitElement("td", 1, n, attribs);
			if (nl)
				m_ctx << "\n";
		}
	 CColumn(CExportContext& ctx) 
	  : CElement(ctx)
		{
			InitElement("td", 1, 0, NULL);
		}
	~CColumn() { }
};

//-------------------------------------------------------------------------
class CAnchor : public CElement
{
	SFString post;
protected:
	void InitAnchor(const SFString& href, const SFString& text, const SFString& pre, const SFString& pst, const SFString& target)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("target", target);
			ADDATTRIB("href", href);
			InitElement("a", 0, n, attribs);
			if (!pre.IsEmpty())
				m_ctx << pre;
			post = pst;
			m_ctx << text;
		}
public:
	CAnchor(CExportContext& ctx) 
	  : CElement(ctx, nullString, 0)
		{ };
	 CAnchor(CExportContext& ctx, const SFString& attribStr)
		: CElement(ctx, "a", attribStr)
		{
		}
	CAnchor(CExportContext& ctx, const SFString& href, const SFString& text, const SFString& pre=nullString, const SFString& pst=nullString, SFBool targetTop=FALSE) 
	  : CElement(ctx, nullString, 0)
		{
			InitAnchor(href, text, pre, pst, ((targetTop) ? SFString("top") : nullString));
		}
	CAnchor(CExportContext& ctx, const SFString& href, const SFString& text, const SFString& style, SFBool targetTop=FALSE) 
	  : CElement(ctx, nullString, 0)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("target", ((targetTop) ? SFString("top") : nullString));
			ADDATTRIB("href",   href);
			ADDATTRIB("style",  style);
			InitElement("a", 0, n, attribs);
			m_ctx << text;
		}
	~CAnchor(void)
		{
			if (!post.IsEmpty())
				m_ctx << post;
		}
};

//-------------------------------------------------------------------------
class CImageAnchor : public CAnchor
{
public:
	CImageAnchor(CExportContext& ctx, const SFString& href, const SFString& image, SFInt32 width, SFInt32 height, const SFString& alt, SFBool targetTop=FALSE, const SFString& align=nullString) 
	  : CAnchor(ctx)
		{
			SFString img;
			if (!image.IsEmpty())
			{
				img = "<img src=\"" + image + "\"";

				if (!alt.IsEmpty())
					img += (" title=\" " + alt + " \"");

				if (!alt.IsEmpty())
					img += (" alt=\" " + alt + " \"");

				if (!align.IsEmpty())
					img += (" align=\"" + align + "\"");
				else
					img += (" align=\"absmiddle\"");

				if (width > 0)
					img += (" width=\"" + asString(width) + "\"");
				else if (width < 0)
					img += (" width=\"" + asString(-width) + "%\""); // negative is percentage

				if (height > 0)
					img += (" height=\"" + asString(height) + "\"");
				else if (height < 0)
					img += (" height=\"" + asString(-height) + "%\""); // negative is percentage

				img += " border=\"0\">";
			}
			InitAnchor(href, img, nullString, nullString, ((targetTop) ? SFString("top") : nullString));
		}
};

//-------------------------------------------------------------------------
class CImage : public CElement
{
public:
	CImage(CExportContext& ctx, const SFString& image, SFInt32 width=0, SFInt32 height=0, const SFString& alt=nullString, const SFString& align=nullString) 
	  : CElement(ctx)
		{
			SFString img;
			if (!image.IsEmpty())
			{
				img = "<img src=\"" + image + "\"";

				if (!alt.IsEmpty())
					img += (" title=\" " + alt + " \"");

				if (!alt.IsEmpty())
					img += (" alt=\" " + alt + " \"");

				if (!align.IsEmpty())
					img += (" align=\"" + align + "\"");
				else
					img += (" align=\"absmiddle\"");

				if (width > 0)
					img += (" width=\"" + asString(width) + "\"");
				else if (width < 0)
					img += (" width=\"" + asString(-width) + "%\""); // negative is percentage

				if (height > 0)
					img += (" height=\"" + asString(height) + "\"");
				else if (height < 0)
					img += (" height=\"" + asString(-height) + "%\""); // negative is percentage

				img += " border=\"0\">";
			}
			ctx << img;
		}
};

//-------------------------------------------------------------------------
class CBackSound : public CElement
{
public:
	CBackSound(CExportContext& ctx, const SFString& source, SFBool loop) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("src",       source);
			ADDATTRIB("loop",      ((loop) ? SFString("infinite") : SFString("false")));
			// Note: only MSIE supports this tag, use embed to support netscape
			InitElement("bgsound", -1, n, attribs);
		}
};

//-------------------------------------------------------------------------
class CSound : public CElement
{
public:
	 CSound(CExportContext& ctx, const SFString& source, SFBool loop=FALSE) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("src",       source);
			ADDATTRIB("autostart", SFString("true"));
			ADDATTRIB("width",     asString(0));
			ADDATTRIB("height",    asString(0));
			ADDATTRIB("loop",      ((loop) ? SFString("true") : SFString("false")));
			// we use embed so it also works under Netscape which does not support the <bgsound tag
			InitElement("embed", -1, n, attribs);
		}
};

//-------------------------------------------------------------------------
class CFormOb : public CElement
{
public:
	 CFormOb(CExportContext& ctx, const SFString& method, const SFString& action, const SFString& name=nullString, const SFString& enctype=nullString, const SFString& onSub=nullString) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("method",   method);
			ADDATTRIB("action",   action);
			ADDATTRIB("name",     name);
			ADDATTRIB("enctype",  enctype);
			ADDATTRIB("onSubmit", onSub);
			InitElement("form", -1, n, attribs);
		}
	~CFormOb(void) { };
};

//-------------------------------------------------------------------------
class CTrueType : public CElement
{
public:
	 CTrueType(CExportContext& ctx) 
	  : CElement(ctx, "tt", 0) 
		{ };
	~CTrueType(void) { };
};

//-------------------------------------------------------------------------
class CLineBreak : public CElement
{
public:
	 CLineBreak(CExportContext& ctx, SFBool repeats=1) 
	  : CElement(ctx, "br", SINGLETON) 
		{
			SFInt32 i=0;
			for (i=0;i<repeats-1;i++)
				ctx << "<br>";
			ctx << "\n";
		}
	~CLineBreak(void) { };
};

//-------------------------------------------------------------------------
class CCenter : public CElement
{
public:
	 CCenter(CExportContext& ctx) 
	  : CElement(ctx, "center", 0) 
		{
		}
	~CCenter(void) { };
};

//-------------------------------------------------------------------------
class CHorizontalLine : public CElement
{
public:
	 CHorizontalLine(CExportContext& ctx, const SFString& attribStr)
		: CElement(ctx, "hr", attribStr)
		{
		}
	 CHorizontalLine(CExportContext& ctx, SFInt32 size=2, SFInt32 noshade=FALSE, const SFString& width=nullString) 
	  : CElement(ctx) 
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIBSINGLE("noshade", noshade);
			ADDATTRIB("size", asString(size, 2));
			ADDATTRIB("width", width);
			InitElement("hr", -1, n, attribs);
		};
	~CHorizontalLine(void)
		{
			m_singleton=TRUE;
		};
};

//-------------------------------------------------------------------------
class SFText : public CElement
{
	SFInt32 bold;
	SFInt32 ital;
	SFInt32 font;

protected:
	void InitText(const SFString& face, SFInt32 size, const SFString& text, const SFString& color, SFInt32 i, SFInt32 b)
		{
			ital=i;
			bold=b;

			SFString fc = face;
			if (fc % "none")
				fc = "";

			font=(!fc.IsEmpty() || size || !color.IsEmpty());

			if (font)
			{
				m_ctx << "<font";
				if (!color.IsEmpty())
				{
					m_ctx << " color=\"" << color << "\"";
				}
				if (!fc.IsEmpty())
				{
					m_ctx << " face=\"" << fc << "\"";
				}
				if (size)
				{
					m_ctx << " size=\"" << asString(0) << "\"";
				}
				m_ctx << ">";
			}
			if (ital)
				m_ctx << "<i>";
			if (bold)
				m_ctx << "<strong>";
			m_ctx << text;
		}

	void InitText(const SFString& face, SFInt32 size, const SFString& text, SFInt32 color, SFInt32 i, SFInt32 b)
		{
			InitText(face, size, text, EMPTY, i, b);
		}

	SFText(CExportContext& ctx)
		: CElement(ctx, nullString, 0)
		{
		}

public:
	SFText(CExportContext& ctx, const SFString& text, SFInt32 color=-1, SFInt32 i=FALSE, SFInt32 b=FALSE) 
	  : CElement(ctx, nullString, 0)
		{
			InitText(nullString, 0, text, color, i, b);
		}
	SFText(CExportContext& ctx, const SFString& text, const SFString& color, SFInt32 i=FALSE, SFInt32 b=FALSE) 
	  : CElement(ctx, nullString, 0)
		{
			InitText(nullString, 0, text, color, i, b);
		}
	SFText(CExportContext& ctx, const SFString& face, SFInt32 size, const SFString& text, SFInt32 color=-1, SFInt32 i=FALSE, SFInt32 b=FALSE) 
	  : CElement(ctx, nullString, 0)
		{
			InitText(face, size, text, color, i, b);
		}
	SFText(CExportContext& ctx, const SFString& face, SFInt32 size, const SFString& text, const SFString& color, SFInt32 i=FALSE, SFInt32 b=FALSE) 
	  : CElement(ctx, nullString, 0)
		{
			InitText(face, size, text, color, i, b);
		}
	~SFText(void)
		{
			if (bold)
				m_ctx << "</strong>";
			if (ital)
				m_ctx << "</i>";
			if (font)
				m_ctx << "</font>";
		}
};

//-------------------------------------------------------------------------
class SFWebPage : public CElement
{
public:
	 SFWebPage(CExportContext& ctx) 
	   : CElement(ctx, "html") 
		 { };
	~SFWebPage(void) { };
};

//-------------------------------------------------------------------------
class CHead : public CElement
{
public:
	 CHead(CExportContext& ctx) 
	   : CElement(ctx, "head") 
		 { };
	~CHead(void) { };
};

//-------------------------------------------------------------------------
class SFTitle : public CElement
{
public:
	 SFTitle(CExportContext& ctx, const SFString& tit) 
	  : CElement(ctx, "title", 1)
		{
			m_ctx << tit;
		};
	~SFTitle(void) { };
};

//-------------------------------------------------------------------------
class CMetaTag : public CElement
{
public:
	 CMetaTag(CExportContext& ctx, const SFString& equiv, const SFString& content) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("http-equiv", equiv);
			ADDATTRIB("content", content);
			InitElement("meta", 0, n, attribs);
		};
	~CMetaTag(void)
		{
			m_ctx << "\n";
			m_tag = nullString;
		};
};

//-------------------------------------------------------------------------
class CBody : public CElement
{
public:
	CBody(CExportContext& ctx, const SFString& onLoadStr=nullString)
		: CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB  ("onLoad", onLoadStr);
			InitElement("body", -1, n, attribs);
		}

	~CBody(void) { };
};

//-------------------------------------------------------------------------
class COption : public CElement
{
public:
	 COption(CExportContext& ctx, const SFString& value, const SFString& name, SFBool selected)
		: CElement(ctx)
		{
			ctx << "\n";
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIBSINGLE("selected", selected);
			ADDATTRIB("value", value);
			InitElement("option", 1, n, attribs);
			m_ctx << name;
		}
  ~COption()
		{
			m_singleton = TRUE;
		};
};

//-------------------------------------------------------------------------
class CSelectionData;
typedef SFBool (*SELFUNC)(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
class CSelectionData
{
public:
	SFString  name;
	SFString  onChangeStr;
	SFString  selStr;
	SFInt32   selInt;
	SFBool    disabled;
	SFBool    sorted;
	SFBool    sortAll;
	SELFUNC   func;
	SFInt32   extraInt;
	SFString  extraStr;
	const void *dataVal;

	CSelectionData(void)
		{
//			SFString name;
//			SFString onChangeStr;
//			SFString selStr;
			selInt   = 0;
			disabled = FALSE;
			sorted   = FALSE;
			sortAll  = FALSE;
			func     = NULL;
			extraInt = FALSE;
//			SFString extraStr;
			dataVal  = NULL;
		}
	void set(SELFUNC f, const SFString& n, SFInt32 sel, SFBool d)
		{
			func     = f;
			name     = n;
			selInt   = sel;
			disabled = d;
		}
	void set(SELFUNC f, const SFString& n, const SFString& sel, SFBool d)
		{
			func   = f;
			name   = n;
			selStr = sel;
			disabled = d;
		}

private:
	CSelectionData(const CSelectionData& data);
	CSelectionData operator=(const CSelectionData& data);
};

//-------------------------------------------------------------------------
class CSelect : public CElement
{
protected:
	void Create(SFInt32 nFields, SFAttribute values[], const SFString& selected)
		{
			SFInt32 i=0;
			for (i=0;i<nFields;i++)
			{
				SFString val  = values[i].getValue();
				SFString name = values[i].getName();
				{COption option(m_ctx, val, name, (val==selected));
					} // EOO
			}
		}
	void Create(SFInt32 nFields, SFAttribute values[], SFInt32 selected)
		{
			SFInt32 i=0;
			for (i=0;i<nFields;i++)
			{
				SFString val  = values[i].getValue();
				SFString name = values[i].getName();
				{COption option(m_ctx, val, name, (i==selected));
					} // EOO
			}
		}
	void Create(SFInt32 nFields, SFString *vals, SFString *prompts, SFInt32 selected)
		{
			SFInt32 i=0;
			for (i=0;i<nFields;i++)
			{
				SFString val  = vals[i];
				SFString name = prompts[i];
				{COption option(m_ctx, val, name, (i==selected));
					} // EOO
			}
		}
public:
	 CSelect(CExportContext& ctx, const SFString& name, SFInt32 size);
	 CSelect(CExportContext& ctx, const SFString& name, SFInt32 size, const SFString& onChangeStr, SFBool disabled);
	 CSelect(CExportContext& ctx, const SFString& name, SFInt32 nFields, SFString *vals, SFString *prompts, SFInt32 selected);
	 CSelect(CExportContext& ctx, const SFString& name, SFInt32 nFields, SFString *vals, SFString *prompts, SFInt32 selected, const SFString& onChangeStr, SFBool disabled);
	 CSelect(CExportContext& ctx, SFAttribute vals[],   SFInt32 nFields, const SFString& name, SFInt32 selected, const SFString& onChangeStr=nullString);
	 CSelect(CExportContext& ctx, CSelectionData *data);
	~CSelect(void) { };
};

//-------------------------------------------------------------------------
class CInput : public CElement
{
public:
	 CInput(CExportContext& ctx, const SFString& type, const SFString& name, SFInt32 size, const SFString& value, SFBool readOnly, SFBool checked, const SFString& onClickStr, SFInt32 maxSize) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("type", type);
			ADDATTRIB("name", name);
			ADDATTRIB("size", asString(size, -1));
			ADDATTRIB("value", value);
			ADDATTRIB("onClick", onClickStr);
			ADDATTRIB("maxlength", asString(maxSize, -1));
			ADDATTRIBSINGLE("checked", checked);
			if (type=="checkbox")
			{
				ADDATTRIB("id", name);
				ADDATTRIBSINGLE("disabled", readOnly);
			} else
			{
				ADDATTRIBSINGLE("readOnly", readOnly);
				ADDATTRIBSINGLE("disabled", readOnly);
			}
			InitElement("input", 1, n, attribs);
		}
	 CInput(CExportContext& ctx, const SFString& type, const SFString& name, SFInt32 size, const SFString& value, SFBool readOnly, SFBool checked=FALSE) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("type", type);
			ADDATTRIB("name", name);
			ADDATTRIB("size", asString(size, -1));
			ADDATTRIB("value", value);
			ADDATTRIBSINGLE("checked", checked);
			if (type=="checkbox")
			{
				ADDATTRIB("id", name);
				ADDATTRIBSINGLE("disabled", readOnly);
			} else
			{
				ADDATTRIBSINGLE("readOnly", readOnly);
				ADDATTRIBSINGLE("disabled", readOnly);
			}
			InitElement("input", 1, n, attribs);
		}
	~CInput()
		{
			m_nls=1;
			m_singleton=TRUE;
		}
};

//-------------------------------------------------------------------------
class CHiddenInput : public CInput
{
public:
	 CHiddenInput(CExportContext& ctx, const SFString& name, const SFString& value) 
	  : CInput(ctx, "hidden", name, -1, value, FALSE, FALSE, nullString, -1)
		{
		}
	 CHiddenInput(CExportContext& ctx, const SFString& name, SFInt32 value) 
		: CInput(ctx, "hidden", name, -1, asString(value, -10012), FALSE, FALSE, nullString, -1) // never defaults
		{
		}
	~CHiddenInput()
		{
			m_ctx << "\n";
		}
};

//-------------------------------------------------------------------------
class CCheckbox : public CInput
{
public:
	CCheckbox(CExportContext &m_out, const SFString& fieldName, const SFString& fieldVal, SFBool readOnly, SFBool checked, const SFString& onClickedStr)
		: CInput(m_out, "checkbox", fieldName, -1, fieldVal, readOnly, checked, onClickedStr, -1)
		{
		}
	~CCheckbox()
		{ };
};

//-------------------------------------------------------------------------
class CRadio: public CInput
{
public:
	CRadio(CExportContext &m_out, const SFString& fieldName, const SFString& fieldVal, SFBool readOnly, SFBool checked, const SFString& onClickedStr)
		: CInput(m_out, "radio", fieldName, -1, fieldVal, readOnly, checked, onClickedStr, -1)
		{
		}
	~CRadio()
		{ };
};

//-------------------------------------------------------------------------
class CLabel : public CElement
{
public:
	 CLabel(CExportContext& ctx, const SFString& id, const SFString& prompt) 
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB("for", id);
			InitElement("label", 0, n, attribs);
			if (!prompt.IsEmpty())	
				m_ctx << prompt;
		}
	~CLabel() { }
};

//-------------------------------------------------------------------------
class CTextArea : public CElement
{
public:
	CTextArea(CExportContext& ctx, const SFString& name, SFInt32 rows, SFInt32 cols, SFBool disabled)
	  : CElement(ctx)
		{
			SFAttribute attribs[25];
			SFInt32 n=0;
			ADDATTRIB      ("name",     name);
			ADDATTRIB      ("rows",     asString(rows));
			ADDATTRIB      ("cols",     asString(cols));
			ADDATTRIBSINGLE("disabled", disabled);
			InitElement("textarea", 0, n, attribs);
		}
	~CTextArea() { }
};

//-------------------------------------------------------------------------
inline SFString smallColoredBox(const SFString& bgColor, const SFString& textColor, const SFString& text)
{
	if (bgColor.Contains("cw_css"))
		return SFString("<span class=" + bgColor + ">" + text + "</span>");

	SFString textPart, bgPart;

	if (!textColor.IsEmpty() && textColor != "default") textPart = "color: "            + textColor + "; ";
	if (!bgColor.IsEmpty()   && bgColor   != "default") bgPart   = "background-color: " + bgColor   + "; ";

	return SFString("<span style=\"") + bgPart + textPart + "\">" + text + "</span>";
}

//-------------------------------------------------------------------------
inline SFString getPopLink(const SFString& text, const SFString& href)
{
	CStringExportContext ctx;
	{CAnchor anchor(ctx, href, text, nullString, nullString, TRUE);
		}
	return ctx.str;
}

//-------------------------------------------------------------------------
inline SFString convertURLs(const SFString& link, const SFString& image, const SFString& style)
{
	CStringExportContext ctx;
	if (!image.IsEmpty())
	{CImageAnchor anchor(ctx, link, image, 0, 0, link, TRUE);
		} else
	{CAnchor      anchor(ctx, link, link, style, TRUE);
		}
	return ctx.str;
}

//-------------------------------------------------------------------------
inline SFString convertEmails(const SFString& inputIn, const SFString& style, SFInt32 maxLen=9998)
{
	SFString input = inputIn;

	SFString ret = "<a href=mailto:" + input;
	if (!style.IsEmpty())
		ret += " style=\"" + style + "\"";
	ret += ">";
	if (inputIn.GetLength() > maxLen)
		input.Replace("@", "@<br>&nbsp;");
	ret += input;
	ret += "</a>";
	return ret;
}

#endif
