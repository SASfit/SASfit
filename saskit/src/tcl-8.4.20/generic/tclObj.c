/* 
 * tclObj.c --
 *
 *	This file contains Tcl object-related procedures that are used by
 * 	many Tcl commands.
 *
 * Copyright (c) 1995-1997 Sun Microsystems, Inc.
 * Copyright (c) 1999 by Scriptics Corporation.
 * Copyright (c) 2001 by ActiveState Corporation.
 * Copyright (c) 2007 Daniel A. Steffen <das@users.sourceforge.net>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tclInt.h"
#include "tclCompile.h"
#include "tclPort.h"

/*
 * Table of all object types.
 */

static Tcl_HashTable typeTable;
static int typeTableInitialized = 0;    /* 0 means not yet initialized. */
TCL_DECLARE_MUTEX(tableMutex)

/*
 * Head of the list of free Tcl_Obj structs we maintain.
 */

Tcl_Obj *tclFreeObjList = NULL;

/*
 * The object allocator is single threaded.  This mutex is referenced
 * by the TclNewObj macro, however, so must be visible.
 */

#ifdef TCL_THREADS
Tcl_Mutex tclObjMutex;
#endif

/*
 * Pointer to a heap-allocated string of length zero that the Tcl core uses
 * as the value of an empty string representation for an object. This value
 * is shared by all new objects allocated by Tcl_NewObj.
 */

char tclEmptyString = '\0';
char *tclEmptyStringRep = &tclEmptyString;


#ifdef TCL_TIP280
/*
 * All static variables used in this file are collected into a single
 * instance of the following structure.  For multi-threaded implementations,
 * there is one instance of this structure for each thread.
 *
 * Notice that different structures with the same name appear in other
 * files.  The structure defined below is used in this file only.
 */

typedef struct ThreadSpecificData {
  Tcl_HashTable* lineCLPtr; /* This table remembers for each Tcl_Obj generated
			     * by a call to the function EvalTokensStandard()
			     * from a literal text where bs+nl sequences
			     * occured in it, if any. I.e. this table keeps
			     * track of invisible/stripped continuation
			     * lines. Its keys are Tcl_Obj pointers, the
			     * values are ContLineLoc pointers.  See the file
			     * tclCompile.h for the definition of this
			     * structure, and for references to all related
			     * places in the core.
			     */
} ThreadSpecificData;

static Tcl_ThreadDataKey dataKey;

static void ContLineLocFree _ANSI_ARGS_((char* clientData));
static void TclThreadFinalizeObjects _ANSI_ARGS_((ClientData clientData));
static ThreadSpecificData* TclGetContinuationTable _ANSI_ARGS_(());
#endif

/*
 * Prototypes for procedures defined later in this file:
 */

static int		SetBooleanFromAny _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Obj *objPtr));
static int		SetDoubleFromAny _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Obj *objPtr));
static int		SetIntFromAny _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Obj *objPtr));
static int		SetIntOrWideFromAny _ANSI_ARGS_((Tcl_Interp* interp,
							 Tcl_Obj *objPtr));
static void		UpdateStringOfBoolean _ANSI_ARGS_((Tcl_Obj *objPtr));
static void		UpdateStringOfDouble _ANSI_ARGS_((Tcl_Obj *objPtr));
static void		UpdateStringOfInt _ANSI_ARGS_((Tcl_Obj *objPtr));
static int		SetWideIntFromAny _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Obj *objPtr));

#ifndef TCL_WIDE_INT_IS_LONG
static void		UpdateStringOfWideInt _ANSI_ARGS_((Tcl_Obj *objPtr));
#endif

/*
 * Prototypes for the array hash key methods.
 */

static Tcl_HashEntry *	AllocObjEntry _ANSI_ARGS_((
			    Tcl_HashTable *tablePtr, VOID *keyPtr));
static int		CompareObjKeys _ANSI_ARGS_((
			    VOID *keyPtr, Tcl_HashEntry *hPtr));
static void		FreeObjEntry _ANSI_ARGS_((
			    Tcl_HashEntry *hPtr));
static unsigned int	HashObjKey _ANSI_ARGS_((
			    Tcl_HashTable *tablePtr,
			    VOID *keyPtr));

/*
 * Prototypes for the CommandName object type.
 */

static void		DupCmdNameInternalRep _ANSI_ARGS_((Tcl_Obj *objPtr,
			    Tcl_Obj *copyPtr));
static void		FreeCmdNameInternalRep _ANSI_ARGS_((
    			    Tcl_Obj *objPtr));
static int		SetCmdNameFromAny _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Obj *objPtr));


/*
 * The structures below defines the Tcl object types defined in this file by
 * means of procedures that can be invoked by generic object code. See also
 * tclStringObj.c, tclListObj.c, tclByteCode.c for other type manager
 * implementations.
 */

Tcl_ObjType tclBooleanType = {
    "boolean",				/* name */
    (Tcl_FreeInternalRepProc *) NULL,   /* freeIntRepProc */
    (Tcl_DupInternalRepProc *) NULL,	/* dupIntRepProc */
    UpdateStringOfBoolean,		/* updateStringProc */
    SetBooleanFromAny			/* setFromAnyProc */
};

Tcl_ObjType tclDoubleType = {
    "double",				/* name */
    (Tcl_FreeInternalRepProc *) NULL,   /* freeIntRepProc */
    (Tcl_DupInternalRepProc *) NULL,	/* dupIntRepProc */
    UpdateStringOfDouble,		/* updateStringProc */
    SetDoubleFromAny			/* setFromAnyProc */
};

Tcl_ObjType tclIntType = {
    "int",				/* name */
    (Tcl_FreeInternalRepProc *) NULL,   /* freeIntRepProc */
    (Tcl_DupInternalRepProc *) NULL,	/* dupIntRepProc */
    UpdateStringOfInt,			/* updateStringProc */
    SetIntFromAny			/* setFromAnyProc */
};

Tcl_ObjType tclWideIntType = {
    "wideInt",				/* name */
    (Tcl_FreeInternalRepProc *) NULL,   /* freeIntRepProc */
    (Tcl_DupInternalRepProc *) NULL,	/* dupIntRepProc */
#ifdef TCL_WIDE_INT_IS_LONG
    UpdateStringOfInt,			/* updateStringProc */
#else /* !TCL_WIDE_INT_IS_LONG */
    UpdateStringOfWideInt,		/* updateStringProc */
#endif
    SetWideIntFromAny			/* setFromAnyProc */
};

/*
 * The structure below defines the Tcl obj hash key type.
 */
Tcl_HashKeyType tclObjHashKeyType = {
    TCL_HASH_KEY_TYPE_VERSION,		/* version */
    0,					/* flags */
    HashObjKey,				/* hashKeyProc */
    CompareObjKeys,			/* compareKeysProc */
    AllocObjEntry,			/* allocEntryProc */
    FreeObjEntry			/* freeEntryProc */
};

/*
 * The structure below defines the command name Tcl object type by means of
 * procedures that can be invoked by generic object code. Objects of this
 * type cache the Command pointer that results from looking up command names
 * in the command hashtable. Such objects appear as the zeroth ("command
 * name") argument in a Tcl command.
 *
 * NOTE: the ResolvedCmdName that gets cached is stored in the
 * twoPtrValue.ptr1 field, and the twoPtrValue.ptr2 field is unused.
 * You might think you could use the simpler otherValuePtr field to
 * store the single ResolvedCmdName pointer, but DO NOT DO THIS.  It
 * seems that some extensions use the second internal pointer field
 * of the twoPtrValue field for their own purposes.
 */

static Tcl_ObjType tclCmdNameType = {
    "cmdName",				/* name */
    FreeCmdNameInternalRep,		/* freeIntRepProc */
    DupCmdNameInternalRep,		/* dupIntRepProc */
    (Tcl_UpdateStringProc *) NULL,	/* updateStringProc */
    SetCmdNameFromAny			/* setFromAnyProc */
};


/*
 * Structure containing a cached pointer to a command that is the result
 * of resolving the command's name in some namespace. It is the internal
 * representation for a cmdName object. It contains the pointer along
 * with some information that is used to check the pointer's validity.
 */

typedef struct ResolvedCmdName {
    Command *cmdPtr;		/* A cached Command pointer. */
    Namespace *refNsPtr;	/* Points to the namespace containing the
				 * reference (not the namespace that
				 * contains the referenced command). */
    long refNsId;		/* refNsPtr's unique namespace id. Used to
				 * verify that refNsPtr is still valid
				 * (e.g., it's possible that the cmd's
				 * containing namespace was deleted and a
				 * new one created at the same address). */
    int refNsCmdEpoch;		/* Value of the referencing namespace's
				 * cmdRefEpoch when the pointer was cached.
				 * Before using the cached pointer, we check
				 * if the namespace's epoch was incremented;
				 * if so, this cached pointer is invalid. */
    int cmdEpoch;		/* Value of the command's cmdEpoch when this
				 * pointer was cached. Before using the
				 * cached pointer, we check if the cmd's
				 * epoch was incremented; if so, the cmd was
				 * renamed, deleted, hidden, or exposed, and
				 * so the pointer is invalid. */
    int refCount;		/* Reference count: 1 for each cmdName
				 * object that has a pointer to this
				 * ResolvedCmdName structure as its internal
				 * rep. This structure can be freed when
				 * refCount becomes zero. */
} ResolvedCmdName;


/*
 *-------------------------------------------------------------------------
 *
 * TclInitObjectSubsystem --
 *
 *	This procedure is invoked to perform once-only initialization of
 *	the type table. It also registers the object types defined in 
 *	this file.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Initializes the table of defined object types "typeTable" with
 *	builtin object types defined in this file.  
 *
 *-------------------------------------------------------------------------
 */

