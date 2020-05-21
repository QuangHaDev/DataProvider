////////////////////////////////////////////////////////////////////////
// JRegistry.CPP
//
// Author(s):  
//
// Purpose    :  Abstraction of Windows NT's registry functions.
//               
//----------------------------------------------------------------------
//
//
////////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "JRegistry.h"
//#include "SysExc.h"
//#include "Tracer.h"
#include <stdio.h>



static const unsigned int MY_SD_SIZE = 10000;
static const unsigned int MY_SID_SIZE = 1000;
static const unsigned int MY_ACL_SIZE = 5000;

static const unsigned int MY_MAX_STR_SIZE = 512;


JRegistry::JRegistry(HKEY hKey, LPCTSTR lpszSubKey ) 
    : my_base_key( hKey ), 
      my_sub_key(lpszSubKey)
{
}

JRegistry::JRegistry(LPCTSTR lpszSubKey ) 
    : my_base_key(HKEY_LOCAL_MACHINE), 
      my_sub_key(lpszSubKey)
{
}

JRegistry::~JRegistry()
{
    // Close the base key if we got it from a RegConnect call.
    if ((my_base_key != HKEY_LOCAL_MACHINE)
        && (my_base_key != HKEY_USERS)
        && (my_base_key != HKEY_CLASSES_ROOT)
        && (my_base_key != HKEY_CURRENT_USER))
    {
        RegCloseKey(my_base_key);
    }
}

void
JRegistry::set_base_key(HKEY hKey)
{
    my_base_key = hKey;
}

int
JRegistry::key_exists(LPCTSTR lpszSubSubKey)
{
    // local variables
    char sub_key[256];
    HKEY hReg;
    int return_value = FALSE;

    if (lpszSubSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszSubSubKey);
        else
            strcpy(sub_key, lpszSubSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);
    
    if (RegOpenKeyEx(my_base_key, sub_key, 0, KEY_READ, &hReg) == ERROR_SUCCESS)
    {
        return_value = TRUE;
        RegCloseKey(hReg);
    }

    return return_value;
}

int
JRegistry::value_exists(LPCTSTR lpszSubSubKey, 
                        LPCTSTR lpszValueName)
{
    int return_value = TRUE;
    char sub_key[256];
    HKEY hReg;

    if (lpszSubSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszSubSubKey);
        else
            strcpy(sub_key, lpszSubSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);
    
    if (RegOpenKeyEx(my_base_key, sub_key, 0, KEY_READ, &hReg) != ERROR_SUCCESS)
        return_value = FALSE;
    else
    {
        if (RegQueryValueEx(hReg, lpszValueName, NULL, NULL, NULL, NULL)
            != ERROR_SUCCESS)
            return_value = FALSE;

        RegCloseKey(hReg);
    }

    return return_value;
}

int
JRegistry::read_value(LPCTSTR lpszSubSubKey, 
                      LPCTSTR lpszValueName, 
                      LPBYTE  lpbData, 
                      LPDWORD lpcbData)
{
    int return_value = TRUE;
    char sub_key[256];
    HKEY hReg;

    if (lpszSubSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszSubSubKey);
        else
            strcpy(sub_key, lpszSubSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);
    
    if (RegOpenKeyEx(my_base_key, sub_key, 0, KEY_READ, &hReg) != ERROR_SUCCESS)
    {
//        Tracer::Trace(COMPONENTS, 
//                    ERROR_LEVEL,
//                      "Registry Key missing - %s", sub_key);

        return_value = FALSE;
    }
    else
    {
        if (RegQueryValueEx(hReg, lpszValueName, NULL, NULL, lpbData, lpcbData)
            != ERROR_SUCCESS)
        {
            if ( stricmp(lpszValueName, "IsTestSystem") != 0 )
            {
//               Tracer::Trace(COMPONENTS,
//                              ERROR_LEVEL,
//                             "Registry value %s missing - %s",
//                              lpszValueName,
//                              sub_key);
            }

            return_value = FALSE;
        }

        RegCloseKey(hReg);
    }

    return return_value;
}

