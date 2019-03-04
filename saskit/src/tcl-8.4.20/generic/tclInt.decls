# tclInt.decls --
#
#	This file contains the declarations for all unsupported
#	functions that are exported by the Tcl library.  This file
#	is used to generate the tclIntDecls.h, tclIntPlatDecls.h,
#	tclIntStub.c, tclPlatStub.c, tclCompileDecls.h and tclCompileStub.c
#	files
#
# Copyright (c) 1998-1999 by Scriptics Corporation.
# Copyright (c) 2001 by Kevin B. Kenny.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.

library tcl

# Define the unsupported generic interfaces.

interface tclInt

# Declare each of the functions in the unsupported internal Tcl
# interface.  These interfaces are allowed to changed between versions.
# Use at your own risk.  Note that the position of functions should not
# be changed between versions to avoid gratuitous incompatibilities.

# Replaced by Tcl_FSAccess in 8.4:
#declare 0 {
#    int TclAccess(const char *path, int mode)
#}
declare 1 {
    int TclAccessDeleteProc(TclAccessProc_ *proc)
}
declare 2 {
    int TclAccessInsertProc(TclAccessProc_ *proc)
}
declare 3 {
    void TclAllocateFreeObjects(void)
}
# Replaced by TclpChdir in 8.1:
#  declare 4 {
#      int TclChdir(Tcl_Interp *interp, char *dirName)
#  }
declare 5 {
    int TclCleanupChildren(Tcl_Interp *interp, int numPids, Tcl_Pid *pidPtr,
	    Tcl_Channel errorChan)
}
declare 6 {
    void TclCleanupCommand(Command *cmdPtr)
}
declare 7 {
    int TclCopyAndCollapse(int count, const char *src, char *dst)
}
declare 8 {
    int TclCopyChannel(Tcl_Interp *interp, Tcl_Channel inChan,
	    Tcl_Channel outChan, int toRead, Tcl_Obj *cmdPtr)
}

# TclCreatePipeline unofficially exported for use by BLT.

