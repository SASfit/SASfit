# -*- coding: utf-8 -*-

'''
This module translates a string from one language to another, using
translations given in a hard-coded dictionary. Various dictionaries exist for
different types of text; e.g. type 'geography' is for tables about places and
regions, and 'city' is for tables about cities and villages.

For each table type, there can be three lists:
* translations - direct replacements. Work in either direction, e.g. if
                 the bot knows that he should replace 'Location' with 'Ligging'
                 when translating from English to Dutch, he can also translate
                 it from Dutch to English.
* regexes      - regular expression replacements. These are more powerful than
                 direct replacements as they support wildcards etc., but only
                 work in one direction.
* includes     - one type can include all items from another type, e.g. when
                 translating a text of the type 'city', the bot also tries to
                 apply the translations and regexes given for type 'geography'
                 because 'city' includes 'geography'.
'''

# (C) Daniel Herding, 2004
#
# Distributed under the terms of the MIT license.
#
#

__version__='$Id: translator.py,v 1.21 2005/12/21 17:51:26 wikipedian Exp $'

types = {
    # translations for images (inside other tables)
    "images": {
         "translations": [
             { "en":"[[image:",     "de":"[[bild:",          "nl":"[[afbeelding:",    "fr":"[[image:",  "af":"[[beeld:"    },
             { "en":"[[Image:",     "de":"[[Bild:",          "nl":"[[Afbeelding:",    "fr":"[[Image:",  "af":"[[Beeld:"    },
             { "en":"larger image", "de":u"Bild vergrößern", "nl":"grotere versie",   "fr":u"En détail", "af":"In detail"  },
             { "en":"larger image", "de":u"Bild vergrößern", "nl":"groter",           "fr":u"En détail", "af":"In detail"  },
             # usually used as link description for articles about flags, coats of arms etc.
             { "en":"Details",      "de":u"Details",         "nl":"details",          "fr":u"Détails", "af":"Details"    },
         ],
    },
    
    # translations for taxoboxes (for biology articles)
    "taxo": {
        "translations": [
            # Background colors for table headers, with or without quotation marks (taxoboxes on de: all have quotation marks)
            { "en":"bgcolor=pink",                         "de":"bgcolor=\"#ffc0c0\"",                       "nl":"bgcolor=#EEEEEE",                                "fr":"bgcolor=pink"                               },
            { "en":"bgcolor=\"pink\"",                     "de":"bgcolor=\"#ffc0c0\"",                       "nl":"bgcolor=\"#EEEEEE\"",                            "fr":"bgcolor=\"pink\""                           },
            # second table header (below the image)
            { "en":"[[Scientific classification]]",        "de":"[[Systematik (Biologie)|Systematik]]",      "nl":"[[Taxonomie|Wetenschappelijke  classificatie]]", "fr":u"Classification [[systématique]]"        },
            # main taxobox content
            { "en":"[[Domain (biology)|Domain]]:",         "de":u"''[[Domäne (Biologie)|Domäne]]:''",  "nl":"[[Domain (biologie)|Domain]]:",                  "fr":"??? (domain)"                               },
            { "en":"Domain:",                              "de":u"''[[Domäne (Biologie)|Domäne]]:''",  "nl":"[[Domain (biologie)|Domain]]:",                  "fr":"??? (domain)"                               },
            { "en":"[[Kingdom (biology)|Kingdom]]:",       "de":"''[[Reich (Biologie)|Reich]]:''",           "nl":"[[Rijk (biologie)|Rijk]]:",                      "fr":u"[[Règne (biologie)|Règne]]:",        },
            { "en":"Kingdom:",       "de":"''[[Reich (Biologie)|Reich]]:''",           "nl":"[[Rijk (biologie)|Rijk]]:",                      "fr":u"[[Règne (biologie)|Règne]]:",        },
            { "en":"[[Division (biology)|Division]]:",      "de":"''[[Abteilung (Biologie)|Abteilung]]:''",                      },
            { "en":"Division:",                            "de":"''[[Abteilung (Biologie)|Abteilung]]:''",                                        },
            { "en":"[[Phylum (biology)|Phylum]]:",         "de":"''[[Stamm (Biologie)|Stamm]]:''",           "nl":"[[Stam (biologie)|Stam]]:",                      "fr":"[[Embranchement]]:",                        },
            { "en":"Phylum:",                              "de":"''[[Stamm (Biologie)|Stamm]]:''",           "nl":"[[Stam (biologie)|Stam]]:",                      "fr":"[[Embranchement]]:",                        },
            { "en":"[[Subphylum]]:",                       "de":"''[[Unterstamm]]:''",                       "nl":"[[Substam (biologie)|Substam]]:",                "fr":"[[Sous-embranchement]]:",                   },
            { "en":"Phylum:",                              "de":"''[[Unterstamm]]:''",                       "nl":"[[Substam (biologie)|Substam]]:",                "fr":"[[Sous-embranchement]]:",                   },
            { "en":"[[Superclass (biology)|Superclass]]:", "de":u"''[[Klasse (Biologie)|Überklasse]]:''", "nl":"[[Superklasse (biologie)|Superklasse]]:",        "fr":"[[Super-classe (biologie)|Super-classe]]:", },
            { "en":"Superclass:",                          "de":u"''[[Klasse (Biologie)|Überklasse]]:''", "nl":"[[Superklasse (biologie)|Superklasse]]:",        "fr":"[[Super-classe (biologie)|Super-classe]]:", },
            { "en":"[[Class (biology)|Class]]:",           "de":"''[[Klasse (Biologie)|Klasse]]:''",         "nl":"[[Klasse (biologie)|Klasse]]:",                  "fr":"[[Classe (biologie)|Classe]]:",             },
            { "en":"Class:",                               "de":"''[[Klasse (Biologie)|Klasse]]:''",         "nl":"[[Klasse (biologie)|Klasse]]:",                  "fr":"[[Classe (biologie)|Classe]]:",             },
            { "en":"[[Subclass]]:",                        "de":"''[[Klasse (Biologie)|Unterklasse]]:''",    "nl":"[[Onderklasse]]:",                               "fr":"[[Sous-classe (biologie)|Sous-classe]]:",   },
            { "en":"Subclass:",                            "de":"''[[Klasse (Biologie)|Unterklasse]]:''",    "nl":"[[Onderklasse]]:",                               "fr":"[[Sous-classe (biologie)|Sous-classe]]:",   },
            { "en":"[[Order (biology)|Superorder]]:",      "de":u"''[[Ordnung (Biologie)|Überordnung]]:''",  "nl":"[[Superorde]]:",       },
            { "en":"[[Order (biology)|Order]]:",           "de":"''[[Ordnung (Biologie)|Ordnung]]:''",       "nl":"[[Orde (biologie)|Orde]]:",                      "fr":"[[Ordre (biologie)|Ordre]]:"                },
            { "en":"Order:",                               "de":"''[[Ordnung (Biologie)|Ordnung]]:''",       "nl":"[[Orde (biologie)|Orde]]:",                      "fr":"[[Ordre (biologie)|Ordre]]:"                },
            { "en":"[[Suborder]]:",                        "de":"''[[Ordnung (Biologie)|Unterordnung]]:''",  "nl":"[[Infraorde (biologie)|Infraorde]]:",            "fr":"[[Sous-ordre (biologie)|Sous-ordre]]:",     },
            { "en":"Suborder:",                            "de":"''[[Ordnung (Biologie)|Unterordnung]]:''",  "nl":"[[Infraorde (biologie)|Infraorde]]:",            "fr":"[[Sous-ordre (biologie)|Sous-ordre]]:",     },
            { "en":"[[Family (biology)|Family]]:",         "de":"''[[Familie (Biologie)|Familie]]:''",       "nl":"[[Familie (biologie)|Familie]]:",                "fr":"[[Famille (biologie)|Famille]]:",           },
            { "en":"Family:",                              "de":"''[[Familie (Biologie)|Familie]]:''",       "nl":"[[Familie (biologie)|Familie]]:",                "fr":"[[Famille (biologie)|Famille]]:",           },
            { "en":"[[Subfamily (biology)|Subfamily]]:",   "de":"''[[Familie (Biologie)|Unterfamilie]]:''",  "nl":"[[Onderfamilie]]:",                              "fr":"[[Sous-famille (biologie)|Sous-famille]]:", },
            { "en":"Subfamily:",                           "de":"''[[Familie (Biologie)|Unterfamilie]]:''",  "nl":"[[Onderfamilie]]:",                              "fr":"[[Sous-famille (biologie)|Sous-famille]]:", },
            { "en":"[[Tribe (biology)|Tribe]]:",           "de":"''[[Tribus (Biologie)|Tribus]]:''",         "nl":"[[Tak (biologie)|Tak]]:",                        "fr":"??? (Tribus)"                               },
            { "en":"Tribe:",                               "de":"''[[Tribus (Biologie)|Tribus]]:''",         "nl":"[[Tak (biologie)|Tak]]:",                        "fr":"??? (Tribus)"                               },
            { "en":"[[Genus]]:",                           "de":"''[[Gattung (Biologie)|Gattung]]:''",       "nl":"[[Geslacht (biologie)|Geslacht]]:",              "fr":"[[Genre]]:"                                 },
            { "en":"Genus:",                               "de":"''[[Gattung (Biologie)|Gattung]]:''",       "nl":"[[Geslacht (biologie)|Geslacht]]:",              "fr":"[[Genre]]:"                                 },
            { "en":"[[Subgenus]]:",                        "de":"''[[Gattung (Biologie)|Untergattung]]:''",  "nl":"[[Ondergeslacht]]:",                             "fr":"??? (Sous-genre)"                           },
            { "en":"Subgenus:",                            "de":"''[[Gattung (Biologie)|Untergattung]]:''",  "nl":"[[Ondergeslacht]]:",                             "fr":"??? (Sous-genre)"                           },
            { "en":"[[Species]]:",                         "de":"''[[Art (Biologie)|Art]]:''",               "nl":"[[Soort]]:",                                     "fr":u"[[Espèce]]:"                            },
            { "en":"Species:",                             "de":"''[[Art (Biologie)|Art]]:''",               "nl":"[[Soort]]:",                                     "fr":u"[[Espèce]]:"                            },
            # table headers for subdivisions of the current group
            { "en":"[[Class (biology)|Classes]]",           "de":"[[Klasse (Biologie)|Klassen]]",            "nl":"[[Klasse (biologie)|Klassen]]",                              },
            { "en":"[[Order (biology)|Orders]]",           "de":"[[Ordnung (Biologie)|Ordnungen]]",          "nl":"[[Orde (biologie)|Orden]]",                      "fr":"[[Ordre (biologie)|Ordres]]"                },
            { "en":"[[Suborder]]s",                        "de":"[[Ordnung (Biologie)|Unterordnungen]]",     "nl":"[[Infraorde (biologie)|Infraorden]]:",           "fr":"[[Sous-ordre (biologie)|Sous-ordres]]",     },
            { "en":"[[Family (biology)|Families]]",        "de":"[[Familie (Biologie)|Familien]]",         "nl":"[[Familie (biologie)|Families]]",                "fr":"[[Famille (biologie)|Familles]]",           },
            { "en":"[[Genus|Genera]]",                     "de":"[[Gattung (Biologie)|Gattungen]]",          "nl":"[[Geslacht (biologie)|Geslachten]]",             "fr":"[[Genre (biologie)|Genre]]"                 },
            { "en":"[[Species]]",                          "de":"[[Art (Biologie)|Arten]]",                  "nl":"[[Soort]]en",                                    "fr":u"??? (Espèces)"                          },
            { "en":"[[Species]] (incomplete)",             "de":"[[Art (Biologie)|Arten (Auswahl)]]",        "nl":"[[Soort]]en (incompleet)",                       "fr":u"??? (Espèces (sélection))"           },
            # table headers for nl: style taxoboxes (current group is listed in a special section at the bottom)
            { "en":"[[Order (biology)|Order]]",            "de":"[[Ordnung (Biologie)|Ordnung]]",            "nl":"[[Orde (biologie)|Orde]]",                       "fr":"[[Ordre (biologie)|Ordre]]"                 },
            { "en":"[[Family (biology)|Family]]",          "de":"[[Familie (Biologie)|Familie]]",            "nl":"[[Familie (biologie)|Familie]]",                 "fr":"[[Famille (biologie)|Famille]]",            },
            { "en":"[[Genus]]",                            "de":"[[Gattung (Biologie)|Gattung]]",            "nl":"[[Geslacht (biologie)|Geslacht]]",               "fr":"[[Genre]]"                                  },
            { "en":"[[Species]]",                          "de":"[[Art (Biologie)|Art]]",                    "nl":"[[Soort]]",                                      "fr":u"[[Espèce]]"                             },
        ],
        "regexes": {
            "en": {
                # de: doesn't have conservation status infos
                "\{\{msg\:Status[^\}]+\}\}": {"de":"", },
            },
        },
        "includes": ["images", "taxo_categories"],
    },

    # this should only include classes etc. which appear very often, not every species!
    "taxo_categories": {
        "translations": [
            # kingdoms
            { "en":"[[Animal]]ia",                      "de":"[[Tiere]] (Animalia)",                  "nl":"Dieren (''[[Animalia]]'')",      },
            { "en":"[[Plant]]ae",                       "de":"[[Pflanzen]] (Plantae)",                },
            # divisions
            { "en":"[[flowering plant|Magnoliophyta]]", "de":u"[[Blütenpflanzen]] (Magnoliophyta)", },
            # phylums
            { "en":"[[Anthropod]]a",                    "de":u"[[Gliederfüßler]] (Anthropoda)",               },
            { "en":"[[Chordata]]",                      "de":"[[Chordatiere]] (Chordata)",            "nl":"Chordadieren (''[[Chordata]]'')",                   },
            { "en":"[[Chordate|Chordata]]",             "de":"[[Chordatiere]] (Chordata)",            "nl":"Chordadieren (''[[Chordata]]'')",                   },
            # subphylums
            { "en":"[[Vertebrata]]",                    "de":"[[Wirbeltiere]] (Vertebrata)",          "nl":"Gewervelden (''[[Vertebrata]]'')", },
            # superclasses
            # classes
            { "en":"[[Aves]]",                          "de":u"[[Vögel]] (Aves)",                     "nl":"Vogels (''[[Aves]]'')",               },
            { "en":"[[Insect]]a",                       "de":"[[Insekten]] (Insecta)",             },
            { "en":"[[Mammal]]ia",                      "de":u"[[Säugetiere]] (Mammalia)",            "nl":"Zoogdieren (''[[Mammalia]]'')",   },
            { "en":"[[Mammalia]]",                      "de":u"[[Säugetiere]] (Mammalia)",            "nl":"Zoogdieren (''[[Mammalia]]'')",   },
            { "en":"[[dicotyledon|Magnoliopsida]]",     "de":u"Zweikeimblättrige (Magnoliopsida)", },
            {                                           "de":"Reptilien (Reptilia)",                  "nl":"Reptielen (''[[Reptilia]]'')",  },
        ],
        "regexes": {
            "de": {
                # change [[Hunde]] (Canidae) to Hunde (''[[Canidae]]'') for nl:
                # and to [[Canidae]] for en:
                "\[\[(?P<german>[^\[]+)\]\] \((?P<latin>.+)\)": {"en":"[[\g<latin>]]", "nl":"\g<german> (\'\'[[\g<latin>]]\'\')", },
            },
            "nl": {
                # change Knaagdieren (''[[Rodentia]]'') to [[Knaagdieren]] (Rodentia)
                "(?P<dutch>[a-zA-Z ]+) \(\[\[\'\'(?P<latin>[^\[]+)\'\'\]\]\)": {"de":"[[\g<dutch>]] (\g<latin>)", },
                "(?P<dutch>[a-zA-Z ]+) \(\'\'\[\[(?P<latin>[^\[]+)\]\]\'\'\)": {"de":"[[\g<dutch>]] (\g<latin>)", },
                "(?P<dutch>[a-zA-Z ]+) \(\[\[\<i\>(?P<latin>[^\[]+)\<\/i\>\]\]\)": {"de":"[[\g<dutch>]] (\g<latin>)", },
                "(?P<dutch>[a-zA-Z ]+) \(\<i\>\[\[(?P<latin>[^\[]+)\]\]\<\/i\>\)": {"de":"[[\g<dutch>]] (\g<latin>)", },
            },
        },
                
    },
            

    # plants get the same table color as animals on de:, but on en: they are green instead of pink
    "plant": {
        "translations": [
            { "en":"bgcolor=lightgreen",               "de":"bgcolor=\"#ffc0c0\"",                     }, 
            { "en":"bgcolor=\"lightgreen\"",           "de":"bgcolor=\"#ffc0c0\"",                     }, 
        ],
        "includes": ["taxo"],
    },

    # regular expressions for number formats
    "numbers": {
        "translations": [
            # miljoen shouldn't be abbreviated on nl:
            { "en":"mill.",      "de":"Mio.",    "nl":"miljoen", },
            { "en":"bill.",      "de":"Mrd." },
        ],
        "regexes": {
            "fr": {
                # fr uses &nbsp; or space to separate thousands, de uses dots
                # note: this doesn't work for numbers > 1,000,000, don't know why
                "(?P<pre>\d+)\&nbsp;(?P<block>\d\d\d)": {"de":"\g<pre>.\g<block>", },
                "(?P<pre>\d+) (?P<block>\d\d\d)": {"de":"\g<pre>.\g<block>", },
            },
            "en": {
                # de uses dots to separate thousands, en uses commas
                # de uses commas to indicate floating point numbers, en uses dots
                # switch both - temporary placeholder required
                "(?P<pre>\d+)\,(?P<block>\d\d\d)":        {"de":"\g<pre>TEMPORARY_DOT\g<block>", },
                "(?P<pre>\d+)\.(?P<block>\d+)":           {"de":"\g<pre>,\g<block>", },
                "TEMPORARY\_DOT": {"de":".", },
            },
            "de": {
                # de uses dots to separate thousands, en uses commas
                # de uses commas to indicate floating point numbers, en uses dots
                # switch both - temporary placeholder required
                "(?P<pre>\d+)\.(?P<block>\d\d\d)":             {"en":"\g<pre>TEMPORARY_COMMA\g<block>", },
                "(?P<pre>\d+)\,(?P<block>\d+)":                {"en":"\g<pre>.\g<block>", },
                "TEMPORARY\_COMMA": {"en":",", },
            },
        },
    },
    
    "months": {
        "translations": [
            { "sl":"januar",    "it":"gennaio",   "en":"January",   "de":"Januar",    "fr":"janvier",    "nl":"januari",   "af":"Januarie"},
            { "sl":"februar",   "it":"febbraio",  "en":"February",  "de":"Februar",   "fr":u"février",   "nl":"februari",  "af":"Februarie"},
            { "sl":"marec",     "it":"marzo",     "en":"March",     "de":u"März",     "fr":"mars",       "nl":"maart",     "af":"Maart"},
            { "sl":"april",     "it":"aprile",    "en":"April",     "de":"April",     "fr":"avril",      "nl":"april",     "af":"April"},
            { "sl":"maj",       "it":"maggio",    "en":"May",       "de":"Mai",       "fr":"mai",        "nl":"mei",       "af":"Mei"},
            { "sl":"junij",     "it":"giugno",    "en":"June",      "de":"Juni",      "fr":"juin",       "nl":"juni",      "af":"Junie"},
            { "sl":"julij",     "it":"luglio",    "en":"July",      "de":"Juli",      "fr":"juillet",    "nl":"juli",      "af":"Julie"},
            { "sl":"avgust",    "it":"agosto",    "en":"August",    "de":"August",    "fr":u"août",      "nl":"augustus",  "af":"Augustus"},
            { "sl":"september", "it":"settembre", "en":"September", "de":"September", "fr":"septembre",  "nl":"september", "af":"September"},
            { "sl":"oktober",   "it":"ottobre",   "en":"October",   "de":"Oktober",   "fr":"octobre",    "nl":"oktober",   "af":"Oktober"},
            { "sl":"november",  "it":"novembre",  "en":"November",  "de":"November",  "fr":"novembre",   "nl":"november",  "af":"November"},
            { "sl":"december",  "it":"dicembre",  "en":"December",  "de":"Dezember",  "fr":u"décembre",  "nl":"december",  "af":"Desember"},
        ]
    },
    
    # conversion between number formats
    "dates": {
        "regexes": {
            "de": {
                # dd.mm.yy and dd.mm.yyyy format
                "(?P<day>\d\d).(?P<month>\d\d).(?P<year>(\d\d)+)": {"nl":"\g<day>-\g<month>-\g<year>", },
            },
        },
    },
           
    
   
    # units of measurement etc.
    # only for internal use
    "units": {
        "translations": [
            { "en":"[[Square kilometre|km&sup2;]]",  "de":"[[Quadratkilometer|km&sup2;]]",  "nl":"[[Vierkante kilometer|km&sup2;]]", },
            { "en":u"[[Square kilometre|km²]]",      "de":u"[[Quadratkilometer|km²]]",      "nl":u"[[Vierkante kilometer|km²]]",     },
            { "en":"as of ",                         "de":"Stand: ",                         },
            { "en":"years",                          "de":"Jahre",                           "nl":"jaar"},
        ]
    },
    
    # general geographical terms etc.
    # only for internal use
    "geography": {
        "translations": [
            # header
            { "en":"Base data",                     "de":"Basisdaten",                     "nl":"Basisgegevens",                       "fr":"Informations",       },
            { "en":"[[Area]]:",                     "de":u"[[Fläche]]:",                "nl":"Oppervlakte:",                        "fr":"[[Superficie]]:",       "eo":"Areo:",},
            { "en":"[[Population]]:",               "de":"[[Einwohner]]:",                 "nl":"Inwoneraantal:",                      "fr":u"[[Population]]:",       "eo":u"Logantaro:",   },
            { "en":"[[Population density]]:",       "de":u"[[Bevölkerungsdichte]]:",    "nl":"[[Bevolkingsdichtheid]]:",              },
            { "en":"inh./km&sup2;",                 "de":"Einw./km&sup2;",                 "nl":"inw./km&sup2;",                       "fr":"hab/km&sup2;", },
            { "en":u"inh./km²",                  "de":u"Einw./km²",                  "nl":u"inw./km²",                        "fr":u"hab/km²",  },
            { "en":"inhabitants/km&sup2;",          "de":"Einwohner/km&sup2;",             "nl":"inwoners / km&sup2;",                },
            { "en":u"inhabitants/km²",           "de":u"Einwohner/km²",              "nl":u"inwoners / km²",               },
            { "en":"inhabitants per km&sup2;",      "de":"Einwohner pro km&sup2;",         "nl":"inwoners per km&sup2;",               }, 
            { "en":u"inhabitants per km²",       "de":u"Einwohner pro km²",          "nl":u"inwoners per km²",                    },
            { "en":"inh.",                          "de":"Einw.",                          "nl":"inw.",                                 "fr":"hab.", },
            { "en":"above [[sea level]]",           "de":u"ü. [[Normalnull|NN]]",       "nl":"boven [[Normaal Amsterdams Peil|NAP]]",                           },
            { "en":"location",                      "de":"Geografische Lage",              "nl":"Ligging",                              "fr":"Localisation",                              },
            # longitude, latitude
            { "en":"' north",                       "de":u"' nördlicher Breite",        "nl":"' NB" },
            { "en":"' north",                       "de":u"' nördl. Breite",            "nl":"' NB" },
            { "en":"' north",                       "de":"' n. Br.",                       "nl":"' NB" },
            { "en":"' east",                        "de":u"' östlicher Länge",       "nl":"' OL" },
            { "en":"' east",                        "de":u"' östl. Länge",           "nl":"' OL" },
            { "en":"' east",                        "de":u"' ö. L.",                    "nl":"' OL" },
            { "en":"Map",                           "de":"Karte",                          "nl":"Kaart",                        },
            { "en":"Coat of Arms",                  "de":"Wappen",                         "nl":"Wapen",                                 "fr":"Blason"      },
        ],
        "includes": ["units"],
    },
            
    "city": {
        "translations": [
            { "en":"[[Location]]:",                          "de":"[[Geografische Lage]]:",                    "nl":"Ligging", },
            { "en":"[[Altitude]]:",                          "de":u"[[Höhe]]:",                             "nl":"Hoogte:", },
            { "en":"Highest point:",                         "de":u"Höchster Punkt:",                       "nl":"Hoogste punt:",},
            { "en":"Lowest point:",                          "de":"Niedrigster Punkt:",                        "nl":"Laagste punt:"},
            { "en":"[[Postal code]]:",                       "de":"[[Postleitzahl]]:",                         "nl":"[[Postcode]]:",                 },
            { "en":"[[Postal code]]s:",                      "de":"[[Postleitzahl]]en:",                       "nl":"[[Postcode]]s:",                 },
            { "en":"[[Area code]]:",                         "de":"[[Telefonvorwahl|Vorwahl]]:",               "nl":"[[Netnummer]]:",             },
            { "en":"[[Area code]]s:",                        "de":"[[Telefonvorwahl|Vorwahlen]]:",             "nl":"[[Netnummer]]s:",             },
            { "en":"[[License plate]]:",                     "de":"[[KFZ-Kennzeichen]]:",                      "nl":"[[Autonummerbord]]:",         },
            { "en":"[[License plate]]:",                     "de":"[[Kfz-Kennzeichen]]:",                      "nl":"[[Autonummerbord]]:",           },
            { "en":"City structure:",                        "de":"Gliederung des Stadtgebiets:",              "nl":"Ondergemeentelijke indeling:",  },
            # town hall snail mail address
            { "en":"Municipality's address:",                "de":"Adresse der Gemeindeverwaltung:",           "nl":"Adres gemeentehuis:",       },
            # city hall snail mail address
            { "en":"Municipality's address:",                "de":"Adresse der Stadtverwaltung:",              "nl":"Adres stadhuis:",       },
            { "en":"Website:",                               "de":"Webseite:",                                 "nl":"Website:"     },
            { "en":"Website:",                               "de":"Website:",                                  "nl":"Website:"     },
            { "en":"E-Mail adress:",                         "de":"[[E-Mail]]-Adresse:",                       "nl":"Email-adres:",               },
            { "en":"E-Mail adress:",                         "de":"E-Mail-Adresse:",                           "nl":"Email-adres:",               },
            # table header
            { "en":"Politics",                               "de":"Politik",                                   "nl":"Politiek",                  },
            # female mayor
            { "en":"[[Mayor]]:",                             "de":u"[[Bürgermeister]]in:",                  "nl":"[[Burgemeester]]:",               },
            { "en":"[[Mayor]]:",                             "de":u"[[Bürgermeisterin]]:",                  "nl":"[[Burgemeester]]:",          },
            # male mayor
            { "en":"[[Mayor]]:",                             "de":u"[[Bürgermeister]]:",                    "nl":"[[Burgemeester]]:",           },
            { "en":"Governing [[Political party|party]]:",   "de":"Regierende [[Politische Partei|Partei]]",   "nl":"Regerende partij",               },
            { "en":"Governing [[Political party|parties]]:", "de":"Regierende [[Politische Partei|Parteien]]", "nl":"Regerende partijen",             },
            { "en":"Majority [[Political party|party]]:",    "de":"[[Politische Partei|Mehrheitspartei]]",     "nl":"Meerderheidspartij"},
            { "en":"Debts:",                                 "de":"Schulden:",                                     },
            { "en":"[[Unemployment]]:",                      "de":"[[Arbeitslosenquote]]:",                    "nl":"Werkloosheidspercentage:", },
            {                                                "de":u"[[Ausländeranteil]]:",                  "nl":"Percentage buitenlanders",            },
            { "en":"Age distribution:",                      "de":"Altersstruktur:",                           "nl":"Leeftijdsopbouw:",          },
            {                                                "de":"Stadtteile",                                "nl":"wijken"},
            {                                                "de":"[[Stadtbezirk]]e",                          "nl":"deelgemeenten"                 },
            {                                                "de":"Stadtbezirke",                              "nl":"deelgemeenten"                 },
            { "en":"Independent",                            "de":"Parteilos",                                 "nl":"geen partij"             },
            { "en":"Region",                                 "de":"[[Region]]",                                "nl":"Landstreek"                },
        ],
        "includes": ["images", "geography", "numbers"],
    },
    
    # translations for cities in Germany
    "city-de": {
        "translations": [
            { "en":"[[Bundesland]]:",          "de":"[[Bundesland]]:",                      "nl":"[[Deelstaat (Duitsland)|Deelstaat]]",     },
            { "en":"[[Regierungsbezirk]]:",    "de":"[[Regierungsbezirk]]:",                "nl":"[[Regierungsbezirk]]:",                   },
            { "en":"[[District]]:",            "de":"[[Landkreis|Kreis]]:",                 "nl":"[[District]]",                            },
            { "en":"[[District]]:",            "de":"[[Landkreis]]:",                       "nl":"[[District]]",                            },
            { "en":"district-free town",       "de":"[[kreisfreie Stadt]]",                 "nl":"[[stadsdistrict]]",                       },
            { "en":"District-free town",       "de":"[[Kreisfreie Stadt]]",                 "nl":"[[Stadsdistrict]]",                       },
            { "en":"District-free town",       "de":"[[Stadtkreis]]",                       "nl":"[[Stadsdistrict]]",                       },
            { "en":"[[Municipality key]]:",    "de":"[[Amtliche Gemeindekennzahl]]:", },
            { "en":"[[Municipality key]]:",    "de":u"[[Amtlicher Gemeindeschlüssel]]:",                                              },
            { "en":"urban districts",          "de":"[[Stadtbezirk]]e",                     "nl":"stadsdelen",                                             },
            # female first mayor, no exact translation in en:
            { "en":"[[Mayor]]:",               "de":u"[[Oberbürgermeisterin]]:",         "nl":"[[Burgemeester]]:"},
            { "en":"[[Mayor]]:",               "de":u"[[Oberbürgermeister]]in:",         "nl":"[[Burgemeester]]:"},
            # male first mayor, no exact translation in en:
            { "en":"[[Mayor]]:",               "de":u"[[Oberbürgermeister]]:",           "nl":"[[Burgemeester]]:"},
            # "bis" is used between postal codes
            { "en":" to ",                     "de":" bis ",                                "nl":"t/m"},          
            # some cities have demographic info which is titled "Bevölkerung" (population). The spaces are important
            # because "Bevölkerung" is also a substring of "Bevölkerungsdichte (population density).
            {                                  "de":u" Bevölkerung ",                      "nl":" Demografie ", },

            # parties
            { "en":"[[Christian Democratic Union of Germany|CDU]]",       "de":"[[CDU]]",                            "nl":"[[Christlich Demokratische Union|CDU]]"},
            { "en":"[[Social Democratic Party of Germany|SPD]]",          "de":"[[SPD]]",                            "nl":"[[Sozialdemokratische Partei Deutschlands|SPD]]"},
            { "en":"[[Christian Social Union in Bavaria|CSU]]",           "de":"[[CSU]]",                            "nl":"[[CSU]]"},
            { "en":"[[Free Democratic Party of Germany|FDP]]",            "de":"[[FDP (Deutschland)|FDP]]",          "nl":"[[FDP]]"},
            { "en":u"[[German Green Party|Bündnis 90/Die Grünen]]", "de":u"[[Bündnis 90/Die Grünen]]",   "nl":u"[[Die Grünen]]"},
            { "en":"[[Party of Democratic Socialism|PDS]]",               "de":"[[PDS]]",                            "nl":"[[PDS]]"},
            # Bundeslaender
            { "en":"[[Bavaria]]",                                         "de":"[[Bayern]]",                         "nl":"[[Beieren]]"},
            { "en":"[[Bremen (state)|Bremen]]",                           "de":"[[Bremen (Land)|Bremen]]",           "nl":"[[Bremen]]"},
            { "en":"[[Hesse]]",                                           "de":"[[Hessen]]",                         "nl":"[[Hessen]]"},
            { "en":"[[Mecklenburg-Western Pomerania]]",                   "de":"[[Mecklenburg-Vorpommern]]",         "nl":"[[Mecklenburg-Voorpommeren]]"},
            { "en":"[[Lower Saxony]]",                                    "de":"[[Niedersachsen]]",                  "nl":"[[Nedersaksen]]"},
            { "en":"[[North Rhine-Westphalia]]",                          "de":"[[Nordrhein-Westfalen]]",            "nl":"[[Noordrijn-Westfalen]]"},
            { "en":"[[Rhineland-Palatinate]]",                            "de":"[[Rheinland-Pfalz]]",                "nl":"[[Rijnland-Palts]]"},
            { "en":"[[Saxony]]",                                          "de":"[[Sachsen (Bundesland)|Sachsen]]",   "nl":"[[Saksen (deelstaat)|Saksen]]"},
            { "en":"[[Saxony-Anhalt]]",                                   "de":"[[Sachsen-Anhalt]]",                 "nl":"[[Saksen-Anhalt]]"},
            { "en":"[[Schleswig-Holstein]]",                              "de":"[[Schleswig-Holstein]]",             "nl":"[[Sleeswijk-Holstein]]"},
            { "en":"[[Thuringia]]",                                       "de":u"[[Thüringen]]",                  "nl":u"[[Thüringen]]",},
        ],
        "regexes": {
            "de": {
                # image alt text
                "Deutschlandkarte, (?P<city>.+) markiert":                                                           {"en":"Map of Germany, \g<city> marked", "nl":"Kaart van Duitsland met de locatie van \g<city>", },
                "Karte Deutschlands, (?P<city>.+) markiert":                                                         {"en":"Map of Germany, \g<city> marked", "nl":"Kaart van Duitsland met de locatie van \g<city>", },
                "Karte (?P<city>.+) in Deutschland":                                                                 {"en":"Map of Germany, \g<city> marked", "nl":"Kaart van Duitsland met de locatie van \g<city>", },
                # nl: doesn't want Municipality Number
                u"\|[-]+ bgcolor=\"#FFFFFF\"[\r\n]+\| *\[\[Amtliche( Gemeindekennzahl|r Gemeindeschlüssel)\]\]\:[ \|\r\n]+[\d -]+[\r\n]+": {                                        "nl":"", },
            },
        },
        "includes": ["city", "dates"],
        
    },
    
    # French départements
    "dep": {
        "translations": [
            # some entries on fr: lack colons, others have spaces before the colons.
            { "de":"[[Region (Frankreich)|Region]]:",              "fr":u"[[Régions françaises|Région]] :", "eo":"[[Francaj regionoj|Regiono]]:", },
            { "de":"[[Region (Frankreich)|Region]]:",              "fr":u"[[Régions françaises|Région]]:",  "eo":"[[Francaj regionoj|Regiono]]:", },
            { "de":u"[[Präfektur (Frankreich)|Präfektur]]:", "fr":u"[[Préfecture]] :",                      "eo":"[[Prefektejo]]:" },
            { "de":u"[[Präfektur (Frankreich)|Präfektur]]:", "fr":u"[[Préfecture]]:",                       "eo":"[[Prefektejo]]:"},
            { "de":u"[[Unterpräfektur]]en:",                    "fr":u"[[Sous-préfecture]]s :",                },
            { "de":u"[[Unterpräfektur]]en:",                    "fr":u"[[Sous-préfecture]]s:",                 },
            { "de":u"[[Unterpräfektur]]:",                      "fr":u"[[Sous-préfecture]] :",                },
            { "de":u"[[Unterpräfektur]]:",                      "fr":u"[[Sous-préfecture]]:",                 },
            { "de":"insgesamt",                                    "fr":"Totale",                                    },
            # the next three items are already in the list "geography", but someone forgot the colons on fr:
            { "de":u"[[Einwohner]]:",                              "fr":u"[[Population]]",                           "eo":u"Lo\u011dantaro:",          },
            { "de":u"[[Bevölkerungsdichte|Dichte]]:",           "fr":u"[[Densité de population|Densité]]",  },
            { "de":u"[[Fläche]]:",                              "fr":"[[Superficie]]",                            "eo":"Areo:",             },
            # another workaround for a forgotten colon
            { "de":"''</small>:",                                  "fr":"''</small>",                           },
            { "de":"[[Arrondissement]]s:",                         "fr":"[[Arrondissement]]s",                       },
            { "de":"[[Kanton (Frankreich)|Kantone]]:",             "fr":u"[[Cantons français|Cantons]]",          },
            { "de":"[[Kommune (Frankreich)|Kommunen]]:",           "fr":"[[Communes de France|Communes]]",           },
            { "de":u"Präsident des<br>[[Generalrat (Frankreich)|Generalrats]]:",
                                                                   "fr":u"[[Président du Conseil général|Président du Conseil<br> général]]", },
        ],
        "regexes": {
            "fr": {
                "\[\[[aA]rrondissements (des |du |de la |de l\'|d\'|de )":           {"de":u"[[Arrondissements im Département ", },
                "\[\[[cC]ommunes (des |du |de la |de l\'|d\'|de )":                  {"de":u"[[Kommunen im Département ",       },
                "\[\[[cC]antons (des |du |de la|de l\'|d\'|de )":                    {"de":u"[[Kantone im Département ",        },
                "Blason (des |du |de la |de l\'|d\'|de )":                           {"de":"Wappen von ",                          },
                # image alt text
                "Localisation (des |du |de la |de l\'|d\'|de )(?P<dep>.+?) en France": {"de":"Lage von \g<dep> in Frankreich",       },
            },  
        },
        "includes": ["numbers", "images", "geography"],
    },          
}

