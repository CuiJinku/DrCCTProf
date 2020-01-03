#ifndef _DRCCTLIB_H_
#define _DRCCTLIB_H_

#include <iostream>
#include <string>
#include <vector>

#include "dr_api.h"
#include "drmgr.h"
#include "drwrap.h"
#include "drsyms.h"
#include "drutil.h"

using namespace std;
#define CCTLIB_N_MAX_FILE_PATH (200)

typedef uint32_t ContextHandle_t;
// Data type returned when a client tool queries  GetFullCallingContext
typedef struct Context_t {
    string functionName;
    string filePath;
    string disassembly;
    ContextHandle_t ctxtHandle;
    uint32_t lineNo;
    uint64_t ip;
} Context_t;
// The handle representing a data object
typedef struct DataHandle_t {
    uint8_t objectType;
    union {
        ContextHandle_t pathHandle;
        uint32_t symName;
    };
}DataHandle_t;
// Client callback function to determine if it wants to track a given INS
typedef bool (*IsInterestingInsFptr)(instr_t *ins);
// Client callback made from CCTLib for each instruction indicated to be tracked by the
// client.
typedef void (*CCTLibInstrumentInsCallback)(void *drcontext, instrlist_t *ilist,
                                            instr_t *ins, void *v, uint slot);
typedef void (*CallbackFunc)();
typedef struct CallbackFuncs{
    CallbackFunc initFunc;
    CallbackFunc finiFunc;
    CallbackFunc threadStartFunc;
    CallbackFunc threadEndFunc;
} * CCTLibCallbackFuncsPtr_t;

typedef int TLS_KEY; 

// Enum representing different data object types
enum ObjectTypeEnum {
    STACK_OBJECT,
    DYNAMIC_OBJECT,
    STATIC_OBJECT,
    UNKNOWN_OBJECT
};
enum CCTLibUsageMode {
    CCT_LIB_MODE_COLLECTION = 1,
    CCT_LIB_MODE_POSTMORTEM = 2
};


// Predefined callback for tracking no INS
DR_EXPORT
inline bool
InterestingInsNone(instr_t *ins)
{
    return false;
}

// Predefined callback for tracking all INS
DR_EXPORT
inline bool
InterestingInsAll(instr_t *ins)
{
    return true;
}

// Predefined callback for tracking all memory INS
DR_EXPORT
inline bool
InterestingInsMemoryAccess(instr_t *instr)
{
    return (instr_reads_memory(instr) || instr_writes_memory(instr));
}



#define INTERESTING_INS_ALL (InterestingInsAll)
#define INTERESTING_INS_NONE (InterestingInsNone)
#define INTERESTING_INS_MEMORY_ACCESS (InterestingInsMemoryAccess)



/*
    Description:
            CCTLib clients must call this before using CCTLib.
    Arguments: 
            isInterestingIns: a client tool callback that should return boolean true/false if a given INS needs to collect context. Following predefined values are available for client tools: INTERESTING_INS_ALL, INTERESTING_INS_NONE, and INTERESTING_INS_MEMORY_ACCESS. 
            logFile: file pointer where CCTLib will put its outputdata. 
            userCallback: a client callback that CCTLib calls on each INS for which isInterestingIns is true passing it userCallbackArg value. See one of the examples in tests directory for example usage. 
            callbackFuncs: a struct contains four callback funcs 
            doDataCentric: should be set to true if the client wants CCTLib to do data-centric attribution.
*/
DR_EXPORT
int
drcctlib_init(IsInterestingInsFptr isInterestingIns, file_t logFile,
             CCTLibInstrumentInsCallback userCallback, void *userCallbackArg, CCTLibCallbackFuncsPtr_t callbackFuncs = nullptr, bool doDataCentric = false);
/*
    Description:
            Reads serialized CCT metadata and rebuilds CCTs for postmortem analysis.
            Caution: This should never be called with PinCCTLibInit().
    Arguments:
            logFile: file pointer where CCTLib will put its output data.
            serializedFilesDirectory: Path to directory where previously files were serialized.
*/
DR_EXPORT
int
DrCCTLibInitForPostmortemAnalysis(FILE *logFile, string serializedFilesDirectory); // undefined

/*
    Description:
            Client tools call this API when they need the calling context handle (ContextHandle_t).
    Arguments: 
            drcontext: 
            slot: 
*/
DR_EXPORT
ContextHandle_t
GetContextHandle(void *drcontext, const uint32_t slot);

/*
    Description:
            Client tools call this API when they need the handle to a data object (DataHandle_t).
    Arguments:
            drcontext:
            address: effectve address for which the data object is needed.
*/
DR_EXPORT
DataHandle_t 
GetDataObjectHandle(void *drcontext, void *address); // undefined

/*
    Description:
            Prints the full calling context whose handle is ctxtHandle.
*/
DR_EXPORT
void
PrintFullCallingContext(const ContextHandle_t ctxtHandle);

/*
        Description:
                Returns the full calling context whose handle is ctxtHandle.

        Arguments:
                ctxtHandle: is the context handle for which the full call path is requested. 
                contextVec: is a vector that will be populated with the full call path.
*/
DR_EXPORT
void
GetFullCallingContext(const ContextHandle_t ctxtHandle, vector<Context_t> &contextVec); // undefined

/*
   Description:
        Dumps all CCTs into DOT files for visualization.
*/
DR_EXPORT
void
DottifyAllCCTs();  // undefined

/*
   Description:
       Given two CCT handles, returns true if the call path from root to leaves, except the leaves themselves are the same.
*/
DR_EXPORT
bool
HaveSameCallerPrefix(ContextHandle_t ctxt1, ContextHandle_t ctxt2); // undefined

DR_EXPORT
bool
IsSameSourceLine(ContextHandle_t ctxt1, ContextHandle_t ctxt2); // undefined

DR_EXPORT
void
AppendLoadModulesToStream(iostream &ios);  // undefined

DR_EXPORT
void
LogContexts(iostream &ios, ContextHandle_t ctxt1, ContextHandle_t ctxt2); // undefined

// }

#endif // _DRCCTLIB_H_
