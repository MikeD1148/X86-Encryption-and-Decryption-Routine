// The text encryption program in C++ and ASM with a simple example of encryption method.
// This version uses the CDECL calling convention
// Edited by Michael Watling 30037932

char EKey = 'a';               

#define StudentName "Michael Watling" 

#define MAXCHARS 6             
                               
#include <string>              // for strings
#include <fstream>             // file I/O
#include <iostream>            // for cin >> and cout <<-+
#include <iomanip>             // for fancy output
#include "TimeUtils.h"         // for GetTime, GetDate, etc.
                               
#define dollarchar '$'         // string terminator
                               
char OChars[MAXCHARS] = { 'a','n','d','y',' ' },          //Original character string 
EChars[MAXCHARS],           // Encrypted character string
DChars[MAXCHARS];           // Decrypted character string

//----------------------------- C++ Functions ----------------------------------------------------------

void get_char(char& a_character)
{
  a_character = (char)_getwche();
  if (a_character == '\r' || a_character == '\n')  // allow the enter key to work as the terminating character too
  {
    a_character = dollarchar;
  }
}

void get_original_chars(int& length)
{
  char next_char = ' ';
  length = 0;
  get_char(next_char);

  while ((length < MAXCHARS) && (next_char != dollarchar))
  {
    OChars[length++] = next_char;
    get_char(next_char); 
  }
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars (int length, char EKey)
{
  char temp_char;                            // Character temporary store

  for (int i = 0; i < length; ++i)           // Encrypt characters one at a time
  {
    temp_char = OChars[i];                   // Get the next char from Original Chars array
                                             
    __asm
    {                                        
     
      push    eax                               // Saving the caller registers as the data inside is important for future functions and so we can freely use the registers in the encrypt_6 function
      push    ecx                                
      push    edx                               
                                               
      or      temp_char,   00100000b            // Making uppercase characters lowercase
      
      lea     eax,         byte ptr[EKey]       // Retrieving the ekey address from memory to save a 32bit address to the stack for use in the encrypt_6 function
      push    eax                               // Pushing the ekey address parameter onto the stack so we can access that data in the encrypt_6 function
      push    byte ptr[temp_char]               // Pushing the character parameter onto the stack so we can access that data in the encrypt_6 function                

      call    encrypt_6                         // Calling the encrypt_6 function to encrypt the character
                                               
      add     esp,         8                    // Scrubbing the stack to use again for a different function
                                               
      mov     temp_char,   dl                   // Copying the encrypted character into the variable to be added to the encrypted characters array so the register can be reused
                                                
      pop     edx                               // Restoring the caller registers as the data inside is important for future functions and the function encrypt_6 has been returned
      pop     ecx                               
      pop     eax                               
    }                                          
                                               
    EChars[i] = temp_char;                      // Stores the encrypted character in the Encrypted Chars array
  }
  
  return;
  
  

  // Inputs:
  //register EAX = 32-bit address of Ekey,
  //register ECX = the character to be encrypted (in the low 8-bit field, CL).

  // Output: register EDX = the encrypted value of the source character (in the low 8-bit field, DL).
  __asm
  {
                                                        // Setting up the subroutine stackframe
   encrypt_6: push    ebp                               // Save call site's stack frame
              mov     ebp,             esp              // Create a new stack frame base for the function encrypt_6
              
              mov     ecx,             [ebp + 08h]      // Retrieving the character from the stack to be encrypted 
              mov     eax,             [ebp + 0Ch]      // Retrieving the address of the ekey from the stack to encrypt the character
   
              ror     byte ptr[eax],   1                // Shifting the value of the ekey in memory, the ekey is not restored to its original value for every character encryption so any alteration to the ekey is carried over to the next character so if the original characters were the same the encrypted characters would still all be different (salting)                                                    
              ror     byte ptr[eax],   1                // Alters and salts the ekey                                                                                              
              ror     byte ptr[eax],   1                // Alters and salts the ekey                                                                                                
              ror     byte ptr[eax],   1                // Alters and salts the ekey                                                                                                
              ror     byte ptr[eax],   1                // Alters and salts the ekey                                                                                                
              push    ecx                               // Saving the character to the stack so it can be used by another register
              not     byte ptr[eax]                     // Altering and salting the value of the ekey so it can be applied to the character to encrypt it
   
              movzx   edx,             byte ptr[eax]    // Clearing the edx register and copying the ekey value into it
              pop     eax                               // Copying the character into the eax register to be encrypted by the ekey
              xor     eax,             edx              // The ekey encrypts the character
              
              ror     al,              1                // Shifting the character value to add encryption
              ror     al,              1                // Shifting the character value to add encryption 
              
              not     al                                // Further encrypting the character value
              add     eax,             0x20             // Adding 20 hex to the encrypted character to finish encrypting the character
              mov     edx,             eax              // Copy the completely encrypted character into the edx register to return to the call site                              
                                                        // Closing the stack frame 
              mov     esp,             ebp              // Ensure stack pointer is pointing at the base pointer
              pop     ebp                               // Restore call site's base pointer
              
              ret                                       // Return to call site as the encrypt_6 function has finished encrypting this character
  }

  //--- End of Assembly code 
}
//*** end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------





//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------
//
void decrypt_chars (int length, char EKey)
{
    char temp_char;                              // Character temporary store

    for (int i = 0; i < length; ++i)             // Decrypt characters one at a time
    {
        temp_char = EChars[i];                   // Get the next char from Encrypted Chars array

        __asm
        {

            push    eax                               // Saving the caller registers as the data inside is important for future functions and so we can freely use the registers in the decrypt_6 function
            push    ecx
            push    edx


            lea     eax,         byte ptr[EKey]       // Retrieving the ekey address from memory to save a 32bit address to the stack for use in the decrypt_6 function
            push    eax                               // Pushing the ekey address parameter onto the stack so we can access that data in the decrypt_6 function
            push    byte ptr[temp_char]               // Pushing the character parameter onto the stack so we can access that data in the decrypt_6 function                

            call    decrypt_6                         // Calling the decrypt_6 function to encrypt the character

            add     esp,         8                    // Scrubbing the stack to use again for a different function

            mov     temp_char,   dl                   // Copying the decrypted character into the variable to be added to the decrypted characters array so the register can be reused

            pop     edx                               // Restoring the caller registers as the data inside is important for future functions and the function decrypt_6 has been returned
            pop     ecx
            pop     eax
        }

        DChars[i] = temp_char;                        // Stores the decrypted character in the Decrypted Chars array
    }

    return;



    // Inputs:
    //register EAX = 32-bit address of Ekey,
    //register ECX = the character to be encrypted (in the low 8-bit field, CL).

    // Output: register EDX = the decrypted value of the encrypted character (in the low 8-bit field, DL).
    __asm
    {
     decrypt_6: push    ebp                                           // Save call site's stack frame
                mov     ebp,             esp                          // Create a new stack frame base for the function decrypt_6
                                                          
                mov     ecx,             [ebp + 08h]                  // Retrieving the character from the stack to be encrypted 
                mov     eax,             [ebp + 0Ch]                  // Retrieving the address of the ekey from the stack to decrypt the character   

                ror     byte ptr[eax],   5                            // Encrypting the ekey as we do in the encrypt function
                not     byte ptr[eax]                                 // Encrypting the ekey as we do in the encrypt function
                movzx   edx,             byte ptr[eax]                // Clearing the edx register and copying the ekey value into it

                sub     ecx,             0x20                         // Subtracting 20 hex from the character to reverse the manipulation of the character
                not     cl                                            // Reversing the encrypted not with another not to decrypt the character
                rol     cl,              2                            // Unshifting the character value to decrypt

                xor     ecx,             edx                          // The ekey decrypts the character

                mov     edx,             ecx                          // Copy the completely decrypted character into the edx register to return to the call site

                mov     esp,             ebp                          // Ensure stack pointer is pointing at the base pointer
                pop     ebp                                           // Restore call site's base pointer

                ret
    }

    //--- End of Assembly code 

  
}

//*** end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------





//************ MAIN PROGRAM *************************************************************************************

int main(void)
{
  int char_count (5);  // The number of actual characters entered (upto MAXCHARS limit).

  std::cout << "\nPlease enter upto " << MAXCHARS << " alphanumeric characters:  ";
  
  //get_original_chars (char_count);	// Input the original character string to be encrypted 


  //*****************************************************
  // Open a file to store results (you can view and edit this file in Visual Studio)
  
  std::ofstream EDump;
  EDump.open("EncryptDump.txt", std::ios::app);
  EDump << "\n\nFoMCA Encryption program results (" << StudentName << ") Encryption key = '" << EKey << "'";
  EDump << "\nDate: " << GetDate() << "  Time: " << GetTime();


  //*****************************************************
  // Display and save to the EDump file the string just input

  std::cout << "\n\nOriginal string =  " << OChars << "\tHex = ";
  EDump << "\n\nOriginal string =  " << OChars << "\tHex = ";

  for (int i = 0; i < char_count; ++i)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";
    EDump << std::hex << std::setw(2) << std::setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";
  }


  //*****************************************************
  // Encrypt the string and display/save the result

  encrypt_chars (char_count, EKey);

  std::cout << "\n\nEncrypted string = " << EChars << "\tHex = ";
  EDump << "\n\nEncrypted string = " << EChars << "\tHex = ";
  for (int i = 0; i < char_count; ++i)
  {
    std::cout << ((int(EChars[i])) & 0xFF) << "  ";
    EDump << ((int(EChars[i])) & 0xFF) << "  ";
  }


  //*****************************************************
  // Decrypt the encrypted string and display/save the result

  decrypt_chars (char_count, EKey);	

  std::cout << "\n\nDecrypted string = " << DChars << "\tHex = ";
  EDump << "\n\nDecrypted string = " << DChars << "\tHex = ";
  for (int i = 0; i < char_count; ++i)
  {
    std::cout << ((int(DChars[i])) & 0xFF) << "  ";
    EDump << ((int(DChars[i])) & 0xFF) << "  ";
  }
  //*****************************************************


  std::cout << "\n\n\n";
  EDump << "\n\n-------------------------------------------------------------";
  EDump.close();

  system("PAUSE"); 

  return 0;
} // end of whole encryption/decryption program --------------------------------------------------------------------
