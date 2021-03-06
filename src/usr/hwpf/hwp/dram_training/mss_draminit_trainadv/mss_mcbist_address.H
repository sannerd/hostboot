/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/hwpf/hwp/dram_training/mss_draminit_trainadv/mss_mcbist_address.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2013,2014                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
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
// $Id: mss_mcbist_address.H,v 1.5 2014/01/24 06:59:30 sasethur Exp $
// *!***************************************************************************
// *! (C) Copyright International Business Machines Corp. 1997, 1998, 2013
// *!           All Rights Reserved -- Property of IBM
// *!                   ***  ***
// *!***************************************************************************
// *! FILENAME             :  mss_mcbist_address.H
// *! TITLE                : 
// *! DESCRIPTION          : MCBIST procedures
// *! CONTEXT              : 
// *!
// *! OWNER  NAME          : 
// *! BACKUP               : 
// *!***************************************************************************
// CHANGE HISTORY:
//-------------------------------------------------------------------------------
// Version:|Author: | Date:   | Comment:
//  1.3    |bellows |03-Apr-13| Added Id for firmware
//  1.4    |preeragh|17-Dec-14| Removed unwanted header includes
//  1.5    |mjjones |20-Jan-14| RAS Review Updates
// --------|--------|---------|--------------------------------------------------
//------------------------------------------------------------------------------
#ifndef MSS_MCBIST_ADDRESS_H
#define MSS_MCBIST_ADDRESS_H

/*****************************************************************************/
/* mss_mcbist_address.H                                                      */
/*****************************************************************************/
#include <fapi.H>
#include <cen_scom_addresses.H>
#include <mss_access_delay_reg.H>
#include <mss_mcbist.H>
#include <string.h>
extern "C"
{
using namespace fapi;

enum interleave_type
{
    BANK_RANK,
    RANK_BANK,
    BANK_ONLY,
    RANK_ONLY,
    RANKS_DIMM0,
    RANKS_DIMM1,
    USER_PATTERN
};

fapi::ReturnCode address_generation(const fapi:: Target & i_target_mba,
                                    uint8_t i_port,
                                    mcbist_addr_mode i_addr_type,
                                    interleave_type i_add_inter_type,
                                    uint8_t i_rank,
                                    uint64_t &io_start_address,
                                    uint64_t &io_end_address);

fapi::ReturnCode parse_addr(const fapi:: Target & i_target_mba,
                            char addr_string[],
                            uint8_t mr3_valid,
                            uint8_t mr2_valid,
                            uint8_t mr1_valid,
                            uint8_t l_dram_rows,
                            uint8_t l_dram_cols,
                            uint8_t l_addr_inter);

}
#endif