void
TclInitObjSubsystem()
{
    Tcl_MutexLock(&tableMutex);
    typeTableInitialized = 1;
    Tcl_InitHashTable(&typeTable, TCL_STRING_KEYS);
    Tcl_MutexUnlock(&tableMutex);

    Tcl_RegisterObjType(&tclBooleanType);
    Tcl_RegisterObjType(&tclByteArrayType);
    Tcl_RegisterObjType(&tclDoubleType);
    Tcl_RegisterObjType(&tclEndOffsetType);
    Tcl_RegisterObjType(&tclIntType);
    Tcl_RegisterObjType(&tclWideIntType);
    Tcl_RegisterObjType(&tclStringType);
    Tcl_RegisterObjType(&tclListType);
    Tcl_RegisterObjType(&tclByteCodeType);
    Tcl_RegisterObjType(&tclProcBodyType);
    Tcl_RegisterObjType(&tclArraySearchType);
    Tcl_RegisterObjType(&tclIndexType);
    Tcl_RegisterObjType(&tclNsNameType);
    Tcl_RegisterObjType(&tclCmdNameType);

#ifdef TCL_COMPILE_STATS
    Tcl_MutexLock(&tclObjMutex);
    tclObjsAlloced = 0;
    tclObjsFreed = 0;
    {
	int i;
	for (i = 0;  i < TCL_MAX_SHARED_OBJ_STATS;  i++) {
	    tclObjsShared[i] = 0;
	}
    }
    Tcl_MutexUnlock(&tclObjMutex);
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * TclFinalizeObjects --
 *
 *	This procedure is called by Tcl_Finalize to clean up all
 *	registered Tcl_ObjType's and to reset the tclFreeObjList.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
TclFinalizeObjects()
{
    Tcl_MutexLock(&tableMutex);
    if (typeTableInitialized) {
        Tcl_DeleteHashTable(&typeTable);
        typeTableInitialized = 0;
    }
    Tcl_MutexUnlock(&tableMutex);

    /* 
     * All we do here is reset the head pointer of the linked list of
     * free Tcl_Obj's to NULL;  the memory finalization will take care
     * of releasing memory for us.
     */
    Tcl_MutexLock(&tclObjMutex);
    tclFreeObjList = NULL;
    Tcl_MutexUnlock(&tclObjMutex);
}

#ifdef TCL_TIP280
/*
 *----------------------------------------------------------------------
 *
 * TclGetContinuationTable --
 *
 *	This procedure is a helper which returns the thread-specific
 *	hash-table used to track continuation line information associated with
 *	Tcl_Obj*.
 *
 * Results:
 *	A reference to the continuation line thread-data.
 *
 * Side effects:
 *	May allocate memory for the thread-data.
 *
 * TIP #280
 *----------------------------------------------------------------------
 */

static ThreadSpecificData*
TclGetContinuationTable()
{
    /*
     * Initialize the hashtable tracking invisible continuation lines.  For
     * the release we use a thread exit handler to ensure that this is done
     * before TSD blocks are made invalid. The TclFinalizeObjects() which
     * would be the natural place for this is invoked afterwards, meaning that
     * we try to operate on a data structure already gone.
     */

    ThreadSpecificData *tsdPtr = TCL_TSD_INIT(&dataKey);
    if (!tsdPtr->lineCLPtr) {
	tsdPtr->lineCLPtr = (Tcl_HashTable*) ckalloc (sizeof (Tcl_HashTable));
	Tcl_InitHashTable(tsdPtr->lineCLPtr, TCL_ONE_WORD_KEYS);
	Tcl_CreateThreadExitHandler (TclThreadFinalizeObjects,NULL);
    }
    return tsdPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TclContinuationsEnter --
 *
 *	This procedure is a helper which saves the continuation line
 *	information associated with a Tcl_Obj*.
 *
 * Results:
 *	A reference to the newly created continuation line location table.
 *
 * Side effects:
 *	Allocates memory for the table of continuation line locations.
 *
 * TIP #280
 *----------------------------------------------------------------------
 */

ContLineLoc*
TclContinuationsEnter(objPtr,num,loc)
     Tcl_Obj* objPtr;
     int num;
     int* loc;
{
  int newEntry;
  ThreadSpecificData *tsdPtr = TclGetContinuationTable();
  Tcl_HashEntry* hPtr =
    Tcl_CreateHashEntry (tsdPtr->lineCLPtr, (char*) objPtr, &newEntry);

  ContLineLoc* clLocPtr = 
    (ContLineLoc*) ckalloc (sizeof(ContLineLoc) + num*sizeof(int));

    if (!newEntry) {
	/*
	 * Somehow we're entering ContLineLoc data for the same value (objPtr)
	 * more than one time.  Not sure whether that's expected, or a sign of
	 * trouble, but at a minimum, we should take care not to leak the old
	 * entry.
	 */

	ckfree((char *) Tcl_GetHashValue(hPtr));
    }

  clLocPtr->num = num;
  memcpy (&clLocPtr->loc, loc, num*sizeof(int));
  clLocPtr->loc[num] = CLL_END; /* Sentinel */
  Tcl_SetHashValue (hPtr, clLocPtr);

  return clLocPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TclContinuationsEnterDerived --
 *
 *	This procedure is a helper which computes the continuation line
 *	information associated with a Tcl_Obj* cut from the middle of a
 *	script.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Allocates memory for the table of continuation line locations.
 *
 * TIP #280
 *----------------------------------------------------------------------
 */

void
TclContinuationsEnterDerived(objPtr, start, clNext)
     Tcl_Obj* objPtr;
     int start;
     int* clNext;
{
    /*
     * We have to handle invisible continuations lines here as well, despite
     * the code we have in EvalTokensStandard (ETS) for that. Why ?
     * Nesting. If our script is the sole argument to an 'eval' command, for
     * example, the scriptCLLocPtr we are using here was generated by a
     * previous call to ETS, and while the words we have here may contain
     * continuation lines they are invisible already, and the call to ETS
     * above had no bs+nl sequences to trigger its code.
     *
     * Luckily for us, the table we have to create here for the current word
     * has to be a slice of the table currently in use, with the locations
     * suitably modified to be relative to the start of the word instead of
     * relative to the script.
     *
     * That is what we are doing now. Determine the slice we need, and if not
     * empty, wrap it into a new table, and save the result into our
     * thread-global hashtable, as usual.
     */

    /*
     * First compute the range of the word within the script.
     */

    int length, end, num;
    int* wordCLLast = clNext;

    Tcl_GetStringFromObj(objPtr, &length);
    /* Is there a better way which doesn't shimmer ? */

    end = start + length; /* first char after the word */

    /*
     * Then compute the table slice covering the range of
     * the word.
     */

    while (*wordCLLast >= 0 && *wordCLLast < end) {
	wordCLLast++;
    }

    /*
     * And generate the table from the slice, if it was
     * not empty.
     */

    num = wordCLLast - clNext;
    if (num) {
	int i;
	ContLineLoc* clLocPtr =
	    TclContinuationsEnter(objPtr, num, clNext);

	/*
	 * Re-base the locations.
	 */

	for (i=0;i<num;i++) {
	    clLocPtr->loc[i] -= start;

	    /*
	     * Continuation lines coming before the string and affecting us
	     * should not happen, due to the proper maintenance of clNext
	     * during compilation.
	     */

	    if (clLocPtr->loc[i] < 0) {
		Tcl_Panic("Derived ICL data for object using offsets from before the script");
	    }
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclContinuationsCopy --
 *
 *	This procedure is a helper which copies the continuation line
 *	information associated with a Tcl_Obj* to another Tcl_Obj*.
 *	It is assumed that both contain the same string/script. Use
 *	this when a script is duplicated because it was shared.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Allocates memory for the table of continuation line locations.
 *
 * TIP #280
 *----------------------------------------------------------------------
 */

void
TclContinuationsCopy(Tcl_Obj* objPtr, Tcl_Obj* originObjPtr)
{
    ThreadSpecificData *tsdPtr = TclGetContinuationTable();
    Tcl_HashEntry* hPtr = Tcl_FindHashEntry (tsdPtr->lineCLPtr, (char*) originObjPtr);

    if (hPtr) {
	ContLineLoc* clLocPtr = (ContLineLoc*) Tcl_GetHashValue (hPtr);

	TclContinuationsEnter(objPtr, clLocPtr->num, clLocPtr->loc);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclContinuationsGet --
 *
 *	This procedure is a helper which retrieves the continuation line
 *	information associated with a Tcl_Obj*, if it has any.
 *
 * Results:
 *	A reference to the continuation line location table, or NULL
 *	if the Tcl_Obj* has no such information associated with it.
 *
 * Side effects:
 *	None.
 *
 * TIP #280
 *----------------------------------------------------------------------
 */

ContLineLoc*
TclContinuationsGet(objPtr)
     Tcl_Obj* objPtr;
{
    ThreadSpecificData *tsdPtr = TclGetContinuationTable();
    Tcl_HashEntry* hPtr = Tcl_FindHashEntry (tsdPtr->lineCLPtr, (char*) objPtr);

    if (hPtr) {
	return (ContLineLoc*) Tcl_GetHashValue (hPtr);
    } else {
	return NULL;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclThreadFinalizeObjects --
 *
 *	This procedure is a helper which releases all continuation line
 *	information currently known. It is run as a thread exit handler.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Releases memory.
 *
 * TIP #280
 *----------------------------------------------------------------------
 */

static void
TclThreadFinalizeObjects (clientData)
     ClientData clientData;
{
    /*
     * Release the hashtable tracking invisible continuation lines.
     */

    Tcl_HashEntry *hPtr;
    Tcl_HashSearch hSearch;
    ThreadSpecificData *tsdPtr = TclGetContinuationTable();

    for (hPtr = Tcl_FirstHashEntry(tsdPtr->lineCLPtr, &hSearch);
	 hPtr != NULL;
	 hPtr = Tcl_NextHashEntry(&hSearch)) {
	/*
	 * We are not using Tcl_EventuallyFree (as in
	 * TclFreeObj()) because here we can be sure that the
	 * compiler will not hold references to the data in the
	 * hashtable, and using TEF might bork the finalization
	 * sequence.
	 */
	ContLineLocFree (Tcl_GetHashValue (hPtr));
	Tcl_DeleteHashEntry (hPtr);
    }
    Tcl_DeleteHashTable (tsdPtr->lineCLPtr);
    tsdPtr->lineCLPtr = NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * ContLineLocFree --
 *
 *	The freProc for continuation line location tables.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Releases memory.
 *
 * TIP #280
 *----------------------------------------------------------------------
 */

static void
ContLineLocFree (clientData)
     char* clientData;
{
  ckfree (clientData);
}
#endif
/*
 *--------------------------------------------------------------
 *
 * Tcl_RegisterObjType --
 *
 *	This procedure is called to register a new Tcl object type
 *	in the table of all object types supported by Tcl.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The type is registered in the Tcl type table. If there was already
 *	a type with the same name as in typePtr, it is replaced with the
 *	new type.
 *
 *--------------------------------------------------------------
 */

void
Tcl_RegisterObjType(typePtr)
    Tcl_ObjType *typePtr;	/* Information about object type;
				 * storage must be statically
				 * allocated (must live forever). */
{
    int new;
    Tcl_MutexLock(&tableMutex);
    Tcl_SetHashValue(
	    Tcl_CreateHashEntry(&typeTable, typePtr->name, &new), typePtr);
    Tcl_MutexUnlock(&tableMutex);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppendAllObjTypes --
 *
 *	This procedure appends onto the argument object the name of each
 *	object type as a list element. This includes the builtin object
 *	types (e.g. int, list) as well as those added using
 *	Tcl_NewObj. These names can be used, for example, with
 *	Tcl_GetObjType to get pointers to the corresponding Tcl_ObjType
 *	structures.
 *
 * Results:
 *	The return value is normally TCL_OK; in this case the object
 *	referenced by objPtr has each type name appended to it. If an
 *	error occurs, TCL_ERROR is returned and the interpreter's result
 *	holds an error message.
 *
 * Side effects:
 *	If necessary, the object referenced by objPtr is converted into
 *	a list object.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_AppendAllObjTypes(interp, objPtr)
    Tcl_Interp *interp;		/* Interpreter used for error reporting. */
    Tcl_Obj *objPtr;		/* Points to the Tcl object onto which the
				 * name of each registered type is appended
				 * as a list element. */
{
    register Tcl_HashEntry *hPtr;
    Tcl_HashSearch search;
    int objc;
    Tcl_Obj **objv;

    /*
     * Get the test for a valid list out of the way first.
     */

    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * Type names are NUL-terminated, not counted strings.
     * This code relies on that.
     */

    Tcl_MutexLock(&tableMutex);
    for (hPtr = Tcl_FirstHashEntry(&typeTable, &search);
	    hPtr != NULL;  hPtr = Tcl_NextHashEntry(&search)) {
	Tcl_ListObjAppendElement(NULL, objPtr,
	        Tcl_NewStringObj(Tcl_GetHashKey(&typeTable, hPtr), -1));
    }
    Tcl_MutexUnlock(&tableMutex);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetObjType --
 *
 *	This procedure looks up an object type by name.
 *
 * Results:
 *	If an object type with name matching "typeName" is found, a pointer
 *	to its Tcl_ObjType structure is returned; otherwise, NULL is
 *	returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Tcl_ObjType *
Tcl_GetObjType(typeName)
    CONST char *typeName;	/* Name of Tcl object type to look up. */
{
    register Tcl_HashEntry *hPtr;
    Tcl_ObjType *typePtr = NULL;

    Tcl_MutexLock(&tableMutex);
    hPtr = Tcl_FindHashEntry(&typeTable, typeName);
    if (hPtr != (Tcl_HashEntry *) NULL) {
        typePtr = (Tcl_ObjType *) Tcl_GetHashValue(hPtr);
    }
    Tcl_MutexUnlock(&tableMutex);
    return typePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertToType --
 *
 *	Convert the Tcl object "objPtr" to have type "typePtr" if possible.
 *
 * Results:
 *	The return value is TCL_OK on success and TCL_ERROR on failure. If
 *	TCL_ERROR is returned, then the interpreter's result contains an
 *	error message unless "interp" is NULL. Passing a NULL "interp"
 *	allows this procedure to be used as a test whether the conversion
 *	could be done (and in fact was done).
 *
 * Side effects:
 *	Any internal representation for the old type is freed.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ConvertToType(interp, objPtr, typePtr)
    Tcl_Interp *interp;		/* Used for error reporting if not NULL. */
    Tcl_Obj *objPtr;		/* The object to convert. */
    Tcl_ObjType *typePtr;	/* The target type. */
{
    if (objPtr->typePtr == typePtr) {
	return TCL_OK;
    }

    /*
     * Use the target type's Tcl_SetFromAnyProc to set "objPtr"s internal
     * form as appropriate for the target type. This frees the old internal
     * representation.
     */

    return typePtr->setFromAnyProc(interp, objPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_NewObj --
 *
 *	This procedure is normally called when not debugging: i.e., when
 *	TCL_MEM_DEBUG is not defined. It creates new Tcl objects that denote
 *	the empty string. These objects have a NULL object type and NULL
 *	string representation byte pointer. Type managers call this routine
 *	to allocate new objects that they further initialize.
 *
 *	When TCL_MEM_DEBUG is defined, this procedure just returns the
 *	result of calling the debugging version Tcl_DbNewObj.
 *
 * Results:
 *	The result is a newly allocated object that represents the empty
 *	string. The new object's typePtr is set NULL and its ref count
 *	is set to 0.
 *
 * Side effects:
 *	If compiling with TCL_COMPILE_STATS, this procedure increments
 *	the global count of allocated objects (tclObjsAlloced).
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG
#undef Tcl_NewObj

Tcl_Obj *
Tcl_NewObj()
{
    return Tcl_DbNewObj("unknown", 0);
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_NewObj()
{
    register Tcl_Obj *objPtr;

    /*
     * Use the macro defined in tclInt.h - it will use the
     * correct allocator.
     */

    TclNewObj(objPtr);
    return objPtr;
}
#endif /* TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbNewObj --
 *
 *	This procedure is normally called when debugging: i.e., when
 *	TCL_MEM_DEBUG is defined. It creates new Tcl objects that denote the
 *	empty string. It is the same as the Tcl_NewObj procedure above
 *	except that it calls Tcl_DbCkalloc directly with the file name and
 *	line number from its caller. This simplifies debugging since then
 *	the [memory active] command will report the correct file name and line
 *	number when reporting objects that haven't been freed.
 *
 *	When TCL_MEM_DEBUG is not defined, this procedure just returns the
 *	result of calling Tcl_NewObj.
 *
 * Results:
 *	The result is a newly allocated that represents the empty string.
 *	The new object's typePtr is set NULL and its ref count is set to 0.
 *
 * Side effects:
 *	If compiling with TCL_COMPILE_STATS, this procedure increments
 *	the global count of allocated objects (tclObjsAlloced).
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG

Tcl_Obj *
Tcl_DbNewObj(file, line)
    register CONST char *file;	/* The name of the source file calling this
				 * procedure; used for debugging. */
    register int line;		/* Line number in the source file; used
				 * for debugging. */
{
    register Tcl_Obj *objPtr;

    /*
     * Use the macro defined in tclInt.h - it will use the
     * correct allocator.
     */

    TclDbNewObj(objPtr, file, line);
    return objPtr;
}
#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_DbNewObj(file, line)
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
    return Tcl_NewObj();
}
#endif /* TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * TclAllocateFreeObjects --
 *
 *	Procedure to allocate a number of free Tcl_Objs. This is done using
 *	a single ckalloc to reduce the overhead for Tcl_Obj allocation.
 *
 *	Assumes mutex is held.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	tclFreeObjList, the head of the list of free Tcl_Objs, is set to the
 *	first of a number of free Tcl_Obj's linked together by their
 *	internalRep.otherValuePtrs.
 *
 *----------------------------------------------------------------------
 */

#define OBJS_TO_ALLOC_EACH_TIME 100

void
TclAllocateFreeObjects()
{
    size_t bytesToAlloc = (OBJS_TO_ALLOC_EACH_TIME * sizeof(Tcl_Obj));
    char *basePtr;
    register Tcl_Obj *prevPtr, *objPtr;
    register int i;

    /*
     * This has been noted by Purify to be a potential leak.  The problem is
     * that Tcl, when not TCL_MEM_DEBUG compiled, keeps around all allocated
     * Tcl_Obj's, pointed to by tclFreeObjList, when freed instead of
     * actually freeing the memory.  TclFinalizeObjects() does not ckfree()
     * this memory, but leaves it to Tcl's memory subsystem finalziation to
     * release it.  Purify apparently can't figure that out, and fires a
     * false alarm.
     */

    basePtr = (char *) ckalloc(bytesToAlloc);
    memset(basePtr, 0, bytesToAlloc);

    prevPtr = NULL;
    objPtr = (Tcl_Obj *) basePtr;
    for (i = 0; i < OBJS_TO_ALLOC_EACH_TIME; i++) {
	objPtr->internalRep.otherValuePtr = (VOID *) prevPtr;
	prevPtr = objPtr;
	objPtr++;
    }
    tclFreeObjList = prevPtr;
}
#undef OBJS_TO_ALLOC_EACH_TIME

/*
 *----------------------------------------------------------------------
 *
 * TclFreeObj --
 *
 *	This procedure frees the memory associated with the argument
 *	object. It is called by the tcl.h macro Tcl_DecrRefCount when an
 *	object's ref count is zero. It is only "public" since it must
 *	be callable by that macro wherever the macro is used. It should not
 *	be directly called by clients.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Deallocates the storage for the object's Tcl_Obj structure
 *	after deallocating the string representation and calling the
 *	type-specific Tcl_FreeInternalRepProc to deallocate the object's
 *	internal representation. If compiling with TCL_COMPILE_STATS,
 *	this procedure increments the global count of freed objects
 *	(tclObjsFreed).
 *
 *----------------------------------------------------------------------
 */

void
TclFreeObj(objPtr)
    register Tcl_Obj *objPtr;	/* The object to be freed. */
{
    register Tcl_ObjType *typePtr = objPtr->typePtr;
    
#ifdef TCL_MEM_DEBUG
    if ((objPtr)->refCount < -1) {
	panic("Reference count for %lx was negative", objPtr);
    }
#endif /* TCL_MEM_DEBUG */

    TCL_DTRACE_OBJ_FREE(objPtr);
    if ((typePtr != NULL) && (typePtr->freeIntRepProc != NULL)) {
	typePtr->freeIntRepProc(objPtr);
    }
    Tcl_InvalidateStringRep(objPtr);

    /*
     * If debugging Tcl's memory usage, deallocate the object using ckfree.
     * Otherwise, deallocate it by adding it onto the list of free
     * Tcl_Obj structs we maintain.
     */

#if defined(TCL_MEM_DEBUG) || defined(PURIFY)
    Tcl_MutexLock(&tclObjMutex);
    ckfree((char *) objPtr);
    Tcl_MutexUnlock(&tclObjMutex);
#elif defined(TCL_THREADS) && defined(USE_THREAD_ALLOC) 
    TclThreadFreeObj(objPtr); 
#else 
    Tcl_MutexLock(&tclObjMutex);
    objPtr->internalRep.otherValuePtr = (VOID *) tclFreeObjList;
    tclFreeObjList = objPtr;
    Tcl_MutexUnlock(&tclObjMutex);
#endif /* TCL_MEM_DEBUG */

#ifdef TCL_TIP280
    /*
     * We cannot use TclGetContinuationTable() here, because that may
     * re-initialize the thread-data for calls coming after the
     * finalization. We have to access it using the low-level call and then
     * check for validity. This function can be called after
     * TclFinalizeThreadData() has already killed the thread-global data
     * structures. Performing TCL_TSD_INIT will leave us with an
     * un-initialized memory block upon which we crash (if we where to access
     * the uninitialized hashtable).
     */

    {
	ThreadSpecificData* tsdPtr = TCL_TSD_INIT(&dataKey);
	if (tsdPtr->lineCLPtr) {
	    Tcl_HashEntry* hPtr = Tcl_FindHashEntry (tsdPtr->lineCLPtr, (char *) objPtr);
	    if (hPtr) {
		Tcl_EventuallyFree (Tcl_GetHashValue (hPtr), ContLineLocFree);
		Tcl_DeleteHashEntry (hPtr);
	    }
	}
    }
#endif
    TclIncrObjsFreed();
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DuplicateObj --
 *
 *	Create and return a new object that is a duplicate of the argument
 *	object.
 *
 * Results:
 *	The return value is a pointer to a newly created Tcl_Obj. This
 *	object has reference count 0 and the same type, if any, as the
 *	source object objPtr. Also:
 *	  1) If the source object has a valid string rep, we copy it;
 *	     otherwise, the duplicate's string rep is set NULL to mark
 *	     it invalid.
 *	  2) If the source object has an internal representation (i.e. its
 *	     typePtr is non-NULL), the new object's internal rep is set to
 *	     a copy; otherwise the new internal rep is marked invalid.
 *
 * Side effects:
 *      What constitutes "copying" the internal representation depends on
 *	the type. For example, if the argument object is a list,
 *	the element objects it points to will not actually be copied but
 *	will be shared with the duplicate list. That is, the ref counts of
 *	the element objects will be incremented.
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj *
Tcl_DuplicateObj(objPtr)
    register Tcl_Obj *objPtr;		/* The object to duplicate. */
{
    register Tcl_ObjType *typePtr = objPtr->typePtr;
    register Tcl_Obj *dupPtr;

    TclNewObj(dupPtr);

    if (objPtr->bytes == NULL) {
	dupPtr->bytes = NULL;
    } else if (objPtr->bytes != tclEmptyStringRep) {
	TclInitStringRep(dupPtr, objPtr->bytes, objPtr->length);
    }
    
    if (typePtr != NULL) {
	if (typePtr->dupIntRepProc == NULL) {
	    dupPtr->internalRep = objPtr->internalRep;
	    dupPtr->typePtr = typePtr;
	} else {
	    (*typePtr->dupIntRepProc)(objPtr, dupPtr);
	}
    }
    return dupPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetString --
 *
 *	Returns the string representation byte array pointer for an object.
 *
 * Results:
 *	Returns a pointer to the string representation of objPtr. The byte
 *	array referenced by the returned pointer must not be modified by the
 *	caller. Furthermore, the caller must copy the bytes if they need to
 *	retain them since the object's string rep can change as a result of
 *	other operations.
 *
 * Side effects:
 *	May call the object's updateStringProc to update the string
 *	representation from the internal representation.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetString(objPtr)
    register Tcl_Obj *objPtr;	/* Object whose string rep byte pointer
				 * should be returned. */
{
    if (objPtr->bytes != NULL) {
	return objPtr->bytes;
    }

    if (objPtr->typePtr->updateStringProc == NULL) {
	panic("UpdateStringProc should not be invoked for type %s",
		objPtr->typePtr->name);
    }
    (*objPtr->typePtr->updateStringProc)(objPtr);
    return objPtr->bytes;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetStringFromObj --
 *
 *	Returns the string representation's byte array pointer and length
 *	for an object.
 *
 * Results:
 *	Returns a pointer to the string representation of objPtr. If
 *	lengthPtr isn't NULL, the length of the string representation is
 *	stored at *lengthPtr. The byte array referenced by the returned
 *	pointer must not be modified by the caller. Furthermore, the
 *	caller must copy the bytes if they need to retain them since the
 *	object's string rep can change as a result of other operations.
 *
 * Side effects:
 *	May call the object's updateStringProc to update the string
 *	representation from the internal representation.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetStringFromObj(objPtr, lengthPtr)
    register Tcl_Obj *objPtr;	/* Object whose string rep byte pointer should
				 * be returned. */
    register int *lengthPtr;	/* If non-NULL, the location where the string
				 * rep's byte array length should * be stored.
				 * If NULL, no length is stored. */
{
    if (objPtr->bytes == NULL) {
	if (objPtr->typePtr->updateStringProc == NULL) {
	    panic("UpdateStringProc should not be invoked for type %s",
		    objPtr->typePtr->name);
	}
	(*objPtr->typePtr->updateStringProc)(objPtr);
    }

    if (lengthPtr != NULL) {
	*lengthPtr = objPtr->length;
    }
    return objPtr->bytes;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_InvalidateStringRep --
 *
 *	This procedure is called to invalidate an object's string
 *	representation. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Deallocates the storage for any old string representation, then
 *	sets the string representation NULL to mark it invalid.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_InvalidateStringRep(objPtr)
     register Tcl_Obj *objPtr;	/* Object whose string rep byte pointer
				 * should be freed. */
{
    if (objPtr->bytes != NULL) {
	if (objPtr->bytes != tclEmptyStringRep) {
	    ckfree((char *) objPtr->bytes);
	}
	objPtr->bytes = NULL;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_NewBooleanObj --
 *
 *	This procedure is normally called when not debugging: i.e., when
 *	TCL_MEM_DEBUG is not defined. It creates a new Tcl_Obj and
 *	initializes it from the argument boolean value. A nonzero
 *	"boolValue" is coerced to 1.
 *
 *	When TCL_MEM_DEBUG is defined, this procedure just returns the
 *	result of calling the debugging version Tcl_DbNewBooleanObj.
 *
 * Results:
 *	The newly created object is returned. This object will have an
 *	invalid string representation. The returned object has ref count 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG
#undef Tcl_NewBooleanObj

Tcl_Obj *
Tcl_NewBooleanObj(boolValue)
    register int boolValue;	/* Boolean used to initialize new object. */
{
    return Tcl_DbNewBooleanObj(boolValue, "unknown", 0);
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_NewBooleanObj(boolValue)
    register int boolValue;	/* Boolean used to initialize new object. */
{
    register Tcl_Obj *objPtr;

    TclNewObj(objPtr);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.longValue = (boolValue? 1 : 0);
    objPtr->typePtr = &tclBooleanType;
    return objPtr;
}
#endif /* TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbNewBooleanObj --
 *
 *	This procedure is normally called when debugging: i.e., when
 *	TCL_MEM_DEBUG is defined. It creates new boolean objects. It is the
 *	same as the Tcl_NewBooleanObj procedure above except that it calls
 *	Tcl_DbCkalloc directly with the file name and line number from its
 *	caller. This simplifies debugging since then the [memory active]
 *	command	will report the correct file name and line number when
 *	reporting objects that haven't been freed.
 *
 *	When TCL_MEM_DEBUG is not defined, this procedure just returns the
 *	result of calling Tcl_NewBooleanObj.
 *
 * Results:
 *	The newly created object is returned. This object will have an
 *	invalid string representation. The returned object has ref count 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG

Tcl_Obj *
Tcl_DbNewBooleanObj(boolValue, file, line)
    register int boolValue;	/* Boolean used to initialize new object. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
    register Tcl_Obj *objPtr;

    TclDbNewObj(objPtr, file, line);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.longValue = (boolValue? 1 : 0);
    objPtr->typePtr = &tclBooleanType;
    return objPtr;
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_DbNewBooleanObj(boolValue, file, line)
    register int boolValue;	/* Boolean used to initialize new object. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
    return Tcl_NewBooleanObj(boolValue);
}
#endif /* TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetBooleanObj --
 *
 *	Modify an object to be a boolean object and to have the specified
 *	boolean value. A nonzero "boolValue" is coerced to 1.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's old string rep, if any, is freed. Also, any old
 *	internal rep is freed.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetBooleanObj(objPtr, boolValue)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    register int boolValue;	/* Boolean used to set object's value. */
{
    register Tcl_ObjType *oldTypePtr = objPtr->typePtr;

    if (Tcl_IsShared(objPtr)) {
	panic("Tcl_SetBooleanObj called with shared object");
    }
    
    if ((oldTypePtr != NULL) && (oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }
    
    objPtr->internalRep.longValue = (boolValue? 1 : 0);
    objPtr->typePtr = &tclBooleanType;
    Tcl_InvalidateStringRep(objPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetBooleanFromObj --
 *
 *	Attempt to return a boolean from the Tcl object "objPtr". If the
 *	object is not already a boolean, an attempt will be made to convert
 *	it to one.
 *
 * Results:
 *	The return value is a standard Tcl object result. If an error occurs
 *	during conversion, an error message is left in the interpreter's
 *	result unless "interp" is NULL.
 *
 * Side effects:
 *	If the object is not already a boolean, the conversion will free
 *	any old internal representation. 
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetBooleanFromObj(interp, objPtr, boolPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object from which to get boolean. */
    register int *boolPtr;	/* Place to store resulting boolean. */
{
    register int result;

    if (objPtr->typePtr == &tclBooleanType) {
	result = TCL_OK;
    } else {
	result = SetBooleanFromAny(interp, objPtr);
    }

    if (result == TCL_OK) {
	*boolPtr = (int) objPtr->internalRep.longValue;
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * SetBooleanFromAny --
 *
 *	Attempt to generate a boolean internal form for the Tcl object
 *	"objPtr".
 *
 * Results:
 *	The return value is a standard Tcl result. If an error occurs during
 *	conversion, an error message is left in the interpreter's result
 *	unless "interp" is NULL.
 *
 * Side effects:
 *	If no error occurs, an integer 1 or 0 is stored as "objPtr"s
 *	internal representation and the type of "objPtr" is set to boolean.
 *
 *----------------------------------------------------------------------
 */

static int
SetBooleanFromAny(interp, objPtr)
    Tcl_Interp *interp;		/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object to convert. */
{
    Tcl_ObjType *oldTypePtr = objPtr->typePtr;
    char *string, *end;
    register char c;
    char lowerCase[10];
    int newBool, length;
    register int i;

    /*
     * Get the string representation. Make it up-to-date if necessary.
     */
    
    string = Tcl_GetStringFromObj(objPtr, &length);

    /*
     * Use the obvious shortcuts for numerical values; if objPtr is not
     * of numerical type, parse its string rep.
     */
	
    if (objPtr->typePtr == &tclIntType) {
	newBool = (objPtr->internalRep.longValue != 0);
    } else if (objPtr->typePtr == &tclDoubleType) {
	newBool = (objPtr->internalRep.doubleValue != 0.0);
    } else if (objPtr->typePtr == &tclWideIntType) {
	newBool = (objPtr->internalRep.wideValue != 0);
    } else {
	/*
	 * Copy the string converting its characters to lower case.
	 */
	
	for (i = 0;  (i < 9) && (i < length);  i++) {
	    c = string[i];
	    /*
	     * Weed out international characters so we can safely operate
	     * on single bytes.
	     */
	    
	    if (c & 0x80) {
		goto badBoolean;
	    }
	    if (Tcl_UniCharIsUpper(UCHAR(c))) {
		c = (char) Tcl_UniCharToLower(UCHAR(c));
	    }
	    lowerCase[i] = c;
	}
	lowerCase[i] = 0;
	
	/*
	 * Parse the string as a boolean. We use an implementation here that
	 * doesn't report errors in interp if interp is NULL.
	 */
	
	c = lowerCase[0];
	if ((c == '0') && (lowerCase[1] == '\0')) {
	    newBool = 0;
	} else if ((c == '1') && (lowerCase[1] == '\0')) {
	    newBool = 1;
	} else if ((c == 'y') && (strncmp(lowerCase, "yes", (size_t) length) == 0)) {
	    newBool = 1;
	} else if ((c == 'n') && (strncmp(lowerCase, "no", (size_t) length) == 0)) {
	    newBool = 0;
	} else if ((c == 't') && (strncmp(lowerCase, "true", (size_t) length) == 0)) {
	    newBool = 1;
	} else if ((c == 'f') && (strncmp(lowerCase, "false", (size_t) length) == 0)) {
	    newBool = 0;
	} else if ((c == 'o') && (length >= 2)) {
	    if (strncmp(lowerCase, "on", (size_t) length) == 0) {
		newBool = 1;
	    } else if (strncmp(lowerCase, "off", (size_t) length) == 0) {
		newBool = 0;
	    } else {
		goto badBoolean;
	    }
	} else {
	    double dbl;
	    /*
	     * Boolean values can be extracted from ints or doubles.  Note
	     * that we don't use strtoul or strtoull here because we don't
	     * care about what the value is, just whether it is equal to
	     * zero or not.
	     */
#ifdef TCL_WIDE_INT_IS_LONG
	    newBool = strtol(string, &end, 0);
	    if (end != string) {
		/*
		 * Make sure the string has no garbage after the end of
		 * the int.
		 */
		while ((end < (string+length))
		       && isspace(UCHAR(*end))) { /* INTL: ISO only */
		    end++;
		}
		if (end == (string+length)) {
		    newBool = (newBool != 0);
		    goto goodBoolean;
		}
	    }
#else /* !TCL_WIDE_INT_IS_LONG */
	    Tcl_WideInt wide = strtoll(string, &end, 0);
	    if (end != string) {
		/*
		 * Make sure the string has no garbage after the end of
		 * the wide int.
		 */
		while ((end < (string+length))
		       && isspace(UCHAR(*end))) { /* INTL: ISO only */
		    end++;
		}
		if (end == (string+length)) {
		    newBool = (wide != Tcl_LongAsWide(0));
		    goto goodBoolean;
		}
	    }
#endif /* TCL_WIDE_INT_IS_LONG */
	    /*
	     * Still might be a string containing the characters representing an
	     * int or double that wasn't handled above. This would be a string
	     * like "27" or "1.0" that is non-zero and not "1". Such a string
	     * would result in the boolean value true. We try converting to
	     * double. If that succeeds and the resulting double is non-zero, we
	     * have a "true". Note that numbers can't have embedded NULLs.
	     */
	    
	    dbl = strtod(string, &end);
	    if (end == string) {
		goto badBoolean;
	    }
	    
	    /*
	     * Make sure the string has no garbage after the end of the double.
	     */
	    
	    while ((end < (string+length))
		   && isspace(UCHAR(*end))) { /* INTL: ISO only */
		end++;
	    }
	    if (end != (string+length)) {
		goto badBoolean;
	    }
	    newBool = (dbl != 0.0);
	}
    }

    /*
     * Free the old internalRep before setting the new one. We do this as
     * late as possible to allow the conversion code, in particular
     * Tcl_GetStringFromObj, to use that old internalRep.
     */

    goodBoolean:
    if ((oldTypePtr != NULL) &&	(oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }

    objPtr->internalRep.longValue = newBool;
    objPtr->typePtr = &tclBooleanType;
    return TCL_OK;

    badBoolean:
    if (interp != NULL) {
	/*
	 * Must copy string before resetting the result in case a caller
	 * is trying to convert the interpreter's result to a boolean.
	 */
	
	char buf[100];
	sprintf(buf, "expected boolean value but got \"%.50s\"", string);
	Tcl_ResetResult(interp);
	Tcl_AppendToObj(Tcl_GetObjResult(interp), buf, -1);
    }
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * UpdateStringOfBoolean --
 *
 *	Update the string representation for a boolean object.
 *	Note: This procedure does not free an existing old string rep
 *	so storage will be lost if this has not already been done. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's string is set to a valid string that results from
 *	the boolean-to-string conversion.
 *
 *----------------------------------------------------------------------
 */

static void
UpdateStringOfBoolean(objPtr)
    register Tcl_Obj *objPtr;	/* Int object whose string rep to update. */
{
    char *s = ckalloc((unsigned) 2);
    
    s[0] = (char) (objPtr->internalRep.longValue? '1' : '0');
    s[1] = '\0';
    objPtr->bytes = s;
    objPtr->length = 1;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_NewDoubleObj --
 *
 *	This procedure is normally called when not debugging: i.e., when
 *	TCL_MEM_DEBUG is not defined. It creates a new double object and
 *	initializes it from the argument double value.
 *
 *	When TCL_MEM_DEBUG is defined, this procedure just returns the
 *	result of calling the debugging version Tcl_DbNewDoubleObj.
 *
 * Results:
 *	The newly created object is returned. This object will have an
 *	invalid string representation. The returned object has ref count 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG
#undef Tcl_NewDoubleObj

Tcl_Obj *
Tcl_NewDoubleObj(dblValue)
    register double dblValue;	/* Double used to initialize the object. */
{
    return Tcl_DbNewDoubleObj(dblValue, "unknown", 0);
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_NewDoubleObj(dblValue)
    register double dblValue;	/* Double used to initialize the object. */
{
    register Tcl_Obj *objPtr;

    TclNewObj(objPtr);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.doubleValue = dblValue;
    objPtr->typePtr = &tclDoubleType;
    return objPtr;
}
#endif /* if TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbNewDoubleObj --
 *
 *	This procedure is normally called when debugging: i.e., when
 *	TCL_MEM_DEBUG is defined. It creates new double objects. It is the
 *	same as the Tcl_NewDoubleObj procedure above except that it calls
 *	Tcl_DbCkalloc directly with the file name and line number from its
 *	caller. This simplifies debugging since then the [memory active]
 *	command	will report the correct file name and line number when
 *	reporting objects that haven't been freed.
 *
 *	When TCL_MEM_DEBUG is not defined, this procedure just returns the
 *	result of calling Tcl_NewDoubleObj.
 *
 * Results:
 *	The newly created object is returned. This object will have an
 *	invalid string representation. The returned object has ref count 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG

Tcl_Obj *
Tcl_DbNewDoubleObj(dblValue, file, line)
    register double dblValue;	/* Double used to initialize the object. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
    register Tcl_Obj *objPtr;

    TclDbNewObj(objPtr, file, line);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.doubleValue = dblValue;
    objPtr->typePtr = &tclDoubleType;
    return objPtr;
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_DbNewDoubleObj(dblValue, file, line)
    register double dblValue;	/* Double used to initialize the object. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
    return Tcl_NewDoubleObj(dblValue);
}
#endif /* TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetDoubleObj --
 *
 *	Modify an object to be a double object and to have the specified
 *	double value.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's old string rep, if any, is freed. Also, any old
 *	internal rep is freed.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetDoubleObj(objPtr, dblValue)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    register double dblValue;	/* Double used to set the object's value. */
{
    register Tcl_ObjType *oldTypePtr = objPtr->typePtr;

    if (Tcl_IsShared(objPtr)) {
	panic("Tcl_SetDoubleObj called with shared object");
    }

    if ((oldTypePtr != NULL) && (oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }
    
    objPtr->internalRep.doubleValue = dblValue;
    objPtr->typePtr = &tclDoubleType;
    Tcl_InvalidateStringRep(objPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetDoubleFromObj --
 *
 *	Attempt to return a double from the Tcl object "objPtr". If the
 *	object is not already a double, an attempt will be made to convert
 *	it to one.
 *
 * Results:
 *	The return value is a standard Tcl object result. If an error occurs
 *	during conversion, an error message is left in the interpreter's
 *	result unless "interp" is NULL.
 *
 * Side effects:
 *	If the object is not already a double, the conversion will free
 *	any old internal representation.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetDoubleFromObj(interp, objPtr, dblPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object from which to get a double. */
    register double *dblPtr;	/* Place to store resulting double. */
{
    register int result;
    
    if (objPtr->typePtr == &tclDoubleType) {
	*dblPtr = objPtr->internalRep.doubleValue;
	return TCL_OK;
    }

    result = SetDoubleFromAny(interp, objPtr);
    if (result == TCL_OK) {
	*dblPtr = objPtr->internalRep.doubleValue;
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * SetDoubleFromAny --
 *
 *	Attempt to generate an double-precision floating point internal form
 *	for the Tcl object "objPtr".
 *
 * Results:
 *	The return value is a standard Tcl object result. If an error occurs
 *	during conversion, an error message is left in the interpreter's
 *	result unless "interp" is NULL.
 *
 * Side effects:
 *	If no error occurs, a double is stored as "objPtr"s internal
 *	representation.
 *
 *----------------------------------------------------------------------
 */

static int
SetDoubleFromAny(interp, objPtr)
    Tcl_Interp *interp;		/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object to convert. */
{
    Tcl_ObjType *oldTypePtr = objPtr->typePtr;
    char *string, *end;
    double newDouble;
    int length;

    /*
     * Get the string representation. Make it up-to-date if necessary.
     */

    string = Tcl_GetStringFromObj(objPtr, &length);

    /*
     * Now parse "objPtr"s string as an double. Numbers can't have embedded
     * NULLs. We use an implementation here that doesn't report errors in
     * interp if interp is NULL.
     */

    errno = 0;
    newDouble = strtod(string, &end);
    if (end == string) {
	badDouble:
	if (interp != NULL) {
	    /*
	     * Must copy string before resetting the result in case a caller
	     * is trying to convert the interpreter's result to an int.
	     */
	    
	    char buf[100];
	    sprintf(buf, "expected floating-point number but got \"%.50s\"",
	            string);
	    Tcl_ResetResult(interp);
	    Tcl_AppendToObj(Tcl_GetObjResult(interp), buf, -1);
	}
	return TCL_ERROR;
    }
    if (errno != 0) {
	if (interp != NULL) {
	    TclExprFloatError(interp, newDouble);
	}
	return TCL_ERROR;
    }

    /*
     * Make sure that the string has no garbage after the end of the double.
     */
    
    while ((end < (string+length))
	    && isspace(UCHAR(*end))) { /* INTL: ISO space. */
	end++;
    }
    if (end != (string+length)) {
	goto badDouble;
    }
    
    /*
     * The conversion to double succeeded. Free the old internalRep before
     * setting the new one. We do this as late as possible to allow the
     * conversion code, in particular Tcl_GetStringFromObj, to use that old
     * internalRep.
     */
    
    if ((oldTypePtr != NULL) &&	(oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }

    objPtr->internalRep.doubleValue = newDouble;
    objPtr->typePtr = &tclDoubleType;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * UpdateStringOfDouble --
 *
 *	Update the string representation for a double-precision floating
 *	point object. This must obey the current tcl_precision value for
 *	double-to-string conversions. Note: This procedure does not free an
 *	existing old string rep so storage will be lost if this has not
 *	already been done.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's string is set to a valid string that results from
 *	the double-to-string conversion.
 *
 *----------------------------------------------------------------------
 */

static void
UpdateStringOfDouble(objPtr)
    register Tcl_Obj *objPtr;	/* Double obj with string rep to update. */
{
    char buffer[TCL_DOUBLE_SPACE];
    register int len;
    
    Tcl_PrintDouble((Tcl_Interp *) NULL, objPtr->internalRep.doubleValue,
	    buffer);
    len = strlen(buffer);
    
    objPtr->bytes = (char *) ckalloc((unsigned) len + 1);
    strcpy(objPtr->bytes, buffer);
    objPtr->length = len;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_NewIntObj --
 *
 *	If a client is compiled with TCL_MEM_DEBUG defined, calls to
 *	Tcl_NewIntObj to create a new integer object end up calling the
 *	debugging procedure Tcl_DbNewLongObj instead.
 *
 *	Otherwise, if the client is compiled without TCL_MEM_DEBUG defined,
 *	calls to Tcl_NewIntObj result in a call to one of the two
 *	Tcl_NewIntObj implementations below. We provide two implementations
 *	so that the Tcl core can be compiled to do memory debugging of the 
 *	core even if a client does not request it for itself.
 *
 *	Integer and long integer objects share the same "integer" type
 *	implementation. We store all integers as longs and Tcl_GetIntFromObj
 *	checks whether the current value of the long can be represented by
 *	an int.
 *
 * Results:
 *	The newly created object is returned. This object will have an
 *	invalid string representation. The returned object has ref count 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG
#undef Tcl_NewIntObj

Tcl_Obj *
Tcl_NewIntObj(intValue)
    register int intValue;	/* Int used to initialize the new object. */
{
    return Tcl_DbNewLongObj((long)intValue, "unknown", 0);
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_NewIntObj(intValue)
    register int intValue;	/* Int used to initialize the new object. */
{
    register Tcl_Obj *objPtr;

    TclNewObj(objPtr);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.longValue = (long)intValue;
    objPtr->typePtr = &tclIntType;
    return objPtr;
}
#endif /* if TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetIntObj --
 *
 *	Modify an object to be an integer and to have the specified integer
 *	value.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's old string rep, if any, is freed. Also, any old
 *	internal rep is freed. 
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetIntObj(objPtr, intValue)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    register int intValue;	/* Integer used to set object's value. */
{
    register Tcl_ObjType *oldTypePtr = objPtr->typePtr;

    if (Tcl_IsShared(objPtr)) {
	panic("Tcl_SetIntObj called with shared object");
    }
    
    if ((oldTypePtr != NULL) && (oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }
    
    objPtr->internalRep.longValue = (long) intValue;
    objPtr->typePtr = &tclIntType;
    Tcl_InvalidateStringRep(objPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetIntFromObj --
 *
 *	Attempt to return an int from the Tcl object "objPtr". If the object
 *	is not already an int, an attempt will be made to convert it to one.
 *
 *	Integer and long integer objects share the same "integer" type
 *	implementation. We store all integers as longs and Tcl_GetIntFromObj
 *	checks whether the current value of the long can be represented by
 *	an int.
 *
 * Results:
 *	The return value is a standard Tcl object result. If an error occurs
 *	during conversion or if the long integer held by the object
 *	can not be represented by an int, an error message is left in
 *	the interpreter's result unless "interp" is NULL.
 *
 * Side effects:
 *	If the object is not already an int, the conversion will free
 *	any old internal representation.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetIntFromObj(interp, objPtr, intPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object from which to get a int. */
    register int *intPtr;	/* Place to store resulting int. */
{
    int result;
    Tcl_WideInt w = 0;

    /*
     * If the object isn't already an integer of any width, try to
     * convert it to one.
     */

    if (objPtr->typePtr != &tclIntType && objPtr->typePtr != &tclWideIntType) {
	result = SetIntOrWideFromAny(interp, objPtr);
	if (result != TCL_OK) {
	    return result;
	}
    }

    /*
     * Object should now be either int or wide. Get its value.
     */

#ifndef TCL_WIDE_INT_IS_LONG
    if (objPtr->typePtr == &tclWideIntType) {
	w = objPtr->internalRep.wideValue;
    } else
#endif
    {
	w = Tcl_LongAsWide(objPtr->internalRep.longValue);
    }

    if ((LLONG_MAX > UINT_MAX)
	    && ((w > UINT_MAX) || (w < -(Tcl_WideInt)UINT_MAX))) {
	if (interp != NULL) {
	    Tcl_SetObjResult(interp, Tcl_NewStringObj(
		"integer value too large to represent as non-long integer",
		-1));
	}
	return TCL_ERROR;
    }
    *intPtr = (int)w;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * SetIntFromAny --
 *
 *	Attempts to force the internal representation for a Tcl object
 *	to tclIntType, specifically.
 *
 * Results:
 *	The return value is a standard object Tcl result.  If an
 *	error occurs during conversion, an error message is left in
 *	the interpreter's result unless "interp" is NULL.
 *
 *----------------------------------------------------------------------
 */

static int
SetIntFromAny( Tcl_Interp* interp, 
				/* Tcl interpreter */
	       Tcl_Obj* objPtr )
				/* Pointer to the object to convert */
{
    int result;

    result = SetIntOrWideFromAny( interp, objPtr );
    if ( result != TCL_OK ) {
	return result;
    }
    if ( objPtr->typePtr != &tclIntType ) {
	if ( interp != NULL ) {
	    char *s = "integer value too large to represent";
	    Tcl_ResetResult(interp);
	    Tcl_AppendToObj(Tcl_GetObjResult(interp), s, -1);
	    Tcl_SetErrorCode(interp, "ARITH", "IOVERFLOW", s, (char *) NULL);
	}
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * SetIntOrWideFromAny --
 *
 *	Attempt to generate an integer internal form for the Tcl object
 *	"objPtr".
 *
 * Results:
 *	The return value is a standard object Tcl result. If an error occurs
 *	during conversion, an error message is left in the interpreter's
 *	result unless "interp" is NULL.
 *
 * Side effects:
 *	If no error occurs, an int is stored as "objPtr"s internal
 *	representation. 
 *
 *----------------------------------------------------------------------
 */

static int
SetIntOrWideFromAny(interp, objPtr)
    Tcl_Interp *interp;		/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object to convert. */
{
    Tcl_ObjType *oldTypePtr = objPtr->typePtr;
    char *string, *end;
    int length;
    register char *p;
    unsigned long newLong;
    int isNegative = 0;
    int isWide = 0;

    /*
     * Get the string representation. Make it up-to-date if necessary.
     */

    p = string = Tcl_GetStringFromObj(objPtr, &length);

    /*
     * Now parse "objPtr"s string as an int. We use an implementation here
     * that doesn't report errors in interp if interp is NULL. Note: use
     * strtoul instead of strtol for integer conversions to allow full-size
     * unsigned numbers, but don't depend on strtoul to handle sign
     * characters; it won't in some implementations.
     */

    errno = 0;
    for ( ;  isspace(UCHAR(*p));  p++) { /* INTL: ISO space. */
	/* Empty loop body. */
    }
    if (*p == '-') {
	p++;
	isNegative = 1;
    } else if (*p == '+') {
	p++;
    }
    if (!isdigit(UCHAR(*p))) {
	badInteger:
	if (interp != NULL) {
	    /*
	     * Must copy string before resetting the result in case a caller
	     * is trying to convert the interpreter's result to an int.
	     */
	    
	    char buf[100];
	    sprintf(buf, "expected integer but got \"%.50s\"", string);
	    Tcl_ResetResult(interp);
	    Tcl_AppendToObj(Tcl_GetObjResult(interp), buf, -1);
	    TclCheckBadOctal(interp, string);
	}
	return TCL_ERROR;
    }
    newLong = strtoul(p, &end, 0);
    if (end == p) {
	goto badInteger;
    }
    if (errno == ERANGE) {
	if (interp != NULL) {
	    char *s = "integer value too large to represent";
	    Tcl_ResetResult(interp);
	    Tcl_AppendToObj(Tcl_GetObjResult(interp), s, -1);
	    Tcl_SetErrorCode(interp, "ARITH", "IOVERFLOW", s, (char *) NULL);
	}
	return TCL_ERROR;
    }

    /*
     * Make sure that the string has no garbage after the end of the int.
     */
    
    while ((end < (string+length))
	    && isspace(UCHAR(*end))) { /* INTL: ISO space. */
	end++;
    }
    if (end != (string+length)) {
	goto badInteger;
    }

    /*
     * If the resulting integer will exceed the range of a long,
     * put it into a wide instead.  (Tcl Bug #868489)
     */

#ifndef TCL_WIDE_INT_IS_LONG
    if ((isNegative && newLong > (unsigned long) (LONG_MAX) + 1)
	    || (!isNegative && newLong > LONG_MAX)) {
	isWide = 1;
    }
#endif

    /*
     * The conversion to int succeeded. Free the old internalRep before
     * setting the new one. We do this as late as possible to allow the
     * conversion code, in particular Tcl_GetStringFromObj, to use that old
     * internalRep.
     */

    if ((oldTypePtr != NULL) &&	(oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }

    if (isWide) {
	objPtr->internalRep.wideValue =
		(isNegative ? -(Tcl_WideInt)newLong : (Tcl_WideInt)newLong);
	objPtr->typePtr = &tclWideIntType;
    } else {
	objPtr->internalRep.longValue =
		(isNegative ? -(long)newLong : (long)newLong);
	objPtr->typePtr = &tclIntType;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * UpdateStringOfInt --
 *
 *	Update the string representation for an integer object.
 *	Note: This procedure does not free an existing old string rep
 *	so storage will be lost if this has not already been done. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's string is set to a valid string that results from
 *	the int-to-string conversion.
 *
 *----------------------------------------------------------------------
 */

static void
UpdateStringOfInt(objPtr)
    register Tcl_Obj *objPtr;	/* Int object whose string rep to update. */
{
    char buffer[TCL_INTEGER_SPACE];
    register int len;
    
    len = TclFormatInt(buffer, objPtr->internalRep.longValue);
    
    objPtr->bytes = ckalloc((unsigned) len + 1);
    strcpy(objPtr->bytes, buffer);
    objPtr->length = len;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_NewLongObj --
 *
 *	If a client is compiled with TCL_MEM_DEBUG defined, calls to
 *	Tcl_NewLongObj to create a new long integer object end up calling
 *	the debugging procedure Tcl_DbNewLongObj instead.
 *
 *	Otherwise, if the client is compiled without TCL_MEM_DEBUG defined,
 *	calls to Tcl_NewLongObj result in a call to one of the two
 *	Tcl_NewLongObj implementations below. We provide two implementations
 *	so that the Tcl core can be compiled to do memory debugging of the 
 *	core even if a client does not request it for itself.
 *
 *	Integer and long integer objects share the same "integer" type
 *	implementation. We store all integers as longs and Tcl_GetIntFromObj
 *	checks whether the current value of the long can be represented by
 *	an int.
 *
 * Results:
 *	The newly created object is returned. This object will have an
 *	invalid string representation. The returned object has ref count 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG
#undef Tcl_NewLongObj

Tcl_Obj *
Tcl_NewLongObj(longValue)
    register long longValue;	/* Long integer used to initialize the
				 * new object. */
{
    return Tcl_DbNewLongObj(longValue, "unknown", 0);
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_NewLongObj(longValue)
    register long longValue;	/* Long integer used to initialize the
				 * new object. */
{
    register Tcl_Obj *objPtr;

    TclNewObj(objPtr);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.longValue = longValue;
    objPtr->typePtr = &tclIntType;
    return objPtr;
}
#endif /* if TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbNewLongObj --
 *
 *	If a client is compiled with TCL_MEM_DEBUG defined, calls to
 *	Tcl_NewIntObj and Tcl_NewLongObj to create new integer or
 *	long integer objects end up calling the debugging procedure
 *	Tcl_DbNewLongObj instead. We provide two implementations of
 *	Tcl_DbNewLongObj so that whether the Tcl core is compiled to do
 *	memory debugging of the core is independent of whether a client
 *	requests debugging for itself.
 *
 *	When the core is compiled with TCL_MEM_DEBUG defined,
 *	Tcl_DbNewLongObj calls Tcl_DbCkalloc directly with the file name and
 *	line number from its caller. This simplifies debugging since then
 *	the [memory active] command will report the caller's file name and
 *	line number when reporting objects that haven't been freed.
 *
 *	Otherwise, when the core is compiled without TCL_MEM_DEBUG defined,
 *	this procedure just returns the result of calling Tcl_NewLongObj.
 *
 * Results:
 *	The newly created long integer object is returned. This object
 *	will have an invalid string representation. The returned object has
 *	ref count 0.
 *
 * Side effects:
 *	Allocates memory.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG

Tcl_Obj *
Tcl_DbNewLongObj(longValue, file, line)
    register long longValue;	/* Long integer used to initialize the
				 * new object. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
    register Tcl_Obj *objPtr;

    TclDbNewObj(objPtr, file, line);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.longValue = longValue;
    objPtr->typePtr = &tclIntType;
    return objPtr;
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_DbNewLongObj(longValue, file, line)
    register long longValue;	/* Long integer used to initialize the
				 * new object. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
    return Tcl_NewLongObj(longValue);
}
#endif /* TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetLongObj --
 *
 *	Modify an object to be an integer object and to have the specified
 *	long integer value.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's old string rep, if any, is freed. Also, any old
 *	internal rep is freed. 
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetLongObj(objPtr, longValue)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    register long longValue;	/* Long integer used to initialize the
				 * object's value. */
{
    register Tcl_ObjType *oldTypePtr = objPtr->typePtr;

    if (Tcl_IsShared(objPtr)) {
	panic("Tcl_SetLongObj called with shared object");
    }

    if ((oldTypePtr != NULL) && (oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }
    
    objPtr->internalRep.longValue = longValue;
    objPtr->typePtr = &tclIntType;
    Tcl_InvalidateStringRep(objPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetLongFromObj --
 *
 *	Attempt to return an long integer from the Tcl object "objPtr". If
 *	the object is not already an int object, an attempt will be made to
 *	convert it to one.
 *
 * Results:
 *	The return value is a standard Tcl object result. If an error occurs
 *	during conversion, an error message is left in the interpreter's
 *	result unless "interp" is NULL.
 *
 * Side effects:
 *	If the object is not already an int object, the conversion will free
 *	any old internal representation.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetLongFromObj(interp, objPtr, longPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object from which to get a long. */
    register long *longPtr;	/* Place to store resulting long. */
{
    register int result;
    
    if (objPtr->typePtr != &tclIntType && objPtr->typePtr != &tclWideIntType) {
	result = SetIntOrWideFromAny(interp, objPtr);
	if (result != TCL_OK) {
	    return result;
	}
    }

#ifndef TCL_WIDE_INT_IS_LONG
    if (objPtr->typePtr == &tclWideIntType) {
	/*
	 * If the object is already a wide integer, don't convert it.
	 * This code allows for any integer in the range -ULONG_MAX to
	 * ULONG_MAX to be converted to a long, ignoring overflow.
	 * The rule preserves existing semantics for conversion of
	 * integers on input, but avoids inadvertent demotion of
	 * wide integers to 32-bit ones in the internal rep.
	 */

	Tcl_WideInt w = objPtr->internalRep.wideValue;
	if (w >= -(Tcl_WideInt)(ULONG_MAX) && w <= (Tcl_WideInt)(ULONG_MAX)) {
	    *longPtr = Tcl_WideAsLong(w);
	    return TCL_OK;
	} else {
	    if (interp != NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendToObj(Tcl_GetObjResult(interp),
			"integer value too large to represent", -1);
	    }
	    return TCL_ERROR;
	}
    }
#endif

    *longPtr = objPtr->internalRep.longValue;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * SetWideIntFromAny --
 *
 *	Attempt to generate an integer internal form for the Tcl object
 *	"objPtr".
 *
 * Results:
 *	The return value is a standard object Tcl result. If an error occurs
 *	during conversion, an error message is left in the interpreter's
 *	result unless "interp" is NULL.
 *
 * Side effects:
 *	If no error occurs, an int is stored as "objPtr"s internal
 *	representation. 
 *
 *----------------------------------------------------------------------
 */

static int
SetWideIntFromAny(interp, objPtr)
    Tcl_Interp *interp;		/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object to convert. */
{
#ifndef TCL_WIDE_INT_IS_LONG
    Tcl_ObjType *oldTypePtr = objPtr->typePtr;
    char *string, *end;
    int length;
    register char *p;
    Tcl_WideInt newWide;

    /*
     * Get the string representation. Make it up-to-date if necessary.
     */

    p = string = Tcl_GetStringFromObj(objPtr, &length);

    /*
     * Now parse "objPtr"s string as an int. We use an implementation here
     * that doesn't report errors in interp if interp is NULL. Note: use
     * strtoull instead of strtoll for integer conversions to allow full-size
     * unsigned numbers, but don't depend on strtoull to handle sign
     * characters; it won't in some implementations.
     */

    errno = 0;
#ifdef TCL_STRTOUL_SIGN_CHECK
    for ( ;  isspace(UCHAR(*p));  p++) { /* INTL: ISO space. */
	/* Empty loop body. */
    }
    if (*p == '-') {
	p++;
	newWide = -((Tcl_WideInt)strtoull(p, &end, 0));
    } else if (*p == '+') {
	p++;
	newWide = strtoull(p, &end, 0);
    } else
#else
	newWide = strtoull(p, &end, 0);
#endif
    if (end == p) {
	badInteger:
	if (interp != NULL) {
	    /*
	     * Must copy string before resetting the result in case a caller
	     * is trying to convert the interpreter's result to an int.
	     */
	    
	    char buf[100];
	    sprintf(buf, "expected integer but got \"%.50s\"", string);
	    Tcl_ResetResult(interp);
	    Tcl_AppendToObj(Tcl_GetObjResult(interp), buf, -1);
	    TclCheckBadOctal(interp, string);
	}
	return TCL_ERROR;
    }
    if (errno == ERANGE) {
	if (interp != NULL) {
	    char *s = "integer value too large to represent";
	    Tcl_ResetResult(interp);
	    Tcl_AppendToObj(Tcl_GetObjResult(interp), s, -1);
	    Tcl_SetErrorCode(interp, "ARITH", "IOVERFLOW", s, (char *) NULL);
	}
	return TCL_ERROR;
    }

    /*
     * Make sure that the string has no garbage after the end of the int.
     */
    
    while ((end < (string+length))
	    && isspace(UCHAR(*end))) { /* INTL: ISO space. */
	end++;
    }
    if (end != (string+length)) {
	goto badInteger;
    }

    /*
     * The conversion to int succeeded. Free the old internalRep before
     * setting the new one. We do this as late as possible to allow the
     * conversion code, in particular Tcl_GetStringFromObj, to use that old
     * internalRep.
     */

    if ((oldTypePtr != NULL) &&	(oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }
    
    objPtr->internalRep.wideValue = newWide;
#else 
    if (TCL_ERROR == SetIntFromAny(interp, objPtr)) {
	return TCL_ERROR;
    }
#endif
    objPtr->typePtr = &tclWideIntType;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * UpdateStringOfWideInt --
 *
 *	Update the string representation for a wide integer object.
 *	Note: This procedure does not free an existing old string rep
 *	so storage will be lost if this has not already been done. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's string is set to a valid string that results from
 *	the wideInt-to-string conversion.
 *
 *----------------------------------------------------------------------
 */

#ifndef TCL_WIDE_INT_IS_LONG
static void
UpdateStringOfWideInt(objPtr)
    register Tcl_Obj *objPtr;	/* Int object whose string rep to update. */
{
    char buffer[TCL_INTEGER_SPACE+2];
    register unsigned len;
    register Tcl_WideInt wideVal = objPtr->internalRep.wideValue;

    /*
     * Note that sprintf will generate a compiler warning under
     * Mingw claiming %I64 is an unknown format specifier.
     * Just ignore this warning. We can't use %L as the format
     * specifier since that gets printed as a 32 bit value.
     */
    sprintf(buffer, "%" TCL_LL_MODIFIER "d", wideVal);
    len = strlen(buffer);
    objPtr->bytes = ckalloc((unsigned) len + 1);
    memcpy(objPtr->bytes, buffer, len + 1);
    objPtr->length = len;
}
#endif /* TCL_WIDE_INT_IS_LONG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_NewWideIntObj --
 *
 *	If a client is compiled with TCL_MEM_DEBUG defined, calls to
 *	Tcl_NewWideIntObj to create a new 64-bit integer object end up calling
 *	the debugging procedure Tcl_DbNewWideIntObj instead.
 *
 *	Otherwise, if the client is compiled without TCL_MEM_DEBUG defined,
 *	calls to Tcl_NewWideIntObj result in a call to one of the two
 *	Tcl_NewWideIntObj implementations below. We provide two implementations
 *	so that the Tcl core can be compiled to do memory debugging of the 
 *	core even if a client does not request it for itself.
 *
 * Results:
 *	The newly created object is returned. This object will have an
 *	invalid string representation. The returned object has ref count 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG
#undef Tcl_NewWideIntObj

Tcl_Obj *
Tcl_NewWideIntObj(wideValue)
    register Tcl_WideInt wideValue;	/* Wide integer used to initialize
					 * the new object. */
{
    return Tcl_DbNewWideIntObj(wideValue, "unknown", 0);
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_NewWideIntObj(wideValue)
    register Tcl_WideInt wideValue;	/* Wide integer used to initialize
					 * the new object. */
{
    register Tcl_Obj *objPtr;

    TclNewObj(objPtr);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.wideValue = wideValue;
    objPtr->typePtr = &tclWideIntType;
    return objPtr;
}
#endif /* if TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbNewWideIntObj --
 *
 *	If a client is compiled with TCL_MEM_DEBUG defined, calls to
 *	Tcl_NewWideIntObj to create new wide integer end up calling
 *	the debugging procedure Tcl_DbNewWideIntObj instead. We
 *	provide two implementations of Tcl_DbNewWideIntObj so that
 *	whether the Tcl core is compiled to do memory debugging of the
 *	core is independent of whether a client requests debugging for
 *	itself.
 *
 *	When the core is compiled with TCL_MEM_DEBUG defined,
 *	Tcl_DbNewWideIntObj calls Tcl_DbCkalloc directly with the file
 *	name and line number from its caller. This simplifies
 *	debugging since then the checkmem command will report the
 *	caller's file name and line number when reporting objects that
 *	haven't been freed.
 *
 *	Otherwise, when the core is compiled without TCL_MEM_DEBUG defined,
 *	this procedure just returns the result of calling Tcl_NewWideIntObj.
 *
 * Results:
 *	The newly created wide integer object is returned. This object
 *	will have an invalid string representation. The returned object has
 *	ref count 0.
 *
 * Side effects:
 *	Allocates memory.
 *
 *----------------------------------------------------------------------
 */

#ifdef TCL_MEM_DEBUG

Tcl_Obj *
Tcl_DbNewWideIntObj(wideValue, file, line)
    register Tcl_WideInt wideValue;	/* Wide integer used to initialize
					 * the new object. */
    CONST char *file;			/* The name of the source file
					 * calling this procedure; used for
					 * debugging. */
    int line;				/* Line number in the source file;
					 * used for debugging. */
{
    register Tcl_Obj *objPtr;

    TclDbNewObj(objPtr, file, line);
    objPtr->bytes = NULL;
    
    objPtr->internalRep.wideValue = wideValue;
    objPtr->typePtr = &tclWideIntType;
    return objPtr;
}

#else /* if not TCL_MEM_DEBUG */

Tcl_Obj *
Tcl_DbNewWideIntObj(wideValue, file, line)
    register Tcl_WideInt wideValue;	/* Long integer used to initialize
					 * the new object. */
    CONST char *file;			/* The name of the source file
					 * calling this procedure; used for
					 * debugging. */
    int line;				/* Line number in the source file;
					 * used for debugging. */
{
    return Tcl_NewWideIntObj(wideValue);
}
#endif /* TCL_MEM_DEBUG */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetWideIntObj --
 *
 *	Modify an object to be a wide integer object and to have the
 *	specified wide integer value.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's old string rep, if any, is freed. Also, any old
 *	internal rep is freed. 
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetWideIntObj(objPtr, wideValue)
    register Tcl_Obj *objPtr;		/* Object w. internal rep to init. */
    register Tcl_WideInt wideValue;	/* Wide integer used to initialize
					 * the object's value. */
{
    register Tcl_ObjType *oldTypePtr = objPtr->typePtr;

    if (Tcl_IsShared(objPtr)) {
	panic("Tcl_SetWideIntObj called with shared object");
    }

    if ((oldTypePtr != NULL) && (oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }
    
    objPtr->internalRep.wideValue = wideValue;
    objPtr->typePtr = &tclWideIntType;
    Tcl_InvalidateStringRep(objPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetWideIntFromObj --
 *
 *	Attempt to return a wide integer from the Tcl object "objPtr". If
 *	the object is not already a wide int object, an attempt will be made
 *	to convert it to one.
 *
 * Results:
 *	The return value is a standard Tcl object result. If an error occurs
 *	during conversion, an error message is left in the interpreter's
 *	result unless "interp" is NULL.
 *
 * Side effects:
 *	If the object is not already an int object, the conversion will free
 *	any old internal representation.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetWideIntFromObj(interp, objPtr, wideIntPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* Object from which to get a wide int. */
    register Tcl_WideInt *wideIntPtr; /* Place to store resulting long. */
{
    register int result;

    if (objPtr->typePtr == &tclWideIntType) {
    gotWide:
	*wideIntPtr = objPtr->internalRep.wideValue;
	return TCL_OK;
    }
    if (objPtr->typePtr == &tclIntType) {
	/*
	 * This cast is safe; all valid ints/longs are wides.
	 */

	objPtr->internalRep.wideValue =
		Tcl_LongAsWide(objPtr->internalRep.longValue);
	objPtr->typePtr = &tclWideIntType;
	goto gotWide;
    }
    result = SetWideIntFromAny(interp, objPtr);
    if (result == TCL_OK) {
	*wideIntPtr = objPtr->internalRep.wideValue;
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbIncrRefCount --
 *
 *	This procedure is normally called when debugging: i.e., when
 *	TCL_MEM_DEBUG is defined. This checks to see whether or not
 *	the memory has been freed before incrementing the ref count.
 *
 *	When TCL_MEM_DEBUG is not defined, this procedure just increments
 *	the reference count of the object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's ref count is incremented.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DbIncrRefCount(objPtr, file, line)
    register Tcl_Obj *objPtr;	/* The object we are registering a
				 * reference to. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
#ifdef TCL_MEM_DEBUG
    if (objPtr->refCount == 0x61616161) {
	fprintf(stderr, "file = %s, line = %d\n", file, line);
	fflush(stderr);
	panic("Trying to increment refCount of previously disposed object.");
    }
#endif
    ++(objPtr)->refCount;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbDecrRefCount --
 *
 *	This procedure is normally called when debugging: i.e., when
 *	TCL_MEM_DEBUG is defined. This checks to see whether or not
 *	the memory has been freed before decrementing the ref count.
 *
 *	When TCL_MEM_DEBUG is not defined, this procedure just decrements
 *	the reference count of the object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's ref count is incremented.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DbDecrRefCount(objPtr, file, line)
    register Tcl_Obj *objPtr;	/* The object we are releasing a reference
				 * to. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
#ifdef TCL_MEM_DEBUG
    if (objPtr->refCount == 0x61616161) {
	fprintf(stderr, "file = %s, line = %d\n", file, line);
	fflush(stderr);
	panic("Trying to decrement refCount of previously disposed object.");
    }
#endif
    if (--(objPtr)->refCount <= 0) {
	TclFreeObj(objPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbIsShared --
 *
 *	This procedure is normally called when debugging: i.e., when
 *	TCL_MEM_DEBUG is defined. It tests whether the object has a ref
 *	count greater than one.
 *
 *	When TCL_MEM_DEBUG is not defined, this procedure just tests
 *	if the object has a ref count greater than one.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_DbIsShared(objPtr, file, line)
    register Tcl_Obj *objPtr;	/* The object to test for being shared. */
    CONST char *file;		/* The name of the source file calling this
				 * procedure; used for debugging. */
    int line;			/* Line number in the source file; used
				 * for debugging. */
{
#ifdef TCL_MEM_DEBUG
    if (objPtr->refCount == 0x61616161) {
	fprintf(stderr, "file = %s, line = %d\n", file, line);
	fflush(stderr);
	panic("Trying to check whether previously disposed object is shared.");
    }
#endif
#ifdef TCL_COMPILE_STATS
    Tcl_MutexLock(&tclObjMutex);
    if ((objPtr)->refCount <= 1) {
	tclObjsShared[1]++;
    } else if ((objPtr)->refCount < TCL_MAX_SHARED_OBJ_STATS) {
	tclObjsShared[(objPtr)->refCount]++;
    } else {
	tclObjsShared[0]++;
    }
    Tcl_MutexUnlock(&tclObjMutex);
#endif
    return ((objPtr)->refCount > 1);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_InitObjHashTable --
 *
 *	Given storage for a hash table, set up the fields to prepare
 *	the hash table for use, the keys are Tcl_Obj *.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	TablePtr is now ready to be passed to Tcl_FindHashEntry and
 *	Tcl_CreateHashEntry.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_InitObjHashTable(tablePtr)
    register Tcl_HashTable *tablePtr;	/* Pointer to table record, which
					 * is supplied by the caller. */
{
    Tcl_InitCustomHashTable(tablePtr, TCL_CUSTOM_PTR_KEYS,
	    &tclObjHashKeyType);
}

/*
 *----------------------------------------------------------------------
 *
 * AllocObjEntry --
 *
 *	Allocate space for a Tcl_HashEntry containing the Tcl_Obj * key.
 *
 * Results:
 *	The return value is a pointer to the created entry.
 *
 * Side effects:
 *	Increments the reference count on the object.
 *
 *----------------------------------------------------------------------
 */

static Tcl_HashEntry *
AllocObjEntry(tablePtr, keyPtr)
    Tcl_HashTable *tablePtr;	/* Hash table. */
    VOID *keyPtr;		/* Key to store in the hash table entry. */
{
    Tcl_Obj *objPtr = (Tcl_Obj *) keyPtr;
    Tcl_HashEntry *hPtr;

    hPtr = (Tcl_HashEntry *) ckalloc((unsigned) (sizeof(Tcl_HashEntry)));
    hPtr->key.oneWordValue = (char *) objPtr;
    Tcl_IncrRefCount (objPtr);

    return hPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * CompareObjKeys --
 *
 *	Compares two Tcl_Obj * keys.
 *
 * Results:
 *	The return value is 0 if they are different and 1 if they are
 *	the same.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
CompareObjKeys(keyPtr, hPtr)
    VOID *keyPtr;		/* New key to compare. */
    Tcl_HashEntry *hPtr;		/* Existing key to compare. */
{
    Tcl_Obj *objPtr1 = (Tcl_Obj *) keyPtr;
    Tcl_Obj *objPtr2 = (Tcl_Obj *) hPtr->key.oneWordValue;
    register CONST char *p1, *p2;
    register int l1, l2;

    /*
     * If the object pointers are the same then they match.
     */
    if (objPtr1 == objPtr2) {
	return 1;
    }

    /*
     * Don't use Tcl_GetStringFromObj as it would prevent l1 and l2 being
     * in a register.
     */
    p1 = TclGetString(objPtr1);
    l1 = objPtr1->length;
    p2 = TclGetString(objPtr2);
    l2 = objPtr2->length;
    
    /*
     * Only compare if the string representations are of the same length.
     */
    if (l1 == l2) {
	for (;; p1++, p2++, l1--) {
	    if (*p1 != *p2) {
		break;
	    }
	    if (l1 == 0) {
		return 1;
	    }
	}
    }

    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * FreeObjEntry --
 *
 *	Frees space for a Tcl_HashEntry containing the Tcl_Obj * key.
 *
 * Results:
 *	The return value is a pointer to the created entry.
 *
 * Side effects:
 *	Decrements the reference count of the object.
 *
 *----------------------------------------------------------------------
 */

static void
FreeObjEntry(hPtr)
    Tcl_HashEntry *hPtr;	/* Hash entry to free. */
{
    Tcl_Obj *objPtr = (Tcl_Obj *) hPtr->key.oneWordValue;

    Tcl_DecrRefCount (objPtr);
    ckfree ((char *) hPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * HashObjKey --
 *
 *	Compute a one-word summary of the string representation of the
 *	Tcl_Obj, which can be used to generate a hash index.
 *
 * Results:
 *	The return value is a one-word summary of the information in
 *	the string representation of the Tcl_Obj.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static unsigned int
HashObjKey(tablePtr, keyPtr)
    Tcl_HashTable *tablePtr;	/* Hash table. */
    VOID *keyPtr;		/* Key from which to compute hash value. */
{
    Tcl_Obj *objPtr = (Tcl_Obj *) keyPtr;
    CONST char *string = TclGetString(objPtr);
    int length = objPtr->length;
    unsigned int result;
    int i;

    /*
     * I tried a zillion different hash functions and asked many other
     * people for advice.  Many people had their own favorite functions,
     * all different, but no-one had much idea why they were good ones.
     * I chose the one below (multiply by 9 and add new character)
     * because of the following reasons:
     *
     * 1. Multiplying by 10 is perfect for keys that are decimal strings,
     *    and multiplying by 9 is just about as good.
     * 2. Times-9 is (shift-left-3) plus (old).  This means that each
     *    character's bits hang around in the low-order bits of the
     *    hash value for ever, plus they spread fairly rapidly up to
     *    the high-order bits to fill out the hash value.  This seems
     *    works well both for decimal and non-decimal strings.
     */

    result = 0;
    for (i=0 ; i<length ; i++) {
	result += (result<<3) + string[i];
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetCommandFromObj --
 *
 *      Returns the command specified by the name in a Tcl_Obj.
 *
 * Results:
 *	Returns a token for the command if it is found. Otherwise, if it
 *	can't be found or there is an error, returns NULL.
 *
 * Side effects:
 *      May update the internal representation for the object, caching
 *      the command reference so that the next time this procedure is
 *	called with the same object, the command can be found quickly.
 *
 *----------------------------------------------------------------------
 */

Tcl_Command
Tcl_GetCommandFromObj(interp, objPtr)
    Tcl_Interp *interp;		/* The interpreter in which to resolve the
				 * command and to report errors. */
    register Tcl_Obj *objPtr;	/* The object containing the command's
				 * name. If the name starts with "::", will
				 * be looked up in global namespace. Else,
				 * looked up first in the current namespace,
				 * then in global namespace. */
{
    Interp *iPtr = (Interp *) interp;
    register ResolvedCmdName *resPtr;
    register Command *cmdPtr;
    Namespace *currNsPtr;
    int result;
    CallFrame *savedFramePtr;
    char *name;

    /*
     * If the variable name is fully qualified, do as if the lookup were
     * done from the global namespace; this helps avoid repeated lookups 
     * of fully qualified names. It costs close to nothing, and may be very
     * helpful for OO applications which pass along a command name ("this"),
     * [Patch 456668]
     */

    savedFramePtr = iPtr->varFramePtr;
    name = Tcl_GetString(objPtr);
    if ((*name++ == ':') && (*name == ':')) {
	iPtr->varFramePtr = NULL;
    }

    /*
     * Get the internal representation, converting to a command type if
     * needed. The internal representation is a ResolvedCmdName that points
     * to the actual command.
     */
    
    if (objPtr->typePtr != &tclCmdNameType) {
        result = tclCmdNameType.setFromAnyProc(interp, objPtr);
        if (result != TCL_OK) {
	    iPtr->varFramePtr = savedFramePtr;
            return (Tcl_Command) NULL;
        }
    }
    resPtr = (ResolvedCmdName *) objPtr->internalRep.twoPtrValue.ptr1;

    /*
     * Get the current namespace.
     */
    
    if (iPtr->varFramePtr != NULL) {
	currNsPtr = iPtr->varFramePtr->nsPtr;
    } else {
	currNsPtr = iPtr->globalNsPtr;
    }

    /*
     * Check the context namespace and the namespace epoch of the resolved
     * symbol to make sure that it is fresh. If not, then force another
     * conversion to the command type, to discard the old rep and create a
     * new one. Note that we verify that the namespace id of the context
     * namespace is the same as the one we cached; this insures that the
     * namespace wasn't deleted and a new one created at the same address
     * with the same command epoch.
     */
    
    cmdPtr = NULL;
    if ((resPtr != NULL)
	    && (resPtr->refNsPtr == currNsPtr)
	    && (resPtr->refNsId == currNsPtr->nsId)
	    && (resPtr->refNsCmdEpoch == currNsPtr->cmdRefEpoch)) {
        cmdPtr = resPtr->cmdPtr;
        if (cmdPtr->cmdEpoch != resPtr->cmdEpoch
		|| (cmdPtr->flags & CMD_IS_DELETED)) {
            cmdPtr = NULL;
        }
    }

    if (cmdPtr == NULL) {
        result = tclCmdNameType.setFromAnyProc(interp, objPtr);
        if (result != TCL_OK) {
	    iPtr->varFramePtr = savedFramePtr;
            return (Tcl_Command) NULL;
        }
        resPtr = (ResolvedCmdName *) objPtr->internalRep.twoPtrValue.ptr1;
        if (resPtr != NULL) {
            cmdPtr = resPtr->cmdPtr;
        }
    }
    iPtr->varFramePtr = savedFramePtr;
    return (Tcl_Command) cmdPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TclSetCmdNameObj --
 *
 *	Modify an object to be an CmdName object that refers to the argument
 *	Command structure.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's old internal rep is freed. It's string rep is not
 *	changed. The refcount in the Command structure is incremented to
 *	keep it from being freed if the command is later deleted until
 *	TclExecuteByteCode has a chance to recognize that it was deleted.
 *
 *----------------------------------------------------------------------
 */

void
TclSetCmdNameObj(interp, objPtr, cmdPtr)
    Tcl_Interp *interp;		/* Points to interpreter containing command
				 * that should be cached in objPtr. */
    register Tcl_Obj *objPtr;	/* Points to Tcl object to be changed to
				 * a CmdName object. */
    Command *cmdPtr;		/* Points to Command structure that the
				 * CmdName object should refer to. */
{
    Interp *iPtr = (Interp *) interp;
    register ResolvedCmdName *resPtr;
    Tcl_ObjType *oldTypePtr = objPtr->typePtr;
    register Namespace *currNsPtr;

    if (oldTypePtr == &tclCmdNameType) {
	return;
    }
    
    /*
     * Get the current namespace.
     */
    
    if (iPtr->varFramePtr != NULL) {
	currNsPtr = iPtr->varFramePtr->nsPtr;
    } else {
	currNsPtr = iPtr->globalNsPtr;
    }
    
    cmdPtr->refCount++;
    resPtr = (ResolvedCmdName *) ckalloc(sizeof(ResolvedCmdName));
    resPtr->cmdPtr = cmdPtr;
    resPtr->refNsPtr = currNsPtr;
    resPtr->refNsId  = currNsPtr->nsId;
    resPtr->refNsCmdEpoch = currNsPtr->cmdRefEpoch;
    resPtr->cmdEpoch = cmdPtr->cmdEpoch;
    resPtr->refCount = 1;
    
    if ((oldTypePtr != NULL) && (oldTypePtr->freeIntRepProc != NULL)) {
	oldTypePtr->freeIntRepProc(objPtr);
    }
    objPtr->internalRep.twoPtrValue.ptr1 = (VOID *) resPtr;
    objPtr->internalRep.twoPtrValue.ptr2 = NULL;
    objPtr->typePtr = &tclCmdNameType;
}

/*
 *----------------------------------------------------------------------
 *
 * FreeCmdNameInternalRep --
 *
 *	Frees the resources associated with a cmdName object's internal
 *	representation.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Decrements the ref count of any cached ResolvedCmdName structure
 *	pointed to by the cmdName's internal representation. If this is 
 *	the last use of the ResolvedCmdName, it is freed. This in turn
 *	decrements the ref count of the Command structure pointed to by 
 *	the ResolvedSymbol, which may free the Command structure.
 *
 *----------------------------------------------------------------------
 */

static void
FreeCmdNameInternalRep(objPtr)
    register Tcl_Obj *objPtr;	/* CmdName object with internal
				 * representation to free. */
{
    register ResolvedCmdName *resPtr =
	(ResolvedCmdName *) objPtr->internalRep.twoPtrValue.ptr1;

    if (resPtr != NULL) {
	/*
	 * Decrement the reference count of the ResolvedCmdName structure.
	 * If there are no more uses, free the ResolvedCmdName structure.
	 */
    
        resPtr->refCount--;
        if (resPtr->refCount == 0) {
            /*
	     * Now free the cached command, unless it is still in its
             * hash table or if there are other references to it
             * from other cmdName objects.
	     */
	    
            Command *cmdPtr = resPtr->cmdPtr;
            TclCleanupCommand(cmdPtr);
            ckfree((char *) resPtr);
        }
    }
}

/*
 *----------------------------------------------------------------------
 *
 * DupCmdNameInternalRep --
 *
 *	Initialize the internal representation of an cmdName Tcl_Obj to a
 *	copy of the internal representation of an existing cmdName object. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	"copyPtr"s internal rep is set to point to the ResolvedCmdName
 *	structure corresponding to "srcPtr"s internal rep. Increments the
 *	ref count of the ResolvedCmdName structure pointed to by the
 *	cmdName's internal representation.
 *
 *----------------------------------------------------------------------
 */

static void
DupCmdNameInternalRep(srcPtr, copyPtr)
    Tcl_Obj *srcPtr;		/* Object with internal rep to copy. */
    register Tcl_Obj *copyPtr;	/* Object with internal rep to set. */
{
    register ResolvedCmdName *resPtr =
        (ResolvedCmdName *) srcPtr->internalRep.twoPtrValue.ptr1;

    copyPtr->internalRep.twoPtrValue.ptr1 = (VOID *) resPtr;
    copyPtr->internalRep.twoPtrValue.ptr2 = NULL;
    if (resPtr != NULL) {
        resPtr->refCount++;
    }
    copyPtr->typePtr = &tclCmdNameType;
}

/*
 *----------------------------------------------------------------------
 *
 * SetCmdNameFromAny --
 *
 *	Generate an cmdName internal form for the Tcl object "objPtr".
 *
 * Results:
 *	The return value is a standard Tcl result. The conversion always
 *	succeeds and TCL_OK is returned.
 *
 * Side effects:
 *	A pointer to a ResolvedCmdName structure that holds a cached pointer
 *	to the command with a name that matches objPtr's string rep is
 *	stored as objPtr's internal representation. This ResolvedCmdName
 *	pointer will be NULL if no matching command was found. The ref count
 *	of the cached Command's structure (if any) is also incremented.
 *
 *----------------------------------------------------------------------
 */

static int
SetCmdNameFromAny(interp, objPtr)
    Tcl_Interp *interp;		/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object to convert. */
{
    Interp *iPtr = (Interp *) interp;
    char *name;
    Tcl_Command cmd;
    register Command *cmdPtr;
    Namespace *currNsPtr;
    register ResolvedCmdName *resPtr;

    if (interp == NULL) {
	return TCL_ERROR;
    }

    /*
     * Get "objPtr"s string representation. Make it up-to-date if necessary.
     */

    name = objPtr->bytes;
    if (name == NULL) {
	name = Tcl_GetString(objPtr);
    }

    /*
     * Find the Command structure, if any, that describes the command called
     * "name". Build a ResolvedCmdName that holds a cached pointer to this
     * Command, and bump the reference count in the referenced Command
     * structure. A Command structure will not be deleted as long as it is
     * referenced from a CmdName object.
     */

    cmd = Tcl_FindCommand(interp, name, (Tcl_Namespace *) NULL,
	    /*flags*/ 0);
    cmdPtr = (Command *) cmd;
    if (cmdPtr != NULL) {
	/*
	 * Get the current namespace.
	 */
	
	if (iPtr->varFramePtr != NULL) {
	    currNsPtr = iPtr->varFramePtr->nsPtr;
	} else {
	    currNsPtr = iPtr->globalNsPtr;
	}
	
	cmdPtr->refCount++;
        resPtr = (ResolvedCmdName *) ckalloc(sizeof(ResolvedCmdName));
        resPtr->cmdPtr        = cmdPtr;
        resPtr->refNsPtr      = currNsPtr;
        resPtr->refNsId       = currNsPtr->nsId;
        resPtr->refNsCmdEpoch = currNsPtr->cmdRefEpoch;
        resPtr->cmdEpoch      = cmdPtr->cmdEpoch;
        resPtr->refCount      = 1;
    } else {
	resPtr = NULL;	/* no command named "name" was found */
    }

    /*
     * Free the old internalRep before setting the new one. We do this as
     * late as possible to allow the conversion code, in particular
     * GetStringFromObj, to use that old internalRep. If no Command
     * structure was found, leave NULL as the cached value.
     */

    if ((objPtr->typePtr != NULL)
	    && (objPtr->typePtr->freeIntRepProc != NULL)) {
	objPtr->typePtr->freeIntRepProc(objPtr);
    }
    
    objPtr->internalRep.twoPtrValue.ptr1 = (VOID *) resPtr;
    objPtr->internalRep.twoPtrValue.ptr2 = NULL;
    objPtr->typePtr = &tclCmdNameType;
    return TCL_OK;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */

