/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/hwpf/hwp/occ/occ_procedures/p8_ocb_init.C $           */
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
// $Id: p8_ocb_init.C,v 1.7 2013/06/05 17:39:01 jimyac Exp $
// $Source: /afs/awd/projects/eclipz/KnowledgeBase/.cvsroot/eclipz/chips/p8/working/procedures/ipl/fapi/p8_ocb_init.C,v $
//------------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2012
// *! All Rights Reserved -- Property of IBM
// *! ***  ***
//------------------------------------------------------------------------------
// *! OWNER NAME: Jim Yacynych         Email: jimyac@us.ibm.com
// *!

/// \file p8_ocb_init.C
/// \brief Setup and configure OCB channels
///
/// \todo add support for linear window mode
///
///   High-level procedure flow:
/// \verbatim
///   o if mode = PM_CONFIG
///     o placeholder - currently do nothing
///   o if mode = PM_INIT
///     o placeholder - currently do nothing
///   o if mode = PM_RESET
///     o reset each register in each OCB channel to its scan0-flush state
///   o if mode = PM_SETUP_PIB or PM_SETUP_ALL
///     o process parameters passed to procedure
///     o Set up channel control/status register based on passed parameters (OCBCSRn)
///     o Set Base Address Register
///       o linear streaming & non-streaming => OCBARn
///       o push queue                       => OCBSHBRn  (only if PM_SETUP_ALL)
///       o pull queue                       => OCBSLBRn  (only if PM_SETUP_ALL)
///     o Set up queue control and status register (only if PM_SETUP_ALL)
///       o push queue  => OCBSHCSn
///       o pull queue  => OCBSLCSn
///
/// Procedure Prereq:
///   o System clocks are running
/// \endverbatim
//------------------------------------------------------------------------------

// ----------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------
#include <fapi.H>
#include "p8_scom_addresses.H"
#include "p8_ocb_init.H"