declare 9 {
    int TclCreatePipeline(Tcl_Interp *interp, int argc, const char **argv,
	    Tcl_Pid **pidArrayPtr, TclFile *inPipePtr, TclFile *outPipePtr,
	    TclFile *errFilePtr)
}
declare 10 {
    int TclCreateProc(Tcl_Interp *interp, Namespace *nsPtr,
	    const char *procName,
	    Tcl_Obj *argsPtr, Tcl_Obj *bodyPtr, Proc **procPtrPtr)
}
declare 11 {
    void TclDeleteCompiledLocalVars(Interp *iPtr, CallFrame *framePtr)
}
declare 12 {
    void TclDeleteVars(Interp *iPtr, Tcl_HashTable *tablePtr)
}
declare 13 {
    int TclDoGlob(Tcl_Interp *interp, char *separators,
	    Tcl_DString *headPtr, char *tail, Tcl_GlobTypeData *types)
}
declare 14 {
    void TclDumpMemoryInfo(FILE *outFile)
}
# Removed in 8.1:
#  declare 15 {
#      void TclExpandParseValue(ParseValue *pvPtr, int needed)
#  }
declare 16 {
    void TclExprFloatError(Tcl_Interp *interp, double value)
}
# Removed in 8.4:
#declare 17 {
#    int TclFileAttrsCmd(Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
#}
#declare 18 {
#    int TclFileCopyCmd(Tcl_Interp *interp, int argc, char **argv)
#}
#declare 19 {
#    int TclFileDeleteCmd(Tcl_Interp *interp, int argc, char **argv)
#}
#declare 20 {
#    int TclFileMakeDirsCmd(Tcl_Interp *interp, int argc, char **argv)
#}
#declare 21 {
#    int TclFileRenameCmd(Tcl_Interp *interp, int argc, char **argv)
#}
declare 22 {
    int TclFindElement(Tcl_Interp *interp, const char *listStr,
	    int listLength, const char **elementPtr, const char **nextPtr,
	    int *sizePtr, int *bracePtr)
}
declare 23 {
    Proc *TclFindProc(Interp *iPtr, const char *procName)
}
declare 24 {
    int TclFormatInt(char *buffer, long n)
}
declare 25 {
    void TclFreePackageInfo(Interp *iPtr)
}
# Removed in 8.1:
#  declare 26 {
#      char *TclGetCwd(Tcl_Interp *interp)
#  }
declare 27 {
    int TclGetDate(char *p, Tcl_WideInt now, long zone,
	    Tcl_WideInt *timePtr)
}
declare 28 {
    Tcl_Channel TclpGetDefaultStdChannel(int type)
}
# Removed in 8.4b2:
#declare 29 {
#    Tcl_Obj *TclGetElementOfIndexedArray(Tcl_Interp *interp,
#	    int localIndex, Tcl_Obj *elemPtr, int flags)
#}
# Replaced by char *TclGetEnv(const char *name, Tcl_DString *valuePtr) in 8.1:
#  declare 30 {
#      char *TclGetEnv(const char *name)
#  }
declare 31 {
    char *TclGetExtension(char *name)
}
declare 32 {
    int TclGetFrame(Tcl_Interp *interp, const char *str,
	    CallFrame **framePtrPtr)
}
declare 33 {
    TclCmdProcType TclGetInterpProc(void)
}
declare 34 {
    int TclGetIntForIndex(Tcl_Interp *interp, Tcl_Obj *objPtr,
	    int endValue, int *indexPtr)
}
# Removed in 8.4b2:
#declare 35 {
#    Tcl_Obj *TclGetIndexedScalar(Tcl_Interp *interp, int localIndex,
#	    int flags)
#}
declare 36 {
    int TclGetLong(Tcl_Interp *interp, const char *str, long *longPtr)
}
declare 37 {
    int TclGetLoadedPackages(Tcl_Interp *interp, char *targetName)
}
declare 38 {
    int TclGetNamespaceForQualName(Tcl_Interp *interp, const char *qualName,
	    Namespace *cxtNsPtr, int flags, Namespace **nsPtrPtr,
	    Namespace **altNsPtrPtr, Namespace **actualCxtPtrPtr,
	    const char **simpleNamePtr)
}
declare 39 {
    TclObjCmdProcType TclGetObjInterpProc(void)
}
declare 40 {
    int TclGetOpenMode(Tcl_Interp *interp, const char *str, int *seekFlagPtr)
}
declare 41 {
    Tcl_Command TclGetOriginalCommand(Tcl_Command command)
}
declare 42 {
    char *TclpGetUserHome(const char *name, Tcl_DString *bufferPtr)
}
declare 43 {
    int TclGlobalInvoke(Tcl_Interp *interp, int argc, CONST84 char **argv, int flags)
}
declare 44 {
    int TclGuessPackageName(const char *fileName, Tcl_DString *bufPtr)
}
declare 45 {
    int TclHideUnsafeCommands(Tcl_Interp *interp)
}
declare 46 {
    int TclInExit(void)
}
# Removed in 8.4b2:
#declare 47 {
#    Tcl_Obj *TclIncrElementOfIndexedArray(Tcl_Interp *interp,
#	    int localIndex, Tcl_Obj *elemPtr, long incrAmount)
#}
# Removed in 8.4b2:
#declare 48 {
#    Tcl_Obj *TclIncrIndexedScalar(Tcl_Interp *interp, int localIndex,
#	    long incrAmount)
#}
declare 49 {
    Tcl_Obj *TclIncrVar2(Tcl_Interp *interp, Tcl_Obj *part1Ptr,
	    Tcl_Obj *part2Ptr, long incrAmount, int part1NotParsed)
}
declare 50 {
    void TclInitCompiledLocals(Tcl_Interp *interp, CallFrame *framePtr,
	    Namespace *nsPtr)
}
declare 51 {
    int TclInterpInit(Tcl_Interp *interp)
}
declare 52 {
    int TclInvoke(Tcl_Interp *interp, int argc, CONST84 char **argv, int flags)
}
declare 53 {
    int TclInvokeObjectCommand(ClientData clientData, Tcl_Interp *interp,
	    int argc, CONST84 char **argv)
}
declare 54 {
    int TclInvokeStringCommand(ClientData clientData, Tcl_Interp *interp,
	    int objc, Tcl_Obj *const objv[])
}
declare 55 {
    Proc *TclIsProc(Command *cmdPtr)
}
# Replaced with TclpLoadFile in 8.1:
#  declare 56 {
#      int TclLoadFile(Tcl_Interp *interp, char *fileName, char *sym1,
#  	    char *sym2, Tcl_PackageInitProc **proc1Ptr,
#  	    Tcl_PackageInitProc **proc2Ptr)
#  }
# Signature changed to take a length in 8.1:
#  declare 57 {
#      int TclLooksLikeInt(char *p)
#  }
declare 58 {
    Var *TclLookupVar(Tcl_Interp *interp, const char *part1, const char *part2,
	    int flags, const char *msg, int createPart1, int createPart2,
	    Var **arrayPtrPtr)
}
# Replaced by Tcl_FSMatchInDirectory in 8.4
#declare 59 {
#    int TclpMatchFiles(Tcl_Interp *interp, char *separators,
#	    Tcl_DString *dirPtr, char *pattern, char *tail)
#}
declare 60 {
    int TclNeedSpace(const char *start, const char *end)
}
declare 61 {
    Tcl_Obj *TclNewProcBodyObj(Proc *procPtr)
}
declare 62 {
    int TclObjCommandComplete(Tcl_Obj *cmdPtr)
}
declare 63 {
    int TclObjInterpProc(ClientData clientData, Tcl_Interp *interp,
	    int objc, Tcl_Obj *const objv[])
}
declare 64 {
    int TclObjInvoke(Tcl_Interp *interp, int objc, Tcl_Obj *const objv[],
	    int flags)
}
declare 65 {
    int TclObjInvokeGlobal(Tcl_Interp *interp, int objc,
	    Tcl_Obj *const objv[], int flags)
}
declare 66 {
    int TclOpenFileChannelDeleteProc(TclOpenFileChannelProc_ *proc)
}
declare 67 {
    int TclOpenFileChannelInsertProc(TclOpenFileChannelProc_ *proc)
}
# Replaced by Tcl_FSAccess in 8.4:
#declare 68 {
#    int TclpAccess(const char *path, int mode)
#}
declare 69 {
    char *TclpAlloc(unsigned int size)
}
#declare 70 {
#    int TclpCopyFile(const char *source, const char *dest)
#}
#declare 71 {
#    int TclpCopyDirectory(const char *source, const char *dest,
#	    Tcl_DString *errorPtr)
#}
#declare 72 {
#    int TclpCreateDirectory(const char *path)
#}
#declare 73 {
#    int TclpDeleteFile(const char *path)
#}
declare 74 {
    void TclpFree(char *ptr)
}
declare 75 {
    unsigned long TclpGetClicks(void)
}
declare 76 {
    unsigned long TclpGetSeconds(void)
}

