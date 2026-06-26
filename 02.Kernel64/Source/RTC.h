#ifndef __RTC_H__
#define __RTC_H__

#include "Types.h"

// IO Ports
#define RTC_COMSADDRESS 0x70
#define RTC_COMSDATA 0x71

// CMOS Memory Addresses
#define RTC_ADDRESS_SECONDS 0x00
#define RTC_ADDRESS_MINUTES 0x02
#define RTC_ADDRESS_HOURS 0x04
#define RTC_ADDRESS_DAYOFWEEK 0x06
#define RTC_ADDRESS_DAYOFMONTH 0x07
#define RTC_ADDRESS_MONTH 0x08
#define RTC_ADDRESS_YEAR 0x09

// Macro to convert BCD to binary
#define BCD_TO_BINARY(val) ((val) = ((val) & 0x0F) + ((val) >> 4) * 10)

// Functions
void kReadRTCTime(BYTE* pbHour, BYTE* pbMinute, BYTE* pbSecond);
void kReadRTCDate(WORD* pwYear, BYTE* pbMonth, BYTE* pbDayOfMonth, 
    BYTE* pbDayOfWeek);
char* kConvertDayOfWeekToString(BYTE bDayOfWeek);

#endif /*__RTC_H__*/