////////////////////////////////////////////////////////////////////////
// JRegistry.H
//
// Author(s):  
//
// Purpose    :  Definition for the JRegistry class which is an abstraction 
//               of the Windows NT's registry functions.
//----------------------------------------------------------------------
//
//
////////////////////////////////////////////////////////////////////////

/*
Specify the highest level common registry key (the equivalent of a directory/folder)
in the constructor.  Then when you use the methods, specify the path between that
key and the key you want.

eg.  HKEY_LOCAL_MACHINE
     +-SOFTWARE
       +-MyApplication     (has Value1 and Value2 as values (equivalent of files))
         +-Parameters      (has Value3 and Value4 as values)
         +-AnotherKey      (has Value5)
           +-AnotherSubKey (has Value6)

To access Value1 - Value5 create a JRegistry by: 
    JRegistry my_reg(HKEY_LOCAL_MACHINE, "SOFTWARE\\MyApplication");

Then to read the values use:
    status = my_reg.read_value(NULL, Value1, data, &data_length) ;
    status = my_reg.read_value(NULL, Value2, data, &data_length) ;
    status = my_reg.read_value("Parameters", Value3, data, &data_length) ;
    status = my_reg.read_value("Parameters", Value4, data, &data_length) ;
    status = my_reg.read_value("AnotherKey", Value5, data, &data_length) ;
    status = my_reg.read_value("AnotherKey\\AnotherSubKey", Value6, data, &data_length) ;

Thus specifing the base key in the constructor saves you from having to use:
    JRegistry my_reg(HKEY_LOCAL_MACHINE, NULL);
    status = my_reg.read_value("SOFTWARE\\MyApplication", 
								Value1, data, &data_length) ;
    status = my_reg.read_value("SOFTWARE\\MyApplication", 
								Value2, data, &data_length) ;
    status = my_reg.read_value("SOFTWARE\\MyApplication\\Parameters", 
								Value3, data, &data_length) ;
    status = my_reg.read_value("SOFTWARE\\MyApplication\\Parameters", 
								Value4, data, &data_length) ;
    status = my_reg.read_value("SOFTWARE\\MyApplication\\AnotherKey", 
								Value5, data, &data_length) ;
    status = my_reg.read_value("SOFTWARE\\MyApplication\\AnotherKey\\AnotherSubKey", 
								Value6, data, &data_length) ;
*/

#ifndef _JReg_H_
#define _JReg_H_

#include "StdAfx.h"

class JRegistry
{
// Public Services
public:
    // Constructors
    JRegistry(LPCTSTR lpszSubKey = "\0");
    JRegistry(HKEY hKey, LPCTSTR lpszSubKey = "\0");

    // Destructor
    ~JRegistry();

    // Set the base key
    void set_base_key(HKEY hKey);

    // Returns TRUE if the key "lpszSubKey + lpszSubSubKey" exists
    int key_exists(LPCTSTR lpszSubSubKey);


	// Returns TRUE if the value "lpszSubKey + lpszSubSubKey + lpszValueName" exists
    int value_exists(	LPCTSTR lpszSubSubKey,
						LPCTSTR lpszValueName);

    /*
     * Reads lpcbData bytes into lpbData from "lpszSubKey + lpszSubSubKey + lpszValueName"
     * The number of bytes acutally read is returned in lpcbData.
     * Return TRUE if successful, FALSE if not.
     * NOTE: If the type of value being read is a string (REG_SZ or REG_MULTI_SZ)
     *       then you MUST append the NULL YOURSELF!  ie lpbData[lpcbData] = '\0';
     */
    int read_value(	LPCTSTR lpszSubSubKey,		// address of key to read
                    LPCTSTR lpszValueName,		// address of value to read
                    LPBYTE  lpbData,			// the type of value 
                    LPDWORD lpcbData);			// return value

    // read a value assuming it is of type DWORD
    BOOL read(	LPCTSTR lpszSubSubKey,			// address of key to read
				LPCTSTR lpszValueName,			// address of value to read 
				DWORD * pDWord);				// return value

    // read a value assuming it is of type REG_SZ or REG_MULTI_SZ
	void read(	LPCTSTR lpszSubSubKey,			// address of key to read
				LPCTSTR lpszValueName,			// address of value to read
				char * pString,					// return value       
				DWORD * cbData );				// return the length of return value