# deprecated
declare 77 {
    void TclpGetTime(Tcl_Time *time)
}
declare 78 {
    int TclpGetTimeZone(Tcl_WideInt time)
}
# Replaced by Tcl_FSListVolumes in 8.4:
#declare 79 {
#    int TclpListVolumes(Tcl_Interp *interp)
#}
# Replaced by Tcl_FSOpenFileChannel in 8.4:
#declare 80 {
#    Tcl_Channel TclpOpenFileChannel(Tcl_Interp *interp, char *fileName,
#	    char *modeString, int permissions)
#}
declare 81 {
    char *TclpRealloc(char *ptr, unsigned int size)
}
#declare 82 {
#    int TclpRemoveDirectory(const char *path, int recursive,
#	    Tcl_DString *errorPtr)
#}
#declare 83 {
#    int TclpRenameFile(const char *source, const char *dest)
#}
# Removed in 8.1:
#  declare 84 {
#      int TclParseBraces(Tcl_Interp *interp, char *str, char **termPtr,
#  	    ParseValue *pvPtr)
#  }
#  declare 85 {
#      int TclParseNestedCmd(Tcl_Interp *interp, char *str, int flags,
#  	    char **termPtr, ParseValue *pvPtr)
#  }
#  declare 86 {
#      int TclParseQuotes(Tcl_Interp *interp, char *str, int termChar,
#  	    int flags, char **termPtr, ParseValue *pvPtr)
#  }
#  declare 87 {
#      void TclPlatformInit(Tcl_Interp *interp)
#  }
declare 88 {
    char *TclPrecTraceProc(ClientData clientData, Tcl_Interp *interp,
	    const char *name1, const char *name2, int flags)
}
declare 89 {
    int TclPreventAliasLoop(Tcl_Interp *interp, Tcl_Interp *cmdInterp,
	    Tcl_Command cmd)
}
# Removed in 8.1 (only available if compiled with TCL_COMPILE_DEBUG):
#  declare 90 {
#      void TclPrintByteCodeObj(Tcl_Interp *interp, Tcl_Obj *objPtr)
#  }
declare 91 {
    void TclProcCleanupProc(Proc *procPtr)
}
declare 92 {
    int TclProcCompileProc(Tcl_Interp *interp, Proc *procPtr,
	    Tcl_Obj *bodyPtr, Namespace *nsPtr, const char *description,
	    const char *procName)
}
declare 93 {
    void TclProcDeleteProc(ClientData clientData)
}
declare 94 {
    int TclProcInterpProc(ClientData clientData, Tcl_Interp *interp,
	    int argc, CONST84 char **argv)
}
# Replaced by Tcl_FSStat in 8.4:
#declare 95 {
#    int TclpStat(const char *path, Tcl_StatBuf *buf)
#}
declare 96 {
    int TclRenameCommand(Tcl_Interp *interp, char *oldName,
            char *newName)
}
declare 97 {
    void TclResetShadowedCmdRefs(Tcl_Interp *interp, Command *newCmdPtr)
}
declare 98 {
    int TclServiceIdle(void)
}
# Removed in 8.4b2:
#declare 99 {
#    Tcl_Obj *TclSetElementOfIndexedArray(Tcl_Interp *interp, int localIndex,
#	    Tcl_Obj *elemPtr, Tcl_Obj *objPtr, int flags)
#}
# Removed in 8.4b2:
#declare 100 {
#    Tcl_Obj *TclSetIndexedScalar(Tcl_Interp *interp, int localIndex,
#	    Tcl_Obj *objPtr, int flags)
#}
declare 101 {
    char *TclSetPreInitScript(char *string)
}
declare 102 {
    void TclSetupEnv(Tcl_Interp *interp)
}
declare 103 {
    int TclSockGetPort(Tcl_Interp *interp, char *str, char *proto,
	    int *portPtr)
}
declare 104 {
    int TclSockMinimumBuffersOld(int sock, int size)
}
# Replaced by Tcl_FSStat in 8.4:
#declare 105 {
#    int TclStat(const char *path, Tcl_StatBuf *buf)
#}
declare 106 {
    int TclStatDeleteProc(TclStatProc_ *proc)
}
declare 107 {
    int TclStatInsertProc(TclStatProc_ *proc)
}
declare 108 {
    void TclTeardownNamespace(Namespace *nsPtr)
}
declare 109 {
    int TclUpdateReturnInfo(Interp *iPtr)
}
declare 110 {
    int TclSockMinimumBuffers(void *sock, int size)
}
# Removed in 8.1:
#  declare 110 {
#      char *TclWordEnd(char *start, char *lastChar, int nested, int *semiPtr)
#  }

