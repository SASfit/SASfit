/****************************************************************
** Windows Meta File extension
**
** Tcl Extension for Windows
** RCS Version $Revision: 1.29 $
** RCS Last Change Date: $Date: 2003/04/28 03:55:21 $
**
** This extension is intended to be an alternate source of
** contexts for the GDI package.
** Unlike the printer contexts provided by the printer package,
** metafile contexts can be used to either write importable files
** or to copy data to the clipboard.
**
** This extension was suggested and originally written by
** Gerlinger Leopold <leopold.gerlinger@siemens.at> for the GDI
** package and put into the wmf package by Michael Schwartz
** to fit better into the intended design paradigm.
** Michael Thomsen <miksen@ideogramic.com> noted problems with the
** enhanced metafile approach, and kindly offered to fund their repair.
** Thanks also to Alain Zuur for suggesting the wmf read function, and 
** challenging wmf to display other application's emf files.
**
** {LICENSE}
** 
** THE AUTHORS HEREBY GRANT PERMISSION TO USE, COPY, MODIFY, DISTRIBUTE,
** AND LICENSE THIS SOFTWARE AND ITS DOCUMENTATION FOR ANY PURPOSE, PROVIDED
** THAT EXISTING COPYRIGHT NOTICES ARE RETAINED IN ALL COPIES AND THAT THIS
** NOTICE IS INCLUDED VERBATIM IN ANY DISTRIBUTIONS. 
**
** NO WRITTEN AGREEMENT, LICENSE, OR ROYALTY FEE IS REQUIRED FOR ANY OF THE
** AUTHORIZED USES.
** 
** MODIFICATIONS TO THIS SOFTWARE MAY BE COPYRIGHTED BY THEIR AUTHORS
** AND NEED NOT FOLLOW THE LICENSING TERMS DESCRIBED HERE, PROVIDED THAT
** THE NEW TERMS ARE CLEARLY INDICATED ON THE FIRST PAGE OF EACH FILE WHERE
** THEY APPLY.
** 
** IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT,
** INDIRECT, SPECIAL, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES ARISING OUT OF
** THE USE OF THIS SOFTWARE, ITS DOCUMENTATION,  OR ANY DERIVATIVES
** THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
** 
** THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF
** MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. 
** THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND
** DISTRIBUTORS HAVE NO OBLIGATION  TO PROVIDE MAINTENANCE, SUPPORT,
** UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
**
** {SYNOPSIS}
**
** This file contains commands to extend Tcl for Windows.
** The commands are:
**
** wmf
**  wmf open
**  wmf close
**  wmf delete
**  wmf copy
**  wmf info
**  wmf paste
**  wmf read
**
** The wmf open command returns an hDC which can be used by the GDI extension
** to draw upon.
**
** Details of each command follow
**
** wmf open [-file filename] [-width width -height height] 
**                     [-app app] [-title title] [-hDC hdc] [-init emffile]
**  DESCRIPTION:
**   Returns an hDC if successful
**   Opens a memory metafile if no file is specified
**   -file  Opens the named "emf" file. Note that this (re)initializes it.
**          Use the "wmf read" function or the -init parameter to read in contents of
**          an already extant metafile.
**   -width and -height specify the size for pasting in hundredths of millimeters.
**   If not specified, the metafile will calculate its own dimensions. This is normally accurate,
**   but on occassion unexpected cropping occurs
**   -app and -title store the name of the application and title of the metafile
**   while in memory. These are not stored in the file.
**   If an HDC is provided, it will be used as the reference DC for creation.
**   Otherwise, the screen DC will be used. 
**   Note that the reference HDC and size are used to calculate the size and position
**   used in pasting the metafile into another application. In particular, if the 
**   drawing is to be done as to a printer, use a printer HDC as the reference.
**   -init will populate the EMF with the contents of an existing enhanced metafile
**   (no additional options are available at this time).
**  LIMITATIONS:
**   None known
**
** wmf close hDC
**  DESCRIPTION:
**   Closes the metafile and returns an emf context that can be used
**   for copying and saving.
**   The DC can no longer be used after this call.
**
** wmf copy emf [-file filename] [-type metafile|enhanced|both]
**  DESCRIPTION:
**   Copies the contents of the enhanced metafile to the clipboard or provided file
**   The copy is done in EMF format, so the clipboard may render the picture without
**   relying on the Tcl program running anymore.
**   The -type argument is for clipboard copies only, and can put old-style metafiles,
**   enhanced metafiles, or both on the clipboard. By default, both types are pasted.
**   If the author wishes to ensure that only "pictures" are available, use the
**   -type metafile switch; if resizable enhanced metafiles only are needed, and
**   only current applications are in use, -type enhanced will be best.
**   The default setting is fine for most uses, but will give different results
**   when selecting options on the "Paste Special" menu.
**
** wmf info emf [-verbose|-canvas]
**  DESCRIPTION:
**   Returns a list of pairs representing the info about the metafile
**   -verbose adds details about each metafile record and its arguments
**   -canvas  prints details about each metafile record with a canvas representation
**            in a format conducive to rendering on a canvas
**  LIMITATIONS
**   Not every metafile command can be well-represented as a canvas command.
**   Also, not every representable command has been implemented at this time.
**   The large case statement that does this translation can easily add any
**   missing portions (use the -verbose switch to get the parameter encoding if needed).
**
** wmf play emf hdc [-bounds "x0 y0 x1 y1"]
**  DESCRIPTION:
**   Copies the enhanced metafile into the given bounds on the provided HDC.
**   If no bounds are provided, a rectangle of size 2000x2000 at the origin is assumed.
**
** wmf read file
**  DESCRIPTION
**   Reads an operating system EMF file and creates an emf context that can be used
**   for copying and saving.
**
** wmf delete emf
**  DESCRIPTION:
**   Deletes the enhanced metafile and frees its resources.
**   The emf handle can no longer be used after this call
**
** wmf paste [-type type] [-list] [-file f]
**  DESCRIPTION:
**   Pastes the contents of the clipboard and returns them to the user
**   The return value is a pair, with the first element being the type--either
**   text, enhanced metafile, image, or unicode text , and the second being
**   the value--either an image name, metafile handle, or text content.
**   However, if the -type argument is provided, the first member of the pair
**   is not included.
**   -list  return the accessible types on the clipboard, from among the list
**          text, enhanced metafile, image, metafile, unicode text
**   -type  return the clipboard item in the requested format, from among the list
**          text, enhanced metafile, image, metafile, unicode text
**          Note that this is the type decoded from the clipboard--and not
**          necessarily the type returned. For instance, when pasting from a
**          metafile on the clipboard, and enhanced metafile is returned.
**          Without -type, the command will return the first available format 
**          from the list text, enhanced metafile, image, metafile, unicode text
**   Images are produced by writing a PPM file and then reading it back. If the
**   -file argument is given, the file is named and retained after the command finishes.
**  LIMITATION:
**   First, this overlaps with the Tcl Builtin clipboard command for text. Text is
**   only provided in this package for convenience. Frankly, if the TIP process wants
**   to move the entire clipboard paste function out of this package, I won't mind a bit.
**   Second, there is a problem with pasting enhanced metafiles from many MS Office
**   applications. There appears to be a trick to the protocol to getting the
**   translation and lock on the global memory. It is worked around here by returning
**   the metafile representation instead. Strangely, if one persists long enough,
**   the enhanced metafile will eventually be produced, locked, and returned.
**   Third, due to the intertwining of the Tk_Photo functions and the Tk extension,
**   the image format is available only when Tk is loaded.
**   Finally, the image portion has only been tested with 256 and 24-bit color images.
**   32-bit images and lower resolution color may not work.
**
** In order to do this right, the wmf extension needs to know
** which contexts are its own. And it needs to know which are
** device contexts and which are emf contexts.
** [ Similarly, the printer extension should be able to which  
** contexts are its own as well.]
** This is be done with hash tables, as in the gd extension.
***************************************************************==*/

/****************************************************************
** This section contains windows-specific includes and structures
** global to the file.
** Windows-specific functions will be found in a section at the
** end of the file
****************************************************************/
#if defined(__WIN32__) || defined (__WIN32S__) || defined (WIN32S)
  #include <windows.h>
  #include <commdlg.h>
  #include <stdlib.h>
  #include <string.h>

  /*
   * VC++ has an alternate entry point called DllMain, so we need to rename
   * our entry point.
   */
  # ifndef STATIC_BUILD
  #   if defined(_MSC_VER)
  #     define EXPORT(a,b) __declspec(dllexport) a b
  #     define DllEntryPoint DllMain
  #   else
  #     if defined(__BORLANDC__)
  #         define EXPORT(a,b) a _export b
  #     else
  #         define EXPORT(a,b) a b
  #     endif
  #   endif
  # endif
#else
  #   define EXPORT(a,b) a b
#endif

#include <tcl.h>
#include <tk.h>
#include <string.h>
#include <stdlib.h>

#include <io.h> /* Windows include for access() */

/* New macros for tcl8.0.3 and later */
#if defined(TCL_STORAGE_CLASS)
#  undef TCL_STORAGE_CLASS
#endif

/* #ifndef _MSC_VER */
#define TCL_STORAGE_CLASS DLLEXPORT
/* #endif */

#if ! defined(EXTERN)
#  define EXTERN
#endif

#if TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION <= 5
/* In this case, must replace Tcl_Alloc(), Tcl_Realloc(), and Tcl_Free()
** with ckalloc(), ckrealloc(), and ckfree()
*/

#define Tcl_Alloc(x)  ckalloc(x)
#define Tcl_Free(x)   ckfree(x)
#define Tcl_Realloc(x,y)  ckrealloc(x,y)

#endif

#define WMF_dc_type 33
#define WMF_type    34

EXPORT(int,Wmf_Init) (Tcl_Interp *interp);
EXPORT(int,Wmf_SafeInit) (Tcl_Interp *interp);

