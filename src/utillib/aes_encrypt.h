#ifndef _AES_ENCRYPT_H_
#define _AES_ENCRYPT_H_

extern SFString hard_encrypt(const SFString& inStr, SFInt32 key);
extern SFString hard_decrypt(const SFString& inStr, SFInt32 key);
extern SFString aes         (char *inStr, bool invert, SFInt32 key);

#endif