# Procedures used in conjunction with Tcl namespaces. They are
# defined here instead of in tcl.decls since they are not stable yet.

declare 111 {
    void Tcl_AddInterpResolvers(Tcl_Interp *interp, const char *name,
	    Tcl_ResolveCmdProc *cmdProc, Tcl_ResolveVarProc *varProc,
	    Tcl_ResolveCompiledVarProc *compiledVarProc)
}
declare 112 {
    int Tcl_AppendExportList(Tcl_Interp *interp, Tcl_Namespace *nsPtr,
	    Tcl_Obj *objPtr)
}
declare 113 {
    Tcl_Namespace *Tcl_CreateNamespace(Tcl_Interp *interp, const char *name,
	    ClientData clientData, Tcl_NamespaceDeleteProc *deleteProc)
}
declare 114 {
    void Tcl_DeleteNamespace(Tcl_Namespace *nsPtr)
}
declare 115 {
    int Tcl_Export(Tcl_Interp *interp, Tcl_Namespace *nsPtr,
	    const char *pattern, int resetListFirst)
}
declare 116 {
    Tcl_Command Tcl_FindCommand(Tcl_Interp *interp, const char *name,
	    Tcl_Namespace *contextNsPtr, int flags)
}
declare 117 {
    Tcl_Namespace *Tcl_FindNamespace(Tcl_Interp *interp, const char *name,
	    Tcl_Namespace *contextNsPtr, int flags)
}
declare 118 {
    int Tcl_GetInterpResolvers(Tcl_Interp *interp, const char *name,
	    Tcl_ResolverInfo *resInfo)
}
declare 119 {
    int Tcl_GetNamespaceResolvers(Tcl_Namespace *namespacePtr,
	    Tcl_ResolverInfo *resInfo)
}
declare 120 {
    Tcl_Var Tcl_FindNamespaceVar(Tcl_Interp *interp, const char *name,
	    Tcl_Namespace *contextNsPtr, int flags)
}
declare 121 {
    int Tcl_ForgetImport(Tcl_Interp *interp, Tcl_Namespace *nsPtr,
	    const char *pattern)
}
declare 122 {
    Tcl_Command Tcl_GetCommandFromObj(Tcl_Interp *interp, Tcl_Obj *objPtr)
}
declare 123 {
    void Tcl_GetCommandFullName(Tcl_Interp *interp, Tcl_Command command,
	    Tcl_Obj *objPtr)
}
declare 124 {
    Tcl_Namespace *Tcl_GetCurrentNamespace(Tcl_Interp *interp)
}
declare 125 {
    Tcl_Namespace *Tcl_GetGlobalNamespace(Tcl_Interp *interp)
}
declare 126 {
    void Tcl_GetVariableFullName(Tcl_Interp *interp, Tcl_Var variable,
	    Tcl_Obj *objPtr)
}
declare 127 {
    int Tcl_Import(Tcl_Interp *interp, Tcl_Namespace *nsPtr,
	    const char *pattern, int allowOverwrite)
}
declare 128 {
    void Tcl_PopCallFrame(Tcl_Interp *interp)
}
declare 129 {
    int Tcl_PushCallFrame(Tcl_Interp *interp, Tcl_CallFrame *framePtr,
	    Tcl_Namespace *nsPtr, int isProcCallFrame)
}
declare 130 {
    int Tcl_RemoveInterpResolvers(Tcl_Interp *interp, const char *name)
}
declare 131 {
    void Tcl_SetNamespaceResolvers(Tcl_Namespace *namespacePtr,
	    Tcl_ResolveCmdProc *cmdProc, Tcl_ResolveVarProc *varProc,
	    Tcl_ResolveCompiledVarProc *compiledVarProc)
}
declare 132 {
    int TclpHasSockets(Tcl_Interp *interp)
}
declare 133 {
    struct tm *TclpGetDate(TclpTime_t time, int useGMT)
}
declare 134 {
    size_t TclpStrftime(char *s, size_t maxsize, const char *format,
	    const struct tm *t, int useGMT)
}
declare 135 {
    int TclpCheckStackSpace(void)
}

