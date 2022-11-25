%%
%% This is file `achemso.bst',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% achemso.dtx  (with options: `bib')
%%  ----------------------------------------------------------------
%%  The achemso package - LaTeX and BibTeX support for American
%%  Chemical Society publications
%%  Maintained by Joseph Wright
%%  E-mail: joseph.wright@morningstar2.co.uk
%%  Released under the LaTeX Project Public License v1.3 or later
%%  See http://www.latex-project.org/lppl.txt
%%  ----------------------------------------------------------------
%% 
ENTRY
  { address
    author
    booktitle
    chapter
    doi
    edition
    editor
    howpublished
    institution
    journal
    note
    number
    organization
    pages
    publisher
    school
    series
    title
    type
    url
    volume
    year
  }
  {}
  {
  label
  }

INTEGERS { output.state before.all mid.sentence after.sentence }
INTEGERS { after.block after.item author.or.editor }
INTEGERS { separate.by.semicolon }

FUNCTION {init.state.consts}
{ #0 'before.all :=
  #1 'mid.sentence :=
  #2 'after.sentence :=
  #3 'after.block :=
  #4 'after.item :=
}

FUNCTION {add.comma}
{ ", " * }

FUNCTION {add.semicolon}
{ "; " * }


FUNCTION {add.comma.or.semicolon}
{ #1 separate.by.semicolon =
    'add.semicolon
    'add.comma
  if$
}

FUNCTION {add.colon}
{ ": " * }

STRINGS { s t }

FUNCTION {output.nonnull}
{ 's :=
  output.state mid.sentence =
    { add.comma write$ }
    { output.state after.block =
      { add.semicolon write$
        newline$
        "\newblock " write$
      }
      { output.state before.all =
          'write$
          { output.state after.item =
            { " " * write$ }
            { add.period$ " " * write$ }
          if$
          }
        if$
        }
      if$
      mid.sentence 'output.state :=
    }
  if$
  s
}

FUNCTION {output}
{ duplicate$ empty$
    'pop$
    'output.nonnull
  if$
}

FUNCTION {output.check}
{ 't :=
  duplicate$ empty$
    { pop$ "Empty " t * " in " * cite$ * warning$ }
    'output.nonnull
  if$
}

FUNCTION {output.bibitem}
{ newline$
  "\bibitem{" write$
  cite$ write$
  "}" write$
  newline$
  ""
  before.all 'output.state :=
}

