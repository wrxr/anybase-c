#include "anybase.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char* dtob(const int64_t iDecimal) {
    char* sBinary = malloc(65 * sizeof(char));
    if (sBinary == NULL) return NULL;
    sBinary[64] = '\0';

    for (int i = 63; i >= 0; i--)
        sBinary[63 - i] = (iDecimal & ((int64_t)1 << i)) ? '1' : '0';

    int iStartPos = 0;
    if (iDecimal == 0)
        iStartPos = 63;
    else {
        char sFirstChar = (iDecimal < 0) ? '1' : '1'; // NOLINT(*-branch-clone)
        while (iStartPos < 63 && sBinary[iStartPos] != sFirstChar)
            iStartPos++;
    }

    int iCompactLength = 64 - iStartPos;
    char* sCompact = malloc((iCompactLength + 1) * sizeof(char));
    if (sCompact == NULL) {
        free(sBinary);
        return NULL;
    }
    strncpy(sCompact, sBinary + iStartPos, iCompactLength);
    sCompact[iCompactLength] = '\0';
    free(sBinary);
    return sCompact;
}

int64_t btod(const char* sBinary) {
    int64_t iDecimal = 0;
    size_t iLength = strlen(sBinary);

    bool isNegative = (iLength >= 32 && sBinary[0] == '1');

    for (size_t i = 0; i < iLength; i++) {
        if (sBinary[i] == '1')
            iDecimal |= (1ULL << (iLength - 1 - i)); // NOLINT(*-narrowing-conversions)
        else if (sBinary[i] != '0') return 0;
    }

    if (isNegative)
        iDecimal -= (1ULL << iLength); // NOLINT(*-narrowing-conversions)

    return iDecimal;
}

char * dtoh(const int64_t iDecimal) {
    if (iDecimal == 0) {
        char* sResult = malloc(2 * sizeof(char));
        if (sResult == NULL) return NULL;
        sResult[0] = '0';
        sResult[1] = '\0';
        return sResult;
    }

    uint64_t uDecimal = (uint64_t)iDecimal;
    int iLength = 0;
    uint64_t iTemp = uDecimal;
    while (iTemp != 0) {
        iTemp >>= 4;
        iLength++;
    }

    char *sHex = malloc(iLength + 1);
    if (sHex == NULL) return NULL;
    sHex[iLength] = '\0';
    for (int i = 0; i < iLength; i++) {
        uint8_t iNibble = (uDecimal >> (i * 4)) & 0x0F;
        sHex[iLength - 1 - i] = (iNibble < 10) ? ('0' + iNibble) : ('A' + iNibble - 10); // NOLINT(*-narrowing-conversions)
    }

    return sHex;
}

int64_t htod(const char * sHex) {
    if (sHex == NULL) return 0;
    while (isspace((unsigned char)*sHex)) sHex++;
    if (*sHex == '\0') return 0;

    int iSign = 1;
    if (*sHex == '-') {
        iSign = -1;
        sHex++;
    } else if (*sHex == '+') sHex++;

    const char *pHex = sHex;
    while (*pHex) {
        if (!isxdigit((unsigned char)*pHex)) return 0;
        pHex++;
    }

    size_t iLength = pHex - sHex;
    if (iLength > 16) return 0;

    uint64_t iDecimal = 0;
    for (; *sHex; sHex++) {
        if (iDecimal > UINT64_MAX >> 4)
            return 0;
        char c = tolower((unsigned char)*sHex); // NOLINT(*-narrowing-conversions)
        uint8_t iDigit = (c >= 'a') ? (10 + c - 'a') : (c - '0');

        iDecimal = (iDecimal << 4) + iDigit;
    }

    if (iSign == 1) {
        if (iDecimal > INT64_MAX) return 0;
    } else {
        if (iDecimal > (uint64_t)INT64_MAX + 1)
            return 0;
    }

    return iSign * (int64_t)iDecimal;
}

char * dtoo(const int64_t iDecimal) {
    if (iDecimal == 0) {
        char* sOctal = malloc(2 * sizeof(char));
        if (sOctal == NULL) return NULL;
        sOctal[0] = '0';
        sOctal[1] = '\0';
        return sOctal;
    }

    bool bIsNegative = iDecimal < 0;
    uint64_t uAbsDecimal = bIsNegative ? (uint64_t)(-iDecimal) : (uint64_t)iDecimal;

    char sBuffer[24] = {0};
    int i = 0;
    if (uAbsDecimal == 0) {
        sBuffer[i++] = '0';
    } else {
        while (uAbsDecimal > 0) {
            sBuffer[i++] = (uAbsDecimal % 8) + '0'; // NOLINT(*-narrowing-conversions)
            uAbsDecimal /= 8;
        }
    }

    if (bIsNegative) sBuffer[i++] = '-';

    for (int j = 0; j < i / 2; j++) {
        char sTemp = sBuffer[j];
        sBuffer[j] = sBuffer[i - j - 1];
        sBuffer[i - j - 1] = sTemp;
    }

    char* sOctal = malloc((i + 1) * sizeof(char));
    if (sOctal == NULL) return NULL;
    strcpy(sOctal, sBuffer);
    return sOctal;
}

int64_t otod(const char * sOctal) {
    if (sOctal == NULL || *sOctal == '\0') return 0;
    while (isspace((unsigned char)*sOctal)) sOctal++;

    bool isNegative = false;
    if (*sOctal == '-') {
        isNegative = true;
        sOctal++;
    } else if (*sOctal == '+') sOctal++;

    const char* pOctal = sOctal;
    while (*pOctal != '\0') {
        if (*pOctal < '0' || *pOctal > '7') return 0;
        pOctal++;
    }

    uint64_t iDecimal = 0;
    while (*sOctal != '\0') {
        iDecimal = iDecimal * 8 + (*sOctal - '0');
        if (iDecimal > INT64_MAX)
            return isNegative ? INT64_MIN : INT64_MAX; // ЧТО ЭТО НАХУЙ КОГДА Я ЭТО ДОБАВЛЯЛ БЛЯТь 
        sOctal++;
    }

    if (isNegative) {
        if (iDecimal > (uint64_t)INT64_MAX + 1) return INT64_MIN;
        return -(int64_t)iDecimal;
    } else {
        if (iDecimal > INT64_MAX) return INT64_MAX;
        return (int64_t)iDecimal;
    }
}
