#!/usr/bin/perl -w
use CGI qw(:standard);
use CGI::Carp qw(warningsToBrowser fatalsToBrowser);
use File::Temp qw/ :POSIX /;
print header;
print start_html("Label printing results");

foreach my $p (param()) {
    print "$p = " . param($p) . "<br>\n";
}

$psname = tmpnam() . "_title_subtitle_label.ps";
print "creating $psname<BR>\n";

# Get strings
$title = "";
$note = "";
foreach my $key (param())
{
	$val = param($key);
	if ($key =~ "title")
	{
		$title = $val;
	}
	elsif ($key =~ "note")
	{
		$note = $val;
	}
}

open(outf, ">$psname");
print outf "%!PS-Adobe-2.0\n";
print outf "%%BoundingBox: 0 0 400 64\n";
print outf "/labelwidth ($title) stringwidth def\n";
print outf "%%BoundingBox: 0 0 labelwidth 64\n";
print outf "/WindsorDemi findfont 32 scalefont setfont\n";
print outf "/labelwidth ($title) stringwidth pop def\n";
if ($note)
{
	print outf "200 30 moveto ($title) labelwidth 2 div neg 0 rmoveto show\n";
}
else
{
	print outf "200 18 moveto ($title) labelwidth 2 div neg 0 rmoveto show\n";
}
print outf "/Helvetica findfont 18 scalefont setfont\n";
print outf "200 4 moveto ($note) dup stringwidth pop 2 div neg 0 rmoveto show\n";
print outf "showpage\n";

close(outf);
print "printing $psname<BR>\n";
system("/usr/local/bin/peggps $psname");
unlink($psname);
print "<BR><BR>all done!<BR>\n";
print end_html;
