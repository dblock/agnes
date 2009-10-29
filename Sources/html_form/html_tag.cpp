#include <agnespch.hpp>

#ifndef HTML_TAG_CPP
#define HTML_TAG_CPP

const html_tag html_("html", 0);
const html_tag head_("head", 0);
const html_tag body_("body", 0);
const html_tag title_("title", 0);
const html_tag base_("base", 
			"HREF", "", 
			"TARGET", "", 0);
const html_tag div_("div", 
			"ALIGN", "", 
			"STYLE", "", 0);
const html_tag center_("center", 0);
const html_tag table_("table", 
			"BORDER", "",
			"BACKGROUND", "",
			"BGCOLOR", "",
			"BORDERCOLOR", "",
			"WIDTH", "",
			"CELLSPACING", "",
			"CELLPADDING", "",
			"FRAME", "",
			"HEIGHT", "",
			"RULES", "",
			0);
const html_tag tr_("tr", 
			"ALIGN", "", 
			"BGCOLOR", "", 
			"BORDERCOLOR", "", 
			"VALIGN", "", 
			"HEIGHT", "", 
			0);
const html_tag td_("td",
			"BACKGROUND", "",
			"BGCOLOR", "",
			"BORDERCOLOR", "",
			"ALIGN", "",
			"NOWRAP", "",	//make a single type option
			"VALIGN", "",
			"ROWSPAN", "",
			"COLSPAN", "",
			"WIDTH", "",
			"HEIGHT", "",
			0);

const html_tag form_("form",
			"ACTION", "",
			"METHOD", "",
			"TARGET", "",
			"ENCTYPE", "",
			0);

const html_tag p_("p",
			"ALIGN", "",
			0);

const html_tag font_("font", 
			"SIZE", "",
			"COLOR", "",
			"FACE", "",
			0);
const html_tag em_("em", 0);
const html_tag br_("br", 0);
const html_tag a_("a",
		"HREF","",
		"NAME","",
		"OnClick","",
		"OnMouseOver","",
		"REV","",
		"REL","",
		"TARGET","",
		"TITLE","",
		0);
const html_tag input_("input",
			"ALIGN", "",
			"CHECKED", "",
			"MAXLENGTH", "",
			"NAME", "",
			"SIZE", "",
			"SRC", "",
			"VALUE","",
			"TYPE","",
			0);
const html_tag img_("img",
			"SRC", "",
			"ALIGN","",
			"WIDTH","",
			"HEIGHT","",
			"USEMAP","",
			"ISMAP","",
			"ALT","",
			"BORDER","",
			"LOWSRC","",
			"VSPACE","",
			"HSPACE","",
			0);
const html_tag textarea_("textarea",
		   "COLS","",
		   "ROWS","",
		   "NAME","",
		   "WRAP","",
		   0);
const html_tag hr_("hr",0);
const html_tag b_("b",0);
const html_tag blockquote_("blockquote",0);
const html_tag select_("select",
		"MULTIPLE","",
		"NAME","",
		"SIZE","",
		0);
const html_tag option_("option", "VALUE","", 0);
const html_tag h1_("h1", "ALIGN","", 0);
const html_tag h2_("h2", "ALIGN","", 0);
const html_tag h3_("h3", "ALIGN","", 0);
const html_tag h4_("h4", "ALIGN","", 0);

#endif