extern "C" {

using namespace fapi;

// ----------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------
ReturnCode proc_ocb_reset(const Target& i_target);

ReturnCode proc_ocb_setup(const Target& i_target, const uint8_t  i_ocb_chan,
                                                  const uint8_t  i_ocb_type,
                                                  const uint32_t i_ocb_bar,
                                                  const uint8_t  i_ocb_upd_reg,
                                                  const uint8_t  i_ocb_q_len,
                                                  const uint8_t  i_ocb_ouflow_en,
                                                  const uint8_t  i_ocb_itp_type);

// ----------------------------------------------------------------------
// Function definitions
// ----------------------------------------------------------------------
/// \param[in]   i_target          => Chip Target
/// \param[in]   i_mode            => PM_CONFIG, PM_RESET, PM_INIT, or PM_SETUP
/// \param[in]   i_ocb_chan        => select channel 0-3 to set up
/// \param[in]   i_ocb_type        => 0=indirect  1=linear stream  2=circular push  3=circular pull
/// \param[in]   i_ocb_bar         => 32-bit channel base address (29 bits + "000")
/// \param[in]   i_ocb_q_len       => 0-31 length of push or pull queue in  (queue_length + 1) * 8B
/// \param[in]   i_ocb_ouflow_en   => 0=disabled 1=enabled
/// \param[in]   i_ocb_itp_type    => 0=full  1=not full  2=empty  3=not empty
/// \retval FAPI_RC_SUCCESS
/// \retval ERROR defined in xml

ReturnCode
p8_ocb_init(const Target& i_target, const uint32_t  i_mode,
                                       const uint8_t  i_ocb_chan,
                                       const uint8_t  i_ocb_type,
                                       const uint32_t i_ocb_bar,
                                       const uint8_t  i_ocb_q_len,
                                       const uint8_t  i_ocb_ouflow_en,
                                       const uint8_t  i_ocb_itp_type)
{
  ReturnCode rc;
  uint8_t upd_reg = OCB_UPD_PIB_REG;

  FAPI_INF("Executing p8_ocb_init ....");

  // -------------------------------------------------------------------------------
  // Config : perform translation of any Platform Attributes into Feature Attributes
  //          that are applied during Initalization
  // -------------------------------------------------------------------------------
  if (i_mode == PM_CONFIG) {
    FAPI_INF(" *** Configuring OCB Indirect Channels 0-3");
  }

  // --------------------------------------------------------------
  // Init : perform order or dynamic operations to initialize
  //        the PMC using necessary Platform or Feature attributes.
  // --------------------------------------------------------------
  else if (i_mode == PM_INIT) {
    FAPI_INF(" *** Initializing OCB Indirect Channels 0-3");
  }

  // --------------------------------------------------------------
  // Reset : perform order or dynamic operations to initialize
  //        the PMC using necessary Platform or Feature attributes.
  // --------------------------------------------------------------
  else if (i_mode == PM_RESET) {
    FAPI_INF(" *** Resetting OCB Indirect Channels 0-3");
    rc = proc_ocb_reset(i_target);
  }

  // --------------------------------------------------------------
  // Setup : perform user setup of an indirect channel
  // --------------------------------------------------------------
  else if (i_mode == PM_SETUP_PIB || i_mode == PM_SETUP_ALL ) {
    FAPI_INF(" *** Setup OCB Indirect Channel %d",i_ocb_chan);

    if (i_mode == PM_SETUP_ALL)
      upd_reg = OCB_UPD_PIB_OCI_REG;

    // call function to setup ocb channel
    rc = proc_ocb_setup(i_target , i_ocb_chan,
                                   i_ocb_type,
                                   i_ocb_bar,
                                   upd_reg,
                                   i_ocb_q_len,
                                   i_ocb_ouflow_en,
                                   i_ocb_itp_type);
  }
  else {
    FAPI_ERR("Unknown mode passed to proc_ocb_init. Mode %x ....\n", i_mode);
    FAPI_SET_HWP_ERROR(rc, RC_PROCPM_OCBINIT_BAD_MODE);
  }

  FAPI_INF("Completing p8_ocb_init ....");

  return rc;
}

// ---------------------
// function definintions
// ---------------------
/// \brief Setup specified channel to type specified
/// \param[in]   i_target          => Chip Target
/// \param[in]   i_ocb_chan        => select channel 0-3 to set up
/// \param[in]   i_ocb_type        => 0=indirect  1=linear stream  2=circular push  3=circular pull
/// \param[in]   i_ocb_bar         => 32-bit channel base address (29 bits + "000")
/// \param[in]   i_ocb_upd_reg     => 0=update PIB registers only  1=update PIB & OCI registers
/// \param[in]   i_ocb_q_len       => 0-31 length of push or pull queue in  (queue_length + 1) * 8B
/// \param[in]   i_ocb_ouflow_en   => 0=disabled 1=enabled
/// \param[in]   i_ocb_itp_type    => 0=full  1=not full  2=empty  3=not empty

ReturnCode proc_ocb_setup(const Target& i_target,  const uint8_t  i_ocb_chan,
                                                   const uint8_t  i_ocb_type,
                                                   const uint32_t i_ocb_bar,
                                                   const uint8_t  i_ocb_upd_reg,
                                                   const uint8_t  i_ocb_q_len,
                                                   const uint8_t  i_ocb_ouflow_en,
                                                   const uint8_t  i_ocb_itp_type)
{
  ReturnCode rc;
  uint32_t   l_ecmdRc = 0;

  ecmdDataBufferBase data(64);
  ecmdDataBufferBase mask_or(64);
  ecmdDataBufferBase mask_and(64);

  uint32_t OCBASE = 0x0;

  // --------------------------------------------
  // verify paramater values for queues are valid
  // --------------------------------------------
  if (i_ocb_type == OCB_TYPE_PUSHQ || i_ocb_type == OCB_TYPE_PULLQ) {

    // check queue_len
    if (i_ocb_q_len > 31) {
      FAPI_ERR("Bad Queue Length Passed to Procedure => %d", i_ocb_q_len);
      FAPI_SET_HWP_ERROR(rc, RC_PROCPM_OCBINIT_BAD_Q_LENGTH_PARM);
      return rc;
    }

    // check queue_itp_type
    if (!(i_ocb_itp_type <= OCB_Q_ITPTYPE_NOTEMPTY)) {
      FAPI_ERR("**** ERROR : Bad Interrupt Type Passed to Procedure => %d", i_ocb_itp_type);
      FAPI_SET_HWP_ERROR(rc, RC_PROCPM_OCBINIT_BAD_ITP_TYPE_PARM);
      return rc;
    }

    // check queue_overunderflow_en
    if (i_ocb_ouflow_en != OCB_Q_OUFLOW_NULL && i_ocb_ouflow_en != OCB_Q_OUFLOW_EN && i_ocb_ouflow_en != OCB_Q_OUFLOW_DIS) {
      FAPI_ERR("**** ERROR : Bad Queue Over/Under Flow Enable Passed to Procedure => %d", i_ocb_ouflow_en);
      FAPI_SET_HWP_ERROR(rc, RC_PROCPM_OCBINIT_BAD_Q_OVER_UNDERFLOW_PARM);
      return rc;
    }
  }

  // check channel number
  if (i_ocb_chan > OCB_CHAN3) {
    FAPI_ERR("**** ERROR : Bad Channel Number Passed to Procedure => %d", i_ocb_chan);
    FAPI_SET_HWP_ERROR(rc, RC_PROCPM_OCBINIT_BAD_CHAN_NUM_PARM);
    return rc;
  }

  // check valid channel type for channel3
  if (i_ocb_chan == OCB_CHAN3 && !(i_ocb_type == OCB_TYPE_LIN || i_ocb_type == OCB_TYPE_LINSTR) ) {
    FAPI_ERR("**** ERROR : Bad Channel Type for Channel 3 Passed to Procedure => %d", i_ocb_type);
    FAPI_SET_HWP_ERROR(rc, RC_PROCPM_OCBINIT_BAD_CHAN3_TYPE_PARM);
    return rc;
  }  

  // --------------------------------------------------------------------
  // Setup Status and Control Register (OCBCSRn, OCBCSRn_AND, OCBCSRn_OR)
  //    bit 2 => pull_read_underflow_en (0=disabled 1=enabled)
  //    bit 3 => push_write_overflow_en (0=disabled 1=enabled)
  //    bit 4 => ocb_stream_mode        (0=disabled 1=enabled)
  //    bit 5 => ocb_stream_type        (0=linear   1=circular)
  // --------------------------------------------------------------------
  l_ecmdRc |= mask_or.flushTo0();  
  l_ecmdRc |= mask_and.flushTo1();

  if (i_ocb_type == OCB_TYPE_LIN) {                    // linear non-streaming
    l_ecmdRc |= mask_and.clearBit(4);
    l_ecmdRc |= mask_and.clearBit(5);
  }
  else if (i_ocb_type == OCB_TYPE_LINSTR) {            // linear streaming
    l_ecmdRc |= mask_or.setBit(4);
    l_ecmdRc |= mask_and.clearBit(5);
  }
  else if (i_ocb_type == OCB_TYPE_CIRC) {              // circular
    l_ecmdRc |= mask_or.setBit(4);
    l_ecmdRc |= mask_or.setBit(5);
  }
  else if (i_ocb_type == OCB_TYPE_PUSHQ) {             // push queue
    l_ecmdRc |= mask_or.setBit(4);
    l_ecmdRc |= mask_or.setBit(5);

    if    (i_ocb_ouflow_en == OCB_Q_OUFLOW_EN) {
      l_ecmdRc |= mask_or.setBit(3);
    }
    else if (i_ocb_ouflow_en == OCB_Q_OUFLOW_DIS) {
      l_ecmdRc |= mask_and.clearBit(3);
    }

  }
  else if (i_ocb_type == OCB_TYPE_PULLQ) {             // pull queue
    l_ecmdRc |= mask_or.setBit(4);
    l_ecmdRc |= mask_or.setBit(5);

    if (i_ocb_ouflow_en == OCB_Q_OUFLOW_EN) {
      l_ecmdRc |= mask_or.setBit(2);
    }
    else if (i_ocb_ouflow_en == OCB_Q_OUFLOW_DIS) {
      l_ecmdRc |= mask_and.clearBit(2);
    }

  }
  else {
    FAPI_ERR("**** ERROR : Bad Channel Type Passed to Procedure => %d", i_ocb_type);
    FAPI_SET_HWP_ERROR(rc, RC_PROCPM_OCBINIT_BAD_CHAN_TYPE_PARM);
    return rc;
  }

  if (l_ecmdRc) {
    FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
    rc.setEcmdError(l_ecmdRc);
    return rc;
  }

  FAPI_DBG("Writing to Channel %d Register : OCB Channel Status & Control", i_ocb_chan);

  // write using OR mask
  rc = fapiPutScom(i_target, OCBCSRn_OR[i_ocb_chan] , mask_or);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel Status & Control");
    return rc;
  }

  // write using AND mask
  rc = fapiPutScom(i_target, OCBCSRn_AND[i_ocb_chan] , mask_and);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel Status & Control");
    return rc;
  }

  // --------------------------------------------------------------------------------------
  // set address base register for linear mode
  //   bits 0:31  =>  32 bit base address
  //                  bits 0:1   => OCI region  (00=PBA  01=Registers  10=reserved  11=SRAM)
  //                  bits 29:31 => "000"
  // --------------------------------------------------------------------------------------
  if (!(i_ocb_type == OCB_TYPE_NULL || i_ocb_type == OCB_TYPE_CIRC)) {    // don't update bar if type null or circular

    if (i_ocb_type == OCB_TYPE_LIN || i_ocb_type == OCB_TYPE_LINSTR) {
      OCBASE = OCBARn[i_ocb_chan];
    }
    else if (i_ocb_type == OCB_TYPE_PUSHQ) {
      OCBASE = OCBSHBRn[i_ocb_chan];
    } else {                                // else PULL -- FIXME - need Linear Window Type Implemented
      OCBASE = OCBSLBRn[i_ocb_chan];
    }

    l_ecmdRc |= data.flushTo0();
    l_ecmdRc |= data.setWord(0, i_ocb_bar);

    if (l_ecmdRc) {
      FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
      rc.setEcmdError(l_ecmdRc);
      return rc;
    }

    FAPI_DBG("Writing to Channel %d Register : OCB Channel Base Address", i_ocb_chan);

    rc = fapiPutScom(i_target, OCBASE , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel Base Address");
      return rc;
    }

  }
  // -------------------------------------
  // set up push queue control register
  //    bits 4:5  => push interrupt action
  //                   00=full
  //                   01=not full
  //                   10=empty
  //                   11=not empty
  //    bits 6:8 => push queue length
  //    bit 31   => push queue enable
  // -------------------------------------
  if (i_ocb_type == OCB_TYPE_PUSHQ && i_ocb_upd_reg == OCB_UPD_PIB_OCI_REG) {
    l_ecmdRc |= data.flushTo0();
    l_ecmdRc |= data.insertFromRight(i_ocb_q_len,6,5);
    l_ecmdRc |= data.insertFromRight(i_ocb_itp_type,4,2);
    l_ecmdRc |= data.setBit(31);

    if (l_ecmdRc) {
      FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
      rc.setEcmdError(l_ecmdRc);
      return rc;
    }

    FAPI_DBG("Writing to Channel %d Register : OCB Channel Push Control/Status Address", i_ocb_chan);

    rc = fapiPutScom(i_target, OCBSHCSn[i_ocb_chan] , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel Push Address");
      return rc;
    }

  }

  // -------------------------------------
  // set up pull queue control register
  //    bits 4:5  => pull interrupt action
  //                   00=full
  //                   01=not full
  //                   10=empty
  //                   11=not empty
  //    bits 6:8 => pull queue length
  //    bit 31   => pull queue enable
  // -------------------------------------
  if (i_ocb_type == OCB_TYPE_PULLQ && i_ocb_upd_reg == OCB_UPD_PIB_OCI_REG) {
    l_ecmdRc |= data.flushTo0();
    l_ecmdRc |= data.insertFromRight(i_ocb_q_len,6,5);
    l_ecmdRc |= data.insertFromRight(i_ocb_itp_type,4,2);
    l_ecmdRc |= data.setBit(31);

    if (l_ecmdRc) {
      FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
      rc.setEcmdError(l_ecmdRc);
      return rc;
    }

    FAPI_DBG("Writing to Channel %d Register : OCB Channel Pull Control/Status Address", i_ocb_chan);

    rc = fapiPutScom(i_target, OCBSLCSn[i_ocb_chan] , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel Pull Address");
      return rc;
    }

  }

  // --------------------------------
  // Print Channel Configuration Info
  // --------------------------------
  FAPI_IMP("-----------------------------------------------------");
  FAPI_IMP("OCB Channel Configuration                            ");
  FAPI_IMP("-----------------------------------------------------");
  FAPI_IMP("  channel number             => %d ", i_ocb_chan);
  FAPI_IMP("  channel type               => %d ", i_ocb_type);

  if (i_ocb_type == OCB_TYPE_PUSHQ || i_ocb_type == OCB_TYPE_PULLQ) {
    FAPI_IMP("  queue length               => %d ", i_ocb_q_len);
    FAPI_IMP("  interrupt type             => %d ", i_ocb_itp_type);

    if (i_ocb_type == OCB_TYPE_PUSHQ) {
       FAPI_IMP("  push write overflow enable => %d ", i_ocb_ouflow_en);
    }
    else {
      FAPI_IMP("  pull write overflow enable => %d ", i_ocb_ouflow_en);
    }

  }

  FAPI_IMP("  channel base address       => 0x%08X ", i_ocb_bar);
  FAPI_IMP("-----------------------------------------------------");

  return rc;
} // end proc_ocb_setup