BOOL
JRegistry::read(
                LPCTSTR lpszSubSubKey, 
                LPCTSTR lpszValueName, 
                DWORD * pDWord)
{
    // local variables
    DWORD cbData = sizeof(DWORD);
	BOOL bReturnValue = FALSE;
    if ( !read_value(lpszSubSubKey, 
                        lpszValueName, 
                        (LPBYTE) pDWord, 
                        &cbData) )
		bReturnValue = TRUE;//        throw SysExc("Unable to read the registry item", GetLastError());
	return bReturnValue;
}

void
JRegistry::read(
                LPCTSTR lpszSubSubKey, 
                LPCTSTR lpszValueName, 
                char * pString,
                DWORD * pcbData )
{
    if ( !read_value(lpszSubSubKey, 
                        lpszValueName, 
                        (LPBYTE) pString, 
                        pcbData) )
//;       throw SysExc("Unable to read the registry item", GetLastError());

    pString[*pcbData] = '\0';
}

int
JRegistry::write_value(
                        LPCTSTR      lpszSubSubKey,
                        LPCTSTR      lpszValueName, 
                        DWORD        fdwType, 
                        const BYTE * lpbData, 
                        DWORD        cbData)
{
    return add_value(lpszSubSubKey, lpszValueName, fdwType, lpbData, cbData);
}

int 
JRegistry::write(
                    LPCTSTR lpszSubSubKey,
                    LPCTSTR lpszValueName,
                    DWORD dwVal)
{
    // local variables
    int result;

    result = add_value(
                        lpszSubSubKey,
                        lpszValueName,
                        REG_DWORD,
                        (const BYTE *) &dwVal,
                        sizeof(DWORD));

    return (result);
}

int 
JRegistry::write(
                  LPCTSTR lpszSubSubKey,
                  LPCTSTR lpszValueName,
                  const char * str)
{
    // local variables
    int result;

    result = add_value(
                        lpszSubSubKey,
                        lpszValueName,
                        REG_SZ,
                        (const BYTE *) str,
                        (strlen(str) + 1));

    return (result);
}

int
JRegistry::add_value(
                     LPCTSTR      lpszSubSubKey,
                     LPCTSTR      lpszValueName, 
                     DWORD        fdwType, 
                     const BYTE * lpbData, 
                     DWORD        cbData)
{
    char sub_key[256];
    HKEY hReg;
    int return_value = FALSE;

    if (lpszSubSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszSubSubKey);
        else
            strcpy(sub_key, lpszSubSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);
    
    if (RegOpenKeyEx(my_base_key, sub_key, 0, KEY_WRITE, &hReg) == ERROR_SUCCESS)
    {
        return_value = (RegSetValueEx(hReg, lpszValueName, 0, fdwType, lpbData, cbData) == ERROR_SUCCESS);
        RegCloseKey(hReg);
    }

    return return_value;
}

int
JRegistry::remove_value(LPCTSTR lpszSubSubKey,
                        LPCTSTR lpszValueName)
{
    char sub_key[256];
    HKEY hReg;
    int return_value = FALSE;

    if (lpszSubSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszSubSubKey);
        else
            strcpy(sub_key, lpszSubSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);
    
    if (RegOpenKeyEx(my_base_key, sub_key, 0, KEY_WRITE, &hReg) == ERROR_SUCCESS)
    {
        return_value = (RegDeleteValue(hReg, lpszValueName) == ERROR_SUCCESS);
        RegCloseKey(hReg);
    }

    return return_value;
}

int
JRegistry::add_key(LPCTSTR lpszNewSubKey)
{
    int  return_value = FALSE;
    char  sub_key[256];
    HKEY  hReg;
    DWORD disposition;
    LONG  result;

    if (lpszNewSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszNewSubKey);
        else
            strcpy(sub_key, lpszNewSubKey);
    }
    else
        return FALSE;

    if ((result = RegCreateKeyEx( my_base_key, 
                                  sub_key, 
                                  0, 
                                  "REG_SZ", 
                                  REG_OPTION_NON_VOLATILE, 
                                  KEY_ALL_ACCESS, 
                                  NULL, 
                                  &hReg, 
                                  &disposition)) == ERROR_SUCCESS)
    {
        RegCloseKey(hReg);
        return_value = TRUE;
    }

    return return_value;
}

