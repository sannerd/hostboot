/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/initservice/initsvcstructs.H $                */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2011,2014              */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#ifndef __COMMON_INITSVCSTRUCTS_H
#define __COMMON_INITSVCSTRUCTS_H
/**
 *  @file initsvcstructs.H
 *
 *  common structs for initservice, extinitsvc, istepdispatcher
 *
 */

/******************************************************************************/
// Includes
/******************************************************************************/
#include <stdint.h>
#include <sys/vfs.h>            // VFS_MODULE_NAME_MAX

#include <initservice/initsvcreasoncodes.H>

// This constant has a corresponding entry in src/build/tools/listdeps.pl.
#define MAX_DEPENDENT_MODULES   7


namespace   INITSERVICE
{

/******************************************************************************/
// Globals/Constants
/******************************************************************************/


/******************************************************************************/
// Typedef/Enumerations
/******************************************************************************/

/**
 * @note    istepname tag - this is a way to call out the istep, substep, and
 *          istep name for spless operation.    scanistepnames.pl will run
 *          through this, find the istepnames, and publish it as a CVS file
 *          for use by the SPLess user console (python) code.
 *          Usage:
 *          @ISTEPNAME( 4, 0, "init_target_states" )
 *
 *          This will be picked up by scanistepnames.pl and put in as the line
 *              4,0,init_target_states
 *
 *          HostBoot will see it as the string "init_target_states" in the
 *          istepNlist.H file
 */
#define ISTEPNAME( ISTEP, SUBSTEP, NAME )      NAME


/**
 * @enum    TaskType
 *  - NONE          ==  placeholder, no task
 *  - INIT_TASK     ==  load and initialize task, but task has no _start()
 *                      (extended image only)
 *  - START_TASK    ==  BASE_IMAGE: call _start() function entry point
 *                      EXT_IMAGE: call _init(), then _start()
 *  - START_FN      ==  task with function pointer entry point
 *  - UNINIT_TASK   ==  call _fini() to call static destructor(s) on the task.
 *                      (extended image only)
 *  - END_TASK_LIST ==  last entry in the task list.
 */

enum    TaskType
{
    UNDEFINED_TT    =   0,
    NONE,
    INIT_TASK,
    START_TASK,
    START_FN,
    UNINIT_TASK,
    END_TASK_LIST,
};


/**
 * @enum    ModuleType
 *  -   BASE_IMAGE == module in the base image
 *  -   EXT_IMAGE == module in the extended image
 */
enum    ModuleType
{
    UNDEFINED_MT    =   0,
    BASE_IMAGE,
    EXT_IMAGE,
};

/**
 * @enum    IplOpType
 * bitmap indicating whether the task is an op in certain IPL modes.
 */
enum IplOpType
{
    IPL_NOOP      = 0x00,
    NORMAL_IPL_OP = 0x01,
    MPIPL_OP      = 0x02,
};

/**
 * @struct  TaskFlags
 *
 *  - run _start() function on start
 *  - module type, BASE_MODULE or EXT_MODULE
 *  - module_id for errorlog if task fails
 *
 */
struct  TaskFlags
{
    TaskType            task_type;          // task execution flags
    ModuleType          module_type;        // BASE_IMAGE or EXT_IMAGE
    uint8_t             ipl_op;             // bitmap of IplOpType enum
                                            // indicating whether task
                                            // is an op in certain IPL modes.
    bool                check_attn;         // flag indicating whether to check
                                            // for attentions and invoke PRD
                                            // for analysis after executing
                                            // the istep substep
                                            // true = check for attentions
};


/**
 * @struct  TaskInfo
 *
 * Holds information on each task in the system.
 *      - taskname
 *      - execution flags, see TaskFlags above
 *
 */
struct  TaskInfo
{
    const char * const  taskname;
    void*               (*taskfn)(void *ptr);
    const   TaskFlags   taskflags;
};


/**
 * @struct  DepModInfo
 *
 * Holds information on other modules that need to be loaded so that this
 * istep will run.
 */
struct  DepModInfo
{
    const char * const modulename[MAX_DEPENDENT_MODULES];
};


/**
 * @struct  ExtTaskInfo
 *
 * struct to store the TaskInfo for the ISteps, plus the number of items in
 * the istep (calculated at compiletime).
 */
struct  ExtTaskInfo
{
    const TaskInfo      *const pti;
    const uint64_t      numitems;
    const DepModInfo    *const depModules;
};

} // namespace

#endif
