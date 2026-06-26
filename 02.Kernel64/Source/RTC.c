#include "RTC.h"
#include "AssemblyUtility.h"

// Function to read the current time from the RTC
void kReadRTCTime(BYTE* pbHour, BYTE* pbMinute, BYTE* pbSecond) {
    
    BYTE bHour, bMinute, bSecond;

    kOutPortByte(RTC_COMSADDRESS, RTC_ADDRESS_SECONDS);
    bSecond = kInPortByte(RTC_COMSDATA);
    
    kOutPortByte(RTC_COMSADDRESS, RTC_ADDRESS_MINUTES);
    bMinute = kInPortByte(RTC_COMSDATA);
    
    kOutPortByte(RTC_COMSADDRESS, RTC_ADDRESS_HOURS);
    bHour = kInPortByte(RTC_COMSDATA);
    
    // Convert BCD to binary
    BCD_TO_BINARY(bSecond);
    BCD_TO_BINARY(bMinute);
    BCD_TO_BINARY(bHour);
    
    // Store the results in the provided pointers
    *pbHour = bHour;
    *pbMinute = bMinute;
    *pbSecond = bSecond;
}

// Function to read the current date from the RTC
void kReadRTCDate(WORD* pwYear, BYTE* pbMonth, BYTE* pbDayOfMonth, 
    BYTE* pbDayOfWeek) 
{
    BYTE bYear, bMonth, bDayOfMonth, bDayOfWeek;

    kOutPortByte(RTC_COMSADDRESS, RTC_ADDRESS_YEAR);
    bYear = kInPortByte(RTC_COMSDATA);

    kOutPortByte(RTC_COMSADDRESS, RTC_ADDRESS_MONTH);
    bMonth = kInPortByte(RTC_COMSDATA);
    
    kOutPortByte(RTC_COMSADDRESS, RTC_ADDRESS_DAYOFMONTH);
    bDayOfMonth = kInPortByte(RTC_COMSDATA);
    
    kOutPortByte(RTC_COMSADDRESS, RTC_ADDRESS_DAYOFWEEK);
    bDayOfWeek = kInPortByte(RTC_COMSDATA);
    
    // Convert BCD to binary
    BCD_TO_BINARY(bYear);
    BCD_TO_BINARY(bMonth);
    BCD_TO_BINARY(bDayOfMonth);
    BCD_TO_BINARY(bDayOfWeek);
    
    // Store the results in the provided pointers
    *pwYear = bYear;
    *pbMonth = bMonth;
    *pbDayOfMonth = bDayOfMonth;
    *pbDayOfWeek = bDayOfWeek;
}

// Function to convert the day of the week to a string representation
char* kConvertDayOfWeekToString(BYTE bDayOfWeek) {
    
    static char* vpcDayOfWeekString[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", 
        "Thursday", "Friday", "Saturday"
    };

    if (bDayOfWeek > 6) {
        return "Unknown";
    }

    return vpcDayOfWeekString[bDayOfWeek];
}