declare 138 {
    CONST84_RETURN char *TclGetEnv(const char *name, Tcl_DString *valuePtr)
}
declare 140 {
    int TclLooksLikeInt(const char *bytes, int length)
}
# This is used by TclX, but should otherwise be considered private
declare 141 {
    CONST84_RETURN char *TclpGetCwd(Tcl_Interp *interp, Tcl_DString *cwdPtr)
}
declare 142 {
    int TclSetByteCodeFromAny(Tcl_Interp *interp, Tcl_Obj *objPtr,
	    CompileHookProc *hookProc, ClientData clientData)
}
declare 143 {
    int TclAddLiteralObj(struct CompileEnv *envPtr, Tcl_Obj *objPtr,
	    LiteralEntry **litPtrPtr)
}
declare 144 {
    void TclHideLiteral(Tcl_Interp *interp, struct CompileEnv *envPtr,
	    int index)
}
declare 145 {
    struct AuxDataType *TclGetAuxDataType(char *typeName)
}
declare 146 {
    TclHandle TclHandleCreate(void *ptr)
}
declare 147 {
    void TclHandleFree(TclHandle handle)
}
declare 148 {
    TclHandle TclHandlePreserve(TclHandle handle)
}
declare 149 {
    void TclHandleRelease(TclHandle handle)
}

