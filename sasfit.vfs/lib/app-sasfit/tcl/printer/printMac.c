/****************************************************************
** MAC-ONLY IMPLEMENTATION
****************************************************************/
#if defined (MAC)
#include <Printing.h>

static
struct print_values {
  THPrint hPrint;
  TPPrPort gPrinterPort;
} default_print_values;

static int PrintOpen(ClientData data, Tcl_Interp *interp, int argc, char **argv)
{
  struct printer_values *ppv = (struct printer_values *)data;

  if ( ppv->hPrint == NULL)
  {
    ppv->hPrint = (THPrint)NewHandle( sizeof(TPrint) );
    PrintDefault(ppv->hPrint);
  }
  PrOpen();

  sprintf(msgbuf, "0x%lx", (unsigned long)ppv->hPrint);
  Tcl_SetResult(interp, msgbuf, TCL_VOLATILE);

  return TCL_OK;
}

static int PrintClose(ClientData data, Tcl_Interp *interp, int argc, char **argv)
{
  struct printer_values *ppv = (struct printer_values *)data;

  PrClose();

  if ( ppv->hPrint != NULL )
  {
    DisposeHandle ( (Handle)(ppv->hPrint) );
    ppv->hPrint = NULL;
  }

  return TCL_OK;
}

static int PrintDialog(ClientData data, Tcl_Interp *interp, int argc, char **argv)
{
  /* Which dialog is requested: one of select, page_setup */
  static char usage_message[] = "printer dialog [select|page_setup]";
  struct printer_values *ppv = (struct printer_values *)data;

  if (argc < 1)
  {
    Tcl_SetResult(interp ,usage_message, TCL_STATIC);
    return TCL_ERROR;
  }

  if ( strcmp(argv[0], "select") == 0)
  {
    PrJobDialog(ppv->hPrint);
    sprintf(msgbuf, "0x%lx", (unsigned long)(ppv->hPrint) );
    Tcl_SetResult(interp, msgbuf, TCL_VOLATILE);
  }
  else if (strcmp(argv[0], "page_setup") == 0 )
  {
    PrStlDialog(ppv->hPrint);
  }
  else
  {
    Tcl_SetResult(interp ,usage_message, TCL_STATIC);
    return TCL_ERROR;
  }

  return TCL_OK;
}

static int PrintJob(ClientData data, Tcl_Interp *interp, int argc, char **argv)
{
  struct printer_values * ppv = (struct printer_values *) data;

  static char usage_message[] = "printer job start [-name docname]\nprinter job end";

  /* Parameters for document name and output file (if any) should be supported */

  /* Should this command keep track of start/end state so two starts in a row
  ** automatically have an end inserted?
  */
  if ( argc >= 1 )
  {
    if ( strcmp (*argv, "start") == 0 )
    {
      ppv->gPrinterPort = PrOpenDoc(ppv->hPrint, NULL, NULL);
      return TCL_OK;
    }
    else if ( strcmp (*argv, "end") == 0 )
    {
      PrCloseDoc (ppv->gPrinterPort);
      ppv->gPrinterPort = NULL;
      return TCL_OK;
    }
  }

  Tcl_SetResult(interp, usage_message, TCL_STATIC);
  return TCL_ERROR;
}

static int PrintPage(ClientData data, Tcl_Interp *interp, int argc, char **argv)
{
  struct printer_values * ppv = (struct printer_values *) data;
  static char usage_message[] = "printer page [start|end]";

  /* Options Start and End should be supported. */
  if ( argc >= 1 )
  {
    if ( strcmp (*argv, "start") == 0 )
    {
      PrOpenPage(ppv->gPrinterPort, NULL);
      return TCL_OK;
    }
    else if ( strcmp (*argv, "end") == 0 )
    {
      PrClosePage (ppv->gPrinterPort);
      return TCL_OK;
    }
  }

  Tcl_SetResult(interp, usage_message, TCL_STATIC);
  return TCL_ERROR;
}

static int PrintAttr(ClientData data, Tcl_Interp *interp, int argc, char **argv)
{
  struct printer_values * ppv = (struct printer_values *) data;

  /* Get and set options? Depends on further arguments? Pattern matching? */
  /* Returns a collection of key/value pairs. Should it use a user-specified array name? */
  /* The attributes of interest are the ones buried in the dialog structures */

  /* For now, just append them to a list--just so we can see the results */
  sprintf(msgbuf, "\"%s\" %d", "copies", ppv->(*hPrint).iCopies);
  Tcl_AppendElement(interp, msgbuf);
  sprintf(msgbuf, "\"%s\" %d", "first page", ppv->(*hPrint).iFstPage);
  Tcl_AppendElement(interp, msgbuf);
  sprintf(msgbuf, "\"%s\" %d", "last page", ppv->(*hPrint).iLstPage);
  Tcl_AppendElement(interp, msgbuf);
  sprintf(msgbuf, "\"%s\" 0x%lx", "hDC", ppv->hPrint);
  Tcl_AppendElement(interp, msgbuf);
  
  return TCL_OK;
}
#else
#error "This implementation is for the Macintosh only"
#endif
/****************************************************************
** END MAC-ONLY IMPLEMENTATION
****************************************************************/
