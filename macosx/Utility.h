//
//  Utility.h
//  10kx driver
//
//  Created by ITzTravelInTime on 27/09/22.
//  Copyright © 2022 kX Project. All rights reserved.
//

#ifndef Utility_h
#define Utility_h

#include "defs.h"
#include <string.h>

static void _kx_strcpy(char* destination, const char* source, dword len)
{
    
    // return if no memory is allocated to the destination
    if (destination == (char*)0 || len == 0) {
        return;
    }
    
    while (*source && len--)
    {
        *destination++ = *source++;
    }
 
    // null terminate destination string
    *destination = '\0';
    
    return;
}

//just to make syntax a little bit better
static inline bool inRange(const long n, const long min, const long max){
    return (n <= max && n >= min);
}


//just for our boot arg purposes, likely libriries provvide a better option but i am not a library expert - ITzTravelInTime
static dword stringToNumber(const char *str){
    
    //ddebug("\nstringToNumber_dummy Interpreting: %s\n", str);
    
    //0xFFFFFFFF is returned in case of error
    dword ret = 0xFFFFFFFF;
    
    
    if (str){
        
        //if the string starts with 0x it's interpreted like a an hex value, otherwise is interpreted as a base 10 value
        static const byte hex_mul = 16, dec_mul = 10;
        const byte mul = ((str[0] == '0') && (str[1] == 'x')) ? hex_mul : dec_mul;
        //ddebug("    mul is %u\n", mul);
        
        for (dword i = ((mul == hex_mul) ? 2 : 0); i < strlen(str) ; i++){
            char c = str[i];
            
            //ddebug("    iteration %lu value %c\n", i, c);
            
            ret *= mul;
            
            if (inRange(c, '0', '9')){
                ret += (c - '0');
                //ddebug("        fits n %u\n", ret);
                
            }else if (mul == hex_mul){
                
                //this implementation is case insensitive, but it's not the best for performance
                if (inRange(c, 'a', 'f')) {
                    ret += (c - 'a' + 10);
                    //ddebug("        fits h %u\n", ret);
                }else if (inRange(c, 'A', 'F')) {
                    ret += (c - 'A' + 10);
                    //ddebug("        fits H %u\n", ret);
                }else{
                    //ddebug("        abort 1!\n");
                    return 0;
                }
                    
            }else{
                //ddebug("        abort 2!\n");
                return 0;
            }
        }
    }
    
    //ddebug("stringToNumber_dummy end: %u\n\n", ret);
    
    return ret;
        
}

#endif /* Utility_h */
