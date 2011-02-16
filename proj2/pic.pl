#
 #	Copyright (C) 2011  Kiel Friedt
 #
 #    This program is free software: you can redistribute it and/or modify
 #    it under the terms of the GNU General Public License as published by
 #    the Free Software Foundation, either version 3 of the License, or
 #    (at your option) any later version.
 #
 #    This program is distributed in the hope that it will be useful,
 #    but WITHOUT ANY WARRANTY; without even the implied warranty of
 #    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #    GNU General Public License for more details.
 #
 #    You should have received a copy of the GNU General Public License
 #    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#! /usr/bin/perl -w
use strict;
use Image::Magick;

sub prints;
sub searchxml;
sub searchjpg;
sub modifyImage;
sub change;

my(@tag, @tagvar, @files, @jpg, @array, @array2, @imagexml,@temparray, 
$strippedxml, $defaultcontent, $b, $k, $xml, $temp, $breakout, $size, $i, 
$ans, $flop, $flip, $width,$annotate, $AnnotateText, $category, $raise, 
$shear, $shearX, $shearY, $shearFill, $sharpen, $xmlflag, $imageflag, 
$saveflag);

#Hard coded tag values.

$flop = "no";
$flip = "no";
$width = 200;
$annotate = "no";
$AnnotateText = "text goes here";
$category = "modified";    
$raise = 0;
$shear = "no";
$shearX = 10;
$shearY = 10;
$shearFill = "black";
$sharpen = "no";


#Opens current directory assigns directory listings to @files

opendir(DIR, ".");
@files = readdir(DIR);
closedir(DIR);

@tag = ("flop", "flip", "width", "annotate", "AnnotateText", "category", 
"raise", "shear", "shearX", "shearY", "shearFill", "sharpen");
@tagvar = ("$flop", "$flip", "$width", "$annotate", "$AnnotateText", 
"$category", "$raise", "$shear", "$shearX", "$shearY", "$shearFill", 
"$sharpen");

sub prints {   
my(@temparray) = @_;
print "your defaults have been set up this way:\n";
print "flop is set to $temparray[0]\n";
print "flip is set to $temparray[1]\n";
print "width is set to $temparray[2] pixels\n";
print "annotate is set to $temparray[3] with $temparray[4]\n";
print "category is set to $temparray[5]\n";
print "raise is set to $temparray[6]\n";
print "shear is set to $temparray[7] with X=$temparray[8] and Y=$temparray[9] and fill=$temparray[10]\n";
print "sharpen is set to $temparray[11]\n";
}

sub searchxml {
my $some = shift;
    my(@xml, $i, $temp);
$temp = 0;
foreach (@files)
{
if($_ =~ m/\.xml$/)
{
push(@xml,"$_");
} 
}
   foreach (@xml)
    {    
     if( ($some . "\.xml" ) eq $_ )
     {           
     $temp = 1;
           }
           }
          return $temp;           
}

#function searches through the current directory and then find all files ending in .jpg
#then removes the .jpg on the end of each and return the array.

sub searchjpg {
my(@grepjpg,$temp, $flag);
$temp = 0;
$flag = 0;
foreach (@files)
{
if($_ =~ m/\.jpg$/)
{
 push(@grepjpg, "$_");
 $flag = 1;
}
}
if ($flag == 1)
{
foreach (@grepjpg)
{
$_ =~ s/.jpg//g;
}
return @grepjpg;
}
else 
{
 return 0;
}
}