import wikipedia, string, re

class Global(object):
    debug = False

# Prints text on the screen only if in debug mode.
# Argument text should be raw unicode.
def print_debug(text):
    if Global.debug:
        wikipedia.output(text)

# Translate the string given as argument 'text' from language 'from_lang' to 
# language 'to_lang', using translation list 'type' in above dictionary.
# if debug_mode=True, status messages are displayed.
def translate(text, type, from_lang, debug_mode=False, to_lang=None):
    if to_lang is None:
        to_lang = wikipedia.getSite().lang        
    if debug_mode:
        Global.debug = True
    if type == "":
        return text
    else:
        print_debug("\n Translating type " + type)
        # check if the translation database knows this type of table
        if not type in types:
            print "Unknown table type: " + type
            return
        if "translations" in types.get(type):
            print_debug("\nDirect translations for type " + type + "\n")
            for item in types.get(type).get("translations"):
                # check if the translation database includes the source language
                if not from_lang in item:
                    print_debug(from_lang + " translation for item not found in translation table, skipping item")
                    continue
                # if it's necessary to replace a substring
                if string.find(text, item.get(from_lang)) > -1:
                     # check if the translation database includes the target language
                     if not to_lang in item:
                         print_debug("Can't translate \"" + item.get(from_lang) + "\". Please make sure that there is a translation in copy_table.py.")
                     else:
                         print_debug(item.get(from_lang) + " => " + item.get(to_lang))
                         # translate a substring
                         text = string.replace(text, item.get(from_lang), item.get(to_lang))
        if 'regexes' in types.get(type):
            # work on regular expressions
            print_debug("\nWorking on regular expressions for type " + type + "\n")
            regexes = types.get(type).get("regexes")
            if from_lang in regexes:
                for item in regexes.get(from_lang):
                    # only work on regular expressions that have a replacement for the target language
                    if to_lang in regexes.get(from_lang).get(item):
                        replacement = regexes.get(from_lang).get(item).get(to_lang)
                        regex = re.compile(item)
                        # if the regular expression doesn't match anyway, we don't want it to print a debug message
                        while re.search(regex, text):
                            print_debug(item + " => " + replacement)
                            text = re.sub(regex, replacement, text)
        # recursively use translation lists which are included in the current list
        if "includes" in types.get(type):
            for inc in types.get(type).get("includes"):
                text = translate(text, inc, from_lang, debug_mode, to_lang)
        return text
