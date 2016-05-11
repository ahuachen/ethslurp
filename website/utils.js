function One(a,b)
{
	if (a == b) document.write("<font color=orange>");
	else document.write("<a href=" + a.toLowerCase() + ".html>");
	document.write(a);
	if (a == b) document.write("</font>");
	else document.write("</a>");
}
function Sep()
{
	document.write(" | ");
}
function Menu(sel)
{
	One("Home", sel);
	Sep();
	One("Documentation", sel);
	Sep();
	One("Contact", sel);
}