int
JRegistry::remove_key(LPCTSTR lpszDelSubKey)
{
    // local variables
    char                sub_key[256];
    unsigned int        index;
    char                enumerated_sub_key[255];
    char                temp_sub_key[255];
    unsigned long int   sub_key_len;
    int                 return_value;

    // initialisation
    index = 0;
    sub_key_len = 255;
    enumerated_sub_key[0] = '\0';

    if (lpszDelSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszDelSubKey);
        else
            strcpy(sub_key, lpszDelSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);

    while (enum_keys(lpszDelSubKey,
                     index,     // we will always look at index = 0
                                // when deleteing all the keys
                     enumerated_sub_key, 
                     &sub_key_len))
    {
        sprintf(temp_sub_key, 
                "%s\\%s", 
                lpszDelSubKey,
                enumerated_sub_key);

        if (!remove_key(temp_sub_key))
            return FALSE;

        // re-init variables for enum_keys
        sub_key_len = 255;
        enumerated_sub_key[0] = '\0';
    }

    // try deleting this key "as is"
    return_value = (RegDeleteKey(my_base_key, sub_key) == ERROR_SUCCESS);
    if (!return_value)
    {
        /*
         * we probably failed because of security permissions on the
         * keys, lets try to re-create the key with all access
         * and then give it another go
         */
         // DOESN'T WORK YET
//        if (add_key(lpszDelSubKey))
//        {
//            return_value = (RegDeleteKey(my_base_key, sub_key) == ERROR_SUCCESS);
//        }
    }

    return (return_value);
}

int
JRegistry::select_computer(LPCTSTR lpszComputerName)
{
    // local variables
    HKEY    new_base_key;
    TCHAR   computername[MAX_COMPUTERNAME_LENGTH+2+1];
    TCHAR * pComputerName;

    // initialisation
    pComputerName = NULL;
    strcpy(computername, "");

    if (lpszComputerName)
    {
        if (*lpszComputerName != '\\')
            strcat(computername, "\\\\");

        strcat(computername, lpszComputerName);
        pComputerName = computername;
    }

    DWORD status;

    // If it is a valid key for remote computer, leave it alone.
    if ((my_base_key != HKEY_LOCAL_MACHINE) 
        && (my_base_key != HKEY_USERS))
    {
        // If it is a "remote key", close it
        if ((my_base_key != HKEY_CLASSES_ROOT)
            && (my_base_key != HKEY_CURRENT_USER))
        {
            status = RegCloseKey(my_base_key);
        }

        // set the default
        my_base_key = HKEY_LOCAL_MACHINE;
    }

    status = RegConnectRegistry((LPTSTR) pComputerName, 
                                      my_base_key, 
                                      &new_base_key);
    if (status == ERROR_SUCCESS)
    {
        my_base_key = new_base_key;
        return TRUE;
    }
    else
        return FALSE;
}

int
JRegistry::enum_keys(LPCTSTR             lpszSubSubKey,
                     unsigned int        index, 
                     char              * sub_key_name, 
                     unsigned long int * sub_key_len)
{
    // First call index = 0
    // Caller SHOULD increment index for each call
    // When fails (RegEnumKeyEx returns ERROR_NO_MORE_ITEMS) then finished

    FILETIME file_time;
    HKEY     hReg;
    int      return_value = FALSE;
    char     sub_key[256];
    long     result;

    if (lpszSubSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszSubSubKey);
        else
            strcpy(sub_key, lpszSubSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);

    if (RegOpenKeyEx(my_base_key, sub_key, 0, KEY_READ, &hReg) != ERROR_SUCCESS)
        return FALSE;

    if ((result = RegEnumKeyEx(hReg, 
                               index, 
                               sub_key_name, 
                               sub_key_len, 
                               NULL, 
                               NULL, 
                               NULL, 
                               &file_time)) != ERROR_SUCCESS)
    {
        return_value = FALSE;
    }
    else
        return_value = TRUE;

    RegCloseKey(hReg);
    return return_value;
}