    /* 
     * Writes cbData bytes of lpbData to "lpszSubKey + lpszSubSubKey + lpszValueName".
     * Automatically adds the value to the registry if it does not exist.  (Does not
     * create keys if don't exist)
     *
     * fdwType can be:
     *   REG_BINARY
     *   REG_DWORD
     *   REG_EXPAND_SZ
     *   REG_MULTI_SZ
     *   REG_SZ
     */
	// return value : TRUE if success, FALSE if not success
    int write_value(	LPCTSTR      lpszSubSubKey,  // address of key to write
						LPCTSTR      lpszValueName,	 // address of value name to write
						DWORD        fdwType,		 // value type
						const BYTE * lpbData,		 // value 
						DWORD        cbData);		 // length of value


    // write a value of type DWORD
	// return value : TRUE if success, FALSE if not success
    int write(	LPCTSTR lpszSubSubKey,  // address of key to write
				LPCTSTR lpszValueName,	// address of value name to write
				DWORD dwVal);			// value 

    // write a value of type REG_SZ
	// return value : TRUE if success, FALSE if not success
    int write(	LPCTSTR lpszSubSubKey,	// address of key to write
				LPCTSTR lpszValueName,	// address of value name to write 
				const char * str);		// value

    // Same as write_value
	// return value : TRUE if success, FALSE if not success
    int add_value(	LPCTSTR      lpszSubSubKey, // address of key to write
					LPCTSTR      lpszValueName, // address of value name to write
					DWORD        fdwType,		// value type
					const BYTE * lpbData,		// value 
					DWORD        cbData);		// the length of value

    // Removes a value from the registry
	// return value : TRUE if success, FALSE if not success
    int remove_value(	LPCTSTR lpszSubSubKey,  // address of key to remove
						LPCTSTR lpszValueName); // address of value name to remove

    // Add the key "lpszSubKey + lpszNewKey" to the registry.
	// return value : TRUE if success, FALSE if not success
    int add_key(LPCTSTR lpszNewSubKey); 

    /*
     * Removes the key "lpszSubKey + lpszDelSubKey" and ALL ITS SUB KEYS
     * from the registry.
     */
	// return value : TRUE if success, FALSE if not success
    int remove_key(LPCTSTR lpszDelSubKey);

  
    /*
     * Selects the computer whose registry to use.
     * After this call, ALL registry access through
     * this object will be directory to the selected 
     * computer.
     */
	// return value : TRUE if success, FALSE if not success
    int select_computer(LPCTSTR lpszComputerName);

    /*
     * Enumerates keys.  ie can tell you all the sub keys for 
     * particular key.
     *
     * For the first call, set index to 0.
     * YOU should then increment the index for each subsequent call.
     * eg.
     *    unsigned int index = 0;
     *    while (enum_keys(key_name, index++, sub_key_name, sub_key_len)) {}
     *
     * Should not use other methods that might effect the key being enumerated.
     *
     * sub_key_name gets the sub key, not the "full path".
     */
	// return value : TRUE if success, FALSE if not success
    int enum_keys(	LPCTSTR             lpszSubSubKey,
					unsigned int        index, 
					char              * sub_key_name, 
					unsigned long int * sub_key_len);
    
    /*
     *
     */
    int enum_values(	LPCTSTR lpszSubSubKey,
						DWORD   index, 
						LPTSTR  value_name, 
						LPDWORD value_name_len,
						LPDWORD value_type,
						LPBYTE  value_data,
						LPDWORD value_data_len);

    /*
     * Find out the type of value (REG_SZ etc).
     */
    int get_value_type(	LPCTSTR lpszSubSubKey,
                        LPCTSTR lpszValueName,
                        LPDWORD lpdwType);


    /*
     * Reads text from a REG_MULTI_SZ registry value
     * and returns it in the format needed by a 
     * multi-line edit box.  (ie Does the conversion
     * from \r\n in the edit box to \0 as the delimiter
     * between lines in the edit box and registry 
     * respectfully. )
     */
    int read_multi_value(	LPCTSTR lpszSubSubKey, 
							LPCTSTR lpszValueName, 
							LPBYTE  lpbData, 
							LPDWORD lpcbData);


// Attributes
private:
    HKEY    my_base_key;    
    LPCTSTR my_sub_key;
};

#endif
