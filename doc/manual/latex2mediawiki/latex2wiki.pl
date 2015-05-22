#!/usr/bin/perl
#
# translate LaTeX to wiki
# written and maintained by User:Jmath666
# with code contributions from  User:Oleg Alexandrov
# archived at User:Jmath666/latex2wiki.pl
#
# February 2007
#
# usage: 
# 1. edit $standalone=1
#
# ./latex2wiki.pl file.tex > file.wiki
#        cat file.tex file.bbl | ./latex2wiki.pl - > file.wiki
#
# 2. as cgi script
 
use strict;                   # 'strict' insists that all variables be declared
use diagnostics;              # 'diagnostics' expands the cryptic warnings
 
# parameters
my $standalone=1;           # 0 to run as cgi, 1 from command line
my $png_inline=0;           # 0 render inline math as PNG:  no, 1 \, 2 \,\!
my $png_display=0;          # 0 render display math as PNG: no, 1 \, 2 \,\!
 
if (!$standalone) {
  require "cgi-lib.pl";	      # can take this out if $standalone=1 below
}
 
undef $/; # undefines the separator. Can read one whole file in one scalar.
 
MAIN: {
  my (%input, $file, $ltext);
 
  if($standalone){
 
    $file = $ARGV[0]; # the command line argument
 
    # read the data from $file into $text
    open (FILE, "<$file"); $ltext = <FILE>; close (FILE);
 
 } else {
 
    # Read in all the variables set by the form
    &ReadParse(\%input);
    $ltext=$input{'ltext'};
 }
 
 # Print the header
 print "Content-type: text/html\n\n";
 
 # process the text  
 $ltext = &parse_latex ($ltext);
 
 # print the processed text in a Wikipedia textbox
 if (!$standalone) {&print_head()};
 
 print "$ltext\n";
 
 if (!$standalone) {&print_foot()};
}
 
 
sub parse_latex{
 
  my $text = shift;
  my ($ms, $me_i, $me_d);
 
  $text =~ s/%.*\n//g; # get rid of comments
 
  $text =~ s/\r//g; # get rid of carriage returns
 
  # emphasize an empty line with more returns
  $text =~ s/\n[\t ]*\n/\n\n\n/g;
 
  # rm otherwise newlines
  $text =~ s/[ \t]*\n[ \t]*([^\n])/ $1/g;
 
  # get rid of artefact space
  $text =~ s/\\ \s*/ /sg; # get rid of explicit space
  $text =~ s/~/ /g; # get rid of unbreakable space
 
  # strip the preamble
  $text =~ s/^.*?\\begin\{document\}//sig;
  $text =~ s/^.*?\\maketitle//sig;
 
  # strip end document but leave after 
  $text =~ s/\\end\{document\}//sig;
 
  # strip abstract
  $text =~ s/\\begin\{abstract\}(.*?)\\end\{abstract\}/\n$1\n\n/sg;
 
  # fix some missing TeX macros
  $text =~ s/\\widetilde/\\tilde/sg;
  $text =~ s/\\operatorname\*\{(\w*?)\}/\\mathrm\{$1\}/sg;
  $text =~ s/\\allowbreak//sg;
  # $text =~ s/\\right([^\w])/$1/sg;
  # $text =~ s/\\left([^\w])/$1/sg;
  $text =~ s/\\-//sg;
  $text =~ s/\\_/_/sg;
  $text =~ s/\\textquotedblleft/\&ldquo\;/gs;
  #$text =~ s/\\textquotedblright/\&rdquo\;/gs;
  $text =~ s/\\begin\{center\}(.*?)\\end\{center\}/$1/sg;
 
  # math tags
  $ms='<math>'; 
  if($png_inline==0) {$me_i='</math>';}
  if($png_inline==1) {$me_i='\\,</math>';}
  if($png_inline==2) {$me_i='\\,\\!</math>';}
  if($png_display==0) {$me_d='</math>';}
  if($png_display==1) {$me_d='\\,</math>';}
  if($png_display==2) {$me_d='\\,\\!</math>';}
  $text =~ s/\s*\$\$\s*(.*?)\s*\$\$\s*/\n\n:$ms$1$me_d\n\n/sg;
  $text =~ s/\$(.*?)\$/$ms\\textstyle $1$me_i/g;
  $text =~ s/\\begin\{equation\}(.*?)\\end\{equation\}/\n\n:$ms$1$me_d\n\n/sg;
  $text =~ s/\\begin\{equation\*\}(.*?)\\end\{equation\*\}/\n\n:$ms$1$me_d\n\n/sg;
  $text =~ s/\\\[(.*?)\\\]/\n\n:$ms$1$me_d\n\n/sg;
  $text =~ s/\\begin\{align\}(.*?)\\end\{align\}/\n\n:$ms\\begin\{align\}$1\\end\{align\}$me_d\n\n/sg;
  $text =~ s/\\begin\{align\*\}(.*?)\\end\{align\*\}/\n\n:$ms\\begin\{align\}$1\\end\{align\}$me_d\n\n/sg;
 
  # get rid of all labels and references to them
  $text =~ s/\\label\{.*?\}//g;
  $text =~ s/\\ref\{.*?\}//g;
 
  # get rid of all tex definitions
  $text =~ s/\\def\\.*?\{.*?\}//g;
 
  # convert sections and subsections
  $text =~ s/\s*\\section.*?\{(.*?)\}\s*/\n\n==$1==\n\n/sig;
  $text =~ s/\s*\\subsection.*?\{(.*?)\}\s*/\n\n===$1===\n\n/sig;
 
  # parse bibliography into %bib with key=label value=entry
  my ($btext, %bib, $mark, @bibarray);
  $btext = $text;
  $btext =~ s/\s*\\newblock\s*/ /g; # odd bibtex command
  $btext =~ s/\s*\\end\{thebibliography\}.*$//s; # strip all after bib entries
  $mark="__bib__";
  $btext =~ s/\\bibitem\{(.*?)\}/$mark$1$mark/g; # bibitem -> mark
  if ($btext =~ /$mark/) {     
    $btext =~ s/^.*?$mark//s; # strip all before bib entries
    $btext =~ s/\{(\w)\}/$1/sg;
    $btext = &convert_font ($btext);
    #$btext =~ s/\{(.*?)\}/$1/sg;  # get rid of {}
    %bib = split($mark,$btext); 
  } else {
    %bib = ();
  }
 
  $text = &convert_font ($text);
 
 
  # replace the bibliography section
  $text =~ s/\s*\\begin\{thebibliography\}.*?\\end\{thebibliography\}/\n\n==References==\n\n<references\/>\n/s;
 
  # preprocess references make by alternatives to \cite
  $text =~ s/\\citet\{/\\cite\{/sg;
  $text =~ s/\\citep\{/\\cite\{/sg;
 
  # get rid of optional arguments to \cite
  $text =~ s/\\cite\[.*?\]/\\cite/sg;
  $text =~ s/\\citep\[.*?\]/\\cite/sg;
  $text =~ s/\\citet\[.*?\]/\\cite/sg;
 
  # split \cite{a,b,..} into separate \cite
  my $e;
  do {
    $e= ($text =~ s/\\cite\{([^\}]*?),(.*?)\}/\\cite\{$1\}\\cite\{$2\}/s);
  } while ($e);  
 
  # add references per [[Wikipedia:Footnote]]
  # replace \cite{foo} by <ref name="foo">entry</ref> or <ref name="foo"/> 
  my ($bibkey);
  # replace first occurence by full entry
  # and remaining occurences by terminated tag
  foreach $bibkey (keys %bib) {
    $text =~ s/\\cite\{$bibkey\}/<ref name="$bibkey">$bib{$bibkey}<\/ref>/s;
    $text =~ s/\\cite\{$bibkey\}/<ref name="$bibkey"\/>/sg;
  }
 
  # get rid of extra bibliography related commands
  $text =~ s/\\bibliographystyle\{.*?\}//;
  $text =~ s/\\bibliography\{.*?\}//;
 
  # get rid of all float code
  $text =~ s/\\begin\{figure\}.*?\\end\{figure\}/::FIGURE DELETED/sg;
  $text =~ s/\\begin\{table\}.*?\\end\{table\}/::TABLE DELETED/sg;
  $text =~ s/\\begin\{tabular\}.*?\\end\{tabular\}/::TABLE DELETED/sg;
 
  # list environments - nested not supported yet
  do {
    $text =~ s/\\begin\{enumerate\}(.*?)\\item(.*?)\\end\{enumerate\}/
    \\begin\{enumerate\}$1\#$2\\end\{enumerate\}/sg;
          $e=$2;
  } while (defined($e));
  $text =~ s/\\begin\{enumerate\}//sg;
  $text =~ s/\\end\{enumerate\}//sg;
  do {
    $text =~ s/\\begin\{itemize\}(.*?)\\item(.*?)\\end\{itemize\}}/\\begin\{itemize\}$1\*$2\\end\{itemize\}/sg;
          $e=$2;
  } while (defined($e));
  $text =~ s/\\begin\{itemize\}//sg;
  $text =~ s/\\end\{itemize\}//sg;
 
  # strip extra newlines and rm space at the beginning and end (this better be the last thing in the code)
  $text =~ s/^\s*(.*?)\s*$/$1/sg;
  $text =~ s/[ \t]*\n[ \t]*\n\s*/\n\n/g;
  return $text;
}
 
sub convert_font{
 
  my $text = shift;
 
  # convert bold and italic and sc
  # there should be nested matching here
  $text =~ s/\{\s*\\bf\s*(.*?)\s*\}/'''$1'''/sg;
  $text =~ s/\{\s*\\sc\s*(.*?)\s*\}/$1/sg;
  $text =~ s/\{\s*\\it\s*(.*?)\s*\}/''$1''/sg;
  $text =~ s/\{\s*\\em\s*(.*?)\s*\}/''$1''/sg;
  $text =~ s/\\emph\s*\{(.*?)\}/''$1''/sg;
 
  return $text;
}
 
# Oleg's routines to deal with the web form
 
sub print_head {
    print '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" dir="ltr" lang="en"><head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<head>
<title>The Wikicode result</title>
</head>
 
<body>
 
<div id="bodyContent">
<div id="contentSub"></div>
 
<div id="wikiPreview"></div><script type="text/javascript">
/*<![CDATA[*/
document.writeln("<div id=\'toolbar\'>");
document.writeln("</div>");
/*]]>*/
</script>
 
<form id="editform" name="editform" method="post" action="http://en.wikipedia.org/w/index.php?title= ... not really, just bootstrapping Wikipedia\'s preview function &amp;action=submit" enctype="multipart/form-data">
 
<center>
<textarea tabindex="1" accesskey="," name="wpTextbox1" rows="25" cols="80">';
}
 
sub print_foot {
  print '</textarea> 
 
<br>
 
<input tabindex="6" id="wpPreview" value="Show preview" name="wpPreview" accesskey="p" title="Preview your changes, please use this before saving! [alt-p]" type="submit"> (this will bootstrap the Wikipedia preview function)
 
<p>
 
';
 
}
1;