static int Wmf (ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int WmfOpen (ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int WmfClose (ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int WmfCopy (ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int WmfDelete (ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int WmfInfo  (ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int WmfCopyDC(ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int WmfPaste (ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int WmfRead (ClientData unused, Tcl_Interp *interp, int argc, char **argv);
static int Version(ClientData unused, Tcl_Interp *interp, int argc, char **argv);

static char usage_message[] = "wmf [close|copy|delete|info|open|paste|version]";

static int using_tk = 1;

/* Hash table stuff */
static void init_wmf_dc_contexts(Tcl_Interp *interp);
static void init_wmf_contexts(Tcl_Interp *interp);
static void delete_wmf_dc_contexts(Tcl_Interp *interp);
static void delete_wmf_contexts(Tcl_Interp *interp);
static const char *make_wmf_dc_name(Tcl_Interp *interp, HDC dc);
static const char *make_wmf_name(Tcl_Interp *interp, HENHMETAFILE dc);
static int wmf_dc_name_valid(Tcl_Interp *interp, const char *name);
static int wmf_name_valid(Tcl_Interp *interp, const char *name);
static int delete_wmf_dc_name(Tcl_Interp *interp, const char *name);
static int delete_wmf_name(Tcl_Interp *interp, const char *name);
static HDC get_wmf_dc(Tcl_Interp *interp, const char *name);
static HENHMETAFILE get_wmf(Tcl_Interp *interp, const char *name);
static HDC get_general_dc (Tcl_Interp *interp, const char *name);
static int is_valid_mf_hdc(HENHMETAFILE hwmf);
static int is_valid_dc_hdc(HDC hdc);
static void add_dc (HDC hdc);
static void delete_dc (HDC hdc);
static void add_mf (HENHMETAFILE hwmf);
static void delete_mf (HENHMETAFILE hwmf);
/* End of hash table stuff */

static int wmfHeader(HENHMETAFILE hwmf, LPENHMETAHEADER *lpemh);

static HDC convert_hdc (const char *string );

/****************************************************************
** Hash table support
**
** Provided by the hdc extension
****************************************************************/
static Tcl_HashTable wmf_mf_hdcs;
static Tcl_HashTable wmf_dc_hdcs;
static int  hdc_loaded = 0;
static void  init_hdc_functions(Tcl_Interp *interp);
static int (*hdc_init) (Tcl_Interp *interp);
static const char * (*hdc_create) (Tcl_Interp *interp, void *ptr, int type);
static int (*hdc_valid) (Tcl_Interp *interp, const char *hdcname, int type);
static int (*hdc_delete) (Tcl_Interp *interp, const char *hdcname);
static void * (*hdc_get) (Tcl_Interp *interp, const char *hdcname);
static int (*hdc_typeof) (Tcl_Interp *interp, const char *hdcname);
static const char * (*hdc_prefixof) (Tcl_Interp *interp, int type, const char *newprefix);
static int (*hdc_list) (Tcl_Interp *interp, int type, const char *out[], int *poutlen);

int debug = 0;
static int Log(char *format, ...);
/****************************************************************
** wmf command
****************************************************************/
static int wmf (ClientData data, Tcl_Interp *interp, int argc, char **argv)
{
  if ( argc > 1 )
  {
    argv++;
    argc--;
    return Wmf(data, interp, argc, argv);
  }

  Tcl_SetResult(interp , usage_message, TCL_STATIC);
  return TCL_ERROR;
}

/* Version information */
static char version_string[] = "0.5.0.2";

/* Version command */
static int Version(ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
  Tcl_SetResult(interp, version_string, TCL_STATIC);
  return TCL_OK;
}

EXPORT(int,Wmf_Init) (Tcl_Interp *interp)
{
#if TCL_MAJOR_VERSION > 7
  #if defined(USE_TCL_STUBS)
    /* First, we have to find the Tcl stubs */
    if ( Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL )
      return TCL_ERROR;
  #endif
#endif
#if TCL_MAJOR_VERSION > 7
  #if defined(USE_TK_STUBS)
    /* First, we have to find the Tk stubs */
    if ( Tk_InitStubs(interp, TCL_VERSION, 0) == NULL )
      using_tk = 0; /* Disable the pasting of DIB images */
  #endif
#endif

  if ( Tcl_PkgRequire(interp, "hdc", "0.2", 0) )
  {
    init_hdc_functions(interp);
    if ( hdc_create == 0 )
      hdc_loaded = 0;
    else
      hdc_loaded = 1;
    init_wmf_dc_contexts(interp);
    init_wmf_contexts(interp);
  }
  else
    hdc_loaded = 0;
  
  Tcl_CreateCommand (interp, "wmf", wmf, 0, 0);

  Tcl_PkgProvide (interp, "wmf", version_string);

  /* Initialize the local hash tables */
  Tcl_InitHashTable(&wmf_dc_hdcs, TCL_ONE_WORD_KEYS);
  Tcl_InitHashTable(&wmf_mf_hdcs, TCL_ONE_WORD_KEYS);
  
  return TCL_OK;
}

EXPORT(int,Wmf_SafeInit) (Tcl_Interp *interp)
{
  return Wmf_Init(interp);
}


static void init_wmf_dc_contexts(Tcl_Interp *interp)
{
  if ( hdc_prefixof )
    hdc_prefixof(interp, WMF_dc_type, "wmfDc");
}

static void init_wmf_contexts(Tcl_Interp *interp)
{
  if ( hdc_prefixof )
    hdc_prefixof(interp, WMF_type, "wmf");
}


static void delete_wmf_dc_contexts(Tcl_Interp *interp)
{
  /* Do NOT use HDC for this, as it may already be unloaded! */
  Tcl_HashEntry *ent;
  Tcl_HashSearch srch;
  for (ent = Tcl_FirstHashEntry(&wmf_dc_hdcs, &srch);
       ent != 0;
       ent = Tcl_NextHashEntry(&srch) )
  {
    HDC hdc;
    if ( (hdc = (HDC)Tcl_GetHashValue(ent) ) != 0 )
    {
      if ( is_valid_dc_hdc(hdc) )
        DeleteDC(hdc);
    }
  }
  Tcl_DeleteHashTable(&wmf_dc_hdcs);
}


static void delete_wmf_contexts(Tcl_Interp *interp)
{
  /* Do NOT use HDC for this, as it may already be unloaded! */
  Tcl_HashEntry *ent;
  Tcl_HashSearch srch;
  for (ent = Tcl_FirstHashEntry(&wmf_mf_hdcs, &srch);
       ent != 0;
       ent = Tcl_NextHashEntry(&srch) )
  {
    HENHMETAFILE hwmf;
    if ( (hwmf = (HENHMETAFILE)Tcl_GetHashValue(ent) ) != 0 )
    {
      if ( is_valid_mf_hdc(hwmf) )
        DeleteEnhMetaFile(hwmf);
    }
  }
  Tcl_DeleteHashTable(&wmf_mf_hdcs);
}

/* The following function is NOT thread safe */
static const char *make_wmf_dc_name(Tcl_Interp *interp, HDC hdc)
{
  add_dc(hdc);
  if (hdc_loaded && hdc_create )
    return hdc_create(interp, hdc, WMF_dc_type);
  else
  {
    static char hex[15];
    sprintf(hex, "0x%lx", hdc);
    return hex;
  }
}


static const char *make_wmf_name(Tcl_Interp *interp, HENHMETAFILE hdc)
{
  add_mf(hdc);
  if ( hdc_loaded == 0 || hdc_create == 0 )
  {
    static char hex[15];
    sprintf(hex, "0x%lx", hdc);
    return hex;
  }
  return hdc_create(interp, hdc, WMF_type);
}


static int wmf_dc_name_valid(Tcl_Interp *interp, const char *name)
{
  if (hdc_loaded && hdc_valid)
    return hdc_valid(interp, name, WMF_dc_type);
  return 0;
}


static int wmf_name_valid(Tcl_Interp *interp, const char *name)
{
  if (hdc_loaded && hdc_valid)
    return hdc_valid(interp, name, WMF_type);
  return 0;
}


static int delete_wmf_dc_name(Tcl_Interp *interp, const char *name)
{
  if (hdc_loaded && hdc_delete)
    return hdc_delete(interp, name);
  return 0;
}


static int delete_wmf_name(Tcl_Interp *interp, const char *name)
{
  if (hdc_loaded && hdc_delete)
    return hdc_delete(interp, name);
  return 0;
}


static HDC get_general_dc (Tcl_Interp *interp, const char *name)
{
  HDC hDC;
  DWORD objtype;
  
  if (hdc_loaded == 0 || hdc_valid == 0 || hdc_get == 0 || hdc_valid(interp, name, -1) == 0 )
  {
    char *strend;
    unsigned long tmp;
    
    /* Perhaps it is a numeric DC */
    tmp = strtoul(name, &strend, 0);
    if ( strend != 0 && strend > name )
    {
      hDC = (HDC)tmp;
    }
    else
    {
      Tcl_AppendResult(interp, "Error: Wrong type of handle for this operation: ",
                       "need a drawing or device context, got: ", name, "\n", 0);
      return 0;
    }
  }
  else
    hDC = (HDC)hdc_get(interp, name);

  /* Now, check the object type: */
  switch ( objtype = GetObjectType((HGDIOBJ)hDC) )
  {
    /* Any of the DC types are OK. */
    case OBJ_DC: case OBJ_MEMDC: case OBJ_METADC: case OBJ_ENHMETADC:
      break;
    /* Anything else is invalid */
    case 0: /* Function failed */
    default:
      hDC = 0;
      Tcl_AppendResult(interp, "Error: Wrong type of handle for this operation: ",
                   "need a drawing or device context, got non-context address: ", 
		   name, "\n", 0);
      break;
  }

  return hDC;
}

static HDC get_wmf_dc(Tcl_Interp *interp, const char *name)
{
  HDC hDC;

  if (hdc_loaded == 0 || hdc_valid == 0 || hdc_get == 0 || hdc_valid(interp, name, WMF_dc_type) == 0 )
  {
    char *strend;
    unsigned long tmp;
    
    /* Perhaps it is a numeric DC */
    tmp = strtoul(name, &strend, 0);
    if ( strend != 0 && strend > name )
    {
      DWORD objtype = GetObjectType((HGDIOBJ)tmp);
      switch (objtype)
      {
        /* Any of the DC types are OK. */
        case OBJ_DC: case OBJ_MEMDC: case OBJ_METADC: case OBJ_ENHMETADC:
          break;
        /* Anything else is invalid */
        case 0: /* Function failed */
        default:
          tmp = 0;
          Tcl_AppendResult(interp, "Error: Wrong type of handle for this operation: ",
                       "need a metafile drawing context, got non-context address: ", name, "\n", 0);
          break;
      }
      return (HDC)tmp;
    }
    else
    {
      Tcl_AppendResult(interp, "Error: Wrong type of handle for this operation: ",
                       "need a metafile drawing context, got: ", name, "\n", 0);
      return 0;
    }
  }
  
  hDC = (HDC)hdc_get(interp, name);
  return hDC;
}


static HENHMETAFILE get_wmf(Tcl_Interp *interp, const char *name)
{
  HENHMETAFILE hDC;

  if (hdc_loaded == 0 || hdc_valid == 0 || hdc_get == 0 || hdc_valid(interp, name, WMF_type) == 0 )
  {
    char *strend;
    unsigned long tmp;
    
    /* Perhaps it is a numeric DC */
    tmp = strtoul(name, &strend, 0);
    if ( strend != 0 && strend > name )
    {
      DWORD objtype = GetObjectType((HGDIOBJ)tmp);
      switch (objtype)
      {
        /* Any of the METAFILE types are OK. */
        case OBJ_METAFILE: case OBJ_ENHMETAFILE:
          break;
        /* Anything else is invalid */
        case 0: /* Function failed */
        default:
          tmp = 0;
          Tcl_AppendResult(interp, "Error: Wrong type of handle for this operation: ",
                       "need a metafile drawing context, got non-context address: ", name, "\n", 0);
          break;
      }
      return (HENHMETAFILE)tmp;
    }
    else
    {
      Tcl_AppendResult(interp, "Error: Wrong type of handle for this operation: ",
                   "need a metafile drawing context, got non-context address: ", name, "\n", 0);
      return 0;
    }
  }
  
  hDC = (HENHMETAFILE)hdc_get(interp, name);

  return hDC;
}

static int Wmf (ClientData data, Tcl_Interp *interp, int argc, char **argv)
{
  static struct wmfcmdstruct {
    const char *name;
    int (*func)(ClientData data, Tcl_Interp *interp, int argc, char **argv);
  } subcommands[] =
  {
    { "close",  WmfClose  },
    { "copy",   WmfCopy   },
    { "delete", WmfDelete },
    { "info",   WmfInfo   },
    { "open",   WmfOpen   },
    { "paste",  WmfPaste  },
    { "play",   WmfCopyDC },
    { "read",   WmfRead },
    { "version", Version  }
  };
  int i;

  if ( argc > 0 )
    for (i=0; i< sizeof(subcommands) / sizeof(subcommands[0]); i++ )
      if ( strcmp(subcommands[i].name, argv[0]) == 0 )
        return subcommands[i].func(data, interp, argc-1, argv+1);
  Tcl_SetResult(interp, usage_message, TCL_STATIC);
  return TCL_ERROR;
}


static int WmfOpen (ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
  static char usage_message[] = "wmf open [-file filename] [-width w -height h] "
                                "[-app app] [-title title] [-hDC hdc] [-init enffile]";

  HDC refHDC   = 0;
  HDC hDC;
  int do_width = 0;
  int do_height = 0;
  int do_hdc = 0;
  int do_emf = 0;
  const char *hdcstring;
  
  int width = 0;
  int height = 0;
  const char *file = 0;
  const char *app = "WMF";
  const char *title = "Picture";
  const char *emffile = 0;
  int i;

  char *cp;
  int alen, tlen;
  
  for (i = 0; i < argc; i++ )
  {
    if ( strcmp(argv[i], "-file") == 0 )
    {
      i++;
      file = argv[i];
    }
    else if ( strcmp(argv[i], "-width") == 0 )
    {
      i++;
      width = atoi(argv[i]);
      do_width = 1;
    }
    else if ( strcmp(argv[i], "-height") == 0 )
    {
      i++;
      height = atoi(argv[i]);
      do_height = 1;
    }
    else if ( strcmp(argv[i], "-app") == 0 )
      app = argv[++i];
    else if ( strcmp(argv[i], "-title") == 0 )
      title = argv[++i];
    else if ( strcmp(argv[i], "-hDC") == 0 )
    {
      do_hdc = 1;
      hdcstring=argv[++i];
    }
    else if ( strcmp(argv[i], "-init") == 0 )
    {
      do_emf = 1;
      emffile = argv[++i];
    }
  }

  alen = strlen(app);
  tlen = strlen(title);
  cp = Tcl_Alloc(alen+1+tlen+1+1);
  if (cp)
  {
    strcpy(cp, app);
    strcpy(cp+alen+1, title);
    cp[alen+1+tlen+1] = '\0';
  }

  if ( do_hdc )
  {
    refHDC = convert_hdc(hdcstring);
    if ( refHDC == 0 )
    {
      if (cp)
        Tcl_Free(cp);
      Tcl_AppendResult(interp, "wmf open: Unrecognized reference hdc: ", hdcstring, 0);
      return TCL_ERROR;
    }
  }
  else
  {
    refHDC = CreateDC("DISPLAY", 0, 0, 0);
  }

  if ( do_width && do_height )
  {
    RECT r;
    r.top = 0;
    r.left = 0;
    r.bottom = height;
    r.right = width;
    hDC = CreateEnhMetaFile(refHDC,file,&r,cp);
  }
  else
    hDC = CreateEnhMetaFile(refHDC,file,0,cp);

  /* If inited, do the work here */
  if ( do_emf && hDC )
  {
    HENHMETAFILE hemf;
    /* See if the file exists */
    /* Open it */
    if ( (hemf = GetEnhMetaFile(emffile)) != NULL ) /* Play it into the new metafile */
    {
      LPENHMETAHEADER lpemh;
      if ( wmfHeader(hemf, &lpemh) == 0 )
      {
        /* RECT and RECTL are identical, per SDK */
        PlayEnhMetaFile ( hDC, hemf, (CONST RECT *)(&lpemh->rclBounds));
        Tcl_Free((char *)lpemh);
      }
    }
    DeleteEnhMetaFile(hemf);
  }
  
  /* I must clean up the reference DC whenever this extension creates it */
  if (do_hdc == 0 )
    DeleteDC(refHDC);
    
  if (cp)
    Tcl_Free(cp);

  /* Check for failure of creating the hDC */
  if ( hDC == 0 )
  {
    long err = GetLastError();
    char buffer[14+1];
    sprintf(buffer, "%ld", err);
    Tcl_AppendResult(interp, "Creation of Windows Metafile failed. Internal error code: ",
                     buffer, 0);
  }

  /* Now we can return the hdc! Until SetResult takes a const char *, need a cast... */
  Tcl_SetResult(interp, (char *)make_wmf_dc_name(interp, hDC), TCL_STATIC);
  return TCL_OK;
}


static int WmfClose (ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
  HENHMETAFILE hwmf;
  HDC hDC;
  const char *retval;
  const char *usage = "wmf close wmfHDC";

  if ( argc == 0 )
  {
    Tcl_SetResult(interp, (char *)usage, TCL_STATIC);
    return TCL_ERROR;
  }

  if ( ( hDC = get_wmf_dc (interp, argv[0]) ) == 0 )
  {
    Tcl_AppendResult(interp, "Can't get dc from ", argv[0], 0);
    return TCL_ERROR;
  }

  hwmf = CloseEnhMetaFile(hDC);
  
  /* Assumption: the hDC doesn't need to be deleted. */

  /* After closing the metafile, we must delete the name */
  delete_wmf_dc_name(interp, argv[0]);  
  delete_dc(hDC);

  retval = make_wmf_name(interp, hwmf);

  Tcl_SetResult(interp, (char *)retval, TCL_VOLATILE);
  
  return TCL_OK;
}

static int WmfRead (ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
    int status = TCL_OK;
    HENHMETAFILE hwmf;
    const char *retval;
    const char *usage = "wmf read emfFileName"; /* Handle by extension, using .emf and .wmf? */
    
    if ( argc == 0 ) {
        Tcl_SetResult(interp, (char *)usage, TCL_STATIC);
        return TCL_ERROR;
    }
    
    hwmf = GetEnhMetaFile(argv[0]);
    if ( hwmf == NULL ) {
        Tcl_AppendResult(interp, "Cannot open metafile ", argv[0], " successfully", 0);
        status = TCL_ERROR;
    } else {
        retval = make_wmf_name(interp, hwmf);
        Tcl_SetResult(interp, (char *)retval, TCL_VOLATILE);
    }
    return status;
}

#define WMF_INVALID_HANDLE    1
#define WMF_OUT_OF_MEMORY     2
#define WMF_GET_HEADER_FAILED 3

static int wmfHeader(HENHMETAFILE hwmf, LPENHMETAHEADER *lpemh)
{
  UINT siz;

  *lpemh = 0;
  if ( ( siz = GetEnhMetaFileHeader(hwmf, 0, NULL) ) == 0 )
    return WMF_INVALID_HANDLE;
  if ( (*lpemh = (LPENHMETAHEADER)Tcl_Alloc(siz)) == 0 )
    return WMF_OUT_OF_MEMORY;
  if ( (siz = GetEnhMetaFileHeader(hwmf, siz, *lpemh) ) == 0 )
  {
    Tcl_Free((char *)lpemh);
    *lpemh = 0;
    return WMF_GET_HEADER_FAILED;
  }
  return 0;
}

#define WMF_CANT_GET_DISPLAY_HDC    33
#define WMF_CANT_GET_METAFILE_BITS  34
static int wmfCopyToRegularMetafile(HENHMETAFILE hwmf, HMETAFILE *phmf)
{
  int status = 0;
  HDC hdc;
  char *buffer=0;
  HMETAFILE hmf;
  int siz;

  /* Set the metafile handle to 0 */
  *phmf = 0;

  /* Get the enhanced metafile into a regular metafile */
  if ( (hdc = CreateDC("Display", 0, 0, 0)) == NULL )
  {
    /* Couldn't get a handle! Should never happen */
    return WMF_CANT_GET_DISPLAY_HDC;
  }
  
  if ( (siz = GetWinMetaFileBits(hwmf, 0, NULL, MM_TEXT, hdc)) <= 0 )
  {
    DeleteDC(hdc);
    return WMF_CANT_GET_METAFILE_BITS;
  }
  
  if ( (buffer = Tcl_Alloc(siz)) == 0 )
  {
    DeleteDC(hdc);
    return WMF_OUT_OF_MEMORY;
  }
  
  if ( (siz = GetWinMetaFileBits(hwmf, siz, buffer, MM_TEXT, hdc)) <= 0 )
  {
    DeleteDC(hdc);
    Tcl_Free(buffer);
    return WMF_CANT_GET_METAFILE_BITS;
  }
  
  DeleteDC(hdc);

  hmf = SetMetaFileBitsEx(siz, buffer);
  Tcl_Free(buffer);

  *phmf = CopyMetaFile(hmf, NULL);
  /* However, we can release hmf, since it was created just for this purpose */
  DeleteMetaFile(hmf);
  
  return status;
}

static int WmfCopy (ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
  static char *usage = "wmf copy wmfDC [-file filename] [-type metafile|enhanced|both]\n"
                       "    defaults to clipboard copy of both";
  HENHMETAFILE hwmf;
  int do_file = 0;
  const char *filename;
  int metafile_type = 3; /* 1=Enhanced metafile, 2=metafile: bit masks */
  int i;
  int status=TCL_OK;
  
  if ( argc == 0 )
  {
    Tcl_SetResult(interp, usage, TCL_STATIC);
    return TCL_ERROR;
  }

  if ( ( hwmf = get_wmf (interp, argv[0]) ) == 0 )
  {
    Tcl_AppendResult(interp, usage, 0);
    return TCL_ERROR;
  }

  for (i = 1; i<argc; i++)
  {
    if ( strcmp(argv[i], "-file") == 0 )
    {
      do_file = 1;
      if ( (i+1) < argc && argv[i+1][0] != '-' )
        filename = argv[++i];
      else
        filename = "picture.emf";
    }
    else if ( strcmp(argv[i], "-type") == 0 )
    {
      i++;
      if ( i < argc )
      {
         switch (argv[i][0])
         {
           case 'M': case 'm':
             metafile_type = 2;
             break;
           case 'B': case 'b':
             metafile_type = 3;
             break;
           case 'E': case 'e':
           default:
             metafile_type = 1;
             break;
         }
      }
    }
  }

  /* For now, files are only done in enhanced format */
  if ( do_file )
  {
    HENHMETAFILE hcopy = CopyEnhMetaFile(hwmf,filename);
    DeleteEnhMetaFile(hcopy);
  }
  else /* Clipboard */
  {
    if ( OpenClipboard(0) == FALSE )
    {
      Tcl_AppendResult(interp, "Can't open the clipboard", 0);
      return TCL_ERROR;
    }

    if ( EmptyClipboard() == FALSE )
    {
      Tcl_AppendResult(interp, "Can't claim ownership of the clipboard", 0);
      CloseClipboard();
      return TCL_ERROR;
    }

    /*
    ** Now that we have the clipboard, build the metafile properly for it:
    */
    if ( metafile_type & 1 )   /* Enhanced metafile */
    {
      HENHMETAFILE hcopy = CopyEnhMetaFile(hwmf, NULL);
      SetClipboardData(CF_ENHMETAFILE, hcopy);
      /* We must NOT free hcopy, or lock its memory--the clipboard owns it */
    }

    if ( metafile_type & 2 )    /* Old-style windows metafile */
    {
      LPENHMETAHEADER lpemh;
      LPMETAFILEPICT  lpMFP;
      int result;
      POINT pt;
      HANDLE hMem;

      
      switch ( result = wmfHeader(hwmf, &lpemh) )
      {
        case 0: /* Normal */
          break;
        case WMF_INVALID_HANDLE: /* Invalid handle */
          Tcl_AppendResult(interp, "Invalid metafile handle ", argv[0], 0);
          CloseClipboard();
          return TCL_ERROR;
        case WMF_OUT_OF_MEMORY: /* Out of Memory */
          Tcl_AppendResult(interp, "Out of Memory", 0);
          CloseClipboard();
          return TCL_ERROR;
        case WMF_GET_HEADER_FAILED: /* Can't get header info */
          Tcl_AppendResult(interp, "Can't get metafile info on ", argv[0], 0);
          CloseClipboard();
          return TCL_ERROR;
        default: /* Unknown error */
          Tcl_AppendResult(interp, "Unknown error in getting metafile information", 0);
          CloseClipboard();
          return TCL_ERROR;
      }

      /* Get the metafile stuff set up */
      hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_DDESHARE, sizeof(METAFILEPICT));
      if ( hMem == NULL )
      {
         Tcl_AppendResult(interp, "Cannot allocate global memory for metafile", 0);
         CloseClipboard();
         return TCL_ERROR;
      }
      
      lpMFP = (LPMETAFILEPICT)GlobalLock(hMem);
      if ( lpMFP == NULL )
      {
         Tcl_AppendResult(interp, "Cannot lock global memory for metafile", 0);
         CloseClipboard();
         GlobalFree(hMem);
         return TCL_ERROR;
      }
      
      lpMFP->mm = MM_ANISOTROPIC;
      if ( lpemh )
      {
        pt.x = lpemh->rclFrame.right - lpemh->rclFrame.left;
        pt.y = lpemh->rclFrame.bottom - lpemh->rclFrame.top;
        /* Could do some mapping mode stuff now if we wanted... */
        Tcl_Free((char *)lpemh);
      }
      else
      {
        /* We have to have SOME kind of default... */
        pt.x = 640;
        pt.y = 480;
      }
    
      /* Set width and height--this might need to be scaled */
      lpMFP->xExt = pt.x>=0?pt.x:-pt.x;
      lpMFP->yExt = pt.y>=0?pt.y:-pt.y;

      switch ( result = wmfCopyToRegularMetafile(hwmf, &(lpMFP->hMF)) )
      {
        case 0: /* normal */
          break;
        case WMF_OUT_OF_MEMORY:
          status = TCL_ERROR;
          Tcl_AppendResult(interp, "Out of memory in metafile conversion", 0 );
          break;
        case WMF_CANT_GET_DISPLAY_HDC:
          status = TCL_ERROR;
          Tcl_AppendResult(interp, "Can't get reference drawing context for metafile conversion", 0);
          break;
        case WMF_CANT_GET_METAFILE_BITS:
          status = TCL_ERROR;
          Tcl_AppendResult(interp, "Can't extract metafile records for clipboard", 0);
          break;
      }
      GlobalUnlock(hMem);
      if ( result == 0 )
        SetClipboardData(CF_METAFILEPICT, hMem);
      else
        GlobalFree(hMem); /* Can't use it now! */
    }
    CloseClipboard();
  }  

  return status;
}

static int WmfCopyDC  (ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
  static char *usage = "wmf play wmfDC hdc [-bounds \"x0 y0 x1 y1\"]";
  HENHMETAFILE hwmf;
  HDC hDC;
  RECT rect;
  
  int i;
  int status=TCL_OK;

  /* Currently requires at least 2 arguments */
  if ( argc < 2 )
  {
    Tcl_SetResult(interp, usage, TCL_STATIC);
    return TCL_ERROR;
  }

  /* Now I need a bounding rectangle, and I'm all set */
  rect.left = rect.top = 0;
  rect.right = rect.bottom = 2000;

  if ( ( hwmf = get_wmf (interp, argv[0]) ) == 0 )
  {
    Tcl_AppendResult(interp, "Metafile context ", argv[0], " not recognized\n", 0);
    Tcl_AppendResult(interp, usage, 0);
    return TCL_ERROR;
  }

  if ( ( hDC = get_general_dc (interp, argv[1]) ) == 0 )
  {
    Tcl_AppendResult(interp, "Target context ", argv[1], " not recognized\n", 0);
    Tcl_AppendResult(interp, usage, 0);
    return TCL_ERROR;
  }

  for ( i = 2; i < argc; i++ )
  {
    if ( strcmp(argv[i], "-bounds") == 0 )
    {
      long r, l, t, b;
      i++;
      if ( i < argc && sscanf(argv[i], "%ld%ld%ld%ld", &l, &t, &r, &b ) == 4 )
      {
        rect.top = t;
        rect.left = l;
        rect.bottom = b;
        rect.right = r;
      }
    }
  }
  
  /* The basic processing is to enumerate the records, and play them into the
  ** given context
  */
  if ( PlayEnhMetaFile(hDC, hwmf, &rect) <= 0 )
  {
    char errstring[12+1];
    sprintf(errstring, "%ld", GetLastError());
    Tcl_AppendResult(interp, "wmf play failed with error ", errstring, 0 );
    status = TCL_ERROR;
  }
  return status;
}

static const char *WmfIntToGDIFunction(int fn)
{
  static 
  struct gdi_lookup {
    int num;
    char *name;
  } fns[] =
  {
    { EMR_HEADER                      , "HEADER" },
    { EMR_POLYBEZIER                  , "POLYBEZIER" },
    { EMR_POLYGON                     , "POLYGON" },
    { EMR_POLYLINE                    , "POLYLINE" },
    { EMR_POLYBEZIERTO                , "POLYBEZIERTO" },
    { EMR_POLYLINETO                  , "POLYLINETO" },
    { EMR_POLYPOLYLINE                , "POLYPOLYLINE" },
    { EMR_POLYPOLYGON                 , "POLYPOLYGON" },
    { EMR_SETWINDOWEXTEX              , "SETWINDOWEXTEX" },
    { EMR_SETWINDOWORGEX              , "SETWINDOWORGEX" },
    { EMR_SETVIEWPORTEXTEX            , "SETVIEWPORTEXTEX" },
    { EMR_SETVIEWPORTORGEX            , "SETVIEWPORTORGEX" },
    { EMR_SETBRUSHORGEX               , "SETBRUSHORGEX" },
    { EMR_EOF                         , "EOF" },
    { EMR_SETPIXELV                   , "SETPIXELV" },
    { EMR_SETMAPPERFLAGS              , "SETMAPPERFLAGS" },
    { EMR_SETMAPMODE                  , "SETMAPMODE" },
    { EMR_SETBKMODE                   , "SETBKMODE" },
    { EMR_SETPOLYFILLMODE             , "SETPOLYFILLMODE" },
    { EMR_SETROP2                     , "SETROP2" },
    { EMR_SETSTRETCHBLTMODE           , "SETSTRETCHBLTMODE" },
    { EMR_SETTEXTALIGN                , "SETTEXTALIGN" },
    { EMR_SETCOLORADJUSTMENT          , "SETCOLORADJUSTMENT" },
    { EMR_SETTEXTCOLOR                , "SETTEXTCOLOR" },
    { EMR_SETBKCOLOR                  , "SETBKCOLOR" },
    { EMR_OFFSETCLIPRGN               , "OFFSETCLIPRGN" },
    { EMR_MOVETOEX                    , "MOVETOEX" },
    { EMR_SETMETARGN                  , "SETMETARGN" },
    { EMR_EXCLUDECLIPRECT             , "EXCLUDECLIPRECT" },
    { EMR_INTERSECTCLIPRECT           , "INTERSECTCLIPRECT" },
    { EMR_SCALEVIEWPORTEXTEX          , "SCALEVIEWPORTEXTEX" },
    { EMR_SCALEWINDOWEXTEX            , "SCALEWINDOWEXTEX" },
    { EMR_SAVEDC                      , "SAVEDC" },
    { EMR_RESTOREDC                   , "RESTOREDC" },
    { EMR_SETWORLDTRANSFORM           , "SETWORLDTRANSFORM" },
    { EMR_MODIFYWORLDTRANSFORM        , "MODIFYWORLDTRANSFORM" },
    { EMR_SELECTOBJECT                , "SELECTOBJECT" },
    { EMR_CREATEPEN                   , "CREATEPEN" },
    { EMR_CREATEBRUSHINDIRECT         , "CREATEBRUSHINDIRECT" },
    { EMR_DELETEOBJECT                , "DELETEOBJECT" },
    { EMR_ANGLEARC                    , "ANGLEARC" },
    { EMR_ELLIPSE                     , "ELLIPSE" },
    { EMR_RECTANGLE                   , "RECTANGLE" },
    { EMR_ROUNDRECT                   , "ROUNDRECT" },
    { EMR_ARC                         , "ARC" },
    { EMR_CHORD                       , "CHORD" },
    { EMR_PIE                         , "PIE" },
    { EMR_SELECTPALETTE               , "SELECTPALETTE" },
    { EMR_CREATEPALETTE               , "CREATEPALETTE" },
    { EMR_SETPALETTEENTRIES           , "SETPALETTEENTRIES" },
    { EMR_RESIZEPALETTE               , "RESIZEPALETTE" },
    { EMR_REALIZEPALETTE              , "REALIZEPALETTE" },
    { EMR_EXTFLOODFILL                , "EXTFLOODFILL" },
    { EMR_LINETO                      , "LINETO" },
    { EMR_ARCTO                       , "ARCTO" },
    { EMR_POLYDRAW                    , "POLYDRAW" },
    { EMR_SETARCDIRECTION             , "SETARCDIRECTION" },
    { EMR_SETMITERLIMIT               , "SETMITERLIMIT" },
    { EMR_BEGINPATH                   , "BEGINPATH" },
    { EMR_ENDPATH                     , "ENDPATH" },
    { EMR_CLOSEFIGURE                 , "CLOSEFIGURE" },
    { EMR_FILLPATH                    , "FILLPATH" },
    { EMR_STROKEANDFILLPATH           , "STROKEANDFILLPATH" },
    { EMR_STROKEPATH                  , "STROKEPATH" },
    { EMR_FLATTENPATH                 , "FLATTENPATH" },
    { EMR_WIDENPATH                   , "WIDENPATH" },
    { EMR_SELECTCLIPPATH              , "SELECTCLIPPATH" },
    { EMR_ABORTPATH                   , "ABORTPATH" },

    { EMR_GDICOMMENT                  , "GDICOMMENT" },
    { EMR_FILLRGN                     , "FILLRGN" },
    { EMR_FRAMERGN                    , "FRAMERGN" },
    { EMR_INVERTRGN                   , "INVERTRGN" },
    { EMR_PAINTRGN                    , "PAINTRGN" },
    { EMR_EXTSELECTCLIPRGN            , "EXTSELECTCLIPRGN" },
    { EMR_BITBLT                      , "BITBLT" },
    { EMR_STRETCHBLT                  , "STRETCHBLT" },
    { EMR_MASKBLT                     , "MASKBLT" },
    { EMR_PLGBLT                      , "PLGBLT" },
    { EMR_SETDIBITSTODEVICE           , "SETDIBITSTODEVICE" },
    { EMR_STRETCHDIBITS               , "STRETCHDIBITS" },
    { EMR_EXTCREATEFONTINDIRECTW      , "EXTCREATEFONTINDIRECTW" },
    { EMR_EXTTEXTOUTA                 , "EXTTEXTOUTA" },
    { EMR_EXTTEXTOUTW                 , "EXTTEXTOUTW" },
    { EMR_POLYBEZIER16                , "POLYBEZIER16" },
    { EMR_POLYGON16                   , "POLYGON16" },
    { EMR_POLYLINE16                  , "POLYLINE16" },
    { EMR_POLYBEZIERTO16              , "POLYBEZIERTO16" },
    { EMR_POLYLINETO16                , "POLYLINETO16" },
    { EMR_POLYPOLYLINE16              , "POLYPOLYLINE16" },
    { EMR_POLYPOLYGON16               , "POLYPOLYGON16" },
    { EMR_POLYDRAW16                  , "POLYDRAW16" },
    { EMR_CREATEMONOBRUSH             , "CREATEMONOBRUSH" },
    { EMR_CREATEDIBPATTERNBRUSHPT     , "CREATEDIBPATTERNBRUSHPT" },
    { EMR_EXTCREATEPEN                , "EXTCREATEPEN" },
    { EMR_POLYTEXTOUTA                , "POLYTEXTOUTA" },
    { EMR_POLYTEXTOUTW                , "POLYTEXTOUTW" },

#if(WINVER >= 0x0400)
    { EMR_SETICMMODE                  , "SETICMMODE" },
    { EMR_CREATECOLORSPACE            , "CREATECOLORSPACE" },
    { EMR_SETCOLORSPACE              , "SETCOLORSPACE" },
    { EMR_DELETECOLORSPACE           , "DELETECOLORSPACE" },
    { EMR_GLSRECORD                  , "GLSRECORD" },
    { EMR_GLSBOUNDEDRECORD           , "GLSBOUNDEDRECORD" },
    { EMR_PIXELFORMAT                , "PIXELFORMAT" },
#endif /* WINVER >= 0x0400 */
      
/* These constants are not in XP */
#if (WINVER >= 0x0500) && ( WINVER < 0x0501 )
    { EMR_DRAWESCAPE                 , "DRAWESCAPE" },
    { EMR_EXTESCAPE                  , "EXTESCAPE" },
    { EMR_STARTDOC                   , "STARTDOC" },
    { EMR_SMALLTEXTOUT               , "SMALLTEXTOUT" },
    { EMR_FORCEUFIMAPPING            , "FORCEUFIMAPPING" },
    { EMR_NAMEDESCAPE                , "NAMEDESCAPE" },
    { EMR_COLORCORRECTPALETTE        , "COLORCORRECTPALETTE" },
    { EMR_SETICMPROFILEA             , "SETICMPROFILEA" },
    { EMR_SETICMPROFILEW             , "SETICMPROFILEW" },
    { EMR_ALPHABLEND                 , "ALPHABLEND" },
    { EMR_ALPHADIBBLEND              , "ALPHADIBBLEND" },
    { EMR_TRANSPARENTBLT             , "TRANSPARENTBLT" },
    { EMR_TRANSPARENTDIB             , "TRANSPARENTDIB" },
    { EMR_GRADIENTFILL               , "GRADIENTFILL" },
    { EMR_SETLINKEDUFIS              , "SETLINKEDUFIS" },
    { EMR_SETTEXTJUSTIFICATION       , "SETTEXTJUSTIFICATION" },
#endif
  };
  int fnsize = sizeof(fns) / sizeof ( struct gdi_lookup );
  int i;

  for (i=0; i<fnsize; i++)
    if ( fns[i].num == fn )
      return fns[i].name;
  return 0;
}

static int output_style = 0;

static int CALLBACK WmfEnhMetaFileProc (HDC hDC, HANDLETABLE FAR *lpHTable,
                                        ENHMETARECORD FAR *lpEMFR,
					int nObj,
					LPARAM tcl)
{
  int nparams;
  Tcl_Interp *interp = (Tcl_Interp *)tcl;
  const char *name;
  char num[15+1+15+1];
  int i;

  static int usewidth = 0;
  static int usefill  = 0;
  static int useoutline = 0;
  static int usetextcolor = 0;
  static int usefontname = 0;
    
  static int do_path_x = 0;
  static int do_path_y = 0;
  
  static char width[15];
  static char fill[16];
  static char outline[16];
  static char textcolor[16];
  static Tcl_DString fontname;
  static char fontsize[16];

  int gotRecord = 1;

  const char *ostring;
  
  Tcl_DString tds;

  if (lpEMFR == 0 )
    return 0;

  Tcl_DStringInit(&tds);

  nparams = (int)( lpEMFR->nSize - 2L * sizeof(DWORD) ) / sizeof (DWORD) ;


  if ( debug )
  {
    Log("WMF: Function %s; %d parameters; size %ld\n", 
        WmfIntToGDIFunction(lpEMFR->iType), 
	nparams,
        (long)lpEMFR->nSize);
  }

  if ( output_style <= 1 ) /* Ordinary verbose: spell out the metafile */
  {
    Tcl_AppendResult(interp, "{ ", 0);
    sprintf(num, "%d", nparams);
    if ( (name = WmfIntToGDIFunction(lpEMFR->iType)) != 0 )
      Tcl_AppendResult(interp, "  ", name, " (", num, "): ", 0);
    else
    {
      sprintf(num, "%d", lpEMFR->iType);
      Tcl_AppendResult(interp, "  unknown: ", num, "\n", 0 );
    }
    for (i=0; i<nparams && i < 200 ; i++)
    {
      sprintf(num, "%d", lpEMFR->dParm[i]);
      Tcl_AppendResult(interp, num, " ", 0);
    }
    Tcl_AppendResult(interp, "} ", 0);
  }
  else /* Output only what can be made sensible on a canvas */
       /* Note: This will lose fine points, like arrowed lines, etc. */
       /* Note: The current mapping is incomplete */
  {
    switch ( lpEMFR->iType )
    {
      case EMR_RECTANGLE:
        Tcl_DStringAppend(&tds, "rectangle ", -1);

        for (i=0; i<nparams; i++)
        {
          sprintf(num, "%d ", lpEMFR->dParm[i]);
          Tcl_DStringAppend(&tds, num, -1);
        }
        if ( usefill ) {
          Tcl_DStringAppend(&tds, " -fill ", -1);
          Tcl_DStringAppend(&tds, fill, -1);
          usefill = 0;
        }
        if ( useoutline ) {
          Tcl_DStringAppend(&tds, " -outline ", -1);
          Tcl_DStringAppend(&tds, outline, -1);
          useoutline = 0;
        }
        if ( usewidth ) {
          Tcl_DStringAppend(&tds, " -width ", -1);
          Tcl_DStringAppend(&tds, width, -1);
          usewidth = 0;
        }
      
        ostring = Tcl_DStringValue(&tds);
        Tcl_AppendElement(interp, ostring);
        break;
      case EMR_ELLIPSE:
        Tcl_DStringAppend(&tds, "oval ", -1);
  
        for (i=0; i<nparams; i++)
        {
          sprintf(num, "%d ", lpEMFR->dParm[i]);
          Tcl_DStringAppend(&tds, num, -1);
        }

        if ( usefill )
        {
          Tcl_DStringAppend(&tds, " -fill ", -1);
          Tcl_DStringAppend(&tds, fill, -1);
          usefill = 0;
        }

        if ( useoutline ) 
	{
          Tcl_DStringAppend(&tds, " -outline ", -1);
          Tcl_DStringAppend(&tds, outline, -1);
          useoutline = 0;
        }
        
        if ( usewidth ) 
	{
          Tcl_DStringAppend(&tds, " -width ", -1);
          Tcl_DStringAppend(&tds, width, -1);
          usewidth = 0;
        }
      
        ostring = Tcl_DStringValue(&tds);
        Tcl_AppendElement(interp, ostring);
        break;
      case EMR_POLYLINE: case EMR_POLYLINE16:
        {
          int npoints;
          /* The 5th line contains the number of points */
          Tcl_DStringAppend(&tds, "line ", -1);

          /* The 5th parameter counts the points */
          if ( nparams >= 5 )
          {
            npoints = lpEMFR->dParm[4];

            for (i=0; i<npoints; i++)
            {
              sprintf(num, "%d %d ", lpEMFR->dParm[5+i] & 0xffff, lpEMFR->dParm[5+i] >> 16 );
              Tcl_DStringAppend(&tds, num, -1);
            }
          }

/*
          if ( usefill ) 
	  {
            Tcl_DStringAppend(&tds, " -fill ", -1);
            Tcl_DStringAppend(&tds, fill, -1);
            usefill = 0;
          }
*/

          if ( useoutline ) 
	  {
            Tcl_DStringAppend(&tds, " -fill ", -1);
            Tcl_DStringAppend(&tds, outline, -1);
            useoutline = 0;
          }

          if ( usewidth ) 
	  {
            Tcl_DStringAppend(&tds, " -width ", -1);
            Tcl_DStringAppend(&tds, width, -1);
            usewidth = 0;
          }
      
          ostring = Tcl_DStringValue(&tds);
          Tcl_AppendElement(interp, ostring);
        }
        break;
      case EMR_CREATEBRUSHINDIRECT:
        {
          /* The 3rd parameter is the RGB triplet */
          if ( nparams >= 3 )
          {
            int x1 = lpEMFR->dParm[2] & 0xff;
            int x2 = (lpEMFR->dParm[2] >> 8 ) & 0xff;
            int x3 = (lpEMFR->dParm[2] >> 16) & 0xff;
            int x4 = (lpEMFR->dParm[2] >> 24) & 0xff;
            sprintf(fill, "#%02x%02x%02x", x1,x2,x3);
            usefill = 1;
          }
        }
        break;
      case EMR_CREATEPEN:
        {
          /* The 5th parameter is the RGB triplet */
          /* The 3rd parameter is the width */
          if ( nparams >= 5 )
          {
            int x1 = lpEMFR->dParm[4] & 0xff;
            int x2 = (lpEMFR->dParm[4] >> 8 ) & 0xff;
            int x3 = (lpEMFR->dParm[4] >> 16) & 0xff;
            int x4 = (lpEMFR->dParm[4] >> 24) & 0xff;
  
            sprintf(width, "%d", lpEMFR->dParm[2]);
            usewidth = 1;
  
            sprintf(outline, "#%02x%02x%02x", x1, x2, x3);
            useoutline = 1;
          }
        }
      break;
      case EMR_EXTCREATEPEN:
        {
          /* The 9th parameter is the RGB triplet */
          /* The 7th parameter is the width */
          if ( nparams >= 5 )
          {
            int x1 = lpEMFR->dParm[8] & 0xff;
            int x2 = (lpEMFR->dParm[8] >> 8 ) & 0xff;
            int x3 = (lpEMFR->dParm[8] >> 16) & 0xff;
            int x4 = (lpEMFR->dParm[8] >> 24) & 0xff;
  
            sprintf(width, "%d", lpEMFR->dParm[6]);
            usewidth = 1;
  
            sprintf(outline, "#%02x%02x%02x", x1, x2, x3);
            useoutline = 1;
          }
        }
      break;
      case EMR_POLYGON16:
        {
          int npoints;
          /* The 5th line contains the number of points */
          Tcl_DStringAppend(&tds, "polygon ", -1);

          /* The 5th parameter counts the points */
          if ( nparams >= 5 )
          {
            npoints = lpEMFR->dParm[4];
  
            for (i=0; i<npoints; i++)
            {
              sprintf(num, "%d %d ", lpEMFR->dParm[5+i] & 0xffff, lpEMFR->dParm[5+i] >> 16 );
              Tcl_DStringAppend(&tds, num, -1);
            }
          }

          if ( usefill ) 
	  {
            Tcl_DStringAppend(&tds, " -fill ", -1);
            Tcl_DStringAppend(&tds, fill, -1);
            usefill = 0;
          }
          
          if ( useoutline ) 
	  {
            Tcl_DStringAppend(&tds, " -outline ", -1);
            Tcl_DStringAppend(&tds, outline, -1);
            useoutline = 0;
          }
          
          if ( usewidth ) 
	  {
            Tcl_DStringAppend(&tds, " -width ", -1);
            Tcl_DStringAppend(&tds, width, -1);
            usewidth = 0;
          }
      
          ostring = Tcl_DStringValue(&tds);
          Tcl_AppendElement(interp, ostring);
        }
        break;
      case EMR_EXTTEXTOUTW: /* Hard case: textual output */
        /* First 4 coordinates are the bounding box? */
        /* I think there are some justification flags here too */
        /* The length of the string is at arg 9? */
        /* The text is in unicode starting at argument 17, and is in unicode */
        /* After the text is an array of character widths, one per character */
        {
          int nchars = lpEMFR->dParm[9];
          Tcl_DString body;
          const char *bstring;
  
          Tcl_DStringInit(&body);
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )
          Tcl_UniCharToUtfDString((CONST Tcl_UniChar *)(&lpEMFR->dParm[17]), nchars, &body);
#else
          /* Tcl UniChar functions not supported. Try a windows hack */
          {
            char *cp;
            int siz;

            siz = WideCharToMultiByte (CP_OEMCP, 0, (LPCWSTR)(&lpEMFR->dParm[17]), -1, 0, 0, 0, 0);
            if ( siz > 0 )
            {
              cp = Tcl_Alloc(siz + 1);
              cp[0] = '\0';
              WideCharToMultiByte(CP_OEMCP, 0, (LPCWSTR)(&lpEMFR->dParm[17]), -1, cp, siz+1, 0, 0);
              Tcl_DStringAppend(&body, cp, -1);
              Tcl_Free(cp);
            }
          }
#endif
          bstring = Tcl_DStringValue(&body);

          Tcl_DStringAppend(&tds, "text ", -1);
          sprintf(num, "%d ", lpEMFR->dParm[0]);
          Tcl_DStringAppend(&tds, num, -1);
          sprintf(num, "%d ", lpEMFR->dParm[1]);
          Tcl_DStringAppend(&tds, num, -1);
          Tcl_DStringAppend(&tds, "-width ", -1);
          sprintf(num, "%d ", lpEMFR->dParm[2] - lpEMFR->dParm[0] + 10);
          Tcl_DStringAppend(&tds, num, -1);
          Tcl_DStringAppend(&tds, "-anchor nw -justify left ", -1);
          Tcl_DStringAppend(&tds, "-text ", -1);
          Tcl_DStringAppendElement(&tds, bstring);
          Tcl_DStringAppend(&tds, " ", -1);
 
          if (usetextcolor)
          {
            Tcl_DStringAppend(&tds, "-fill ", -1);
            Tcl_DStringAppend(&tds, textcolor, -1);
          }

          if (usefontname)
          {
            Tcl_DStringAppend(&tds, "-font {", -1);
            Tcl_DStringAppendElement(&tds, Tcl_DStringValue(&fontname));
            Tcl_DStringAppend(&tds, " ", -1);
            Tcl_DStringAppend(&tds, fontsize, -1);
            Tcl_DStringAppend(&tds, "} ", -1);
          }

          ostring = Tcl_DStringValue(&tds);
          Tcl_AppendElement(interp, ostring);
        }
        break;
      case EMR_EXTCREATEFONTINDIRECTW:
      {
        /* Font name begins in parameter 8 as a Unicode name. */
        /* What can we extract from here that can be used by a canvas-like font?
         * We can get the name and size and style--but probably not the angle and such */
        /* Size appears to be the second parameter, in absolute terms. */
        /* However, this size may not be right, and should be checked... */
        int len;
        
        Tcl_DStringInit(&fontname);
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )
        len = Tcl_UniCharLen((Tcl_UniChar *)(&lpEMFR->dParm[8])); /* Prototype at variance with documentation */
        Tcl_UniCharToUtfDString((CONST Tcl_UniChar *)(&lpEMFR->dParm[8]), len, &fontname);
#else
          /* Tcl UniChar functions not supported. Try a windows hack */
          {
            char *cp;
            int siz;

            siz = WideCharToMultiByte (CP_OEMCP, 0, (LPCWSTR)(&lpEMFR->dParm[8]), -1, 0, 0, 0, 0);
            if ( siz > 0 )
            {
              cp = Tcl_Alloc(siz + 1);
              cp[0] = '\0';
              WideCharToMultiByte(CP_OEMCP, 0, (LPCWSTR)(&lpEMFR->dParm[8]), -1, cp, siz+1, 0, 0);
              Tcl_DStringAppend(&fontname, cp, -1);
              Tcl_Free(cp);
            }
          }
#endif
        sprintf(fontsize, "%d ", -(int)((int)(lpEMFR->dParm[1]) / 4.0) );
        usefontname = 1;
      }
      break;
      case EMR_SETTEXTCOLOR:
        {
          int x1 = lpEMFR->dParm[0] & 0xff;
          int x2 = (lpEMFR->dParm[0] >> 8 ) & 0xff;
          int x3 = (lpEMFR->dParm[0] >> 16) & 0xff;
          int x4 = (lpEMFR->dParm[0] >> 24) & 0xff;

          sprintf(textcolor, "#%02x%02x%02x ", x1, x2, x3);
          usetextcolor = 1;
        }
        break;
      /* Now for some of the more prosaic ones */
      case EMR_MOVETOEX:
      {
        do_path_x = lpEMFR->dParm[0];
        do_path_y = lpEMFR->dParm[1];
      }
      break;
      case EMR_LINETO:
      {
        Tcl_DStringAppend(&tds, "line ", -1);
        sprintf(num, "%d %d ", do_path_x, do_path_y );
        Tcl_DStringAppend(&tds, num, -1);
        sprintf(num, "%d %d ", do_path_x = lpEMFR->dParm[0], do_path_y = lpEMFR->dParm[1]);
        Tcl_DStringAppend(&tds, num, -1);
          
        if ( useoutline )
        {
          Tcl_DStringAppend(&tds, " -fill ", -1);
          Tcl_DStringAppend(&tds, outline, -1);
          /* May need more of the same outline--don't zero it */
        }
        if ( usewidth )
        {
          Tcl_DStringAppend(&tds, " -width ", -1 );
          Tcl_DStringAppend(&tds, width, -1);
          /* May need more of the same width -- don't zero it */
        }
        ostring = Tcl_DStringValue(&tds);
        Tcl_AppendElement(interp, ostring);
      }
      break;
      case EMR_POLYLINETO:
      {
          int npoints;
          /* The 5th line contains the number of points */
          Tcl_DStringAppend(&tds, "line ", -1);
          /* Like Polyline, but starts at the "current" point */
          sprintf(num, "%d %d ", do_path_x, do_path_y );
          Tcl_DStringAppend(&tds, num, -1);

          /* The 5th parameter counts the points */
          if ( nparams >= 5 )
          {
            npoints = lpEMFR->dParm[4];

            for (i=0; i<npoints; i++)
            {
                sprintf(num, "%d %d ", 
                        do_path_x = lpEMFR->dParm[5+i] & 0xffff, 
                        do_path_y = lpEMFR->dParm[5+i] >> 16 );
              Tcl_DStringAppend(&tds, num, -1);
            }
          }

          if ( useoutline ) 
	  {
            Tcl_DStringAppend(&tds, " -fill ", -1);
            Tcl_DStringAppend(&tds, outline, -1);
            useoutline = 0;
          }

          if ( usewidth ) 
	  {
            Tcl_DStringAppend(&tds, " -width ", -1);
            Tcl_DStringAppend(&tds, width, -1);
            usewidth = 0;
          }
      
          ostring = Tcl_DStringValue(&tds);
          Tcl_AppendElement(interp, ostring);
      }
      break;
      case EMR_POLYBEZIERTO: case EMR_POLYBEZIERTO16:
      {
          int npoints;
          /* The bezier is replaced by lines, ignoring the control points.
           * Sadly, there is no good canvas equivalent to a bezier...
           */
          Tcl_DStringAppend(&tds, "line ", -1);
          
          sprintf(num, "%d %d ", do_path_x, do_path_y );
          Tcl_DStringAppend(&tds, num, -1);

          /* The 5th parameter counts the points */
          if ( nparams >= 5 )
          {
            npoints = lpEMFR->dParm[4];

            for (i=2; i<npoints; i+=3)
            {
                sprintf(num, "%d %d ", 
                        do_path_x = lpEMFR->dParm[5+i] & 0xffff, 
                        do_path_y = lpEMFR->dParm[5+i] >> 16 );
              Tcl_DStringAppend(&tds, num, -1);
            }
          }

          if ( useoutline ) 
	  {
            Tcl_DStringAppend(&tds, " -fill ", -1);
            Tcl_DStringAppend(&tds, outline, -1);
            useoutline = 0;
          }

          if ( usewidth ) 
	  {
            Tcl_DStringAppend(&tds, " -width ", -1);
            Tcl_DStringAppend(&tds, width, -1);
            usewidth = 0;
          }
      
          ostring = Tcl_DStringValue(&tds);
          Tcl_AppendElement(interp, ostring);
      }
      break;
      case EMR_POLYBEZIER: case EMR_POLYBEZIER16:
      {
          int npoints;
          /* The bezier is replaced by lines, ignoring the control points.
           * Sadly, there is no good canvas equivalent to a bezier...
           */
          Tcl_DStringAppend(&tds, "line ", -1);

          /* The 5th parameter counts the points */
          if ( nparams >= 5 )
          {
            npoints = lpEMFR->dParm[4];

            for (i=0; i<npoints; i+=3)
            {
                sprintf(num, "%d %d ", 
                        do_path_x = lpEMFR->dParm[5+i] & 0xffff, 
                        do_path_y = lpEMFR->dParm[5+i] >> 16 );
              Tcl_DStringAppend(&tds, num, -1);
            }
          }

          if ( useoutline ) 
	  {
            Tcl_DStringAppend(&tds, " -fill ", -1);
            Tcl_DStringAppend(&tds, outline, -1);
            useoutline = 0;
          }

          if ( usewidth ) 
	  {
            Tcl_DStringAppend(&tds, " -width ", -1);
            Tcl_DStringAppend(&tds, width, -1);
            usewidth = 0;
          }
      
          ostring = Tcl_DStringValue(&tds);
          Tcl_AppendElement(interp, ostring);
      }
      break;
      case EMR_ENDPATH: case EMR_ABORTPATH:
        usewidth = usefill = useoutline = 0;
      break;
      default:
        gotRecord = 0;
        break;
    }
  }

  if ( gotRecord )
    Tcl_AppendResult(interp, "\n", 0);
    
  Tcl_DStringFree(&tds);
  return 1;
}

static int WmfEnumMetafileRecords(ClientData mf, Tcl_Interp *interp, int argc, char **argv)
{
  HENHMETAFILE hwmf = (HENHMETAFILE)mf;
  RECT rect;
  HDC hdc = 0;
  
  rect.top=0;
  rect.bottom = 2000;
  rect.left = 0;
  rect.right = 2000;

  /* Set up the HDC to represent style of output we want */
  if ( argc > 0 )
  {
    if ( argv[0][0] == 'v' )
      output_style = 1;
    else
      output_style = 2;
  }
  
  Tcl_AppendResult(interp, "{\n", 0);
  EnumEnhMetaFile(hdc, hwmf, (ENHMFENUMPROC)WmfEnhMetaFileProc, (LPVOID)interp, &rect);
  Tcl_AppendResult(interp, "}\n", 0);

  return 1;
}

static int WmfInfo  (ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
  const char *usage = "wmf info wmfDC [-verbose|-canvas]";
  HENHMETAFILE hwmf;
  LPENHMETAHEADER lpemh = 0;
  char buffer[1024+1];
  int verbose = 0;
  int i;
  
  if ( argc == 0 )
  {
    Tcl_SetResult(interp, (char *)usage, TCL_STATIC);
    return TCL_ERROR;
  }

  if ( ( hwmf = get_wmf (interp, argv[0]) ) == 0 )
  {
    Tcl_AppendResult(interp, usage, 0);
    return TCL_ERROR;
  }

  debug = 0; /* Reset the debug */
  /* Process other command line arguments */
  for (i=1; i<argc; i++)
  {
    if ( strcmp(argv[i], "-verbose") == 0 )
      verbose = 1;
    else if ( strcmp(argv[i], "-canvas") == 0 )
      verbose = 2;
    else if ( strcmp(argv[i], "-debug") == 0 )
      debug = 1;
  }
  
  /* First, figure out how big an array I need */
  switch ( wmfHeader(hwmf, &lpemh) )
  {
    case 0: /* Normal */
      break;
    case WMF_INVALID_HANDLE: /* Invalid handle */
      Tcl_AppendResult(interp, "Invalid metafile handle ", argv[0], 0);
      return TCL_ERROR;
    case WMF_OUT_OF_MEMORY: /* Out of Memory */
      Tcl_AppendResult(interp, "Out of Memory", 0);
      return TCL_ERROR;
    case WMF_GET_HEADER_FAILED: /* Can't get header info */
      Tcl_AppendResult(interp, "Can't get metafile info on ", argv[0], 0);
      return TCL_ERROR;
    default: /* Unknown error */
      Tcl_AppendResult(interp, "Unknown error in getting metafile information", 0);
      return TCL_ERROR;
  }  

  /* Now format the data for the caller */
  sprintf(buffer, "%ld", (long)lpemh->nBytes);
  Tcl_AppendResult(interp, "{ size ", buffer, "} ", 0);
  sprintf(buffer, "%ld %ld %ld %ld", lpemh->rclBounds.left, lpemh->rclBounds.top,
                                     lpemh->rclBounds.right, lpemh->rclBounds.bottom );
  Tcl_AppendResult(interp, "{ {bounding box} { ", buffer, " } } ", 0);
  sprintf(buffer, "%ld %ld %ld %ld", lpemh->rclFrame.left, lpemh->rclFrame.top,
                                     lpemh->rclFrame.right, lpemh->rclFrame.bottom );
  Tcl_AppendResult(interp, "{ {bounding frame} { ", buffer, " } } ", 0);

  /* This portion gets and outputs the description, taking care of
  ** internal to UTF translations for TCL 8.1 and above
  */
  {
    char *cp;
    if ( lpemh->offDescription != 0 && lpemh->nDescription != 0 )
    {
      if ( (cp = Tcl_Alloc(lpemh->nDescription + 1)) != 0 )
      {
        if ( GetEnhMetaFileDescription(hwmf, lpemh->nDescription, cp) > 0 )
        {
          Tcl_AppendResult(interp, "{ application ", 0);
#if TCL_MAJOR_VERSION > 8 || ( TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )
          {
             const char *ostring;
             Tcl_DString tds;
             Tcl_DStringInit(&tds);
             Tcl_UtfToExternalDString(NULL, cp, -1, &tds);
             ostring = Tcl_DStringValue(&tds);
             Tcl_AppendElement(interp, ostring);
             Tcl_DStringFree(&tds);
          }
#else
          Tcl_AppendElement(interp, cp);
#endif
          Tcl_AppendResult(interp, "} ", 0);
          Tcl_AppendResult(interp, "{ title ", 0);
#if TCL_MAJOR_VERSION > 8 || ( TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )
          {
             const char *ostring;
             Tcl_DString tds;
             Tcl_DStringInit(&tds);
             Tcl_UtfToExternalDString(NULL, cp + strlen(cp) + 1, -1, &tds);
             ostring = Tcl_DStringValue(&tds);
             Tcl_AppendElement(interp, ostring);
             Tcl_DStringFree(&tds);
          }
#else
          Tcl_AppendElement(interp, cp + strlen(cp) + 1);
#endif
          Tcl_AppendResult(interp, "} ", 0);
        }
        else
          Tcl_AppendResult(interp, "{ application unknown } { title unknown } ", 0);
        Tcl_Free(cp);
      }
      else
        Tcl_AppendResult(interp, "{ application unknown } { title unknown } ", 0);
    }
    else
      Tcl_AppendResult(interp, "{ application unknown } { title unknown } ", 0);
  }
  sprintf(buffer, "%ld %ld", (long)lpemh->szlDevice.cx, (long)lpemh->szlDevice.cy);
  Tcl_AppendResult(interp, "{ ", 0);
  Tcl_AppendElement(interp, "device resolution");
  Tcl_AppendElement(interp, buffer);
  Tcl_AppendResult(interp, "} ", 0 );
  
  sprintf(buffer, "%ld %ld", (long)lpemh->szlMillimeters.cx, (long)lpemh->szlMillimeters.cy);
  Tcl_AppendResult(interp, "{ ", 0);
  Tcl_AppendElement(interp, "device millimeters");
  Tcl_AppendElement(interp, buffer);
  Tcl_AppendResult(interp, "} ", 0 );

  /* Free the allocated memory */
  Tcl_Free((char *)lpemh);

  /* Now take care of requests for verbose info on the metafile records */
  if ( verbose > 0 )
  {
    int ac = 1;
    char *av[2];
    av[0] = (verbose==1?"verbose":"canvas");
    av[1] = 0;
    WmfEnumMetafileRecords((ClientData)hwmf, interp, ac, av);
  }
    
  return TCL_OK;
}

static int WmfDelete (ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
  const char *usage = "wmf delete wmfDC";
  HENHMETAFILE hwmf;

  if ( argc == 0 )
  {
    Tcl_SetResult(interp, (char *)usage, TCL_STATIC);
    return TCL_ERROR;
  }

  if ( ( hwmf = get_wmf (interp, argv[0]) ) == 0 )
    return TCL_ERROR;

  /* Delete the metafile handle itself */
  DeleteEnhMetaFile (hwmf);
  delete_mf(hwmf);  

  /* Delete the name from the HDC list */
  delete_wmf_name(interp, argv[0]);

  return TCL_OK;
}

static struct clip_type
{
  const char *name;
  UINT       format;
} formats[] =
{
  { "text", CF_TEXT },
  { "enhanced metafile", CF_ENHMETAFILE },
  { "image", CF_DIB },
  { "metafile", CF_METAFILEPICT },
  { "unicode text", CF_UNICODETEXT },
};

static int wmf_paste_metafile(Tcl_Interp *interp, HGLOBAL hglob, int do_type)
{
  int result = TCL_OK;
  METAFILEPICT *hsrcmf;
  HENHMETAFILE hdstwmf;
  
  char *buffer = 0;
  UINT siz = 0;
    
  if ( (hsrcmf = (HMETAFILEPICT)GlobalLock(hglob)) != NULL )
  {
    if ( (siz = GetMetaFileBitsEx(hsrcmf->hMF, 0, 0)) > 0 )
    {
      if ( (buffer = Tcl_Alloc(siz)) != 0 )
      {
        if ( GetMetaFileBitsEx(hsrcmf->hMF, siz, buffer) > 0 )
          hdstwmf = SetWinMetaFileBits(siz, buffer, 0, hsrcmf);
        Tcl_Free(buffer);
      }
      else
      {
        Tcl_AppendResult(interp, "Can't allocate memory for metafile", 0);
        result = TCL_ERROR;
      }
    }
    else
    {
      Tcl_AppendResult(interp, "Can't get metafile bits", 0);
      result = TCL_ERROR;
    }
    GlobalUnlock(hglob);
  }
  else
  {
    Tcl_AppendResult(interp, "Can't lock clipboard memory", 0);
    result = TCL_ERROR;
  }

  /* Close the clipboard as soon as possible */
  CloseClipboard();
  
  /* Name and return the enhanced metafile handle */
  if ( result == TCL_OK )
  {
    const char *retval;
    retval = make_wmf_name(interp, hdstwmf);
    Tcl_AppendResult(interp, "{ \"enhanced metafile\" ", (char *)retval, " }", 0);
  }
  
  return result;
}

static int wmf_paste_enhmetafile(Tcl_Interp *interp, HGLOBAL hglob, int do_type)
{
  HENHMETAFILE hsrcwmf;
  HENHMETAFILE hdstwmf;
  int result = TCL_OK;

  if ( (hsrcwmf = (HENHMETAFILE)GlobalLock(hglob)) == NULL )
  {
      char errnum[36];

      /* For some reason, this does not always succeed on the first try... */
      Sleep(150);
      if ( (hsrcwmf = (HENHMETAFILE)GlobalLock(hglob)) == NULL )
      {
         /* Substitute with a regular metafile and return... */
         return wmf_paste_metafile(interp, hglob, do_type);
      }
      
      if ( hsrcwmf == NULL )
      {
        sprintf(errnum, "%ld 0x%lx", (long)GetLastError(), (long)hglob);
        Tcl_AppendResult(interp, "Can't lock clipboard contents: ", errnum, 0);
        CloseClipboard();
        return TCL_ERROR;
      }
  }
  /* Close the clipboard as soon as possible */
  CloseClipboard();
  
  /* Copy the enhanced metafile to a memory metafile */
  hdstwmf = CopyEnhMetaFile(hsrcwmf, 0);

  /* Unlock the global memory */
  GlobalUnlock(hglob);

  /* Name and return the enhanced metafile handle */
  if ( result == TCL_OK )
  {
    const char *retval;
    retval = make_wmf_name(interp, hdstwmf);
    Tcl_AppendResult(interp, "{ \"enhanced metafile\" ", (char *)retval, " }", 0);
  }
  
  return result;
}

static int wmf_paste_text(Tcl_Interp *interp, HGLOBAL hglob, int do_type)
{
  int result = TCL_OK;
  char *text;
  HGLOBAL handle;
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )
  Tcl_Encoding encoding = NULL;
#endif
  int locale;
  Tcl_DString tds;
  char *data;
  
  Tcl_DStringInit(&tds);

#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )
  if ( IsClipboardFormatAvailable(CF_LOCALE))
  {
    if ( (handle = GetClipboardData(CF_LOCALE)) != 0 )
    {
      Tcl_DStringAppend(&tds, "cp######", -1);
      if ( (data = GlobalLock(handle) ) != 0 )
      {
        locale = LANGIDFROMLCID(*((int*)data));
	GetLocaleInfo(locale, LOCALE_IDEFAULTANSICODEPAGE,
		    Tcl_DStringValue(&tds)+2, Tcl_DStringLength(&tds)-2);
	GlobalUnlock(handle);

	encoding = Tcl_GetEncoding(NULL, Tcl_DStringValue(&tds));
	Tcl_DStringFree(&tds);
      }
      else
      {
	encoding = NULL;
      }
    }
    else
    {
      CloseClipboard();
      Tcl_AppendResult(interp, "WMF: Paste text: Can find, but not access, locale", 0 );
      return TCL_ERROR;
    }
  }
#endif
  
  if ( (text = (char *)GlobalLock(hglob)) != 0 )
  {
    char *value;
    char *cleanvalue;
    
    Tcl_DStringInit(&tds);
    
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )
    Tcl_ExternalToUtfDString(encoding, text, -1, &tds);
#else
    Tcl_DStringAppend(&tds, text, -1);
#endif

    GlobalUnlock(hglob);

    if ( do_type == 0 )
      Tcl_AppendElement(interp, "text");

    value = cleanvalue = Tcl_DStringValue(&tds);
    while (*value)
    {
      if (value[0] == '\r' && value[1] == '\n')
        value++;
      else
        *cleanvalue++ = *value++;
    }
    *cleanvalue-- = '\0';
    Tcl_AppendElement(interp, Tcl_DStringValue(&tds));
  }
  else
  {
    Tcl_AppendResult(interp, "WMF: Paste text: Cannot lock global memory", 0);
    result = TCL_ERROR;
  }
  
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )
  if (encoding)
    Tcl_FreeEncoding(encoding);
#endif
  Tcl_DStringFree(&tds);
  CloseClipboard();
  return result;
}

static int wmf_paste_unicodetext(Tcl_Interp *interp, HGLOBAL hglob, int do_type)
{
  int result = TCL_OK;
  char *text;
  Tcl_DString tds;

  Tcl_DStringInit(&tds);
  
  if ( (text = (char *)GlobalLock(hglob)) != 0 )
  {
    char *value;
    char *cleanvalue;

#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1 )    
    Tcl_UniCharToUtfDString((Tcl_UniChar *)text,
                            Tcl_UniCharLen((Tcl_UniChar *)text), &tds);
#else
    Tcl_DStringAppend(&tds, text, -1);
#endif

    GlobalUnlock(hglob);
    if ( do_type == 0 )
      Tcl_AppendElement(interp, "text");

    value = cleanvalue = Tcl_DStringValue(&tds);
    while (*value)
    {
      if (value[0] == '\r' && value[1] == '\n')
        value++;
      else
        *cleanvalue++ = *value++;
    }
    *cleanvalue-- = '\0';
    Tcl_AppendElement(interp, Tcl_DStringValue(&tds));
  }

  Tcl_DStringFree(&tds);
  CloseClipboard();
  return result;
}

static int wmf_paste_dib (Tcl_Interp *interp, HGLOBAL hglob, int do_type, char *filename, int do_image, int do_file)
{
  int result = TCL_OK;
  char *photo;
  char cwidth[13];
  char cheight[13];
  long datsize = 0;
  int colortablesize;
  BITMAPINFO *lbmi;
  Tk_PhotoHandle ph;
  Tcl_DString tds;
  char fn[1024]; /* Should be maximum file size */
  char *imagename = fn;
  
  if ( do_file && filename && filename[0])
  {
    int len = strlen(filename);
    if (strcmp(filename+len-4, ".ppm") != 0 )
    {
      sprintf(fn, "%s.ppm", filename);
    }
    else
      imagename = filename;
  }
  else
  {
    int i;
    char *cp = getenv("TEMP");
    if ( cp == 0 )
      cp = "C:";
    else
    {
      int len = strlen(cp);
      for (i=0; i<len; i++)
        if ( cp[i] == '\\')
          cp[i] = '/';
    }
    for(i=0; i<999; i++)
    {
      sprintf(fn, "%s/wmf%03d.ppm", cp, i);
      if ( access(imagename, 0) != 0 )
        break;
    }
    if (i==1000)
    {
      Tcl_AppendResult(interp, 
                "Can't build accessible temp file. "
	        "Check TEMP environment variable or remove unneeded PPM files.", 
		0);
      CloseClipboard();
      return TCL_ERROR;
    }
  }
    
  Tcl_DStringInit(&tds);
    
  if ( ( lbmi = (BITMAPINFO *)GlobalLock(hglob)) != NULL )
  {
    long h = lbmi->bmiHeader.biHeight;
    int bitfields = 0;
    
    switch ( lbmi->bmiHeader.biBitCount )
    {
      case 24:
      case 32:
        colortablesize = 0;
        break;
      default:
        colortablesize = (1<<(lbmi->bmiHeader.biBitCount)) * sizeof(RGBQUAD);
    }

    /* We don't do decompression */
    switch ( lbmi->bmiHeader.biCompression )
    {
      case BI_RGB:
        break;
      case BI_BITFIELDS: /* Colortable is 3 entries with bitmasks */
        /* Not supported yet... */
      default:
        GlobalUnlock(hglob);
        CloseClipboard();
        Tcl_AppendResult(interp, "Image on clipboard not in supported format", 0);
        return TCL_ERROR;
    }
      
    /* A little diversion... */
    {
      FILE *fp;
      
#if defined(DO_BMP_FILE)
      if ( (fp = fopen("C:\\TEMP\\wmf.bmp", "wb")) != NULL )
      {
        BITMAPFILEHEADER bpfh;
        strncpy((char *)&bpfh.bfType, "BM", 2);
        bpfh.bfSize = lbmi->bmiHeader.biSize + 
                      lbmi->bmiHeader.biSizeImage + 
		      sizeof (BITMAPFILEHEADER);
	memset((char *)&bpfh.bfReserved1, 0, 2 * sizeof(WORD));
	bpfh.bfOffBits = lbmi->bmiHeader.biSize + colortablesize;
	fwrite(&bpfh, sizeof (BITMAPFILEHEADER), 1, fp);
	fwrite(lbmi, lbmi->bmiHeader.biSize + colortablesize + lbmi->bmiHeader.biSizeImage, 1, fp);
	fclose(fp);
      }
#endif
      /* Yet another excursion -- Write a PPM file */
        
      /* This could be made more general */
      if ( (fp = fopen(imagename, "wb")) != NULL )
      {
        int i;
        int k;
        long w = lbmi->bmiHeader.biWidth;
        long h = lbmi->bmiHeader.biHeight;
        unsigned char *rawdata = (unsigned char *)lbmi + 
                                 lbmi->bmiHeader.biSize + colortablesize;
        int sll;
          
        sll = w;
        sll = ( (w + 3) / 4 ) * 4; /* Multiple of 4 */
          
        fprintf(fp, "P6\n%ld\n%ld\n%ld\n", w, h, 255);

        for ( k=h-1; k>=0; k--) /* Images are stored bottom to top */
        {
          for (i=0; i<w; i++)
          {
            unsigned char p[3];
            int pix;
            RGBQUAD rgb;
            if (colortablesize != 0)
            {
              pix = rawdata[k*sll+i];
              rgb = lbmi->bmiColors[pix];
              p[0] = rgb.rgbRed;
              p[1] = rgb.rgbGreen;
              p[2] = rgb.rgbBlue;
            }
            else if ( colortablesize % 8 == 0 )
            {
              int byte = lbmi->bmiHeader.biBitCount / 8;
              p[0] = rawdata[k*sll*byte + i*byte + 2];
              p[1] = rawdata[k*sll*byte + i*byte + 1];
              p[2] = rawdata[k*sll*byte + i*byte + 0];
            }
            else /* This leaves cases 1, 2, and 4 */
            {
            }
            fwrite(p, 3, 1, fp);
          }
        }

	fclose(fp);
      }
        
    }

    if (do_image)
    {  
      /* It seems the only way to get an image name is to use the Tk procedure */
      sprintf(cwidth,  "%ld", (long)lbmi->bmiHeader.biWidth);
      sprintf(cheight, "%ld", -(long)lbmi->bmiHeader.biHeight);

      /* Make the photo and save its image id */
      Tcl_ResetResult(interp);
      Tcl_VarEval(interp, "image create photo ",
	                  " -file ", imagename,
			  0);
      Tcl_DStringGetResult(interp, &tds);
      photo = Tcl_DStringValue(&tds);
      if ( (ph = Tk_FindPhoto(interp, photo)) != 0 )
      {
        /* Now we have a photo name of the right size */
        /* photo may need to be "deUnicoded" before printing */
        if ( do_type == 0 )
          Tcl_AppendElement(interp, "image");
        Tcl_AppendElement(interp, photo);
      }
      else /* Photo does not exist ?! */
      {
      }
    }
    else /* Just the file -- no real image */
    {
      if (do_type == 0 )
        Tcl_AppendElement(interp, "image file");
      Tcl_AppendElement(interp, imagename);
    }
  }
  else
  {
    char code[14];
    sprintf(code, "%ld", (long)GetLastError());
    Tcl_AppendResult(interp, "GlobalLock failed with code: ", code, 0);
    result = TCL_ERROR;
  }

  if ( do_file == 0 && imagename && imagename[0] )
  {
    remove (imagename);
  }
    
  GlobalUnlock(hglob);

  CloseClipboard();
  Tcl_DStringFree(&tds);
  return result;
}

static int WmfPaste (ClientData unused, Tcl_Interp *interp, int argc, char **argv)
{
  const char *usage = "wmf paste [ [-type type] [-file f] | [-list] ]";
  HGLOBAL hglob;
  int result = TCL_OK;  
  UINT clip_format;
  int do_type = 0;
  char *type;
  int do_list = 0;
  int do_file = 0;
  char *filename = 0;
  int do_image = 1;
  int arg;
  int gotone = 1;
  
  /* Parse the arguments */
  for (arg=0; arg<argc; arg++)
  {
    if ( strcmp(argv[arg], "-type") == 0 )
    {
      if ( arg+1 < argc)
      {
        do_type = 1;
        type = argv[++arg];
      }
    }
    else if (strcmp(argv[arg], "-list") == 0 )
      do_list = 1;
    else if (strcmp(argv[arg], "-file") == 0 )
    {
      if ( arg+1 < argc)
      {
        do_file = 1;
        filename = argv[++arg];
      }
    }
  }

  if ( do_type + do_file > 0 && do_list > 0 )
  {
    Tcl_AppendResult(interp, usage, 0);
    return TCL_ERROR;
  }

  if (using_tk == 0)
  {
    do_image = 0;
    if (do_file == 0)
    {
      do_file = 1;
    }
  }
  
  /* Open the clipboard */
  if ( OpenClipboard(0) == FALSE )
  {
    Tcl_AppendResult(interp, "Can't open the clipboard", 0);
    CloseClipboard();
    return TCL_ERROR;
  }

  if ( do_list ) /* Just list what's available, and only from what we will handle */
  {
    for ( clip_format = EnumClipboardFormats(0); 
          clip_format != 0; 
  	  clip_format = EnumClipboardFormats(clip_format))
    {
      int i;
      for (i=0; i< sizeof formats / sizeof (struct clip_type); i++)
      {
        if ( clip_format == formats[i].format &&
             (using_tk==1 || clip_format != CF_DIB ) )
        {
          Tcl_AppendElement(interp, formats[i].name);
          break;
        }
      }
    }
    CloseClipboard();
    return TCL_OK;
  }
  
  /* Check for the formats of interest, in order */
  if ( do_type )
  {
    int i;

    clip_format = -1;
    for (i=0; i < sizeof formats / sizeof (struct clip_type); i++)
    {
      if ( strcmp(type, formats[i].name) == 0 )
      {
        /* Special test to avoid images when using tcl only */
        if (using_tk == 1 || formats[i].format != CF_DIB )
        {
          clip_format = formats[i].format;
          break;
        }
      }
    }

    if (clip_format == -1 )
    {
      CloseClipboard();
      Tcl_AppendResult(interp, "Format %s is not recognized\n", usage, 0);
      return TCL_ERROR;
    }

    gotone = ( (hglob = GetClipboardData(clip_format)) != 0) ;
  }
  else
  {
    gotone = 0;
    /* Check for the formats of interest */
    for ( clip_format = EnumClipboardFormats(0); 
          clip_format != 0; 
	  clip_format = EnumClipboardFormats(clip_format))
    {
      int i;
      for ( i = 0; i <  sizeof formats / sizeof (struct clip_type); i++ )
      {
        if ( formats[i].format == clip_format )
        {
          hglob = GetClipboardData(clip_format);
          gotone = 1;
          break;
        }
      }
      if ( gotone == 1 )
        break;
    }
  }
  
  if ( gotone == 0 )  
  {
    Tcl_AppendResult(interp, "Can't get clipboard in compatible format", 0);
    CloseClipboard();
    return TCL_ERROR;
  }

  if ( hglob == 0 )
  {
    Tcl_AppendResult(interp, "Can't get clipboard handle", 0);
    CloseClipboard();
    return TCL_ERROR;
  }

  if ( clip_format == CF_ENHMETAFILE )
  {
    return wmf_paste_enhmetafile(interp, hglob, do_type);
  }
  else if ( clip_format == CF_METAFILEPICT ) /* It's a regular metafile */
  {
    return wmf_paste_metafile(interp, hglob, do_type);
  }
  else if ( clip_format == CF_UNICODETEXT )
  {
    return wmf_paste_unicodetext(interp, hglob, do_type);
  }
  else if ( clip_format == CF_TEXT )
  {
    return wmf_paste_text(interp, hglob, do_type);
  }
  else if ( clip_format == CF_DIB )
  {
    return wmf_paste_dib(interp, hglob, do_type, filename, do_image, do_file);
  }

  return result;
}

static int is_valid_mf_hdc(HENHMETAFILE hwmf)
{
  int retval = 0;
  DWORD objtype = GetObjectType((HGDIOBJ)hwmf);
  switch (objtype)
  {
    /* Only enhanced metafile DCs are OK */
    case OBJ_ENHMETADC:
      retval = 1;
      break;
    /* Anything else is invalid */
    case 0: /* Function failed */
    default:
      break;
  }
  return retval;
}

static int is_valid_dc_hdc(HDC hdc)
{
  int retval = 0;
  DWORD objtype = GetObjectType((HGDIOBJ)hdc);
  switch (objtype)
  {
    /* Any of these DC types are OK. */
    case OBJ_DC: case OBJ_MEMDC: 
      retval = 1;
      break;
    /* Anything else is invalid */
    case 0: /* Function failed */
    default:
      break;
  }
  return retval;
}

static void add_dc (HDC hdc)
{
  Tcl_HashEntry *data;
  int status;
  data = Tcl_CreateHashEntry(&wmf_dc_hdcs, (const char *)hdc, &status);
  Tcl_SetHashValue(data,(const char *)hdc);
}

static void delete_dc (HDC hdc)
{
  Tcl_HashEntry *data;
  if ( (data = Tcl_FindHashEntry(&wmf_dc_hdcs, (const char *)hdc)) != 0 )
    Tcl_DeleteHashEntry(data);
}

static void add_mf (HENHMETAFILE hwmf)
{
  Tcl_HashEntry *data;
  int status;
  data = Tcl_CreateHashEntry(&wmf_mf_hdcs, (const char *)hwmf, &status);
  Tcl_SetHashValue(data,(const char *)hwmf);
}

static void delete_mf (HENHMETAFILE hwmf)
{
  Tcl_HashEntry *data;
  if ( (data = Tcl_FindHashEntry(&wmf_mf_hdcs, (const char *)hwmf)) != 0 )
    Tcl_DeleteHashEntry(data);
}

/* Convert HDC string into appropriate HDC */
static HDC convert_hdc (const char *string )
{
  HDC hdc = 0;
  
  if ( string == 0 )
    return 0;

  /* First, see if it's an hdc extension name */
  if ( hdc_get != 0 )
    hdc = hdc_get(0, string);

  /* If not, see if it's an address */
  if ( hdc == 0 )
  {
    unsigned long tmp;
    char *strend;
    tmp = strtoul(string, &strend, 0);
    if ( tmp > 0 && strend > string )
      hdc = (HDC)tmp;
  }

  return hdc;
}

/* Exported symbols */
BOOL APIENTRY DllEntryPoint (HINSTANCE hInstance, DWORD reason, LPVOID lpCmdLine)
{
  switch (reason)
  {
    case DLL_PROCESS_ATTACH:
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_PROCESS_DETACH:
      /* Clean up the hash table: It's OK to 0 interp, it's not used by the calls involved */
      delete_wmf_contexts(0);
      delete_wmf_dc_contexts(0);
      break;
    case DLL_THREAD_DETACH:
      break;
  }
  /* Don't do anything, so just return true */
  return TRUE;
}

static void init_hdc_functions(Tcl_Interp *interp)
{
  void *fn[7];
  int result;
  const char *cp;
  Tcl_Eval(interp, "hdc FunctionVector");
  cp = Tcl_GetStringResult(interp);
  /* Does cp need to be freed when I'm done? */
  result = sscanf(cp, "%lx%lx%lx%lx%lx%lx%lx", &fn[0], &fn[1], &fn[2], &fn[3], 
                                               &fn[4], &fn[5], &fn[6]);
  if ( result == 7)
  {
    hdc_create = fn[0];
    hdc_delete = fn[1];
    hdc_get    = fn[2];
    hdc_typeof = fn[3];
    hdc_prefixof = fn[4];
    hdc_list     = fn[5];
    hdc_valid    = fn[6];
  }
}

#include <stdarg.h>
static int Log(char *format, ...)
{
  static FILE *fp=0;
  
  va_list va;
  
  va_start(va, format);

  if (fp == 0 )
  {
    char *cp;
    char filename[1024];
    if ( (cp = getenv("TEMP")) != 0 && strlen(cp) < 1000 )
    {
      sprintf(filename, "%s/wmf.log", cp);
    }
    else
      strcpy(filename, "c:/wmf.log");

    if ( ( fp = fopen(filename, "a") ) == 0 )
      return 0;
  }

  vfprintf(fp, format, va);
  fflush(fp);
  
  va_end(va);
  return 1;
}

