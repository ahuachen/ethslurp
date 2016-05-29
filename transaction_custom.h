#ifndef _TRANSACTION_CUSTOM_H_
#define _TRANSACTION_CUSTOM_H_

// EXISTING_CODE
// EXISTING_CODE

//---------------------------------------------------------------------------
extern SFString nextTransactionChunk_custom(const SFString& fieldIn, SFBool& force, const void *data);
extern void     finishParse(CTransaction *transaction);

#endif