int JRegistry::enum_values(LPCTSTR lpszSubSubKey,
                            DWORD   index, 
                            LPTSTR  value_name, 
                            LPDWORD value_name_len,
                            LPDWORD value_type,
                            LPBYTE  value_data,
                            LPDWORD value_data_len)
{
    // First call index = 0
    // Caller SHOULD increment index for each call
    // When fails (RegEnumValueEx returns ERROR_NO_MORE_ITEMS) then finished

    HKEY     hReg;
    int     return_value = FALSE;
    char     sub_key[256];
    long     result;

    if (lpszSubSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszSubSubKey);
        else
            strcpy(sub_key, lpszSubSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);

    if (RegOpenKeyEx(my_base_key, sub_key, 0, KEY_READ, &hReg) != ERROR_SUCCESS)
        return FALSE;

    if ((result = RegEnumValue(hReg,                               // handle of key to query 
                               index,                              // index of value to query 
                               value_name,                         // address of buffer for value string 
                               value_name_len,                     // address for size of value buffer 
                               NULL,                               // reserved 
                               value_type,                         // address of buffer for type code 
                               value_data,                         // address of buffer for value data 
                               value_data_len)) != ERROR_SUCCESS)  // address for size of data buffer 
    {
        return_value = FALSE;
    }
    else
        return_value = TRUE;

    RegCloseKey(hReg);
    return return_value;
}

int JRegistry::get_value_type(LPCTSTR lpszSubSubKey,
                               LPCTSTR lpszValueName,
                               LPDWORD lpdwType)
{
    int return_value = TRUE;
    char sub_key[256];
    HKEY hReg;

    if (lpszSubSubKey != NULL)
    {
        if (my_sub_key[0] != '\0')
            sprintf(sub_key, "%s\\%s", my_sub_key, lpszSubSubKey);
        else
            strcpy(sub_key, lpszSubSubKey);
    }
    else
        strcpy(sub_key, my_sub_key);
    
    if (RegOpenKeyEx(my_base_key, sub_key, 0, KEY_READ, &hReg) != ERROR_SUCCESS)
        return_value = FALSE;
    else
    {
        if (RegQueryValueEx(hReg, lpszValueName, NULL, lpdwType, NULL, NULL) != ERROR_SUCCESS)
            return_value = FALSE;

        RegCloseKey(hReg);
    }

    return return_value;
}



/*
 * Reads text from a REG_MULTI_SZ registry value
 * and returns it in the format needed by a 
 * multi-line edit box.  (ie Does the conversion
 * from \r\n in the edit box to \0 as the delimiter
 * between lines in the edit box and registry 
 * respectfully. )
 */
int
JRegistry::read_multi_value(LPCTSTR lpszSubSubKey, 
                            LPCTSTR lpszValueName, 
                            LPBYTE  lpbData, 
                            LPDWORD lpcbData)
{
    if (read_value(lpszSubSubKey, 
                   lpszValueName, 
                   lpbData, 
                   lpcbData))
    {
        // parse multiple strings until NULL NULL reached
        char edit_box_string[512] = "\0";
        char * reg_string = (char *)lpbData;

        while (*reg_string != '\0')
        {
            /*
             * Append this registry string onto the end of
             * the edit box string.
             */
            strcat(edit_box_string, reg_string);

            /*
             * The next registry string is at then end of
             * this one, after the NULL (\0).
             */
            reg_string += strlen(reg_string) + 1;

            /*
             * If there is another string (ie the first
             * character is not the second NULL (\0) to
             * indicate the end of the strings) then
             * append the carriage return and line feed
             * pair (\r\n) so it can be used in an edit 
             * box.
             */
            if (*reg_string != '\0')
                strcat(edit_box_string, "\r\n");
        }

        /*
         * Copy the edit_box_string into the string
         * passed to us as a parameter.
         */
        strcpy((char *)lpbData, edit_box_string);

        // Set the length parameter.
        *lpcbData = (unsigned long)strlen((char *)lpbData);
        
        // Success
        return TRUE;
    }
    else
    {
        // Could not read from the registry.
        return FALSE;
    }
}

