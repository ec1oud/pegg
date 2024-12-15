#!/usr/bin/perl -w
use CGI qw(:standard);
use CGI::Carp qw(warningsToBrowser fatalsToBrowser);
use File::Temp qw/ :POSIX /;
use File::Basename;
print header;
print start_html("Label printing results");

sub runOrDie
{
	my($cmd) = @_;
	print "$cmd<br/>\n";
	$ret = system($cmd);
	if ($ret) {
		print("$cmd failed with exit code $ret\n");
		exit($ret);
	}
}

$tm = tmpnam();
$tmpname = "/tmp/http/" . basename($tm);
$tmpurl = "/tmp/" . basename($tm);
$psname = $tmpname . "_plain_label.ps";

# Get strings
$label = "";
$preview = 0;
$font = "WindsorDemi";
foreach my $key (param())
{
	$val = param($key);
	if ($key =~ "label")
	{
		$label = $val;
	}
	elsif ($key =~ "action")
	{
		if ($val =~ "Preview") {
			$preview = 1;
		}
	}
	elsif ($key =~ "font")
	{
		$font = $val;
	}
	elsif ($key =~ "bold")
	{
		if ($val =~ "on") {
			$font = $font . "-Bold"
		}
	}
	elsif ($key =~ "italic")
	{
		if ($val =~ "on") {
			$font = $font . "-Italic"
		}
	}
}

open(outf, ">$psname");
print outf "%!PS-Adobe-2.0\n";
print outf "%%BoundingBox: 0 0 512 64\n";
print outf "/labelwidth ($label) stringwidth def\n";
print outf "%%BoundingBox: 0 0 labelwidth 64\n";
print outf "/$font findfont 50 scalefont setfont\n";
print outf "/labelwidth ($label) stringwidth pop def\n";
print outf "256 20 moveto ($label) labelwidth 2 div neg 0 rmoveto show\n";
print outf "showpage\n";
close(outf);

if ($preview) {
	print "<body bgcolor=\"#555\">";
	$pngname = $tmpname . "_plain_label.png";
	$pngurl = $tmpurl . "_plain_label.png";
	&runOrDie("/usr/bin/magick $psname -background white -splice 0x1 -background black -splice 0x1 -trim +repage -chop 0x1 $pngname");
	print "<img src=\"$pngurl\"/>";
	print "</body>";
} else {
	foreach my $p (param()) {
		print "$p = " . param($p) . "<br>\n";
	}
	print "creating $psname<BR>\n";
	print "printing $psname<BR>\n";
	system("/usr/local/bin/peggps $psname");
	unlink($psname);
	print "<BR><BR>all done!<BR>\n";
}
print end_html;
unlink($psname);
