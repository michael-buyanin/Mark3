/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!

    \file   memutil.cpp

    \brief  Implementation of memory, string, and conversion routines
*/


#include "kerneltypes.h"
#include "mark3cfg.h"
#include "kernel_debug.h"
#include "memutil.h"

//---------------------------------------------------------------------------
void MemUtil::DecimalToHex( K_UCHAR ucData_, char *szText_ )
{
    K_UCHAR ucTmp = ucData_;
    K_UCHAR ucMax;

    KERNEL_ASSERT( szText_ );

    if (ucTmp >= 0x10) 
    {
        ucMax = 2;
    } 
    else 
    {
        ucMax = 1;
    }

    ucTmp = ucData_;
    szText_[ucMax] = 0;
    while (ucMax--)
    {
        if ((ucTmp & 0x0F) <= 9) 
        {
            szText_[ucMax] = '0' + (ucTmp & 0x0F);
        } 
        else 
        {
            szText_[ucMax] = 'A' + ((ucTmp & 0x0F) - 10);
        }
        ucTmp>>=4;
    }
}

//--------------------------------------------------------------------------- 
void MemUtil::DecimalToString( K_UCHAR ucData_, char *szText_ )
{
    K_UCHAR ucTmp = ucData_;
    K_UCHAR ucMax;

    KERNEL_ASSERT(szText_);

    // Find max index to print...
    if (ucData_ >= 100)
    {
        ucMax = 3;
    } 
    else if (ucData_ >= 10)
    {
        ucMax = 2;
    }
    else 
    {
        ucMax = 1;
    }

    szText_[ucMax] = 0;
    while (ucMax--)
    {
        szText_[ucMax] = '0' + (ucTmp % 10);
        ucTmp/=10;
    }
}

//---------------------------------------------------------------------------
// Basic checksum routines
K_UCHAR  MemUtil::Checksum8( const void *pvSrc_, K_USHORT usLen_ )
{
    K_UCHAR ucRet = 0;
    K_UCHAR *pcData = (K_UCHAR*)pvSrc_;

    KERNEL_ASSERT(pvSrc_);

    // 8-bit CRC, computed byte at a time
    while (usLen_--)
    {
        ucRet += *pcData++;
    }
    return ucRet;
}

//---------------------------------------------------------------------------
K_USHORT MemUtil::Checksum16( const void *pvSrc_, K_USHORT usLen_ )
{
    K_USHORT usRet = 0;
    K_UCHAR *pcData = (K_UCHAR*)pvSrc_;

    KERNEL_ASSERT(pvSrc_);

    // 16-bit CRC, computed byte at a time
    while (usLen_--)
    {
        usRet += *pcData++;
    }
    return usRet;
}

//---------------------------------------------------------------------------
// Basic string routines
K_USHORT MemUtil::StringLength( const char *szStr_ )
{
    K_UCHAR *pcData = (K_UCHAR*)szStr_;
    K_USHORT usLen = 0;

    KERNEL_ASSERT(szStr_);

    while (*pcData++)
    {
        usLen++;
    }
    return usLen;
}

//---------------------------------------------------------------------------
bool   MemUtil::CompareStrings( const char *szStr1_, const char *szStr2_ )
{
    char *szTmp1 = (char*) szStr1_;
    char *szTmp2 = (char*) szStr2_;

    KERNEL_ASSERT(szStr1_);
    KERNEL_ASSERT(szStr2_);

    while (*szTmp1 && *szTmp2)
    {
        if (*szTmp1++ != *szTmp2++)
        {
            return false;
        }
    }

    // Both terminate at the same length
    if (!(*szTmp1) && !(*szTmp2))
    {
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
void MemUtil::CopyMemory( void *pvDst_, const void *pvSrc_, K_USHORT usLen_ )
{
    char *szDst = (char*) pvDst_;
    char *szSrc = (char*) pvSrc_;

    KERNEL_ASSERT(pvDst_);
    KERNEL_ASSERT(pvSrc_);

    // Run through the strings verifying that each character matches
    // and the lengths are the same.
    while (usLen_--)
    {
        *szDst++ = *szSrc++;
    }
}

//---------------------------------------------------------------------------
void MemUtil::CopyString( char *szDst_, const char *szSrc_ )
{
    char *szDst = (char*) szDst_;
    char *szSrc = (char*) szSrc_;

    KERNEL_ASSERT(szDst_);
    KERNEL_ASSERT(szSrc_);

    // Run through the strings verifying that each character matches
    // and the lengths are the same.
    while (*szSrc) 
    {
        *szDst++ = *szSrc++;
    }
}

//---------------------------------------------------------------------------
K_SHORT MemUtil::StringSearch( const char *szBuffer_, const char *szPattern_ )
{
    char *szTmpPat = (char*)szPattern_;
    K_SHORT i16Idx = 0;
    K_SHORT i16Start;
    KERNEL_ASSERT( szBuffer_ );
    KERNEL_ASSERT( szPattern_ );

    // Run through the big buffer looking for a match of the pattern 
    while (szBuffer_[i16Idx])
    {
        // Reload the pattern
        i16Start = i16Idx;
        szTmpPat = (char*)szPattern_; 
        while (*szTmpPat && szBuffer_[i16Idx]) 
        {
            if (*szTmpPat != szBuffer_[i16Idx])
            {
                break;
            }
            szTmpPat++;
            i16Idx++;
        }
        // Made it to the end of the pattern, it's a match.
        if (*szTmpPat == '\0')
        {
            return i16Start;
        }
        i16Idx++;
    }

    return -1;
}

//---------------------------------------------------------------------------
bool MemUtil::CompareMemory( const void *pvMem1_, const void *pvMem2_, K_USHORT usLen_ )
{
    char *szTmp1 = (char*) pvMem1_;
    char *szTmp2 = (char*) pvMem2_;

    KERNEL_ASSERT(pvMem1_);
    KERNEL_ASSERT(pvMem2_);

    // Run through the strings verifying that each character matches
    // and the lengths are the same.
    while (usLen_--)
    {
        if (*szTmp1++ != *szTmp2++)
        {
            return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------
void MemUtil::SetMemory( void *pvDst_, K_UCHAR ucVal_, K_USHORT usLen_ )
{
    char *szDst = (char*)pvDst_;

    KERNEL_ASSERT(pvDst_);
    
    while (usLen_--)
    {
        *szDst++ = ucVal_;
    }
}