/// \brief Reset OCB Channels to default state (ie. scan-0 flush state)
/// \param[in]   i_target          => Chip Target

ReturnCode proc_ocb_reset(const Target& i_target) {
  ReturnCode rc;
  uint32_t   l_ecmdRc = 0;

  ecmdDataBufferBase data(64);
  uint8_t i = 0;

  // ---------------------------------------
  // Loop over PIB Registers in Channels 0-3
  // ---------------------------------------
  for (i = 0; i <= 3; i++) {
    l_ecmdRc |= data.flushTo0();

    if (l_ecmdRc) {
      FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
      rc.setEcmdError(l_ecmdRc);
      return rc;
    }

    // clear out OCBAR
    rc = fapiPutScom(i_target, OCBARn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d BAR Register", i);
      return rc;
    }

    // clear out OCBCSR
    //  - clear out using AND
    //  - set bit 5 (circular mode) using OR  (for channels 0-2)
    //  - set bit 4 (stream mode) using OR    (for channel 3)
    rc = fapiPutScom(i_target, OCBCSRn_AND[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Control & Status AND Register", i);
      return rc;
    }

    if (i == 3) 
      l_ecmdRc |= data.setBit(4);
    else
      l_ecmdRc |= data.setBit(5);

    if (l_ecmdRc) {
      FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
      rc.setEcmdError(l_ecmdRc);
      return rc;
    }

    rc = fapiPutScom(i_target, OCBCSRn_OR[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Control & Status OR Register", i);
      return rc;
    }

    l_ecmdRc |= data.flushTo0();

    if (l_ecmdRc) {
      FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
      rc.setEcmdError(l_ecmdRc);
      return rc;
    }

    // clear out Error Status
    rc = fapiPutScom(i_target, OCBESRn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Error Status Register", i);
      return rc;
    }
  }// end for loop

  // ---------------------------------------
  // Loop over OCI Registers in Channels 0-2
  // ---------------------------------------
  for (i = 0; i <= 2; i++) {
    l_ecmdRc |= data.flushTo0();
  
    // clear out Pull Base
    rc = fapiPutScom(i_target, OCBSLBRn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Pull Base Register", i);
      return rc;
    }

    // clear out Push Base
    rc = fapiPutScom(i_target, OCBSHBRn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Push Base Register", i);
      return rc;
    }

    // clear out Pull Control & Status
    rc = fapiPutScom(i_target, OCBSLCSn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Pull Control & Status Register", i);
      return rc;
    }

    // clear out Push Control & Status
    rc = fapiPutScom(i_target, OCBSHCSn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Push Control & Status Register", i);
      return rc;
    }

    // clear out Stream Error Status
    rc = fapiPutScom(i_target, OCBSESn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Stream Error Status Register", i);
      return rc;
    }

    // clear out Untrusted Control
    rc = fapiPutScom(i_target, OCBICRn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Untrusted Control Register", i);
      return rc;
    }

    // clear out Linear Window Control
    rc = fapiPutScom(i_target, OCBLWCRn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Linear Window Control Register", i);
      return rc;
    }

    // clear out Linear Window Base
    //  - set bits 2:9
    l_ecmdRc |= data.setWord(0, 0x3FC00000);

    if (l_ecmdRc) {
       FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
      rc.setEcmdError(l_ecmdRc);
      return rc;
    }

    rc = fapiPutScom(i_target, OCBLWSBRn[i]   , data);
    if (!rc.ok()) {
      FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCB Channel %d Linear Window Base Register", i);
      return rc;
    }
  } // end for loop

  // -----------------------------------------
  // Set Interrupt Source Mask Registers 0 & 1 
  // OIMR0/1 @ 0X0006A006 & 0X0006A016
  // -----------------------------------------
  l_ecmdRc  = data.flushTo0();
  l_ecmdRc |= data.setWord(0, 0xFFFFFFFF);  // keep word1 0's for simics
    
  if (l_ecmdRc) {
    FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
    rc.setEcmdError(l_ecmdRc);
    return rc;
  }  
  
  rc = fapiPutScom(i_target, OCC_ITP_MASK0_MASK_OR_0x0006A006, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Source Mask Register0 (OIMR0)");
    return rc;
  }  

  rc = fapiPutScom(i_target, OCC_ITP_MASK1_MASK_OR_0x0006A016, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Source Mask Register1 (OIMR1)");
    return rc;
  }  

  // ---------------------------------------------------------------------------------
  // Clear OCC Interrupt Controller Registers
  //   - OITR0/1  Interrupt Type 0/1                         @ 0x0006A008 & 0x0006A018
  //   - OIEPR0/1 Interrupt Edge Polarity 0/1                @ 0x0006A009 & 0x0006A019
  //   - OISR0/1  Interrupt Source 0/1                       @ 0x0006A001 & 0x0006A011  
  //   - OCIR0/1  Interrupt Critical Enable 0/1              @ 0x0006A00A & 0x0006A01A
  //   - ODHER0/1 Interrupt Debug Halt Enable 0/1            @ 0x0006A00A & 0x0006A01A
  //   - OUDER0/1 Interrupt Unconditional Debug Event Enable @ 0x0006A00C & 0x0006A01C
  // ---------------------------------------------------------------------------------
  l_ecmdRc = data.flushTo0();  
  if (l_ecmdRc) {
    FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
    rc.setEcmdError(l_ecmdRc);
    return rc;
  }  

  rc = fapiPutScom(i_target, OCC_ITP_TYPE0_0x0006A008, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Type Register0 (OITR0)");
    return rc;
  }  
  
  rc = fapiPutScom(i_target, OCC_ITP_TYPE1_0x0006A018, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Type Register1 (OITR1)");
    return rc;
  }   


  rc = fapiPutScom(i_target, OCC_ITP_EDGE_POLARITY0_0x0006A009, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Edge Polarity Register0 (OIEPR0)");
    return rc;
  }  

  rc = fapiPutScom(i_target, OCC_ITP_EDGE_POLARITY1_0x0006A019, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Edge Polarity Register1 (OIEPR1)");
    return rc;
  }  


 rc = fapiPutScom(i_target, OCC_ITP_SOURCE0_MASK_AND_0x0006A001, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Source Register0 (OISR0)");
    return rc;
  }  
  
  rc = fapiPutScom(i_target, OCC_ITP_SOURCE1_MASK_AND_0x0006A011, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Source Register1 (OISR1)");
    return rc;
  }  


  rc = fapiPutScom(i_target, OCC_ITP_CRITICAL_EN0_0x0006A00A, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Critical Enable Register0 (OCIR0)");
    return rc;
  }  

  rc = fapiPutScom(i_target, OCC_ITP_CRITICAL_EN1_0x0006A01A, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Critical Enable Register1 (OCIR1)");
    return rc;
  }  


  rc = fapiPutScom(i_target, OCC_ITP_DEBUG_HALT_EN0_0x0006A00E, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Debug Halt Enable Register0 (ODHER0)");
    return rc;
  }  

  rc = fapiPutScom(i_target, OCC_ITP_DEBUG_HALT_EN1_0x0006A01E, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Debug Halt Enable Register1 (ODHER1)");
    return rc;
  }  

 
  rc = fapiPutScom(i_target, OCC_ITP_UNCOND_DEBUG_EN0_0x0006A00C, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Unconditional Debug Event Enable Register0 (OUDER0)");
    return rc;
  }  
  
  rc = fapiPutScom(i_target, OCC_ITP_UNCOND_DEBUG_EN1_0x0006A01C, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Unconditional Debug Event Enable Register1 (OUDER1)");
    return rc;
  }    
    
  // ----------------------------------------------------------
  // Clear OCC Interrupt Timer Registers 0 & 1 
  // OTR0/1 @ 0x0006A100 & 0x0006A101
  // ----------------------------------------------------------  
  // clear OTR0/1     0x0006A100 & 0x0006A101 need bits 0&1 set to clear register
  l_ecmdRc = data.flushTo0(); 
  l_ecmdRc |= data.setBit(0);
  l_ecmdRc |= data.setBit(1);
  
  if (l_ecmdRc) {
    FAPI_ERR("Error (0x%x) setting up ecmdDataBufferBase", l_ecmdRc);
    rc.setEcmdError(l_ecmdRc);
    return rc;
  }  

  rc = fapiPutScom(i_target, OCC_ITP_TIMER0_0x0006A100, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Timer0 Register (OTR0)");
    return rc;
  }  

  rc = fapiPutScom(i_target, OCC_ITP_TIMER1_0x0006A101, data);
  if (!rc.ok()) {
    FAPI_ERR("**** ERROR : Unexpected error encountered in write to OCC Interrupt Timer1 Register (OTR1)");
    return rc;
  }  

  return rc;
}

} //end extern C

/*
*************** Do not edit this area ***************
This section is automatically updated by CVS when you check in this file.
Be sure to create CVS comments when you commit so that they can be included here.

$Log: p8_ocb_init.C,v $
Revision 1.7  2013/06/05 17:39:01  jimyac
fixed SW207126 - simics issue of writing non-zero value to reserved bits




*/
