/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/test/prdfsimFspMfgThresholdFile.C $         */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2009,2014              */
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

#include <prdfsimFspMfgThresholdFile.H>
#include <prdfMfgThresholdMgr.H>
#include <prdfGlobal.H>
#include <prdfErrlUtil.H>
#include <prdfTrace.H>

namespace PRDF
{

void SimFspMfgThresholdFile::packThresholdDataIntoBuffer(
                             uint8_t* & o_buffer,
                             uint32_t i_sizeOfBuf)
{
    #define FUNC "[SimFspMfgThresholdFile::packThresholdDataIntoBuffer]"
    PRDF_ENTER(FUNC" i_sizeOfBuf: %d", i_sizeOfBuf);
    PRDF_TRAC("iv_thresholds.size: %d, sizeof(SyncThreshold_t): %d, total: %d",
              iv_thresholds.size(), sizeof(SyncThreshold_t),
              iv_thresholds.size() * sizeof(SyncThreshold_t));

    assert(i_sizeOfBuf ==
       (iv_thresholds.size() * sizeof(SyncThreshold_t)));

    assert(NULL != o_buffer);

    SyncThreshold_t* l_ptr = reinterpret_cast<SyncThreshold_t*>(o_buffer);

    for(Threshold_t::iterator i = iv_thresholds.begin();
        i != iv_thresholds.end();
        ++i)
    {
        PRDF_TRAC("threshold: %d, count: %d", i->first, i->second);
        l_ptr->hash  = htonl(i->first);
        l_ptr->value = i->second;
        l_ptr++;
    }

    PRDF_EXIT(FUNC);

    #undef FUNC
}

void SimFspMfgThresholdFile::overrideThreshold()
{
    #define FUNC "[SimFspMfgThresholdFile::overrideThreshold]"
    PRDF_ENTER(FUNC);

    // just hardcode the default override value for now
    static const uint8_t MFG_THRES_OVERRIDE_VALUE = 10;

#define PRDF_MFGTHRESHOLD_TABLE_BEGIN
#define PRDF_MFGTHRESHOLD_TABLE_END
#define PRDF_MFGTHRESHOLD_ENTRY(a,b,c) \
        iv_thresholds[b] = MFG_THRES_OVERRIDE_VALUE;
#include <prdfMfgThresholds.H>

    for(Threshold_t::iterator i = iv_thresholds.begin();
        i != iv_thresholds.end();
        ++i)
    {
        uint16_t value = MfgThresholdMgr::getInstance()->
                          getThreshold(i->first);
        PRDF_TRAC("hash: %d, value - override: %d, "
                  "default: %d", i->first, i->second, value);
    }

    PRDF_EXIT(FUNC);

    #undef FUNC
}

} // end namespace PRDF