FUNCTION {new.block}
{ output.state before.all =
    'skip$
    { after.block 'output.state := }
  if$
}

FUNCTION {new.sentence}
{ output.state after.block =
    'skip$
    { output.state before.all =
        'skip$
        { after.sentence 'output.state := }
      if$
    }
  if$
}

FUNCTION {fin.entry}
{ add.period$
  write$
  newline$
}
FUNCTION {not}
{   { #0 }
    { #1 }
  if$
}

FUNCTION {and}
{   'skip$
    { pop$ #0 }
  if$
}

FUNCTION {or}
{   { pop$ #1 }
    'skip$
  if$
}

FUNCTION {field.or.null}
{ duplicate$ empty$
    { pop$ "" }
    'skip$
  if$
}

FUNCTION {emphasize}
{ duplicate$ empty$
    { pop$ "" }
    { "\emph{" swap$ * "}" * }
  if$
}

FUNCTION {boldface}
{ duplicate$ empty$
    { pop$ "" }
    { "\textbf{" swap$ * "}" * }
  if$
}

FUNCTION {paren}
{ duplicate$ empty$
    { pop$ "" }
    { "(" swap$ * ")" * }
  if$
}

FUNCTION {bbl.and}
{ "and" }

FUNCTION {bbl.chapter}
{ "Chapter" }

FUNCTION {bbl.editor}
{ "Ed." }

FUNCTION {bbl.editors}
{ "Eds." }

FUNCTION {bbl.edition}
{ "ed." }

FUNCTION {bbl.etal}
{ "et~al." }

FUNCTION {bbl.in}
{ "In" }

FUNCTION {bbl.inpress}
{ "in press" }

FUNCTION {bbl.msc}
{ "M.Sc.\ thesis" }

FUNCTION {bbl.page}
{ "p" }

FUNCTION {bbl.pages}
{ "pp" }

FUNCTION {bbl.phd}
{ "Ph.D.\ thesis" }

FUNCTION {bbl.submitted}
{ "submitted for publication" }

FUNCTION {bbl.techreport}
{ "Technical Report" }

FUNCTION {bbl.version}
{ "version" }

FUNCTION {bbl.volume}
{ "Vol." }

FUNCTION {bbl.first}
{ "1st" }

FUNCTION {bbl.second}
{ "2nd" }

FUNCTION {bbl.third}
{ "3rd" }

FUNCTION {bbl.fourth}
{ "4th" }

FUNCTION {bbl.fifth}
{ "5th" }

FUNCTION {bbl.st}
{ "st" }

FUNCTION {bbl.nd}
{ "nd" }

FUNCTION {bbl.rd}
{ "rd" }

FUNCTION {bbl.th}
{ "th" }

FUNCTION {eng.ord}
{ duplicate$ "1" swap$ *
  #-2 #1 substring$ "1" =
     { bbl.th * }
     { duplicate$ #-1 #1 substring$
       duplicate$ "1" =
         { pop$ bbl.st * }
         { duplicate$ "2" =
             { pop$ bbl.nd * }
             { "3" =
                 { bbl.rd * }
                 { bbl.th * }
               if$
             }
           if$
          }
       if$
     }
   if$
}

FUNCTION{is.a.digit}
{ duplicate$ "" =
    {pop$ #0}
    {chr.to.int$ #48 - duplicate$
     #0 < swap$ #9 > or not}
  if$
}

FUNCTION{is.a.number}
{
  { duplicate$ #1 #1 substring$ is.a.digit }
    {#2 global.max$ substring$}
  while$
  "" =
}

FUNCTION {extract.num}
{ duplicate$ 't :=
  "" 's :=
  { t empty$ not }
  { t #1 #1 substring$
    t #2 global.max$ substring$ 't :=
    duplicate$ is.a.number
      { s swap$ * 's := }
      { pop$ "" 't := }
    if$
  }
  while$
  s empty$
    'skip$
    { pop$ s }
  if$
}

FUNCTION {bibinfo.check}
{ swap$
  duplicate$ missing$
    { pop$ pop$
      ""
    }
    { duplicate$ empty$
        {
          swap$ pop$
        }
        { swap$
          pop$
        }
      if$
    }
  if$
}

FUNCTION {convert.edition}
{ extract.num "l" change.case$ 's :=
  s "first" = s "1" = or
    { bbl.first 't := }
    { s "second" = s "2" = or
        { bbl.second 't := }
        { s "third" = s "3" = or
            { bbl.third 't := }
            { s "fourth" = s "4" = or
                { bbl.fourth 't := }
                { s "fifth" = s "5" = or
                    { bbl.fifth 't := }
                    { s #1 #1 substring$ is.a.number
                        { s eng.ord 't := }
                        { edition 't := }
                      if$
                    }
                  if$
                }
              if$
            }
          if$
        }
      if$
    }
  if$
  t
}

FUNCTION {tie.or.space.connect}
{ duplicate$ text.length$ #3 <
    { "~" }
    { " " }
  if$
  swap$ * *
}

FUNCTION {space.connect}
{ " " swap$ * * }

INTEGERS { nameptr namesleft numnames }

FUNCTION {format.names}
{ 's :=
  #1 'nameptr :=
  s num.names$ 'numnames :=
  numnames 'namesleft :=
  numnames #15 >
    { s #1 "{vv~}{ll,}{~f.}{,~jj}" format.name$ 't :=
      t bbl.etal space.connect
    }
    {
       { namesleft #0 > }
       { s nameptr "{vv~}{ll,}{~f.}{,~jj}" format.name$ 't :=
           nameptr #1 >
             { namesleft #1 >
               { add.comma.or.semicolon t * }
               { numnames #2 >
                 { "" * }
                 'skip$
               if$
               t "others," =
                 { bbl.etal space.connect }
                 { add.comma.or.semicolon t * }
               if$
               }
             if$
             }
           't
         if$
         nameptr #1 + 'nameptr :=
         namesleft #1 - 'namesleft :=
         }
     while$
  }
  if$
}

FUNCTION {format.authors}
{ author empty$
    { "" }
    { #1 'author.or.editor :=
      #1 'separate.by.semicolon :=
      author format.names
    }
  if$
}

FUNCTION {format.editors}
{ editor empty$
    { "" }
    { #2 'author.or.editor :=
      #0 'separate.by.semicolon :=
      editor format.names
      add.comma
      editor num.names$ #1 >
        { bbl.editors }
        { bbl.editor }
      if$
      *
    }
  if$
}

FUNCTION {n.separate.multi}
{ 't :=
  ""
  #0 'numnames :=
  t text.length$ #4 > t is.a.number and
    {
      { t empty$ not }
      { t #-1 #1 substring$ is.a.number
          { numnames #1 + 'numnames := }
          { #0 'numnames := }
        if$
        t #-1 #1 substring$ swap$ *
        t #-2 global.max$ substring$ 't :=
        numnames #4 =
          { duplicate$ #1 #1 substring$ swap$
            #2 global.max$ substring$
            "," swap$ * *
            #1 'numnames :=
          }
          'skip$
        if$
      }
      while$
    }
    { t swap$ * }
  if$
}

FUNCTION {format.bvolume}
{ volume empty$
    { "" }
    { bbl.volume volume tie.or.space.connect }
  if$
}

FUNCTION {format.title.noemph}
{ 't :=
  t empty$
    { "" }
    { t }
  if$
}

FUNCTION {format.title}
{ 't :=
  t empty$
    { "" }
    { t emphasize }
  if$
}

FUNCTION {format.number.series}
{ volume empty$
    { number empty$
       { series field.or.null }
       { series empty$
         { "There is a number but no series in " cite$ * warning$ }
         { series number space.connect }
       if$
       }
      if$
    }
    { "" }
  if$
}

FUNCTION {format.url}
{ url empty$
    { "" }
    { new.sentence "\url{" url * "}" * }
  if$
}

FUNCTION {n.dashify}
{ 't :=
  ""
    { t empty$ not }
    { t #1 #1 substring$ "-" =
    { t #1 #2 substring$ "--" = not
        { "--" *
          t #2 global.max$ substring$ 't :=
        }
        {   { t #1 #1 substring$ "-" = }
        { "-" *
          t #2 global.max$ substring$ 't :=
        }
          while$
        }
      if$
    }
    { t #1 #1 substring$ *
      t #2 global.max$ substring$ 't :=
    }
      if$
    }
  while$
}

FUNCTION {format.date}
{ year empty$
    { "" }
    { year boldface }
  if$
}

FUNCTION {format.bdate}
{ year empty$
    { "There's no year in " cite$ * warning$ }
    'year
  if$
}

FUNCTION {either.or.check}
{ empty$
    'pop$
    { "Can't use both " swap$ * " fields in " * cite$ * warning$ }
  if$
}

FUNCTION {format.edition}
{ edition duplicate$ empty$
    'skip$
    { convert.edition
      bbl.edition bibinfo.check
      " " * bbl.edition *
    }
  if$
}

INTEGERS { multiresult }

FUNCTION {multi.page.check}
{ 't :=
  #0 'multiresult :=
    { multiresult not
      t empty$ not
      and
    }
    { t #1 #1 substring$
      duplicate$ "-" =
      swap$ duplicate$ "," =
      swap$ "+" =
      or or
        { #1 'multiresult := }
        { t #2 global.max$ substring$ 't := }
      if$
    }
  while$
  multiresult
}

FUNCTION {format.pages}
{ pages empty$
    { "" }
    { pages multi.page.check
      { bbl.pages pages n.dashify tie.or.space.connect }
      { bbl.page pages tie.or.space.connect }
    if$
    }
  if$
}

FUNCTION {format.pages.required}
{ pages empty$
    { ""
      "There are no page numbers for " cite$ * warning$
      output
    }
    { pages multi.page.check
      { bbl.pages pages n.dashify tie.or.space.connect }
      { bbl.page pages tie.or.space.connect }
    if$
    }
  if$
}

FUNCTION {format.pages.nopp}
{ pages empty$
    { ""
      "There are no page numbers for " cite$ * warning$
      output
    }
    { pages multi.page.check
      { pages n.dashify space.connect }
      { pages space.connect }
    if$
    }
  if$
}

FUNCTION {format.pages.patent}
{ pages empty$
    { "There is no patent number for " cite$ * warning$ }
    { pages multi.page.check
      { pages n.dashify }
      { pages n.separate.multi }
      if$
    }
  if$
}

FUNCTION {format.vol.pages}
{ volume emphasize field.or.null
  duplicate$ empty$
    { pop$ format.pages.required }
    { add.comma pages n.dashify * }
  if$
}

FUNCTION {format.chapter.pages}
{ chapter empty$
    'format.pages
    { type empty$
    { bbl.chapter }
    { type "l" change.case$ }
      if$
      chapter tie.or.space.connect
      pages empty$
    'skip$
    { add.comma format.pages * }
      if$
    }
  if$
}

FUNCTION {format.title.in}
{ 's :=
  s empty$
    { "" }
    { editor empty$
      { bbl.in s format.title space.connect }
      { bbl.in s format.title space.connect
        add.semicolon format.editors *
      }
    if$
    }
  if$
}

FUNCTION {format.pub.address}
{ publisher empty$
    { "" }
    { address empty$
        { publisher }
        { publisher add.colon address *}
      if$
    }
  if$
}

FUNCTION {format.school.address}
{ school empty$
    { "" }
    { address empty$
        { school }
        { school add.colon address *}
      if$
    }
  if$
}

FUNCTION {format.organization.address}
{ organization empty$
    { "" }
    { address empty$
        { organization }
        { organization add.colon address *}
      if$
    }
  if$
}

FUNCTION {format.version}
{ edition empty$
    { "" }
    { bbl.version edition tie.or.space.connect }
  if$
}

FUNCTION {empty.misc.check}
{ author empty$ title empty$ howpublished empty$
  year empty$ note empty$ url empty$
  and and and and and
    { "all relevant fields are empty in " cite$ * warning$ }
    'skip$
  if$
}

FUNCTION {empty.doi.note}
{ doi empty$ note empty$ and
    { "Need either a note or DOI for " cite$ * warning$ }
    'skip$
  if$
}

FUNCTION {format.thesis.type}
{ type empty$
    'skip$
    { pop$
      type emphasize
    }
  if$
}

FUNCTION {article}
{ output.bibitem
  format.authors "author" output.check
  after.item 'output.state :=
  journal emphasize "journal" output.check
  after.item 'output.state :=
  format.date "year" output.check
  volume empty$
    { ""
      format.pages.nopp output
    }
    { format.vol.pages output }
  if$
  note output
  fin.entry
}

FUNCTION {book}
{ output.bibitem
  author empty$
    { booktitle empty$
        { title format.title "title" output.check }
        { booktitle format.title "booktitle" output.check }
      if$
      format.edition output
      new.block
      editor empty$
        { "Need either an author or editor for " cite$ * warning$ }
        { "" format.editors * "editor" output.check }
      if$
    }
    { format.authors output
      after.item 'output.state :=
      "author and editor" editor either.or.check
      booktitle empty$
        { title format.title "title" output.check }
        { booktitle format.title "booktitle" output.check }
      if$
      format.edition output
    }
  if$
  new.block
  format.number.series output
  new.block
  format.pub.address "publisher" output.check
  format.bdate "year" output.check
  new.block
  format.bvolume output
  pages empty$
    'skip$
    { format.pages output }
  if$
  note output
  fin.entry
}

FUNCTION {booklet}
{ output.bibitem
  format.authors output
  after.item 'output.state :=
  title format.title "title" output.check
  howpublished output
  address output
  format.date output
  note output
  fin.entry
}

FUNCTION {inbook}
{ output.bibitem
  author empty$
    { title format.title "title" output.check
      format.edition output
      new.block
      editor empty$
      { "Need at least an author or an editor for " cite$ * warning$ }
      { "" format.editors * "editor" output.check }
    if$
    }
    { format.authors output
      after.item 'output.state :=
      title format.title.in "title" output.check
      format.edition output
    }
  if$
  new.block
  format.number.series output
  new.block
  format.pub.address "publisher" output.check
  format.bdate "year" output.check
  new.block
  format.bvolume output
  format.chapter.pages "chapter and pages" output.check
  note output
  fin.entry
}

FUNCTION {incollection}
{ output.bibitem
  author empty$
    { booktitle format.title "booktitle" output.check
      format.edition output
      new.block
      editor empty$
        { "Need at least an author or an editor for " cite$ * warning$ }
        { "" format.editors * "editor" output.check }
      if$
    }
    { format.authors output
      after.item 'output.state :=
      title empty$
        'skip$
        { title format.title.noemph output }
      if$
      after.sentence 'output.state :=
      booktitle format.title.in "booktitle" output.check
      format.edition output
    }
  if$
  new.block
  format.number.series output
  new.block
  format.pub.address "publisher" output.check
  format.bdate "year" output.check
  new.block
  format.bvolume output
  format.chapter.pages "chapter and pages" output.check
  note output
  fin.entry
}

FUNCTION {inpress}
{ output.bibitem
  format.authors "author" output.check
  after.item 'output.state :=
  journal emphasize "journal" output.check
  doi empty$
    {  bbl.inpress output }
    {  after.item 'output.state :=
       format.date output
       "DOI:" doi tie.or.space.connect output
    }
  if$
  note output
  fin.entry
}

FUNCTION {inproceedings}
{ output.bibitem
  format.authors "author" output.check
  after.item 'output.state :=
  title empty$
    'skip$
    { title format.title.noemph output
      after.sentence 'output.state :=
    }
  if$
  booktitle format.title output
  address output
  format.bdate "year" output.check
  pages empty$
    'skip$
    { new.block
      format.pages output }
  if$
  note output
  fin.entry
}

FUNCTION {manual}
{ output.bibitem
  format.authors output
  after.item 'output.state :=
  title format.title "title" output.check
  format.version output
  new.block
  format.organization.address output
  format.bdate output
  note output
  fin.entry
}

FUNCTION {mastersthesis}
{ output.bibitem
  format.authors "author" output.check
  after.item 'output.state :=
  bbl.msc format.thesis.type output
  format.school.address "school" output.check
  format.bdate "year" output.check
  note output
  fin.entry
}

FUNCTION {misc}
{ output.bibitem
  format.authors output
  after.item 'output.state :=
  title empty$
    'skip$
    { title format.title output }
  if$
  howpublished output
  year output
  format.url output
  note output
  fin.entry
  empty.misc.check
}

FUNCTION {patent}
{ output.bibitem
  format.authors "author" output.check
  after.item 'output.state :=
  journal "journal" output.check
  after.item 'output.state :=
  format.pages.patent "pages" output.check
  format.bdate "year" output.check
  note output
  fin.entry
}

FUNCTION {phdthesis}
{ output.bibitem
  format.authors "author" output.check
  after.item 'output.state :=
  bbl.phd format.thesis.type output
  format.school.address "school" output.check
  format.bdate "year" output.check
  note output
  fin.entry
}

FUNCTION {proceedings}
{ output.bibitem
  title format.title.noemph "title" output.check
  address output
  format.bdate "year" output.check
  pages empty$
    'skip$
    { new.block
      format.pages output }
  if$
  note output
  fin.entry
}

FUNCTION {remark}
{ output.bibitem
  note "note" output.check
  fin.entry
}

FUNCTION {submitted}
{ output.bibitem
  format.authors "author" output.check
  bbl.submitted output
  fin.entry
}

FUNCTION {techreport}
{ output.bibitem
  format.authors "author" output.check
  after.item 'output.state :=
  title format.title "title" output.check
  new.block
  type empty$
    'bbl.techreport
    'type
  if$
  number empty$
    'skip$
    { number tie.or.space.connect }
  if$
  output
  format.pub.address output
  format.bdate "year" output.check
  pages empty$
    'skip$
    { new.block
      format.pages output }
  if$
  note output
  fin.entry
}

FUNCTION {unpublished}
{ output.bibitem
  format.authors "author" output.check
  after.item 'output.state :=
  journal empty$
    'skip$
    { journal emphasize "journal" output.check }
  if$
  doi empty$
    {  note output }
    {  after.item 'output.state :=
       format.date output
       "DOI:" doi tie.or.space.connect output
    }
  if$
  fin.entry
  empty.doi.note
}

FUNCTION {conference} {inproceedings}

FUNCTION {other} {patent}

FUNCTION {default.type} {misc}

MACRO {jan} {"Jan."}
MACRO {feb} {"Feb."}
MACRO {mar} {"Mar."}
MACRO {apr} {"Apr."}
MACRO {may} {"May"}
MACRO {jun} {"June"}
MACRO {jul} {"July"}
MACRO {aug} {"Aug."}
MACRO {sep} {"Sept."}
MACRO {oct} {"Oct."}
MACRO {nov} {"Nov."}
MACRO {dec} {"Dec."}

MACRO {acchemr} {"Acc.\ Chem.\ Res."}
MACRO {aacsa} {"Adv.\ {ACS} Abstr."}
MACRO {anchem} {"Anal.\ Chem."}
MACRO {bioch} {"Biochemistry"}
MACRO {bicoc} {"Bioconj.\ Chem."}  % ***
MACRO {bitech} {"Biotechnol.\ Progr."}  % ***
MACRO {chemeng} {"Chem.\ Eng.\ News"}
MACRO {chs} {"Chem.\ Health Safety"} % ***
MACRO {crt} {"Chem.\ Res.\ Toxicol."} % ***
MACRO {chemrev} {"Chem.\ Rev."} % ***
MACRO {cmat} {"Chem.\ Mater."} % ***
MACRO {chemtech} {"{CHEMTECH}"} % ***
MACRO {enfu} {"Energy Fuels"} % ***
MACRO {envst} {"Environ.\ Sci.\ Technol."}
MACRO {iecf} {"Ind.\ Eng.\ Chem.\ Fundam."}
MACRO {iecpdd} {"Ind.\ Eng.\ Chem.\ Proc.\ Des.\ Dev."}
MACRO {iecprd} {"Ind.\ Eng.\ Chem.\ Prod.\ Res.\ Dev."}
MACRO {iecr} {"Ind.\ Eng.\ Chem.\ Res."} % ***
MACRO {inor} {"Inorg.\ Chem."}
MACRO {jafc} {"J.~Agric.\ Food Chem."}
MACRO {jacs} {"J.~Am.\ Chem.\ Soc."}
MACRO {jced} {"J.~Chem.\ Eng.\ Data"}
MACRO {jcics} {"J.~Chem.\ Inf.\ Comput.\ Sci."}
MACRO {jmc} {"J.~Med.\ Chem."}
MACRO {joc} {"J.~Org.\ Chem."}
MACRO {jps} {"J.~Pharm.\ Sci."}
MACRO {jpcrd} {"J.~Phys.\ Chem.\ Ref.\ Data"} % ***
MACRO {jpc} {"J.~Phys.\ Chem."}
MACRO {jpca} {"J.~Phys.\ Chem.~A"}
MACRO {jpcb} {"J.~Phys.\ Chem.~B"}
MACRO {lang} {"Langmuir"}
MACRO {macro} {"Macromolecules"}
MACRO {orgmet} {"Organometallics"}
MACRO {orglett} {"Org.\ Lett."}

MACRO {jft} {"J.~Chem.\ Soc., Faraday Trans."}
MACRO {jft1} {"J.~Chem.\ Soc., Faraday Trans.\ 1"}
MACRO {jft2} {"J.~Chem.\ Soc., Faraday Trans.\ 2"}
MACRO {tfs} {"Trans.\ Faraday Soc."}
MACRO {jcis} {"J.~Colloid Interface Sci."}
MACRO {acis} {"Adv.~Colloid Interface Sci."}
MACRO {cs} {"Colloids Surf."}
MACRO {csa} {"Colloids Surf.\ A"}
MACRO {csb} {"Colloids Surf.\ B"}
MACRO {pcps} {"Progr.\ Colloid Polym.\ Sci."}
MACRO {jmr} {"J.~Magn.\ Reson."}
MACRO {jmra} {"J.~Magn.\ Reson.\ A"}
MACRO {jmrb} {"J.~Magn.\ Reson.\ B"}
MACRO {sci} {"Science"}
MACRO {nat} {"Nature"}
MACRO {jcch} {"J.~Comput.\ Chem."}
MACRO {cca} {"Croat.\ Chem.\ Acta"}
MACRO {angew} {"Angew.\ Chem., Int.\ Ed."}
MACRO {chemeurj} {"Chem.---Eur.\ J."}

MACRO {poly} {"Polymer"}
MACRO {ajp} {"Am.\ J.\ Phys."}
MACRO {rsi} {"Rev.\ Sci.\ Instrum."}
MACRO {jcp} {"J.~Chem.\ Phys."}
MACRO {cpl} {"Chem.\ Phys.\ Lett."}
MACRO {molph} {"Mol.\ Phys."}
MACRO {pac} {"Pure Appl.\ Chem."}
MACRO {jbc} {"J.~Biol.\ Chem."}
MACRO {tl} {"Tetrahedron Lett."}
MACRO {psisoe} {"Proc.\ SPIE-Int.\ Soc.\ Opt.\ Eng."}
MACRO {prb} {"Phys.\ Rev.\ B:\ Condens.\ Matter Mater. Phys."}
MACRO {jap} {"J.~Appl.\ Phys."}
MACRO {pnac} {"Proc.\ Natl.\ Acad.\ Sci.\ U.S.A."}
MACRO {bba} {"Biochim.\ Biophys.\ Acta"}
MACRO {nar} {"Nucleic.\ Acid Res."}

READ

STRINGS { longest.label }

INTEGERS { number.label longest.label.width }

FUNCTION {initialize.longest.label}
{ "" 'longest.label :=
  #1 'number.label :=
  #0 'longest.label.width :=
}

FUNCTION {longest.label.pass}
{ number.label int.to.str$ 'label :=
  number.label #1 + 'number.label :=
  label width$ longest.label.width >
    { label 'longest.label :=
      label width$ 'longest.label.width :=
    }
    'skip$
  if$
}

EXECUTE {initialize.longest.label}

ITERATE {longest.label.pass}


FUNCTION {begin.bib}
{ preamble$ empty$
    'skip$
    { preamble$ write$ newline$ }
  if$
  "\providecommand{\url}[1]{\texttt{#1}}"
  write$ newline$
  "\providecommand{\refin}[1]{\\ \textbf{Referenced in:} #1}"
  write$ newline$
  "\begin{thebibliography}{"  longest.label  * "}" *
  write$ newline$
}

EXECUTE {begin.bib}

EXECUTE {init.state.consts}

ITERATE {call.type$}

FUNCTION {end.bib}
{ newline$
  "\end{thebibliography}" write$ newline$
}

EXECUTE {end.bib}
%% 
%%
%% End of file `achemso.bst'.
