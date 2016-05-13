function One(text,link,sel)
{
	if (text == sel) document.write("<font color=orange>");
	else document.write("<a href=" + link + ".html>");
	document.write(text);
	if (text == sel) document.write("</font>");
	else document.write("</a>");
}

function Menu(sel)
{
	One("Home", "index", sel);
	document.write(" | ");
	One("Video", "video", sel);
	document.write(" | ");
	One("Documentation", "documentation", sel=="Config"?"Documentation":sel);
	document.write(" | ");
	One("Contact", "contact", sel);
}

function Two(text,link,sel)
{
	document.write("<small>");
	if (text == sel) document.write("<font color=orange>");
	else document.write("<a href=" + link + ".html>");
	document.write(text);
	if (text == sel) document.write("</font>");
	else document.write("</a>");
	document.write("</small>");
}

function DocMenu(sel)
{
	document.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|--")
	Two("index", "documentation", sel);
	document.write("|");
	Two("config file", "config", sel);
	document.write("|");
	Two("display strings", "display_strings", sel);
	document.write("|");
	Two("detail", "display_strings_det", sel);
}