# Added in 8.2:

declare 150 {
    int TclRegAbout(Tcl_Interp *interp, Tcl_RegExp re)
}
declare 151 {
    void TclRegExpRangeUniChar(Tcl_RegExp re, int index, int *startPtr,
	    int *endPtr)
}
declare 152 {
    void TclSetLibraryPath(Tcl_Obj *pathPtr)
}
declare 153 {
    Tcl_Obj *TclGetLibraryPath(void)
}

# moved to tclTest.c (static) in 8.3.2:
#declare 154 {
#    int TclTestChannelCmd(ClientData clientData,
#    Tcl_Interp *interp, int argc, char **argv)
#}
#declare 155 {
#    int TclTestChannelEventCmd(ClientData clientData,
#	     Tcl_Interp *interp, int argc, char **argv)
#}

declare 156 {
    void TclRegError(Tcl_Interp *interp, const char *msg,
	    int status)
}
declare 157 {
    Var *TclVarTraceExists(Tcl_Interp *interp, const char *varName)
}
declare 158 {
    void TclSetStartupScriptFileName(const char *filename)
}
declare 159 {
    CONST84_RETURN char *TclGetStartupScriptFileName(void)
}
#declare 160 {
#    int TclpMatchFilesTypes(Tcl_Interp *interp, char *separators,
#	    Tcl_DString *dirPtr, char *pattern, char *tail,
#	    GlobTypeData *types)
#}

# new in 8.3.2/8.4a2
declare 161 {
    int TclChannelTransform(Tcl_Interp *interp, Tcl_Channel chan,
	    Tcl_Obj *cmdObjPtr)
}
declare 162 {
    void TclChannelEventScriptInvoker(ClientData clientData, int flags)
}

# ALERT: The result of 'TclGetInstructionTable' is actually a
# "const InstructionDesc*" but we do not want to describe this structure in
# "tclInt.h". It is described in "tclCompile.h". Use a cast to the
# correct type when calling this procedure.

declare 163 {
    void *TclGetInstructionTable(void)
}

# ALERT: The argument of 'TclExpandCodeArray' is actually a
# "CompileEnv*" but we do not want to describe this structure in
# "tclInt.h". It is described in "tclCompile.h".

declare 164 {
    void TclExpandCodeArray(void *envPtr)
}