sub modifyImage {
my $some = shift;
my $image = new Image::Magick;
$image->Read("$some\.jpg");
if ($imageflag == 1){
if ($imagexml[0] eq "yes") {
$image->Flop;
}
if ($imagexml[1] =~ m/yes/) {
$image->Flip;
}
my $w = $image->Get('columns');
my $h = $image->Get('rows');
$image->Scale(width=>$imagexml[2], height=>$h#$imagexml[2]/$w);
if ($imagexml[3] =~ m/yes/) {
$image->Annotate(font=>'Arial-Bold',
pointsize=> 24,
fill=>'white',
gravity=>'SouthEast',
geometry=>"$image->Get('columns')"."x"."$image->Get('rows')",
text=>$imagexml[4]);
}
if ($imagexml[6] != 0) {
$image->Raise(geometry=>"$imagexml[6]"."x"."$imagexml[6]");
}
if ($imagexml[7] =~ m/yes/) {
$image->Shear(x=>$imagexml[8], y=>$imagexml[9], fill=>$imagexml[10]);
}
if ($imagexml[11] =~ m/yes/) {
$image->Sharpen(radius=>0.5, sigma=>0.5);
}
$imagexml[5] =~ s/\n//;
mkdir("$imagexml[5]", 0770);
chdir("$imagexml[5]");
$image->Write("$some\.jpg");
chdir("..");
}
elsif (($xmlflag == 1)||($saveflag == 1)) {
if ($array[0] eq "yes") {
$image->Flop;
}
if ($array[1] =~ m/yes/) {
$image->Flip;
}
my $w = $image->Get('columns');
my $h = $image->Get('rows');
$image->Scale(width=>$array[2], height=>$h#$array[2]/$w);
if ($array[3] =~ m/yes/) {
$image->Annotate(font=>'Arial-Bold',
pointsize=> 24,
fill=>'white',
gravity=>'SouthEast',
geometry=>"$image->Get('columns')"."x"."$image->Get('rows')",
text=>$array[4]);
}
if ($array[6] != 0) {
$image->Raise(geometry=>"$array[6]"."x"."$array[6]");
}
if ($array[7] =~ m/yes/) {
$image->Shear(x=>$array[8], y=>$array[9], fill=>$array[10]);
}
if ($array[11] =~ m/yes/) {
$image->Sharpen(radius=>0.5, sigma=>0.5);
}
mkdir("$array[5]", 0770);
chdir("$array[5]");
$image->Write("$some\.jpg");
chdir("..");
}
else
{
if ($flop =~ m/yes/) {
$image->Flop;
}
if ($flip =~ m/yes/) {
$image->Flip;
}
my $w = $image->Get('columns');
my $h = $image->Get('rows');
$image->Scale(width=>$width, height=>$h#$width/$w);
if ($annotate =~ m/yes/) {
$image->Annotate(font=>'Arial-Bold',
pointsize=> 24,
fill=>'white',
gravity=>'SouthEast',
geometry=>"$image->Get('columns')"."x"."$image->Get('rows')",
text=>$AnnotateText);
}
if ($raise != 0) {
$image->Raise(geometry=>"$raise"."x"."$raise");
}
if ($shear =~ m/yes/) {
$image->Shear(x=>$shearX, y=>$shearY, fill=>$shearFill);
}
if ($sharpen =~ m/yes/) {
$image->Sharpen(radius=>0.5, sigma=>0.5);
}
 mkdir("$array[5]", 0770);
chdir("$array[5]");
$image->Write("$some\.jpg");
chdir("..");
}
} #end sub modifyImage

sub change {
$ans = 0;
my(@temparray);
print "flip is set to $flip. 'yes' to flip, 'no' to not flip\n";
chomp ($ans = <>);
push(@temparray, "$ans");
print "flop is set to $flop. 'yes' to flop, 'no' to not flop\n";
chomp ($ans = <>);
push(@temparray, "$ans");
print "width is set to $width. 0 means do not change the original\n
width of the image. Type in new width or just press ENTER to accept 
200\n";
chomp ($ans = <>);
if ($ans == 0){
push(@temparray, "200");
}
else {
push(@temparray, "$ans");
}
print "annotate is set to $annotate with text: $AnnotateText\n
'yes' to annotate with text, 'no' to not annotate.\n";
chomp ($ans = <>);
push(@temparray, "$ans");
print "annotate is set to $annotate with text: $AnnotateText \n
Change the text? (Type in new text or just press ENTER to accept $AnnotateText.)\n";
chomp ($ans = <> );
if ($ans ne "" ){
push(@temparray, "$ans");
}

print "category is set to $category\n
(Type in new category or just press ENTER to accept modified.\n";
chomp ($ans = <>);
if ($ans eq "") {
push(@temparray,"modified");
}
else {
push(@temparray, "$ans");
}

print "raise is set to $raise. 0 means do not raise.\n
Type in new amount or just press ENTER to accept 0.\n";
chomp ($ans = <> );
if ($ans == 0) {
push(@temparray, "0");
}
else {
push(@temparray, "$ans");

}

print "shear is set to $shear 'yes' to shear and 'no' to not shear.\n";
chomp ($ans = <>);
push(@temparray, "$ans");

print "shearX is set to $shearX. Type in new shearX or just press ENTER to 
accept 10.\n";
chomp ($ans = <>);
if ($ans eq "") {
push(@temparray, "10");
}
else {
push(@temparray, "$ans");
}

print "shearY is set to $shearY. Type in new shearY or just press ENTER to 
accept 10.\n";
chomp ($ans = <>);
if ($ans eq "") {
push(@temparray, "10");
}
else {
push(@temparray, "$ans");
}

print "shearFill is set to $shearFill. A sample of your choices:\n
white, tan, orange, gold, yellow, beige, green, cyan, blue, purple, pink, 
red.\n
What color would you like? Or just press ENTER to accept black.\n";
chomp ($ans = <>);
if ($ans eq "") {
push(@temparray, "black");
}
else {
push(@temparray, "$ans");
}

print "sharpen is set to $sharpen. 'yes' to sharpen, 'no' to not sharpen. \n";
chomp ($ans = <>);
push(@temparray, "$ans");

prints(@temparray);
return @temparray;
}


$xml = 0;
$saveflag = 0;
$xmlflag = 0;
$imageflag = 0;
foreach $i (@files)
{
 if ($i eq "default.xml")
 {
  $xml = 1;
 }
}
if ($xml == 1)
{

	$xmlflag = 1;
	open(DEFAULT, "default.xml");
	while(<DEFAULT>)
	{
	push(@array, "$_");
	}
	close(DEFAULT);
    
	foreach (@array)
	{
	$_ =~ s/<[^>]#>//g;
	chomp($_);
	}
	prints(@array);
	print "would you like to change these defaults? (y or n)\n";
	chomp( $ans = <>);
	if ($ans eq "y")
	{
	@temparray = change(@array);
	print " do you want to set these to the default (y or n)\n";
	chomp( $ans = <>);
	if ($ans eq "y")
	{
	$saveflag = 1;
	@array = @temparray;
	
 open(MYFILE, ">default.xml");
 $k = 0;
 for($k = 0; $k < 12; ++$k) 
 {
print MYFILE "\<$tag[$k]\>$array[$k]\<\/$tag[$k]\>\n";
 }
 close (MYFILE);
}
}
}
else
{
print "I didn't find a default.xml file. I will ask a series of questions
to set up the defaults for this execution. Then I will give you an
opportunity to save them into a default.xml file.\n\n";
@temparray = change;
print " do you want to set these to the default (y or n)\n";
chomp( $ans = <>);
if ($ans eq "y")
{
$saveflag = 1;
@array = @temparray;
 open(MYFILE, ">default.xml");
 $k = 0;
 for($k = 0; $k < 12; ++$k) 
 {
print MYFILE "\<$tag[$k]\>$array[$k]\<\/$tag[$k]\>\n";
 }
 close (MYFILE);
}
else
{
push(@array, "no" );
push(@array, "no");
push(@array, "200");
push(@array, "no");
push(@array, "text goes here");
push(@array, "modified");
push(@array, "0");
push(@array, "no");
push(@array, "10");
push(@array, "10");
push(@array, "black");
push(@array, "no");
}
}

@jpg = searchjpg;
$size = @jpg;
if($size > 0){
 foreach (@jpg)
{
$_ =~ s/.jpg//gs;
chomp($_);
}
$temp = 0;
foreach  $i (@jpg)
{
my $pass = $i;
$temp = searchxml($i);
if ($temp == 1)
{
open(DEFAULT, "$i.xml");
foreach (@array)
{
push(@imagexml, "$_");
}
my @temps = <DEFAULT>;
close(DEFAULT);

my ($tempflop, $tempflip, $tempwidth,$tempannotate, $tempAnnotateText, $tempcategory, $tempraise, 
$tempshear, $tempshearX, $tempshearY, $tempshearFill, $tempsharpen);

$tempflop = $imagexml[0];
$tempflip = $imagexml[1];
$tempwidth = $imagexml[2];
$tempannotate = $imagexml[3];
$tempAnnotateText = $imagexml[4];
$tempcategory = $imagexml[5];
$tempraise = $imagexml[6];
$tempshear = $imagexml[7];
$tempshearX = $imagexml[8];
$tempshearY = $imagexml[9];
$tempshearFill = $imagexml[10];
$tempsharpen = $imagexml[11];

foreach (@temps)
{
if ($_ =~ m/<flip>(.#)<\/flip>/)
{
$tempflip = $1;
}

if ($_ =~ m/<flop>(.#)<\/flop>/)
{
$tempflop = $1;
}

if ($_ =~ m/<width>(.#)<\/width>/)
{
$tempwidth = $1;
}

if ($_ =~ m/<annotate>(.#)<\/annotate>/)
{
$tempannotate = $1;
}

if ($_ =~ m/<AnnotateText>(.#)<\/AnnotateText>/)
{
$tempAnnotateText = $1;
}

if ($_ =~ m/<category>(.#)<\/category>/)
{
$tempcategory = $1;
}

if ($_ =~ m/<raise>(.#)<\/raise>/)
{
$tempraise = $1;
}

if ($_ =~ m/<shear>(.#)<\/shear>/)
{
$tempshear = $1;
}
if ($_ =~ m/<shearX>(.#)<\/shearX>/)
{
$tempshearX = $1;
}
if ($_ =~ m/<shearY>(.#)<\/shearY>/)
{
$tempshearY = $1;
}
if ($_ =~ m/<shearFill>(.#)<\/shearFill>/)
{
$tempshearFill = $1;
}
if ($_ =~ m/<sharpen>(.#)<\/sharpen>/)
{
$tempsharpen = $1;
}
}

$imagexml[0] = $tempflop;
$imagexml[1] = $tempflip;
$imagexml[2] = $tempwidth;
$imagexml[3] = $tempannotate; 
$imagexml[4] = $tempAnnotateText;
$imagexml[5] = $tempcategory;
$imagexml[6] = $tempraise;
$imagexml[7] = $tempshear; 
$imagexml[8] = $tempshearX;
$imagexml[9] = $tempshearY; 
$imagexml[10] = $tempshearFill;
$imagexml[11] = $tempsharpen ;

foreach (@imagexml)
{
$_ =~ s/<[^>]#>//g;
}
$imageflag = 1;
}

modifyImage($i);
$imageflag = 0;
}
}