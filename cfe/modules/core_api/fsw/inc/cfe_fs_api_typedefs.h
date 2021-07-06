/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/**
 * @file
 *
 * Purpose:  cFE File Services (FS) library API header file
 *
 * Author:   S.Walling/Microtel
 *
 */

#ifndef CFE_FS_API_TYPEDEFS_H
#define CFE_FS_API_TYPEDEFS_H

/*
** Required header files...
*/
#include "common_types.h"
#include "osconfig.h"
#include "cfe_fs_extern_typedefs.h"

/**
 * \brief Generalized file types/categories known to FS
 *
 * This defines different categories of files, where they
 * may reside in different default locations of the virtualized file system.
 *
 * This is different from, and should not be confused with, the "SubType"
 * field in the FS header.  This value is only used at runtime for FS APIs
 * and should not actually appear in any output file or message.
 */
typedef enum
{
    CFE_FS_FileCategory_UNKNOWN,          /**< Placeholder, unknown file category */
    CFE_FS_FileCategory_DYNAMIC_MODULE,   /**< Dynamically loadable apps/libraries (e.g. .so, .o, .dll, etc) */
    CFE_FS_FileCategory_BINARY_DATA_DUMP, /**< Binary log file generated by various data dump commands */
    CFE_FS_FileCategory_TEXT_LOG,         /**< Text-based log file generated by various commands */
    CFE_FS_FileCategory_SCRIPT,           /**< Text-based Script files (e.g. ES startup script) */
    CFE_FS_FileCategory_TEMP,             /**< Temporary/Ephemeral files */
    CFE_FS_FileCategory_MAX               /**< Placeholder, keep last */
} CFE_FS_FileCategory_t;

/*
 * Because FS is a library not an app, it does not have its own context or
 * event IDs.  The file writer runs in the context of the ES background task
 * on behalf of whatever App requested the file write.
 *
 * This is a list of abstract events associated with background file write jobs.
 * An app requesting the file write must supply a callback function to translate
 * these into its own event IDs for feedback (i.e. file complete, error conditions, etc).
 */
typedef enum
{
    CFE_FS_FileWriteEvent_UNDEFINED, /* placeholder, no-op, keep as 0 */

    CFE_FS_FileWriteEvent_COMPLETE,           /**< File is completed successfully */
    CFE_FS_FileWriteEvent_CREATE_ERROR,       /**< Unable to create/open file */
    CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR, /**< Unable to write FS header */
    CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR, /**< Unable to write data record */

    CFE_FS_FileWriteEvent_MAX /* placeholder, no-op, keep last */

} CFE_FS_FileWriteEvent_t;

/**
 * Data Getter routine provided by requester
 *
 * Outputs a data block.  Should return true if the file is complete (last record/EOF), otherwise return false.
 */
typedef bool (*CFE_FS_FileWriteGetData_t)(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize);

/**
 * Event generator routine provided by requester
 *
 * Invoked from certain points in the file write process.  Implementation may invoke CFE_EVS_SendEvent() appropriately
 * to inform of progress.
 */
typedef void (*CFE_FS_FileWriteOnEvent_t)(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum,
                                          size_t BlockSize, size_t Position);

/**
 * \brief External Metadata/State object associated with background file writes
 *
 * Applications intending to schedule background file write jobs should instantiate
 * this object in static/global data memory.  This keeps track of the state of the
 * file write request(s).
 */
typedef struct CFE_FS_FileWriteMetaData
{
    volatile bool IsPending; /**< Whether request is pending (volatile as it may be checked outside lock) */

    char FileName[OS_MAX_PATH_LEN]; /**< Name of file to write */

    /* Data for FS header */
    uint32 FileSubType;                          /**< Type of file to write (for FS header) */
    char   Description[CFE_FS_HDR_DESC_MAX_LEN]; /**< Description of file (for FS header) */

    CFE_FS_FileWriteGetData_t GetData; /**< Application callback to get a data record */
    CFE_FS_FileWriteOnEvent_t OnEvent; /**< Application callback for abstract event processing */

} CFE_FS_FileWriteMetaData_t;

#endif /* CFE_FS_API_TYPEDEFS_H */