# These functions are vfs aware, but are generally only useful internally.
declare 165 {
    void TclpSetInitialEncodings(void)
}

# New function due to TIP #33
declare 166 {
    int TclListObjSetElement(Tcl_Interp *interp, Tcl_Obj *listPtr,
	    int index, Tcl_Obj *valuePtr)
}

# VFS-aware versions of Tcl*StartupScriptFileName (158 and 159 above)
declare 167 {
    void TclSetStartupScriptPath(Tcl_Obj *pathPtr)
}
declare 168 {
    Tcl_Obj *TclGetStartupScriptPath(void)
}
# variant of Tcl_UtfNCmp that takes n as bytes, not chars
declare 169 {
    int TclpUtfNcmp2(const char *s1, const char *s2, unsigned long n)
}
declare 170 {
    int TclCheckInterpTraces(Tcl_Interp *interp, const char *command,
	    int numChars, Command *cmdPtr, int result, int traceFlags,
	    int objc, Tcl_Obj *const objv[])
}
declare 171 {
    int TclCheckExecutionTraces(Tcl_Interp *interp, const char *command,
	    int numChars, Command *cmdPtr, int result, int traceFlags,
	    int objc, Tcl_Obj *const objv[])
}
declare 172 {
    int TclInThreadExit(void)
}

# Added in 8.4.2:

declare 173 {
    int TclUniCharMatch(const Tcl_UniChar *string, int strLen,
	    const Tcl_UniChar *pattern, int ptnLen, int nocase)
}

# TclpGmtime and TclpLocaltime promoted to the generic interface from unix

declare 182 {
     struct tm *TclpLocaltime(TclpTime_t_CONST clock)
}
declare 183 {
     struct tm *TclpGmtime(TclpTime_t_CONST clock)
}

declare 199 {
    int TclMatchIsTrivial(const char *pattern)
}

declare 249 {
    void TclUnusedStubEntry(void)
}

##############################################################################

# Define the platform specific internal Tcl interface. These functions are
# only available on the designated platform.

interface tclIntPlat

################################
# Windows specific functions

declare 0 win {
    void TclWinConvertError(DWORD errCode)
}
declare 1 win {
    void TclWinConvertWSAError(DWORD errCode)
}
declare 2 win {
    struct servent *TclWinGetServByName(const char *nm,
	    const char *proto)
}
declare 3 win {
    int TclWinGetSockOpt(SOCKET s, int level, int optname,
	    char *optval, int *optlen)
}
declare 4 win {
    HINSTANCE TclWinGetTclInstance(void)
}
# new for 8.4.20+/8.5.12+ Cygwin only
declare 5 win {
    int TclUnixWaitForFile(int fd, int mask, int timeout)
}
# Removed in 8.1:
#  declare 5 win {
#      HINSTANCE TclWinLoadLibrary(char *name)
#  }
declare 6 win {
    unsigned short TclWinNToHS(unsigned short ns)
}
declare 7 win {
    int TclWinSetSockOpt(SOCKET s, int level, int optname,
	    const char *optval, int optlen)
}
declare 8 win {
    int TclpGetPid(Tcl_Pid pid)
}
declare 9 win {
    int TclWinGetPlatformId(void)
}
# new for 8.4.20+/8.5.12+ Cygwin only
declare 10 win {
    Tcl_DirEntry *TclpReaddir(DIR *dir)
}
# Removed in 8.3.1 (for Win32s only):
#declare 10 win {
#    int TclWinSynchSpawn(void *args, int type, void **trans, Tcl_Pid *pidPtr)
#}

# Pipe channel functions

declare 11 win {
    void TclGetAndDetachPids(Tcl_Interp *interp, Tcl_Channel chan)
}
declare 12 win {
    int TclpCloseFile(TclFile file)
}
declare 13 win {
    Tcl_Channel TclpCreateCommandChannel(TclFile readFile,
	    TclFile writeFile, TclFile errorFile, int numPids, Tcl_Pid *pidPtr)
}
declare 14 win {
    int TclpCreatePipe(TclFile *readPipe, TclFile *writePipe)
}
declare 15 win {
    int TclpCreateProcess(Tcl_Interp *interp, int argc,
	    const char **argv, TclFile inputFile, TclFile outputFile,
	    TclFile errorFile, Tcl_Pid *pidPtr)
}
# new for 8.4.20+/8.5.12+ Cygwin only
declare 16 win {
    int TclpIsAtty(int fd)
}
# Signature changed in 8.1:
#  declare 16 win {
#      TclFile TclpCreateTempFile(char *contents, Tcl_DString *namePtr)
#  }
#  declare 17 win {
#      char *TclpGetTZName(void)
#  }
declare 18 win {
    TclFile TclpMakeFile(Tcl_Channel channel, int direction)
}
declare 19 win {
    TclFile TclpOpenFile(const char *fname, int mode)
}
declare 20 win {
    void TclWinAddProcess(HANDLE hProcess, DWORD id)
}
# new for 8.4.20+/8.5.12+
declare 21 win {
    char *TclpInetNtoa(struct in_addr addr)
}
# Removed in 8.4:
#declare 21 win {
#    void TclpAsyncMark(Tcl_AsyncHandler async)
#}

# Added in 8.1:
declare 22 win {
    TclFile TclpCreateTempFile(const char *contents)
}
declare 23 win {
    char *TclpGetTZName(int isdst)
}
declare 24 win {
    char *TclWinNoBackslash(char *path)
}
declare 25 win {
    TclPlatformType *TclWinGetPlatform(void)
}
declare 26 win {
    void TclWinSetInterfaces(int wide)
}

# Added in 8.3.3:

declare 27 win {
    void TclWinFlushDirtyChannels(void)
}

# Added in 8.4.2:

declare 28 win {
    void TclWinResetInterfaces(void)
}

################################
# Unix specific functions

# Pipe channel functions

declare 0 unix {
    void TclGetAndDetachPids(Tcl_Interp *interp, Tcl_Channel chan)
}
declare 1 unix {
    int TclpCloseFile(TclFile file)
}
declare 2 unix {
    Tcl_Channel TclpCreateCommandChannel(TclFile readFile,
	    TclFile writeFile, TclFile errorFile, int numPids, Tcl_Pid *pidPtr)
}
declare 3 unix {
    int TclpCreatePipe(TclFile *readPipe, TclFile *writePipe)
}
declare 4 unix {
    int TclpCreateProcess(Tcl_Interp *interp, int argc,
	    const char **argv, TclFile inputFile, TclFile outputFile,
	    TclFile errorFile, Tcl_Pid *pidPtr)
}
# Signature changed in 8.1:
#  declare 5 unix {
#      TclFile TclpCreateTempFile(char *contents, Tcl_DString *namePtr)
#  }
declare 6 unix {
    TclFile TclpMakeFile(Tcl_Channel channel, int direction)
}
declare 7 unix {
    TclFile TclpOpenFile(const char *fname, int mode)
}
declare 8 unix {
    int TclUnixWaitForFile(int fd, int mask, int timeout)
}

# Added in 8.1:

declare 9 unix {
    TclFile TclpCreateTempFile(const char *contents)
}

# Added in 8.4:

declare 10 unix {
    Tcl_DirEntry *TclpReaddir(DIR *dir)
}
# Slots 11 and 12 are forwarders for functions that were promoted to
# generic Stubs
declare 11 unix {
    struct tm *TclpLocaltime_unix(TclpTime_t_CONST clock)
}
declare 12 unix {
    struct tm *TclpGmtime_unix(TclpTime_t_CONST clock)
}
declare 13 unix {
    char *TclpInetNtoa(struct in_addr addr)
}
declare 29 {win unix} {
    int TclWinCPUID(unsigned int index, unsigned int *regs)
}

# Local Variables:
# mode: tcl
# End:
