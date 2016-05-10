#ifndef _h_Extern_
#define _h_Extern_

#include <stdexcept>
#include <memory>
#include <new>

#define FMX_PACK					__attribute__ ((packed))
#define FMX_PROCPTR(retType, name)	typedef retType (*name)
#define FMX_AUTOPTRRESET(x)			reset(x)

class Text;
class Data;
class ExprEnv;
typedef unsigned short		uint16;
typedef short				int16;

typedef int					int32;
typedef unsigned int		uint32;

typedef long long			int64;
typedef unsigned long long	uint64;
typedef uint32				ptrtype;
typedef int16				bundleid;

typedef short			errcode;
typedef unsigned short	unichar;

extern "C"
{
	typedef Text		FMX_Text;
	typedef Data		FMX_Data;
	typedef ExprEnv	    FMX_ExprEnv;
	typedef uchar		FMX_UChar;
	typedef int16		FMX_Int16;
	typedef uint16		FMX_UInt16;
	typedef int32		FMX_Int32;
	typedef uint32		FMX_UInt32;
	typedef unichar	    FMX_UniChar;
	typedef	errcode	    FMX_ErrorCode;
	typedef ptrtype	    FMX_PtrType;
	typedef bundleid	FMX_BundleID;
	typedef char		FMX_Char;
	typedef char		FMX_Boolean;
		
	// New public callbacks
	typedef FMX_UChar	FMX_ScriptControl;
	enum
	{
		kFMXT_Halt,
		kFMXT_Exit,
		kFMXT_Resume,
		kFMXT_Pause
	};
	
	FMX_PROCPTR(FMX_ErrorCode,	FMX_StartScriptCall)(const FMX_Text* fileName, const FMX_Text* scriptName, FMX_ScriptControl control, const FMX_Data* parameter);
	FMX_PROCPTR(FMX_ErrorCode,	FMX_CurrentEnvCall)(FMX_ExprEnv* env);
	
#define FMX_StartScript(fileName, scriptName, control, parameter)		(gFMX_ExternCallPtr->cStartScript)(fileName, scriptName, control, parameter)
#define FMX_SetToCurrentEnv(env)										(gFMX_ExternCallPtr->cCurrentEnv)(env)
	
	//::::::::::::::::::::::::::::    Call Back Struct    ::::::::::::::::::::::::::::
	
	enum
	{
		kBadExtnVersion		= -1,
		kDoNotEnable		= -2,
		k40ExtnVersion		= 11,
		k41ExtnVersion		= 12,
		k50ExtnVersion		= 14,
		k60ExtnVersion		= 17,
		k70ExtnVersion		= 50,			// Jumping to 50
		k80ExtnVersion		= 51,
		k110ExtnVersion		= 52,
		k120ExtnVersion		= 53,			// Support for 64-bit plugins
		kCurrentExtnVersion	= 53,
		kMinExtnVersion		= 4,
		kMaxExtnVersion		= 255
	};
	
	typedef FMX_UChar	FMX_ExternCallSwitch;
	enum
	{
		kFMXT_Init				= 0,
		kFMXT_Idle				= 1,
		kFMXT_External			= 3,
		kFMXT_Shutdown			= 4,
		kFMXT_DoAppPreferences	= 5,
		kFMXT_GetString			= 7
	};
	
	typedef FMX_UChar	FMX_Strings;		// Different strings that may be asked for by kFMXT_GetString
	enum
	{
		kFMXT_NameStr			= 128,		// Name of the plug-in
		kFMXT_AppConfigStr		= 129,		// Help text to display in FileMaker application preferences
		kFMXT_OptionsStr		= 131		// The option string
	};
	
	typedef FMX_UChar	FMX_IdleLevel;
	enum
	{
		kFMXT_UserIdle			= 0,		// The user hasn't done anything for 30 seconds or more.
		kFMXT_UserNotIdle		= 1,		// The user has done something within 30 seconds.
		kFMXT_ScriptPaused		= 2,		// The user is running a script that is paused.
		kFMXT_ScriptRunning		= 3,		// The user is running a script.
		kFMXT_Unsafe			= 4			// Same as if unsafeCalls is set to true.
	};
	
	typedef FMX_UChar	FMX_Application;
	enum
	{
		kFMXT_Developer			= 0,
		kFMXT_Pro				= 1,
		kFMXT_Runtime			= 2,
		kFMXT_Server			= 3,
		kFMXT_Web				= 4,
		kFMXT_Mobile			= 5,
		kFMXT_XDBC              = 6, // This process does not currently load plug-ins
		kFMXT_SASE              = 7, // Server scripting process
		kFMXT_IWP               = 8 // Instant Web Publishing process
	};
	
	struct				FMX_ExternCallStruct;
	typedef struct		FMX_ExternCallStruct *FMX_ExternCallPtr;
	typedef 			void (*FMX_ExternCallProc)(FMX_ExternCallPtr);
	
	struct FMX_ExternCallStruct
	{
		
		// Passed in every call
		FMX_Int16				extnVersion;					// Version of this structure, set to the value of kCurrentExtnVersion
		FMX_BundleID			resourceID;						// The resource fork ID of the plug-in
		FMX_ExternCallProc		entryPoint;						// The pointer the application calls
		FMX_Boolean				cfmCalls;						// True if plug-in is old style CFM format
		FMX_ExternCallSwitch	whichCall;						// Which call FileMaker is making into the plug-in
		FMX_Boolean				unsafeCalls;					// True if unsafe for non-memory calls
		
		// Parameters for the calls; any parameter not used by a call is set to zero.
		FMX_UChar				parm1;
		FMX_PtrType				parm2;
		FMX_PtrType				parm3;
		
		// The meanings of parm1..parm3 in terms of the various messages:
		//	Msg	=					Parm1						Parm2							Parm3
		//	kFMXT_Init				FMX_Application value		App vers unicode c str* 		[unused]
		//	kFMXT_Idle				FMX_IdleLevel value			[reserved]						[unused]
		//	kFMXT_External			[unused]					Funct str index					Parameter text**
		//	kFMXT_Shutdown			[unused]					[unused]						[unused]
		//	kFMXT_DoAppPreferences	[unused]					[unused]						[unused]
		//	kFMXT_GetString			FMX_Strings value			Windows lang ID					Maximum size of string to return
		//
		//(* same as GetAppVersion, e.g. "Pro 7.0v4" )
		//(** Parameter from calculation as text, in kEncoding_ASCII_Mac or kEncoding_ShiftJIS_Mac; Result passed back to FileMaker must match encoding.)
		
		// Passed in every call
		FMX_PtrType				instanceID;						// ID of the plug-in
																//		On Windows, it is the Instance ID of the plug-in.
																//		On Mac OS X, it is the BundleRef of the plug-in.
		
		// Result from a call
		FMX_PtrType				result;							// kFMXT_Init returns it's kCurrentExtnVersion
		
		// Unused
		FMX_PtrType				unused;
		
//REMOVE_THESE
		// Call backs for old plug-ins
//		FMX_NewHandleCall		cNewHandle;
//		FMX_SetHandleSizeCall	cSetHandleSize;
//		FMX_GetHandleSizeCall	cGetHandleSize;
//		FMX_DisposeHandleCall	cDisposeHandle;
//		FMX_MoveHHiCall			cMoveHHi;
//		FMX_LockHandleCall		cLockHandle;
//		FMX_UnlockHandleCall	cUnlockHandle;
//		FMX_NewPointerCall		cNewPointer;
//		FMX_DisposePointerCall	cDisposePointer;
//		FMX_MemoryErrorCall		cMemoryError;
		
		// New public callbacks
		FMX_StartScriptCall		cStartScript;
		FMX_CurrentEnvCall		cCurrentEnv;
		
	} FMX_PACK;
	
	// These need to be defined in the plug-in.
	
	extern void FMExternCallProc(FMX_ExternCallPtr pb);
	extern FMX_ExternCallPtr gFMX_ExternCallPtr;
	
}

struct _fmxcpt
{
	enum
	{
		kNoErr,
		kBadAlloc,
		kUnknown
	};
	
	_fmxcpt ()
	: m_vers ( 1 )
	, m_code ( kNoErr )
	{
	}
	
	void Check ()
	{
		switch ( m_code )
		{
			case kNoErr:
				break;
			case kBadAlloc:
				throw std::bad_alloc ();
			case kUnknown:
				throw std::exception ();
		}
	}
	
	int32 m_vers;
	int32 m_code;
};

class QuadChar
{
public:
	const QuadChar &operator = ( const QuadChar &value );
	uchar operator [] ( int i );
	const uchar operator [] ( int i ) const;
	bool operator == ( const QuadChar &value ) const;
	bool operator != ( const QuadChar &value ) const;
	bool operator < ( const QuadChar &value ) const;
	bool operator <= ( const QuadChar &value ) const;
	bool operator > ( const QuadChar &value ) const;
	bool operator >= ( const QuadChar &value ) const;
	uint32 GetMacType () const;
	void SetMacType ( uint32 value );
	void operator delete ( void *obj );
	
private:
	QuadChar ();
	QuadChar ( const QuadChar &value );
	
};

class Locale
{
public:
	
	enum Type
	{
		kType_None                              = 0,    // Empty
		kType_System                    = 1,    // Uses system settings
		kType_UnicodeRaw                = 2,    // Use raw unicode bytes (like the old ASCII ordering)
		kType_UnicodeStandard   = 3,    // Standard unicode rules
		kType_Catalog                   = 4,    // FileMaker list rules
		kType_CAT                               = 16,   // Catalan
		kType_HRV                               = 17,   // Croatian
		kType_CES                               = 18,   // Czech
		kType_DAN                               = 19,   // Danish
		kType_NLD                               = 20,   // Dutch
		kType_ENG                               = 21,   // English
		kType_FIN                               = 22,   // Finnish
		kType_FIN_fmi                   = 23,   // Finnish (FileMaker custom)
		kType_FRA                               = 24,   // French
		kType_DEU                               = 25,   // German
		kType_DEU_dictionary    = 26,   // German (dictionary ordering)
		kType_ELL                               = 27,   // Greek
		kType_HUN                               = 28,   // Hungarian
		kType_ISL                               = 29,   // Icelandic
		kType_ITA                               = 30,   // Italian
		kType_JPN                               = 31,   // Japanese
		kType_NOR                               = 32,   // Norwegian
		kType_POL                               = 33,   // Polish
		kType_POR                               = 34,   // Portuguese
		kType_RON                               = 35,   // Romanian
		kType_RUS                               = 36,   // Russian
		kType_SLK                               = 37,   // Slovak
		kType_SLV                               = 38,   // Slovenian
		kType_SPA                               = 39,   // Spanish
		kType_SPA_traditional   = 40,   // Spanish (traditional)
		kType_SWE                               = 41,   // Swedish
		kType_SWE_fmi                   = 42,   // Swedish (FileMaker custom)
		kType_TUR                               = 43,   // Turkish
		kType_UKR                               = 44,   // Ukrainian
		
		// Chinese was added in 8.5
		kType_CHI                               = 45,   // Chinese (PinYin)
		kType_CHI_stroke                = 46,   // Chinese (Stroke-radical)
		
		// These returned by versions distributed by WinSoft
		kType_HE                                = 47,
		kType_HI                                = 48,
		kType_AR                                = 49,
		kType_ET                                = 50,
		kType_LT                                = 51,
		kType_LV                                = 52,
		kType_SR                                = 53,
		kType_FA                                = 54,
		kType_BG                                = 55,
		kType_VI                                = 56,
		kType_TH                                = 57,
		kType_ELL_Mixed                 = 58, // Greek Mixed
		
		kType_BEN                               = 59, // Bengali
		kType_TEL                               = 60, // Telugu
		kType_MAR                               = 61, // Marathi
		kType_TAM                               = 62, // Tamil
		kType_GUJ                               = 63, // Gujarati
		kType_KAN                               = 64, // Kannada
		kType_MAL                               = 65, // Malayalam
		kType_ORI                               = 66, // Oriya
		kType_PAN                               = 67, // Panjabi
		kType_SIN                               = 68, // Sinhala
		kType_URD                               = 69, // Urdu
		kType_DIV                               = 70, // Divehi (Thanaa)
		kType_BUR                               = 71, // Burmese (Myanmar)
		kType_SAN                               = 72, // Sanskrit
		
		kType_LAO                               = 73, // Lao
		kType_KHM                               = 74, // Khmer
		kType_BOD                               = 75, // Tibetan
		kType_Invalid                   = 0xFFFF
	};
	
	Locale &operator = ( const Locale &rhs );
	void operator delete ( void *obj );
	
private:
	Locale ( Type inputType );
	Locale ( const Locale &copyConstruct );
	
};

class QuadCharAutoPtr : public std::auto_ptr<QuadChar>
{
	typedef QuadCharAutoPtr UpCaster;
public:
	QuadCharAutoPtr ();
	QuadCharAutoPtr ( char c0, char c1, char c2, char c3 );
	QuadCharAutoPtr ( const QuadChar &value );
	
};

class LocaleAutoPtr : public std::auto_ptr<Locale>
{
	typedef LocaleAutoPtr   UpCaster;
public:
	LocaleAutoPtr ( Locale::Type inputType = Locale::kType_System );
	LocaleAutoPtr ( const Locale &copyConstruct );
	
};

#if defined(__GNUC__)
#define DEPRECATED __attribute__((deprecated))
#else
#define DEPRECATED
#endif

class Text;
class FixPt;
class DateTime;
class BinaryData;
class Data;
class DataVect;
class ExprEnv;

FMX_PROCPTR( errcode, ExtPluginType ) ( short functionId, const ExprEnv& env, const DataVect& parms, Data& result );

class DataVect
{
public:
    uint32 Size () const;
    void Clear ();
    bool IsEmpty () const;
    void PushBack ( const Data &data );
    Data *PopBack ();
    const Data &At ( uint32 position ) const;
    const Text &AtAsText ( uint32 position ) const;
    const FixPt &AtAsNumber ( uint32 position ) const;
    const DateTime &AtAsDate ( uint32 position ) const;
    const DateTime &AtAsTime ( uint32 position ) const;
    const DateTime &AtAsTimeStamp ( uint32 position ) const;
    bool AtAsBoolean ( uint32 position ) const;
    const BinaryData &AtAsBinaryData ( uint32 position ) const;
    void operator delete ( void *obj );
	
private:
    DataVect ();
    DataVect ( const DataVect& other );
};

class RowVect
{
public:
    uint32 Size () const;
    bool IsEmpty () const;
    const DataVect &At ( uint32 position ) const;
    void operator delete ( void *obj );
	
private:
    RowVect ();
    RowVect ( const RowVect& other );
};

class ExprEnv
{
public:
    enum
    {
        kMayEvaluateOnServer    = 0x00000001,   // If this flag is not set then calcs containing the function will not be
                                                // evaluated on host as part of a find operation on an unstored calculation
                                                // field. Instead the client will be doing all the evaluations of calculations
                                                // to create the found set. If this flag is set then the plug-in must also
                                                // be loaded on the host.
        kDisplayCalcFields      = 0x00000100,   // Calculated fields
        kDisplayAutoEnter       = 0x00000200,   // Auto enter calculation
        kDisplayValidation      = 0x00000400,   // Validation by calculation
        kDisplayCustomFunctions = 0x00000800,   // Custom function definition
        kDisplayPrivileges      = 0x00001000,   // Row level access calculations
        kDisplayInFuture1       = 0x00002000,   // As yet undefined calculation dialog
        kDisplayInFuture2       = 0x00004000,   // As yet undefined calculation dialog
        kDisplayGeneric         = 0x00008000,   // Dialog used by scripting and replace
        kDisplayInAllDialogs    = 0x0000FF00
    };
	
    errcode Evaluate ( const Text &expression, Data &result ) const;
	errcode ExecuteFileSQL ( const Text &expression, const Text &filename, const DataVect &parameters, RowVect& result ) const;
	
    // Dynamic Registration of Plug-in Functions
    //
    // RegisterExternalFunction enables the plug-in to register a function with the application,
    // so that function appears in the calculation dialog in the application.
    //
    // "pluginId" should be the unique four-digit identifier for your plug-in that you use for the "options" string.
    // "functionId" is the unique cookie that you can use to represent which function was called, it will be passed back to the
    //    registered function as the first parameter (see the parameter of the same name in "ExtPluginType").
    // "functionName" is the name of the function as it should appear in the calculation formula.
    // "functionPrototype" is the suggested syntax that will appear in the list of functions in the calculation dialog.
    // "minArgs" is the number of required parameters for the function.  0 is the smallest valid value.
    // "maxArgs" is the maximum number of parameters that they user should be able to specify in the calculation dialog
    //     and still have correct syntax usage for the function.  Use -1 to allow a variable number of parameters up to the
    //     number supported by calculation formulas in the application.
    // "typeFlags" is options for the function.  Currently only represents where the function will appear in the various kinds
    //     of calculation dialogs in the application.  See the enum above.
    // "funcPtr" is the pointer to the function that must match the signature defined by ExtPluginType.
    //
    //
    // UnRegisterExternalFunction removes the function entry from the list of external functions in any calculation dialog.
    //
    static errcode RegisterExternalFunction ( const QuadChar &pluginId, short functionId, const Text &functionName,
													const Text &functionPrototype, short minArgs, short maxArgs, uint32 typeFlags, ExtPluginType funcPtr );
    static errcode UnRegisterExternalFunction ( const QuadChar &pluginId, short functionId );
    void operator delete ( void *obj );
	
private:
    ExprEnv ();
    ExprEnv ( const ExprEnv& other );
};

class DataVectAutoPtr : public std::auto_ptr<DataVect>
{
    typedef DataVectAutoPtr     UpCaster;
public:
    DataVectAutoPtr ();
};

class RowVectAutoPtr : public std::auto_ptr<RowVect>
{
    typedef RowVectAutoPtr      UpCaster;
public:
    RowVectAutoPtr ();
};

class ExprEnvAutoPtr : public std::auto_ptr<ExprEnv>
{
    typedef ExprEnvAutoPtr      UpCaster;
public:
    ExprEnvAutoPtr ();
};

class Locale;
class Text;
class FixPt;

class DateTime
{
public:
	bool operator == ( const DateTime &that ) const;
	bool operator != ( const DateTime &that ) const;
	bool IsLeapYear () const;
	int16 DayOfWeek () const;
	int16 DayOfYear () const;
	int16 WeekOfYear () const;
	void Now ();
	void SetDate ( const DateTime &datetime );
	errcode SetNormalizedDate ( int16 month, int16 day, int16 year );
	errcode SetNormalizedDate ( const FixPt &year, const FixPt &month, const FixPt &day );
	void SetDaysSinceEpoch ( int64 days );
	void SetTime ( const DateTime &datetime );
	errcode SetNormalizedTime ( int64 hour, int16 minute, int16 sec, int32 usec = 0 );
	errcode SetNormalizedTime ( const FixPt &hour, const FixPt &minute, const FixPt &sec );
	void SetSecsSinceMidnight ( const FixPt &secs );
	void SetSecondsSinceEpoch ( const FixPt &seconds );
	int16 GetYear () const;
	int16 GetMonth () const;
	int16 GetDay () const;
	int32 GetDaysSinceEpoch () const;
	int32 GetHour () const;
	int16 GetMinute () const;
	int16 GetSec () const;
	int32 GetUSec () const;
	void GetSeconds ( FixPt &results ) const;
	void GetSecsSinceMidnight ( FixPt &results ) const;
	void GetSecondsSinceEpoch ( FixPt &results ) const;
	void operator delete ( void *obj );
	
private:
	DateTime ();
	DateTime ( const DateTime &other );
	
};

class DateTimeAutoPtr : public std::auto_ptr<DateTime>
{
	typedef DateTimeAutoPtr     UpCaster;
public:
	DateTimeAutoPtr ();
	DateTimeAutoPtr ( const unichar* dateString, uint32 dateLength, const Locale& intl );
	DateTimeAutoPtr ( const Text &dateText, const Locale &intl );
};

class FixPt
{
public:
	
	enum
	{
		kDefltFixedPrecision = 16
	};
	
	void AssignInt ( int32 that );
	void AssignDouble ( double that );
	void AssignFixPt ( const FixPt &that );
	bool operator == ( const FixPt &that ) const;
	bool operator != ( const FixPt &that ) const;
	bool operator < ( const FixPt &that ) const;
	bool operator <= ( const FixPt &that ) const;
	bool operator > ( const FixPt &that ) const;
	bool operator >= ( const FixPt &that ) const;
	void Increment ( int32 n );
	void Decrement ( int32 n );
	void Negate ();
	
	// Obtain and modify the decimal precision from the standard 16 digits. For more information, see SetPrecision calculation function in FileMaker Pro Advanced Help.
	int GetPrecision () const;
	void SetPrecision ( int precision );
	
	void Add ( const FixPt &arg, FixPt &result ) const;
	void Subtract ( const FixPt &arg, FixPt &result ) const;
	void Multiply ( const FixPt &arg, FixPt &result ) const;
	errcode Divide ( const FixPt &arg, FixPt &result ) const;
	bool AsBool () const;
	int32 AsLong () const;
	double AsFloat () const;
	void operator delete ( void *obj );
	
private:
	FixPt ();
	FixPt ( const FixPt &other );
	
};

class FixPtAutoPtr : public std::auto_ptr<FixPt>
{
	typedef FixPtAutoPtr	UpCaster;
public:
	FixPtAutoPtr ( int32 val = 0, int precision = FixPt::kDefltFixedPrecision );
	FixPtAutoPtr ( int32 val, const FixPt &precisionExample );
	
};

class CharacterStyle;

class Text
{
public:
	
	enum Encoding
	{
		kEncoding_Native,                                       // The platform's default
		kEncoding_UTF8,
		kEncoding_ASCII_DOS,                            // OEM/DOS
		kEncoding_ASCII_Windows,                        // Windows ANSI
		kEncoding_ASCII_Mac,                            // Mac Roman
		kEncoding_ISO_8859_1,                           // Latin 1
		kEncoding_ShiftJIS_Mac,                         // Japanese Shift JIS Mac
		kEncoding_ShiftJIS_Win,                         // Japanese Shift JIS Windows
		kEncoding_Korean_Mac,                           // Mac Korean
		kEncoding_Korean_Win,                           // Windows Korean Unified Hangul (949)
		kEncoding_Korean_Johab,                         // Korean Johab (1361)
		kEncoding_ChineseTrad_Mac,                      // Mac Traditional Chinese
		kEncoding_ChineseTrad_Win,                      // Windows Traditional Chinese (950)
		kEncoding_ChineseSimp_Mac,                      // Mac Simplified Chinese
		kEncoding_ChineseSimp_Win,                      // Windows Simplified Chinese (936)
		kEncoding_Cyrillic_Mac,                         // Mac Russian
		kEncoding_Cyrillic_Win,                         // Windows Russian (1251)
		kEncoding_ISO_8859_5,                           // ISO Cyrillic
		kEncoding_CentralEurope_Mac,            // Central Europe
		kEncoding_EasternEurope_Win,            // Eastern Europe (1250)
		kEncoding_ISO_8859_2,                           // Central Europe
		kEncoding_Turkish_Mac,                          // Mac Turkish
		kEncoding_Turkish_Win,                          // Windows Turkish (1254)
		kEncoding_ISO_8859_3,                           // Latin 3
		kEncoding_ISO_8859_9,                           // Latin 5
		kEncoding_Baltic_Win,                           // Windows Baltic (1257)
		kEncoding_ISO_8859_4,                           // Baltic
		kEncoding_Arabic_Mac,                           // Mac Arabic
		kEncoding_Arabic_Win,                           // Windows Arabic (1256)
		kEncoding_ISO_8859_6,                           // Arabic
		kEncoding_Greek_Mac,                            // Mac Greek
		kEncoding_Greek_Win,                            // Windows Greek (1253)
		kEncoding_ISO_8859_7,                           // Greek
		kEncoding_Hebrew_Mac,                           // Mac Hebrew
		kEncoding_Hebrew_Win,                           // Windows Hebrew (1255)
		kEncoding_ISO_8859_8,                           // Hebrew: Visual Ordering
		kEncoding_ISO_8859_15                           // Latin 9
	};
	
	enum
	{
		kSize_Invalid = 0xFFFFFFFF,
		kSize_End = 0xFFFFFFFF
	};
	
	uint32 GetSize() const;
	
	// These three methods expect null terminated strings.
	void Assign ( const char *s, Encoding encoding = kEncoding_Native );
	void AssignWide ( const wchar_t *s );
	void AssignUnicode ( const uint16 *s );
	
	void AssignWithLength ( const char *s, uint32 strlength, Encoding encoding = kEncoding_Native );
	void AssignWideWithLength ( const wchar_t *s, uint32 strlength );
	void AssignUnicodeWithLength ( const uint16 *s, uint32 strlength );
	
	// Buffer size is in bytes, position and size are in Unicode characters, encoded bytes copied to buffer with a null terminator
	void GetBytes ( char *charbuffer, uint32 buffersize, uint32 position = 0, uint32 size = kSize_End, Encoding encoding = kEncoding_Native ) const;
	
	// Position and size are in Unicode characters, characters copied to unicodebuffer without any terminating null character
	void GetUnicode ( uint16 *unicodebuffer, uint32 position, uint32 size ) const;
	
	void SetText ( const Text &other, uint32 position = 0, uint32 size = kSize_End );
	void AppendText ( const Text &other, uint32 position = 0, uint32 size = kSize_End );
	void InsertText ( const Text &other, uint32 position );
	void DeleteText ( uint32 positionToDelete, uint32 sizeToDelete = kSize_End );
	
	uint32 Find ( const Text &other, uint32 position ) const;
	uint32 FindPrev ( const Text &other, uint32 position ) const;
	uint32 FindIgnoringCase ( const Text &other, uint32 position ) const;
	uint32 FindPrevIgnoringCase ( const Text &other, uint32 position ) const;
	
	void Uppercase ();
	void Lowercase ();
	
	void GetStyle( CharacterStyle& style, uint32 position ) const;
	void GetDefaultStyle( CharacterStyle& style ) const;
	
	// Overwrites any existing CharacterStyle in this Text at position..(position+size)
	void SetStyle( const CharacterStyle& style, uint32 position = 0, uint32 size = kSize_End );
	
	// Removes the the given style.  This will remove elements of the given style
	// that match exactly with the current styles.  For example, if the given style
	// specifies a size of 12, then styles in the text that are size 12 will be
	// disabled, thus picking up the default style.
	void RemoveStyle( const CharacterStyle& style );
	
	// This will reset all character and paragraph style information to the default settings
	void ResetAllStyleBuffers();
	
	// These six operators use the raw unicode values for comparison.
	bool operator == ( const Text &that ) const;
	bool operator != ( const Text &that ) const;
	bool operator < ( const Text &that ) const;
	bool operator <= ( const Text &that ) const;
	bool operator > ( const Text &that ) const;
	bool operator >= ( const Text &that ) const;
	
	void operator delete ( void *obj );
	
private:
	Text ();
	Text ( const Text &source);
	
};

class TextAutoPtr : public std::auto_ptr<Text>
{
	typedef TextAutoPtr     UpCaster;
public:
	TextAutoPtr ();
	
};

class CharacterStyle
{
public:
	typedef unsigned short Face;
	typedef unsigned short FontID;
	typedef unsigned short FontSize;
	typedef unsigned char ColorChannel;
	typedef unsigned short ColorChannel16;
	
	typedef unsigned short FontScript;
	enum
	{
		kRoman					= 0,
		kGreek					= 1,
		kCyrillic				= 2,
		kCentralEurope			= 3,
		kShiftJIS				= 4,
		kTraditionalChinese		= 5,
		kSimplifiedChinese		= 6,
		kOEM					= 7,
		kSymbol					= 8,
		kNative					= 9,
		kOther					= 10,
		kKorean					= 11,
		kDingbats				= 12,
		kTurkish				= 13,
		
		kInvalidFont			= 0xFFFF
	};
	
	class Color
	{
	public:
		enum
		{
			kColorChannel_ON = 255,
			kColorChannel_OFF = 0
		};
		void SetRed ( ColorChannel r );
		void SetGreen ( ColorChannel g );
		void SetBlue ( ColorChannel b );
		void SetAlpha ( ColorChannel a );
		ColorChannel GetRed () const;
		ColorChannel GetGreen () const;
		ColorChannel GetBlue () const;
		ColorChannel GetAlpha () const;
		ColorChannel16 GetRedUpsample () const;
		ColorChannel16 GetGreenUpsample () const;
		ColorChannel16 GetBlueUpsample () const;
		ColorChannel16 GetAlphaUpsample () const;
		Color &operator = ( const Color &rhs );
		bool operator == ( const Color &rhs ) const;
		bool operator != ( const Color &rhs ) const;
		static const Color &White () throw ();
		static const Color &Black () throw ();
		void operator delete ( void *obj );
		
	private:
		Color ();
		Color ( const Color &color );
		
	};
	
	
	enum
	{
		kFace_Plain = 0,
		kFace_Bold = 256,
		kFace_Italic = 512,
		kFace_Underline = 1024,
		kFace_Outline = 2048,
		kFace_Shadow = 4096,
		kFace_Condense = 8192,
		kFace_Extend = 16384,
		kFace_Strikethrough = 1,
		kFace_SmallCaps = 2,
		kFace_Superscript = 4,
		kFace_Subscript = 8,
		kFace_Uppercase = 16,
		kFace_Lowercase = 32,
		kFace_Titlecase = ( kFace_Uppercase | kFace_Lowercase ),
		kFace_WordUnderline = 64,
		kFace_DoubleUnderline = 128,
		kFace_AllStyles = ( kFace_Plain | kFace_Bold | kFace_Italic | kFace_Underline | kFace_Outline | kFace_Shadow
						   | kFace_Condense | kFace_Extend | kFace_Strikethrough | kFace_SmallCaps | kFace_Superscript
						   | kFace_Subscript | kFace_Uppercase | kFace_Lowercase | kFace_Titlecase | kFace_WordUnderline
						   | kFace_DoubleUnderline )
	};
	
	// These methods enables you to specify only one trait, and not overwrite others. A newly constructed CharacterStyle
	// will have everything disabled. Using any of the Set methods further down will also enable that trait in this style.
	// Disabled traits will be defaulted to the settings of the field on the layout.
	void EnableFont ();
	void EnableFace ( Face face );
	void EnableSize ();
	void EnableColor ();
	void DisableFont ();
	void DisableFace ( Face face );
	void DisableAllFaces ();
	void DisableSize ();
	void DisableColor ();
	void DisableAll ();
	bool IsAllDisabled () const;
	bool IsFontEnabled () const;
	bool IsFaceEnabled ( Face face ) const;
	bool IsAnyFaceEnabled () const;
	bool IsSizeEnabled () const;
	bool IsColorEnabled () const;
	
	void SetFontInformation ( FontID font, Face face, FontSize size );
	void GetFontInformation ( FontID &font, Face &face, FontSize &size );
	void SetFont ( FontID font );
	void SetFace ( Face face );
	void SetSize ( FontSize size );
	FontID GetFont () const;
	Face GetFace () const;
	FontSize GetSize () const;
	void SetColor ( const Color &color );
	const Color &GetColor () const;
	CharacterStyle &operator = ( const CharacterStyle &rhs );
	bool operator == ( const CharacterStyle &rhs ) const;
	bool operator != ( const CharacterStyle &rhs ) const;
	void operator delete ( void *obj );
	
private:
	CharacterStyle ();
	CharacterStyle ( const CharacterStyle &style );
	
};

class ColorAutoPtr : public std::auto_ptr<CharacterStyle::Color>
{
	typedef ColorAutoPtr	UpCaster;
public:
	ColorAutoPtr ();
	ColorAutoPtr ( CharacterStyle::ColorChannel red, CharacterStyle::ColorChannel green, CharacterStyle::ColorChannel blue,
						 CharacterStyle::ColorChannel alpha = CharacterStyle::Color::kColorChannel_ON );
	ColorAutoPtr ( const CharacterStyle::Color &mask );
	
};

class CharacterStyleAutoPtr : public std::auto_ptr<CharacterStyle>
{
	typedef CharacterStyleAutoPtr	UpCaster;
public:
	CharacterStyleAutoPtr ();
	CharacterStyleAutoPtr ( CharacterStyle::FontID font, CharacterStyle::Face face, CharacterStyle::FontSize size,
								  const CharacterStyle::Color &color = CharacterStyle::Color::Black() );
	CharacterStyleAutoPtr ( const CharacterStyle &style );
	
};

// A BinaryData type is a collection of streams. Each stream has two parts, a TypeId and Data.
//      There can be multiple related streams, but there should never be more than one
//      stream of the same type.  Streams should not be empty.
// Container fields can be considered to be of type BinaryData when data is stored in the database
//  as opposed to objects inserted by reference (path of file as text).
//
// To iterate through the streams of a BinaryData, use GetCount and then call GetIndex to
//      get the TypeID for each stream.
//
// To create a new BinaryData object, use the autoptr version to construct the object, then
//      Add() streams.
class BinaryData
{
public:
    // Here are some common QuadChar types you may encounter, and notes on required usage.
    // The actual format of the data should adhere to the standard documented by
    // the format creator.
    //
    //  "FNAM" is a filename stream. This is used either to display the filename of the file
    //  in the field (non-images or images that cannot be displayed) and for a initial
    //  suggested filename when exporting field contents. Please use the special accessors
    //  below ("GetFNAMData and AddFNAMData) to get a text representation of this data.
    //
    //  Images:
    //  { "JPEG", "GIFf", "EPS ", "PICT", "META", "PNGf", "BMPf", "PDF " }
    //  Images must have at least a JPEG or GIFf stream in addition to any
    //  other image type.  A PICT stream must not have the 512 byte "file" header. The
    //  PICT stream should match the format that is used on the clipboard.  A "SIZE"
    //  stream is also required for FileMaker to display the image. Please use the special
    //  accessors below (GetSIZEData and AddSIZEData) to get the height and width values
    //  for the image.
    // When generating other kinds of graphical data, use JPEG for a preview,
    // and the other format will be preferred if possible. For example, to generate
    // a PNG picture, you have to add "PNGf" and "JPEG" streams.
    // Combining "PNGf" and "GIFf" streams will cause the PNG to be ignored.
    //
    //  Files:
    //  { "FILE" (binary file data), "FORK" (Mac only resource fork) }
    //
    //  Sound:
    //  { "snd " }
    //  Always Mac raw format.
	
    // Any other types you encounter are not documented for plug-in use. Use them at your own risk.
	
    BinaryData &operator = ( const BinaryData &source );
    bool operator == ( const BinaryData &compareData ) const;
    bool operator != ( const BinaryData &compareData ) const;
    int32 GetCount () const;
    int32 GetIndex ( const QuadChar& dataType ) const;
    uint32 GetTotalSize () const;
    void GetType ( int32 index, QuadChar& dataType ) const;
    uint32 GetSize ( int32 index ) const;
    errcode GetData ( int32 index, uint32 offset, uint32 amount, void *buffer ) const;
    
    // You should exercise great care in using the following functions. Removing or modifying a required stream
    // could have undesirable and undefined results.
    errcode Add ( const QuadChar& dataType, uint32 amount, void *buffer );
    bool Remove ( const QuadChar& dataType );
    void RemoveAll ();
	
    // Special accessors for FileMaker specific storage formats
    //  [ ONLY IN FM8 (API VERSION 51) AND LATER ]
    errcode GetFNAMData ( Text &filepathlist ) const;
    errcode AddFNAMData ( const Text &filepathlist );
    errcode GetSIZEData ( short &width, short &height ) const;
    errcode AddSIZEData ( short width, short height );
	
    void operator delete ( void *obj );
	
private:
    BinaryData ();
    BinaryData ( const BinaryData &source );
	
};

class BinaryDataAutoPtr : public std::auto_ptr<BinaryData>
{
    typedef BinaryDataAutoPtr   UpCaster;
public:
    BinaryDataAutoPtr ();
    BinaryDataAutoPtr ( const BinaryData &sourceData );
};

class Locale;
class Text;
class FixPt;
class DateTime;
class BinaryData;
class ExprEnv;

class Data
{
public:
	
	enum DataType
	{
		kDTInvalid,
		kDTText,
		kDTNumber,
		kDTDate,
		kDTTime,
		kDTTimeStamp,
		kDTBinary,
		kDTBoolean
	};
	
	bool IsEmpty () const;
	bool IsValid () const;
	bool IsFindRequest () const;
	void Clear ( DataType newNativeType = kDTInvalid );
	const Text &GetAsText () const;
	const FixPt &GetAsNumber () const;
	const DateTime &GetAsDate () const;
	const DateTime &GetAsTime () const;
	const DateTime &GetAsTimeStamp () const;
	bool GetAsBoolean () const;
	const BinaryData &GetBinaryData () const;
	const Locale &GetLocale () const;
	const DataType GetNativeType () const;
	void ConvertData ( DataType nativeType );
	errcode SetAsText ( const Text &textData, const Locale &sourceLocale, const DataType nativeType = kDTText );
	void SetAsNumber ( const FixPt &numericData, const DataType nativeType = kDTNumber );
	void SetAsDate ( const DateTime &dateData, const DataType nativeType = kDTDate );
	void SetAsTime ( const DateTime &timeData, const DataType nativeType = kDTTime );
	void SetAsTimeStamp ( const DateTime &timeStampData, const DataType nativeType = kDTTimeStamp );
	void SetDateTime ( const DateTime &dateTimeData, const DataType dateTimeType, const DataType nativeType );
	void SetBinaryData ( const BinaryData &binaryData, bool forceBinaryNativeType = true );
	void operator delete ( void *obj );
	
	// GetFontID will create a new id if the specified font name and font script has not been
	// used before in the database. A font id of zero is returned if the font could not be created.
	CharacterStyle::FontID GetFontID( const Text &fontName, CharacterStyle::FontScript fontScript, const ExprEnv &env );
	
	// GetFontInfo will return false if the given font id is not found in the database
	bool GetFontInfo( CharacterStyle::FontID font, Text &fontName, CharacterStyle::FontScript &fontScript, const ExprEnv &env ) const;
	
private:
	Data ();
	Data ( const Data &other );
	
};

class DataAutoPtr : public std::auto_ptr<Data>
{
	typedef DataAutoPtr	UpCaster;
public:
	DataAutoPtr ();
	
};

extern "C"
{
    QuadChar *FM_QuadChar_Constructor1 ( _fmxcpt &_x ) throw ();
    QuadChar *FM_QuadChar_Constructor2 ( char c0, char c1, char c2, char c3, _fmxcpt &_x ) throw ();
    QuadChar *FM_QuadChar_Constructor3 ( const QuadChar &value, _fmxcpt &_x ) throw ();
    void FM_QuadChar_Delete ( void *_self, _fmxcpt &_x ) throw ();
	
    Locale *FM_Locale_Constructor1 ( int32 inputType, _fmxcpt &_x ) throw ();
    Locale *FM_Locale_Constructor2 ( const Locale &copyConstruct, _fmxcpt &_x ) throw ();
    void FM_Locale_Delete ( void *_self, _fmxcpt &_x ) throw ();
}
extern "C++"
{
    const QuadChar &FM_QuadChar_operatorAS ( void *_self, const QuadChar &value, _fmxcpt &_x ) throw ();
	uchar FM_QuadChar_operatorAR ( void *_self, int i, _fmxcpt &_x ) throw ();
    const uchar FM_QuadChar_operatorCAR ( const void *_self, int i, _fmxcpt &_x ) throw ();
    bool FM_QuadChar_operatorEQ ( const void *_self, const QuadChar &value, _fmxcpt &_x ) throw ();
    bool FM_QuadChar_operatorNE ( const void *_self, const QuadChar &value, _fmxcpt &_x ) throw ();
    bool FM_QuadChar_operatorLT ( const void *_self, const QuadChar &value, _fmxcpt &_x ) throw ();
    bool FM_QuadChar_operatorLE ( const void *_self, const QuadChar &value, _fmxcpt &_x ) throw ();
    bool FM_QuadChar_operatorGT ( const void *_self, const QuadChar &value, _fmxcpt &_x ) throw ();
    bool FM_QuadChar_operatorGE ( const void *_self, const QuadChar &value, _fmxcpt &_x ) throw ();
    uint32 FM_QuadChar_GetMacType ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_QuadChar_SetMacType ( void *_self, uint32 value, _fmxcpt &_x ) throw ();
    Locale &FM_Locale_operatorAS ( void *_self, const Locale &rhs, _fmxcpt &_x ) throw ();
}


extern "C"
{
    DataVect *FM_DataVect_Constructor1 ( _fmxcpt &_x ) throw ();
    void FM_DataVect_Delete ( void *_self, _fmxcpt &_x ) throw ();
	
    RowVect *FM_RowVect_Constructor1 ( _fmxcpt &_x ) throw ();
    void FM_RowVect_Delete ( void *_self, _fmxcpt &_x ) throw ();
	
    ExprEnv *FM_ExprEnv_Constructor1 ( _fmxcpt &_x ) throw ();
    void FM_ExprEnv_Delete ( void *_self, _fmxcpt &_x ) throw ();
	
    uint32 FM_DataVect_Size ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_DataVect_Clear ( void *_self, _fmxcpt &_x ) throw ();
    bool FM_DataVect_IsEmpty ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_DataVect_PushBack ( void *_self, const Data &data, _fmxcpt &_x ) throw ();
    Data *FM_DataVect_PopBack ( void *_self, _fmxcpt &_x ) throw ();
    bool FM_DataVect_AtAsBoolean ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
	
    uint32 FM_RowVect_Size ( const void *_self, _fmxcpt &_x ) throw ();
    bool FM_RowVect_IsEmpty ( const void *_self, _fmxcpt &_x ) throw ();
    errcode FM_ExprEnv_Evaluate ( const void *_self, const Text &expression, Data &result, _fmxcpt &_x ) throw ();
	errcode FM_ExprEnv_ExecuteFileSQL ( const void *_self, const Text &expression, const Text &filename, const DataVect &parameters, RowVect& result, _fmxcpt &_x ) throw ();
    errcode FM_ExprEnv_RegisterExternalFunction ( const QuadChar &pluginId, short functionId, const Text &functionName, const Text &functionPrototype, short minArgs, short maxArgs, uint32 typeFlags, ExtPluginType funcPtr, _fmxcpt &_x ) throw ();
    errcode FM_ExprEnv_UnRegisterExternalFunction ( const QuadChar &pluginId, short functionId, _fmxcpt &_x ) throw ();
	
    const Text &FM_DataVect_AtAsText ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
    const Data &FM_DataVect_At ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
}

extern "C++"
{
    const BinaryData &FM_DataVect_AtAsBinaryData ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
    const FixPt &FM_DataVect_AtAsNumber ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
    const DateTime &FM_DataVect_AtAsDate ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
    const DateTime &FM_DataVect_AtAsTime ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
    const DateTime &FM_DataVect_AtAsTimeStamp ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
    const DataVect &FM_RowVect_At ( const void *_self, uint32 position, _fmxcpt &_x ) throw ();
}

extern "C"
{
    DateTime *FM_DateTime_Constructor1 ( _fmxcpt &_x ) throw ();
    DateTime *FM_DateTime_Constructor2 ( const unichar* dateString, uint32 dateLength, const Locale& intl, _fmxcpt &_x ) throw ();
    DateTime *FM_DateTime_Constructor3 ( const Text &dateText, const Locale &intl, _fmxcpt &_x ) throw ();
    bool FM_DateTime_operatorEQ ( const void *_self, const DateTime &that, _fmxcpt &_x ) throw ();
    bool FM_DateTime_operatorNE ( const void *_self, const DateTime &that, _fmxcpt &_x ) throw ();
    bool FM_DateTime_IsLeapYear ( const void *_self, _fmxcpt &_x ) throw ();
	int16 FM_DateTime_DayOfWeek ( const void *_self, _fmxcpt &_x ) throw ();
    int16 FM_DateTime_DayOfYear ( const void *_self, _fmxcpt &_x ) throw ();
    int16 FM_DateTime_WeekOfYear ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_DateTime_Now ( void *_self, _fmxcpt &_x ) throw ();
    void FM_DateTime_SetDate ( void *_self, const DateTime &datetime, _fmxcpt &_x ) throw ();
    errcode FM_DateTime_SetNormalizedDate1 ( void *_self, int16 month, int16 day, int16 year, _fmxcpt &_x ) throw ();
    errcode FM_DateTime_SetNormalizedDate2 ( void *_self, const FixPt &year, const FixPt &month, const FixPt &day, _fmxcpt &_x ) throw ();
    void FM_DateTime_SetDaysSinceEpoch ( void *_self, int64 days, _fmxcpt &_x ) throw ();
    void FM_DateTime_SetTime ( void *_self, const DateTime &datetime, _fmxcpt &_x ) throw ();
	errcode FM_DateTime_SetNormalizedTime1 ( void *_self, int64 hour, int16 minute, int16 sec, int32 usec, _fmxcpt &_x ) throw ();
    errcode FM_DateTime_SetNormalizedTime2 ( void *_self, const FixPt &hour, const FixPt &minute, const FixPt &sec, _fmxcpt &_x ) throw ();
    void FM_DateTime_SetSecsSinceMidnight ( void *_self, const FixPt &secs, _fmxcpt &_x ) throw ();
    void FM_DateTime_SetSecondsSinceEpoch ( void *_self, const FixPt &seconds, _fmxcpt &_x ) throw ();
    int16 FM_DateTime_GetYear ( const void *_self, _fmxcpt &_x ) throw ();
    int16 FM_DateTime_GetMonth ( const void *_self, _fmxcpt &_x ) throw ();
    int16 FM_DateTime_GetDay ( const void *_self, _fmxcpt &_x ) throw ();
	int32 FM_DateTime_GetDaysSinceEpoch ( const void *_self, _fmxcpt &_x ) throw ();
    int32 FM_DateTime_GetHour ( const void *_self, _fmxcpt &_x ) throw ();
    int16 FM_DateTime_GetMinute ( const void *_self, _fmxcpt &_x ) throw ();
    int16 FM_DateTime_GetSec ( const void *_self, _fmxcpt &_x ) throw ();
    int32 FM_DateTime_GetUSec ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_DateTime_GetSeconds ( const void *_self, FixPt &results, _fmxcpt &_x ) throw ();
    void FM_DateTime_GetSecsSinceMidnight ( const void *_self, FixPt &results, _fmxcpt &_x ) throw ();
    void FM_DateTime_GetSecondsSinceEpoch ( const void *_self, FixPt &results, _fmxcpt &_x ) throw ();
    void FM_DateTime_Delete ( void *_self, _fmxcpt &_x ) throw ();
}

extern "C"
{
    FixPt *FM_FixPt_Constructor1 ( int32 val, int precision, _fmxcpt &_x ) throw ();
    FixPt *FM_FixPt_Constructor2 ( int32 val, const FixPt &precisionExample, _fmxcpt &_x ) throw ();
    void FM_FixPt_AssignInt ( void *_self, int32 that, _fmxcpt &_x ) throw ();
    void FM_FixPt_AssignDouble ( void *_self, double that, _fmxcpt &_x ) throw ();
    void FM_FixPt_AssignFixPt ( void *_self, const FixPt &that, _fmxcpt &_x ) throw ();
    bool FM_FixPt_operatorEQ ( const void *_self, const FixPt &that, _fmxcpt &_x ) throw ();
    bool FM_FixPt_operatorNE ( const void *_self, const FixPt &that, _fmxcpt &_x ) throw ();
    bool FM_FixPt_operatorLT ( const void *_self, const FixPt &that, _fmxcpt &_x ) throw ();
    bool FM_FixPt_operatorLE ( const void *_self, const FixPt &that, _fmxcpt &_x ) throw ();
    bool FM_FixPt_operatorGT ( const void *_self, const FixPt &that, _fmxcpt &_x ) throw ();
    bool FM_FixPt_operatorGE ( const void *_self, const FixPt &that, _fmxcpt &_x ) throw ();
    void FM_FixPt_Increment ( void *_self, int32 n, _fmxcpt &_x ) throw ();
    void FM_FixPt_Decrement ( void *_self, int32 n, _fmxcpt &_x ) throw ();
    void FM_FixPt_Negate ( void *_self, _fmxcpt &_x ) throw ();
    int FM_FixPt_GetPrecision ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_FixPt_SetPrecision ( void *_self, int precision, _fmxcpt &_x ) throw ();
    void FM_FixPt_Add ( const void *_self, const FixPt &arg, FixPt &result, _fmxcpt &_x ) throw ();
    void FM_FixPt_Subtract ( const void *_self, const FixPt &arg, FixPt &result, _fmxcpt &_x ) throw ();
    void FM_FixPt_Multiply ( const void *_self, const FixPt &arg, FixPt &result, _fmxcpt &_x ) throw ();
    errcode FM_FixPt_Divide ( const void *_self, const FixPt &arg, FixPt &result, _fmxcpt &_x ) throw ();
    bool FM_FixPt_AsBool ( const void *_self, _fmxcpt &_x ) throw ();
    int32 FM_FixPt_AsLong ( const void *_self, _fmxcpt &_x ) throw ();
    double FM_FixPt_AsFloat ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_FixPt_Delete ( void *_self, _fmxcpt &_x ) throw ();
	
}

extern "C"
{
    Text *FM_Text_Constructor1 ( _fmxcpt &_x ) throw ();
    uint32 FM_Text_GetSize ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_Text_Assign ( void *_self, const char *s, int32 encoding, _fmxcpt &_x ) throw ();
    void FM_Text_GetBytes ( const void *_self, char *buffer, uint32 buffersize,  uint32 position, uint32 size, int32 encoding, _fmxcpt &_x ) throw ();
    void FM_Text_AssignWide ( void *_self, const wchar_t *s, _fmxcpt &_x ) throw ();
    void FM_Text_AssignUnicode ( void *_self, const uint16 *s, _fmxcpt &_x ) throw ();
    void FM_Text_AssignWithLength ( void *_self, const char *s, uint32 strlength, int32 encoding, _fmxcpt &_x ) throw ();
    void FM_Text_AssignWideWithLength ( void *_self, const wchar_t *s, uint32 strlength, _fmxcpt &_x ) throw ();
    void FM_Text_AssignUnicodeWithLength ( void *_self, const uint16 *s, uint32 strlength, _fmxcpt &_x ) throw ();
    void FM_Text_GetUnicode ( const void *_self, uint16 *s, uint32 position, uint32 size, _fmxcpt &_x ) throw ();
    void FM_Text_SetText ( void *_self, const Text &other, uint32 position, uint32 size, _fmxcpt &_x ) throw ();
    void FM_Text_AppendText ( void *_self, const Text &other, uint32 position, uint32 size, _fmxcpt &_x ) throw ();
    void FM_Text_InsertText ( void *_self, const Text &other, uint32 position, _fmxcpt &_x ) throw ();
    void FM_Text_DeleteText ( void *_self, uint32 positionToDelete, uint32 sizeToDelete, _fmxcpt &_x ) throw ();
    uint32 FM_Text_Find ( const void *_self, const Text &other, uint32 position, _fmxcpt &_x ) throw ();
    uint32 FM_Text_FindPrev ( const void *_self, const Text &other, uint32 position, _fmxcpt &_x ) throw ();
    uint32 FM_Text_FindIgnoringCase ( const void *_self, const Text &other, uint32 position, _fmxcpt &_x ) throw ();
    uint32 FM_Text_FindPrevIgnoringCase ( const void *_self, const Text &other, uint32 position, _fmxcpt &_x ) throw ();
    void FM_Text_Uppercase ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_Text_Lowercase ( const void *_self, _fmxcpt &_x ) throw ();
	void FM_Text_GetStyle( const void *_self, CharacterStyle& style, uint32 position, _fmxcpt &_x ) throw ();
	void FM_Text_GetDefaultStyle( const void *_self, CharacterStyle& style, _fmxcpt &_x ) throw ();
	void FM_Text_SetStyle( void *_self, const CharacterStyle& style, uint32 position, uint32 size, _fmxcpt &_x ) throw ();
	void FM_Text_RemoveStyle( void *_self, const CharacterStyle& style, _fmxcpt &_x ) throw ();
	void FM_Text_ResetAllStyleBuffers( void *_self, _fmxcpt &_x ) throw ();
    bool FM_Text_operatorEQ ( const void *_self, const Text &that, _fmxcpt &_x ) throw ();
    bool FM_Text_operatorNE ( const void *_self, const Text &that, _fmxcpt &_x ) throw ();
    bool FM_Text_operatorLT ( const void *_self, const Text &that, _fmxcpt &_x ) throw ();
    bool FM_Text_operatorLE ( const void *_self, const Text &that, _fmxcpt &_x ) throw ();
    bool FM_Text_operatorGT ( const void *_self, const Text &that, _fmxcpt &_x ) throw ();
    bool FM_Text_operatorGE ( const void *_self, const Text &that, _fmxcpt &_x ) throw ();
    void FM_Text_Delete ( void *_self, _fmxcpt &_x ) throw ();
	
}

extern "C"
{
	
    CharacterStyle::Color *FM_Color_Constructor1 ( _fmxcpt &_x ) throw ();
    CharacterStyle::Color *FM_Color_Constructor2 ( CharacterStyle::ColorChannel red, CharacterStyle::ColorChannel green, CharacterStyle::ColorChannel blue, CharacterStyle::ColorChannel alpha, _fmxcpt &_x ) throw ();
    CharacterStyle::Color *FM_Color_Constructor3 ( const CharacterStyle::Color &color, _fmxcpt &_x ) throw ();
    void FM_Color_Delete ( void *_self, _fmxcpt &_x ) throw ();
	
	
    CharacterStyle *FM_CharacterStyle_Constructor1 ( _fmxcpt &_x ) throw ();
    CharacterStyle *FM_CharacterStyle_Constructor2 ( CharacterStyle::FontID font, CharacterStyle::Face face, CharacterStyle::FontSize size, const CharacterStyle::Color &color, _fmxcpt &_x ) throw ();
    CharacterStyle *FM_CharacterStyle_Constructor3 ( const CharacterStyle &style, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_Delete ( void *_self, _fmxcpt &_x ) throw ();
}

extern "C++"
{
    void FM_Color_SetRed ( void *_self, CharacterStyle::ColorChannel r, _fmxcpt &_x ) throw ();
    void FM_Color_SetGreen ( void *_self, CharacterStyle::ColorChannel g, _fmxcpt &_x ) throw ();
    void FM_Color_SetBlue ( void *_self, CharacterStyle::ColorChannel b, _fmxcpt &_x ) throw ();
    void FM_Color_SetAlpha ( void *_self, CharacterStyle::ColorChannel a, _fmxcpt &_x ) throw ();
    CharacterStyle::ColorChannel FM_Color_GetRed ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::ColorChannel FM_Color_GetGreen ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::ColorChannel FM_Color_GetBlue ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::ColorChannel FM_Color_GetAlpha ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::ColorChannel16 FM_Color_GetRedUpsample ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::ColorChannel16 FM_Color_GetGreenUpsample ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::ColorChannel16 FM_Color_GetBlueUpsample ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::ColorChannel16 FM_Color_GetAlphaUpsample ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::Color &FM_Color_operatorAS ( void *_self, const CharacterStyle::Color &rhs, _fmxcpt &_x ) throw ();
    bool FM_Color_operatorEQ ( const void *_self, const CharacterStyle::Color &rhs, _fmxcpt &_x ) throw ();
    bool FM_Color_operatorNE ( const void *_self, const CharacterStyle::Color &rhs, _fmxcpt &_x ) throw ();
    const CharacterStyle::Color &FM_Color_White () throw ();
    const CharacterStyle::Color &FM_Color_Black () throw ();
    void FM_CharacterStyle_EnableFont ( void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_EnableFace ( void *_self, CharacterStyle::Face face, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_EnableSize ( void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_EnableColor ( void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_DisableFont ( void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_DisableFace ( void *_self, CharacterStyle::Face face, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_DisableAllFaces ( void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_DisableSize ( void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_DisableColor ( void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_DisableAll ( void *_self, _fmxcpt &_x ) throw ();
    bool FM_CharacterStyle_IsAllDisabled ( const void *_self, _fmxcpt &_x ) throw ();
    bool FM_CharacterStyle_IsFontEnabled ( const void *_self, _fmxcpt &_x ) throw ();
    bool FM_CharacterStyle_IsFaceEnabled ( const void *_self, CharacterStyle::Face face, _fmxcpt &_x ) throw ();
    bool FM_CharacterStyle_IsAnyFaceEnabled ( const void *_self, _fmxcpt &_x ) throw ();
    bool FM_CharacterStyle_IsSizeEnabled ( const void *_self, _fmxcpt &_x ) throw ();
    bool FM_CharacterStyle_IsColorEnabled ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_SetFontInformation ( void *_self, CharacterStyle::FontID font, CharacterStyle::Face face, CharacterStyle::FontSize size, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_GetFontInformation ( void *_self, CharacterStyle::FontID &font, CharacterStyle::Face &face, CharacterStyle::FontSize &size, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_SetFont ( void *_self, CharacterStyle::FontID font, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_SetFace ( void *_self, CharacterStyle::Face face, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_SetSize ( void *_self, CharacterStyle::FontSize size, _fmxcpt &_x ) throw ();
    CharacterStyle::FontID FM_CharacterStyle_GetFont ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::Face FM_CharacterStyle_GetFace ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle::FontSize FM_CharacterStyle_GetSize ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_CharacterStyle_SetColor ( void *_self, const CharacterStyle::Color &color, _fmxcpt &_x ) throw ();
    const CharacterStyle::Color &FM_CharacterStyle_GetColor ( const void *_self, _fmxcpt &_x ) throw ();
    CharacterStyle &FM_CharacterStyle_operatorAS ( void *_self, const CharacterStyle &rhs, _fmxcpt &_x ) throw ();
    bool FM_CharacterStyle_operatorEQ ( const void *_self, const CharacterStyle &rhs, _fmxcpt &_x ) throw ();
    bool FM_CharacterStyle_operatorNE ( const void *_self, const CharacterStyle &rhs, _fmxcpt &_x ) throw ();
}

extern "C"
{
    BinaryData *FM_BinaryData_Constructor1 ( _fmxcpt &_x ) throw ();
    BinaryData *FM_BinaryData_Constructor2 ( const BinaryData &sourceData, _fmxcpt &_x ) throw ();
    void FM_BinaryData_Delete ( void *_self, _fmxcpt &_x ) throw ();
}

extern "C++"
{
    BinaryData &FM_BinaryData_operatorAS ( void *_self, const BinaryData &source, _fmxcpt &_x ) throw ();
    bool FM_BinaryData_operatorEQ ( const void *_self, const BinaryData &compareData, _fmxcpt &_x ) throw ();
    bool FM_BinaryData_operatorNE ( const void *_self, const BinaryData &compareData, _fmxcpt &_x ) throw ();
	int32 FM_BinaryData_GetCount ( const void *_self, _fmxcpt &_x ) throw ();
    int32 FM_BinaryData_GetIndex ( const void *_self, const QuadChar& dataType, _fmxcpt &_x ) throw ();
    uint32 FM_BinaryData_GetTotalSize ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_BinaryData_GetType ( const void *_self, int32 index, QuadChar& dataType, _fmxcpt &_x ) throw ();
    uint32 FM_BinaryData_GetSize ( const void *_self, int32 index, _fmxcpt &_x ) throw ();
    errcode FM_BinaryData_GetData ( const void *_self, int32 index, uint32 offset, uint32 amount, void *buffer, _fmxcpt &_x ) throw ();
    errcode FM_BinaryData_Add ( void *_self, const QuadChar& dataType, uint32 amount, void *buffer, _fmxcpt &_x ) throw ();
    bool FM_BinaryData_Remove ( void *_self, const QuadChar& dataType, _fmxcpt &_x ) throw ();
    void FM_BinaryData_RemoveAll ( void *_self, _fmxcpt &_x ) throw ();
    errcode FM_BinaryData_GetFNAMData ( const void *_self, Text &filepathlist, _fmxcpt &_x ) throw ();
    errcode FM_BinaryData_AddFNAMData ( void *_self, const Text &filepathlist, _fmxcpt &_x ) throw ();
    errcode FM_BinaryData_GetSIZEData ( const void *_self, short &width, short &height, _fmxcpt &_x ) throw ();
    errcode FM_BinaryData_AddSIZEData ( void *_self, short width, short height, _fmxcpt &_x ) throw ();
}

extern "C"
{
	Data *FM_Data_Constructor1 ( _fmxcpt &_x ) throw ();
    void FM_Data_Delete ( void *_self, _fmxcpt &_x ) throw ();
	
    bool FM_Data_IsEmpty ( const void *_self, _fmxcpt &_x ) throw ();
    bool FM_Data_IsValid ( const void *_self, _fmxcpt &_x ) throw ();
    bool FM_Data_IsFindRequest ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_Data_Clear ( void *_self, int32 newNativeType, _fmxcpt &_x ) throw ();
    bool FM_Data_GetAsBoolean ( const void *_self, _fmxcpt &_x ) throw ();
    const int32 FM_Data_GetNativeType ( const void *_self, _fmxcpt &_x ) throw ();
    void FM_Data_ConvertData ( void *_self, int32 nativeType, _fmxcpt &_x ) throw ();
    errcode FM_Data_SetAsText ( void *_self, const Text &textData, const Locale &sourceLocale, int32 nativeType, _fmxcpt &_x ) throw ();
    void FM_Data_SetAsNumber ( void *_self, const FixPt &numericData, int32 nativeType, _fmxcpt &_x ) throw ();
    void FM_Data_SetAsDate ( void *_self, const DateTime &dateData, int32 nativeType, _fmxcpt &_x ) throw ();
    void FM_Data_SetAsTime ( void *_self, const DateTime &timeData, int32 nativeType, _fmxcpt &_x ) throw ();
    void FM_Data_SetAsTimeStamp ( void *_self, const DateTime &timeStampData, int32 nativeType, _fmxcpt &_x ) throw ();
    void FM_Data_SetDateTime ( void *_self, const DateTime &dateTimeData, int32 dateTimeType, int32 nativeType, _fmxcpt &_x ) throw ();
    void FM_Data_SetBinaryData ( void *_self, const BinaryData &binaryData, bool forceBinaryNativeType, _fmxcpt &_x ) throw ();
	
    CharacterStyle::FontID FM_Data_GetFontID ( void *_self, const Text &fontName, CharacterStyle::FontScript fontScript, const ExprEnv &env,  _fmxcpt &_x ) throw ();
    bool FM_Data_GetFontInfo ( const void *_self, CharacterStyle::FontID font, Text &fontName, CharacterStyle::FontScript &fontScript, const ExprEnv &env, _fmxcpt &_x ) throw ();
    const Locale &FM_Data_GetLocale ( const void *_self, _fmxcpt &_x ) throw ();
}

extern "C++"
{
    const Text &FM_Data_GetAsText ( const void *_self, _fmxcpt &_x ) throw ();
    const FixPt &FM_Data_GetAsNumber ( const void *_self, _fmxcpt &_x ) throw ();
    const DateTime &FM_Data_GetAsDate ( const void *_self, _fmxcpt &_x ) throw ();
    const DateTime &FM_Data_GetAsTime ( const void *_self, _fmxcpt &_x ) throw ();
    const DateTime &FM_Data_GetAsTimeStamp ( const void *_self, _fmxcpt &_x ) throw ();
    const BinaryData &FM_Data_GetBinaryData ( const void *_self, _fmxcpt &_x ) throw ();
}

//----------------------------------------------------------------------------------------
inline bool Data::IsEmpty () const
{
	_fmxcpt _x;
	bool _rtn = FM_Data_IsEmpty ( this, _x );
	_x.Check ();
	return _rtn;
}
inline bool Data::IsValid () const
{
	_fmxcpt _x;
	bool _rtn = FM_Data_IsValid ( this, _x );
	_x.Check ();
	return _rtn;
}
inline bool Data::IsFindRequest () const
{
	_fmxcpt _x;
	bool _rtn = FM_Data_IsFindRequest ( this, _x );
	_x.Check ();
	return _rtn;
}
inline void Data::Clear ( DataType newNativeType )
{
	_fmxcpt _x;
	FM_Data_Clear ( this, newNativeType, _x );
	_x.Check ();
}
inline const Text &Data::GetAsText () const
{
	_fmxcpt _x;
	const Text &_rtn = FM_Data_GetAsText ( this, _x );
	_x.Check ();
	return _rtn;
}
inline const FixPt &Data::GetAsNumber () const
{
	_fmxcpt _x;
	const FixPt &_rtn = FM_Data_GetAsNumber ( this, _x );
	_x.Check ();
	return _rtn;
}
inline const DateTime &Data::GetAsDate () const
{
	_fmxcpt _x;
	const DateTime &_rtn = FM_Data_GetAsDate ( this, _x );
	_x.Check ();
	return _rtn;
}
inline const DateTime &Data::GetAsTime () const
{
	_fmxcpt _x;
	const DateTime &_rtn = FM_Data_GetAsTime ( this, _x );
	_x.Check ();
	return _rtn;
}
inline const DateTime &Data::GetAsTimeStamp () const
{
	_fmxcpt _x;
	const DateTime &_rtn = FM_Data_GetAsTimeStamp ( this, _x );
	_x.Check ();
	return _rtn;
}
inline bool Data::GetAsBoolean () const
{
	_fmxcpt _x;
	bool _rtn = FM_Data_GetAsBoolean ( this, _x );
	_x.Check ();
	return _rtn;
}
inline const BinaryData &Data::GetBinaryData () const
{
	_fmxcpt _x;
	const BinaryData &_rtn = FM_Data_GetBinaryData ( this, _x );
	_x.Check ();
	return _rtn;
}
inline const Locale &Data::GetLocale () const
{
	_fmxcpt _x;
	const Locale &_rtn = FM_Data_GetLocale ( this, _x );
	_x.Check ();
	return _rtn;
}
inline const Data::DataType Data::GetNativeType () const
{
	_fmxcpt _x;
	const DataType _rtn = static_cast<DataType>(FM_Data_GetNativeType ( this, _x ));
	_x.Check ();
	return _rtn;
}
inline void Data::ConvertData ( DataType nativeType )
{
	_fmxcpt _x;
	FM_Data_ConvertData ( this, nativeType, _x );
	_x.Check ();
}
inline errcode Data::SetAsText ( const Text &textData, const Locale &sourceLocale, const DataType nativeType )
{
	_fmxcpt _x;
	errcode _rtn = FM_Data_SetAsText ( this, textData, sourceLocale, nativeType, _x );
	_x.Check ();
	return _rtn;
}
inline void Data::SetAsNumber ( const FixPt &numericData, const DataType nativeType )
{
	_fmxcpt _x;
	FM_Data_SetAsNumber ( this, numericData, nativeType, _x );
	_x.Check ();
}
inline void Data::SetAsDate ( const DateTime &dateData, const DataType nativeType )
{
	_fmxcpt _x;
	FM_Data_SetAsDate ( this, dateData, nativeType, _x );
	_x.Check ();
}
inline void Data::SetAsTime ( const DateTime &timeData, const DataType nativeType )
{
	_fmxcpt _x;
	FM_Data_SetAsTime ( this, timeData, nativeType, _x );
	_x.Check ();
}
inline void Data::SetAsTimeStamp ( const DateTime &timeStampData, const DataType nativeType )
{
	_fmxcpt _x;
	FM_Data_SetAsTimeStamp ( this, timeStampData, nativeType, _x );
	_x.Check ();
}
inline void Data::SetDateTime ( const DateTime &dateTimeData, const DataType dateTimeType, const DataType nativeType )
{
	_fmxcpt _x;
	FM_Data_SetDateTime ( this, dateTimeData, dateTimeType, nativeType, _x );
	_x.Check ();
}
inline void Data::SetBinaryData ( const BinaryData &binaryData, bool forceBinaryNativeType )
{
	_fmxcpt _x;
	FM_Data_SetBinaryData ( this, binaryData, forceBinaryNativeType, _x );
	_x.Check ();
}
inline void Data::operator delete ( void *obj )
{
	_fmxcpt _x;
	FM_Data_Delete ( obj, _x );
	_x.Check ();
}

inline CharacterStyle::FontID Data::GetFontID( const Text &fontName, CharacterStyle::FontScript fontScript, const ExprEnv &env )
{
	_fmxcpt _x;
	CharacterStyle::FontID _rtn = FM_Data_GetFontID ( this, fontName, fontScript, env, _x );
	_x.Check ();
	return _rtn;
}
inline bool Data::GetFontInfo( CharacterStyle::FontID font, Text &fontName, CharacterStyle::FontScript &fontScript, const ExprEnv &env ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Data_GetFontInfo ( this, font, fontName, fontScript, env, _x );
	_x.Check ();
	return _rtn;
}

//----------------------------------------------------------------------------------------
inline const QuadChar &QuadChar::operator = ( const QuadChar &value )
{
	_fmxcpt _x;
	const QuadChar &_rtn = FM_QuadChar_operatorAS ( this, value, _x );
	_x.Check ();
	return _rtn;
}
inline uchar QuadChar::operator [] ( int i )
{
	_fmxcpt _x;
	uchar _rtn = FM_QuadChar_operatorAR ( this, i, _x );
	_x.Check ();
	return _rtn;
}
inline const uchar QuadChar::operator [] ( int i ) const
{
	_fmxcpt _x;
	const uchar _rtn = FM_QuadChar_operatorCAR ( this, i, _x );
	_x.Check ();
	return _rtn;
}
inline bool QuadChar::operator == ( const QuadChar &value ) const
{
	_fmxcpt _x;
	bool _rtn = FM_QuadChar_operatorEQ ( this, value, _x );
	_x.Check ();
	return _rtn;
}
inline bool QuadChar::operator != ( const QuadChar &value ) const
{
	_fmxcpt _x;
	bool _rtn = FM_QuadChar_operatorNE ( this, value, _x );
	_x.Check ();
	return _rtn;
}
inline bool QuadChar::operator < ( const QuadChar &value ) const
{
	_fmxcpt _x;
	bool _rtn = FM_QuadChar_operatorLT ( this, value, _x );
	_x.Check ();
	return _rtn;
}
inline bool QuadChar::operator <= ( const QuadChar &value ) const
{
	_fmxcpt _x;
	bool _rtn = FM_QuadChar_operatorLE ( this, value, _x );
	_x.Check ();
	return _rtn;
}
inline bool QuadChar::operator > ( const QuadChar &value ) const
{
	_fmxcpt _x;
	bool _rtn = FM_QuadChar_operatorGT ( this, value, _x );
	_x.Check ();
	return _rtn;
}
inline bool QuadChar::operator >= ( const QuadChar &value ) const
{
	_fmxcpt _x;
	bool _rtn = FM_QuadChar_operatorGE ( this, value, _x );
	_x.Check ();
	return _rtn;
}
inline uint32 QuadChar::GetMacType () const
{
	_fmxcpt _x;
	uint32 _rtn = FM_QuadChar_GetMacType ( this, _x );
	_x.Check ();
	return _rtn;
}
inline void QuadChar::SetMacType ( uint32 value )
{
	_fmxcpt _x;
	FM_QuadChar_SetMacType ( this, value, _x );
	_x.Check ();
}
inline void QuadChar::operator delete ( void *obj )
{
	_fmxcpt _x;
	FM_QuadChar_Delete ( obj, _x );
	_x.Check ();
}

//----------------------------------------------------------------------------------------
inline Locale &Locale::operator = ( const Locale &rhs )
{
	_fmxcpt _x;
	Locale &_rtn = FM_Locale_operatorAS ( this, rhs, _x );
	_x.Check ();
	return _rtn;
}
inline void Locale::operator delete ( void *obj )
{
	_fmxcpt _x;
	FM_Locale_Delete ( obj, _x );
	_x.Check ();
}

//----------------------------------------------------------------------------------------
inline uint32 DataVect::Size () const
{
    _fmxcpt _x;
    uint32 _rtn = FM_DataVect_Size ( this, _x );
    _x.Check ();
    return _rtn;
}
inline void DataVect::Clear ()
{
    _fmxcpt _x;
    FM_DataVect_Clear ( this, _x );
    _x.Check ();
}
inline bool DataVect::IsEmpty () const
{
    _fmxcpt _x;
    bool _rtn = FM_DataVect_IsEmpty ( this, _x );
    _x.Check ();
    return _rtn;
}
inline void DataVect::PushBack ( const Data &data )
{
    _fmxcpt _x;
    FM_DataVect_PushBack ( this, data, _x );
    _x.Check ();
}
inline Data *DataVect::PopBack ()
{
    _fmxcpt _x;
    Data *_rtn = FM_DataVect_PopBack ( this, _x );
    _x.Check ();
    return _rtn;
}
inline const Data &DataVect::At ( uint32 position ) const
{
    _fmxcpt _x;
    const Data &_rtn = FM_DataVect_At ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline const Text &DataVect::AtAsText ( uint32 position ) const
{
    _fmxcpt _x;
    const Text &_rtn = FM_DataVect_AtAsText ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline const FixPt &DataVect::AtAsNumber ( uint32 position ) const
{
    _fmxcpt _x;
    const FixPt &_rtn = FM_DataVect_AtAsNumber ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline const DateTime &DataVect::AtAsDate ( uint32 position ) const
{
    _fmxcpt _x;
    const DateTime &_rtn = FM_DataVect_AtAsDate ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline const DateTime &DataVect::AtAsTime ( uint32 position ) const
{
    _fmxcpt _x;
    const DateTime &_rtn = FM_DataVect_AtAsTime ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline const DateTime &DataVect::AtAsTimeStamp ( uint32 position ) const
{
    _fmxcpt _x;
    const DateTime &_rtn = FM_DataVect_AtAsTimeStamp ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline bool DataVect::AtAsBoolean ( uint32 position ) const
{
    _fmxcpt _x;
    bool _rtn = FM_DataVect_AtAsBoolean ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline const BinaryData &DataVect::AtAsBinaryData ( uint32 position ) const
{
    _fmxcpt _x;
    const BinaryData &_rtn = FM_DataVect_AtAsBinaryData ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline void DataVect::operator delete ( void *obj )
{
    _fmxcpt _x;
    FM_DataVect_Delete ( obj, _x );
    _x.Check ();
}

//----------------------------------------------------------------------------------------
inline uint32 RowVect::Size () const
{
    _fmxcpt _x;
    uint32 _rtn = FM_RowVect_Size ( this, _x );
    _x.Check ();
    return _rtn;
}
inline bool RowVect::IsEmpty () const
{
    _fmxcpt _x;
    bool _rtn = FM_RowVect_IsEmpty ( this, _x );
    _x.Check ();
    return _rtn;
}
inline const DataVect &RowVect::At ( uint32 position ) const
{
    _fmxcpt _x;
    const DataVect &_rtn = FM_RowVect_At ( this, position, _x );
    _x.Check ();
    return _rtn;
}
inline void RowVect::operator delete ( void *obj )
{
    _fmxcpt _x;
    FM_RowVect_Delete ( obj, _x );
    _x.Check ();
}

//----------------------------------------------------------------------------------------
inline errcode ExprEnv::Evaluate ( const Text &expression, Data &result ) const
{
    _fmxcpt _x;
    errcode _rtn = FM_ExprEnv_Evaluate ( this, expression, result, _x );
    _x.Check ();
    return _rtn;
}
inline errcode ExprEnv::ExecuteFileSQL ( const Text &expression, const Text &filename, const DataVect &parameters, RowVect& result ) const
{
    _fmxcpt _x;
    errcode _rtn = FM_ExprEnv_ExecuteFileSQL ( this, expression, filename, parameters, result, _x );
    _x.Check ();
    return _rtn;
}
inline errcode ExprEnv::RegisterExternalFunction ( const QuadChar &pluginId, short functionId, const Text &functionName, const Text &functionPrototype, short minArgs, short maxArgs, uint32 typeFlags, ExtPluginType funcPtr )
{
    _fmxcpt _x;
    errcode _rtn = FM_ExprEnv_RegisterExternalFunction ( pluginId, functionId, functionName, functionPrototype, minArgs, maxArgs, typeFlags, funcPtr, _x );
    _x.Check ();
    return _rtn;
}
inline errcode ExprEnv::UnRegisterExternalFunction ( const QuadChar &pluginId, short functionId )
{
    _fmxcpt _x;
    errcode _rtn = FM_ExprEnv_UnRegisterExternalFunction ( pluginId, functionId, _x );
    _x.Check ();
    return _rtn;
}
inline void ExprEnv::operator delete ( void *obj )
{
    _fmxcpt _x;
    FM_ExprEnv_Delete ( obj, _x );
    _x.Check ();
}

//----------------------------------------------------------------------------------------
inline bool DateTime::operator == ( const DateTime &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_DateTime_operatorEQ ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool DateTime::operator != ( const DateTime &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_DateTime_operatorNE ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool DateTime::IsLeapYear () const
{
	_fmxcpt _x;
	bool _rtn = FM_DateTime_IsLeapYear ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int16 DateTime::DayOfWeek () const
{
	_fmxcpt _x;
	int16 _rtn = FM_DateTime_DayOfWeek ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int16 DateTime::DayOfYear () const
{
	_fmxcpt _x;
	int16 _rtn = FM_DateTime_DayOfYear ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int16 DateTime::WeekOfYear () const
{
	_fmxcpt _x;
	int16 _rtn = FM_DateTime_WeekOfYear ( this, _x );
	_x.Check ();
	return _rtn;
}
inline void DateTime::Now ()
{
	_fmxcpt _x;
	FM_DateTime_Now ( this, _x );
	_x.Check ();
}
inline void DateTime::SetDate ( const DateTime &datetime )
{
	_fmxcpt _x;
	FM_DateTime_SetDate ( this, datetime, _x );
	_x.Check ();
}
inline errcode DateTime::SetNormalizedDate ( int16 month, int16 day, int16 year )
{
	_fmxcpt _x;
	errcode _rtn = FM_DateTime_SetNormalizedDate1 ( this, month, day, year, _x );
	_x.Check ();
	return _rtn;
}
inline errcode DateTime::SetNormalizedDate ( const FixPt &year, const FixPt &month, const FixPt &day )
{
	_fmxcpt _x;
	errcode _rtn = FM_DateTime_SetNormalizedDate2 ( this, year, month, day, _x );
	_x.Check ();
	return _rtn;
}
inline void DateTime::SetDaysSinceEpoch ( int64 days )
{
	_fmxcpt _x;
	FM_DateTime_SetDaysSinceEpoch ( this, days, _x );
	_x.Check ();
}
inline void DateTime::SetTime ( const DateTime &datetime )
{
	_fmxcpt _x;
	FM_DateTime_SetTime ( this, datetime, _x );
	_x.Check ();
}
inline errcode DateTime::SetNormalizedTime ( int64 hour, int16 minute, int16 sec, int32 usec )
{
	_fmxcpt _x;
	errcode _rtn = FM_DateTime_SetNormalizedTime1 ( this, hour, minute, sec, usec, _x );
	_x.Check ();
	return _rtn;
}
inline errcode DateTime::SetNormalizedTime ( const FixPt &hour, const FixPt &minute, const FixPt &sec )
{
	_fmxcpt _x;
	errcode _rtn = FM_DateTime_SetNormalizedTime2 ( this, hour, minute, sec, _x );
	_x.Check ();
	return _rtn;
}
inline void DateTime::SetSecsSinceMidnight ( const FixPt &secs )
{
	_fmxcpt _x;
	FM_DateTime_SetSecsSinceMidnight ( this, secs, _x );
	_x.Check ();
}
inline void DateTime::SetSecondsSinceEpoch ( const FixPt &seconds )
{
	_fmxcpt _x;
	FM_DateTime_SetSecondsSinceEpoch ( this, seconds, _x );
	_x.Check ();
}
inline int16 DateTime::GetYear () const
{
	_fmxcpt _x;
	int16 _rtn = FM_DateTime_GetYear ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int16 DateTime::GetMonth () const
{
	_fmxcpt _x;
	int16 _rtn = FM_DateTime_GetMonth ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int16 DateTime::GetDay () const
{
	_fmxcpt _x;
	int16 _rtn = FM_DateTime_GetDay ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int32 DateTime::GetDaysSinceEpoch () const
{
	_fmxcpt _x;
	int32 _rtn = FM_DateTime_GetDaysSinceEpoch ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int32 DateTime::GetHour () const
{
	_fmxcpt _x;
	int32 _rtn = FM_DateTime_GetHour ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int16 DateTime::GetMinute () const
{
	_fmxcpt _x;
	int16 _rtn = FM_DateTime_GetMinute ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int16 DateTime::GetSec () const
{
	_fmxcpt _x;
	int16 _rtn = FM_DateTime_GetSec ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int32 DateTime::GetUSec () const
{
	_fmxcpt _x;
	int32 _rtn = FM_DateTime_GetUSec ( this, _x );
	_x.Check ();
	return _rtn;
}
inline void DateTime::GetSeconds ( FixPt &results ) const
{
	_fmxcpt _x;
	FM_DateTime_GetSeconds ( this, results, _x );
	_x.Check ();
}
inline void DateTime::GetSecsSinceMidnight ( FixPt &results ) const
{
	_fmxcpt _x;
	FM_DateTime_GetSecsSinceMidnight ( this, results, _x );
	_x.Check ();
}
inline void DateTime::GetSecondsSinceEpoch ( FixPt &results ) const
{
	_fmxcpt _x;
	FM_DateTime_GetSecondsSinceEpoch ( this, results, _x );
	_x.Check ();
}
inline void DateTime::operator delete ( void *obj )
{
	_fmxcpt _x;
	FM_DateTime_Delete ( obj, _x );
	_x.Check ();
}

//----------------------------------------------------------------------------------------
inline void FixPt::AssignInt ( int32 that )
{
	_fmxcpt _x;
	FM_FixPt_AssignInt ( this, that, _x );
	_x.Check ();
}
inline void FixPt::AssignDouble ( double that )
{
	_fmxcpt _x;
	FM_FixPt_AssignDouble ( this, that, _x );
	_x.Check ();
}
inline void FixPt::AssignFixPt ( const FixPt &that )
{
	_fmxcpt _x;
	FM_FixPt_AssignFixPt ( this, that, _x );
	_x.Check ();
}
inline bool FixPt::operator == ( const FixPt &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_FixPt_operatorEQ ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool FixPt::operator != ( const FixPt &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_FixPt_operatorNE ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool FixPt::operator < ( const FixPt &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_FixPt_operatorLT ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool FixPt::operator <= ( const FixPt &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_FixPt_operatorLE ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool FixPt::operator > ( const FixPt &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_FixPt_operatorGT ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool FixPt::operator >= ( const FixPt &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_FixPt_operatorGE ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline void FixPt::Increment ( int32 n )
{
	_fmxcpt _x;
	FM_FixPt_Increment ( this, n, _x );
	_x.Check ();
}
inline void FixPt::Decrement ( int32 n )
{
	_fmxcpt _x;
	FM_FixPt_Decrement ( this, n, _x );
	_x.Check ();
}
inline void FixPt::Negate ()
{
	_fmxcpt _x;
	FM_FixPt_Negate ( this, _x );
	_x.Check ();
}
inline int FixPt::GetPrecision () const
{
	_fmxcpt _x;
	int _rtn = FM_FixPt_GetPrecision ( this, _x );
	_x.Check ();
	return _rtn;
}
inline void FixPt::SetPrecision ( int precision )
{
	_fmxcpt _x;
	FM_FixPt_SetPrecision ( this, precision, _x );
	_x.Check ();
}
inline void FixPt::Add ( const FixPt &arg, FixPt &result ) const
{
	_fmxcpt _x;
	FM_FixPt_Add ( this, arg, result, _x );
	_x.Check ();
}
inline void FixPt::Subtract ( const FixPt &arg, FixPt &result ) const
{
	_fmxcpt _x;
	FM_FixPt_Subtract ( this, arg, result, _x );
	_x.Check ();
}
inline void FixPt::Multiply ( const FixPt &arg, FixPt &result ) const
{
	_fmxcpt _x;
	FM_FixPt_Multiply ( this, arg, result, _x );
	_x.Check ();
}
inline errcode FixPt::Divide ( const FixPt &arg, FixPt &result ) const
{
	_fmxcpt _x;
	errcode _rtn = FM_FixPt_Divide ( this, arg, result, _x );
	_x.Check ();
	return _rtn;
}
inline bool FixPt::AsBool () const
{
	_fmxcpt _x;
	bool _rtn = FM_FixPt_AsBool ( this, _x );
	_x.Check ();
	return _rtn;
}
inline int32 FixPt::AsLong () const
{
	_fmxcpt _x;
	int32 _rtn = FM_FixPt_AsLong ( this, _x );
	_x.Check ();
	return _rtn;
}
inline double FixPt::AsFloat () const
{
	_fmxcpt _x;
	double _rtn = FM_FixPt_AsFloat ( this, _x );
	_x.Check ();
	return _rtn;
}
inline void FixPt::operator delete ( void *obj )
{
	_fmxcpt _x;
	FM_FixPt_Delete ( obj, _x );
	_x.Check ();
}

//----------------------------------------------------------------------------------------
inline uint32 Text::GetSize () const
{
	_fmxcpt _x;
	uint32 _rtn = FM_Text_GetSize ( this, _x );
	_x.Check ();
	return _rtn;
}
inline void Text::Assign ( const char *s, Encoding encoding )
{
	_fmxcpt _x;
	FM_Text_Assign ( this, s, encoding, _x );
	_x.Check ();
}
inline void Text::GetBytes ( char *buffer, uint32 buffersize, uint32 position, uint32 size, Encoding encoding ) const
{
	_fmxcpt _x;
	FM_Text_GetBytes ( this, buffer, buffersize, position, size, encoding, _x );
	_x.Check ();
}
inline void Text::AssignWide ( const wchar_t *s )
{
	_fmxcpt _x;
	FM_Text_AssignWide ( this, s, _x );
	_x.Check ();
}
inline void Text::AssignUnicode ( const uint16 *s )
{
	_fmxcpt _x;
	FM_Text_AssignUnicode ( this, s, _x );
	_x.Check ();
}
inline void Text::AssignWithLength ( const char *s, uint32 strlength, Encoding encoding )
{
	_fmxcpt _x;
	FM_Text_AssignWithLength ( this, s, strlength, encoding, _x );
	_x.Check ();
}
inline void Text::AssignWideWithLength ( const wchar_t *s, uint32 strlength )
{
	_fmxcpt _x;
	FM_Text_AssignWideWithLength ( this, s, strlength, _x );
	_x.Check ();
}
inline void Text::AssignUnicodeWithLength ( const uint16 *s, uint32 strlength )
{
	_fmxcpt _x;
	FM_Text_AssignUnicodeWithLength ( this, s, strlength, _x );
	_x.Check ();
}
inline void Text::GetUnicode ( uint16 *s, uint32 position, uint32 size ) const
{
	_fmxcpt _x;
	FM_Text_GetUnicode ( this, s, position, size, _x );
	_x.Check ();
}
inline void Text::SetText ( const Text &other, uint32 position, uint32 size )
{
	_fmxcpt _x;
	FM_Text_SetText ( this, other, position, size, _x );
	_x.Check ();
}
inline void Text::AppendText ( const Text &other, uint32 position, uint32 size )
{
	_fmxcpt _x;
	FM_Text_AppendText ( this, other, position, size, _x );
	_x.Check ();
}
inline void Text::InsertText ( const Text &other, uint32 position )
{
	_fmxcpt _x;
	FM_Text_InsertText ( this, other, position, _x );
	_x.Check ();
}
inline void Text::DeleteText ( uint32 positionToDelete, uint32 sizeToDelete )
{
	_fmxcpt _x;
	FM_Text_DeleteText ( this, positionToDelete, sizeToDelete, _x );
	_x.Check ();
}
inline uint32 Text::Find ( const Text &other, uint32 position ) const
{
	_fmxcpt _x;
	uint32 _rtn = FM_Text_Find ( this, other, position, _x );
	_x.Check ();
	return _rtn;
}
inline uint32 Text::FindPrev ( const Text &other, uint32 position ) const
{
	_fmxcpt _x;
	uint32 _rtn = FM_Text_FindPrev ( this, other, position, _x );
	_x.Check ();
	return _rtn;
}
inline uint32 Text::FindIgnoringCase ( const Text &other, uint32 position ) const
{
	_fmxcpt _x;
	uint32 _rtn = FM_Text_FindIgnoringCase ( this, other, position, _x );
	_x.Check ();
	return _rtn;
}
inline uint32 Text::FindPrevIgnoringCase ( const Text &other, uint32 position ) const
{
	_fmxcpt _x;
	uint32 _rtn = FM_Text_FindPrevIgnoringCase ( this, other, position, _x );
	_x.Check ();
	return _rtn;
}
inline void Text::Uppercase ()
{
	_fmxcpt _x;
	FM_Text_Uppercase ( this, _x );
	_x.Check ();
}
inline void Text::Lowercase ()
{
	_fmxcpt _x;
	FM_Text_Lowercase ( this, _x );
	_x.Check ();
}
inline void Text::GetStyle( CharacterStyle& style, uint32 position ) const
{
	_fmxcpt _x;
	FM_Text_GetStyle ( this, style, position, _x );
	_x.Check ();
}
inline void Text::GetDefaultStyle( CharacterStyle& style ) const
{
	_fmxcpt _x;
	FM_Text_GetDefaultStyle ( this, style, _x );
	_x.Check ();
}
inline void Text::SetStyle( const CharacterStyle& style, uint32 position, uint32 size )
{
	_fmxcpt _x;
	FM_Text_SetStyle ( this, style, position, size, _x );
	_x.Check ();
}
inline void Text::RemoveStyle( const CharacterStyle& style )
{
	_fmxcpt _x;
	FM_Text_RemoveStyle ( this, style, _x );
	_x.Check ();
}
inline void Text::ResetAllStyleBuffers()
{
	_fmxcpt _x;
	FM_Text_ResetAllStyleBuffers ( this, _x );
	_x.Check ();
}
inline bool Text::operator == ( const Text &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Text_operatorEQ ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool Text::operator != ( const Text &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Text_operatorNE ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool Text::operator < ( const Text &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Text_operatorLT ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool Text::operator <= ( const Text &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Text_operatorLE ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool Text::operator > ( const Text &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Text_operatorGT ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline bool Text::operator >= ( const Text &that ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Text_operatorGE ( this, that, _x );
	_x.Check ();
	return _rtn;
}
inline void Text::operator delete ( void *obj )
{
	_fmxcpt _x;
	FM_Text_Delete ( obj, _x );
	_x.Check ();
}

//----------------------------------------------------------------------------------------
inline void CharacterStyle::Color::SetRed ( ColorChannel r )
{
	_fmxcpt _x;
	FM_Color_SetRed ( this, r, _x );
	_x.Check ();
}
inline void CharacterStyle::Color::SetGreen ( ColorChannel g )
{
	_fmxcpt _x;
	FM_Color_SetGreen ( this, g, _x );
	_x.Check ();
}
inline void CharacterStyle::Color::SetBlue ( ColorChannel b )
{
	_fmxcpt _x;
	FM_Color_SetBlue ( this, b, _x );
	_x.Check ();
}
inline void CharacterStyle::Color::SetAlpha ( ColorChannel a )
{
	_fmxcpt _x;
	FM_Color_SetAlpha ( this, a, _x );
	_x.Check ();
}
inline CharacterStyle::ColorChannel CharacterStyle::Color::GetRed () const
{
	_fmxcpt _x;
	ColorChannel _rtn = FM_Color_GetRed ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::ColorChannel CharacterStyle::Color::GetGreen () const
{
	_fmxcpt _x;
	ColorChannel _rtn = FM_Color_GetGreen ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::ColorChannel CharacterStyle::Color::GetBlue () const
{
	_fmxcpt _x;
	ColorChannel _rtn = FM_Color_GetBlue ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::ColorChannel CharacterStyle::Color::GetAlpha () const
{
	_fmxcpt _x;
	ColorChannel _rtn = FM_Color_GetAlpha ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::ColorChannel16 CharacterStyle::Color::GetRedUpsample () const
{
	_fmxcpt _x;
	ColorChannel16 _rtn = FM_Color_GetRedUpsample ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::ColorChannel16 CharacterStyle::Color::GetGreenUpsample () const
{
	_fmxcpt _x;
	ColorChannel16 _rtn = FM_Color_GetGreenUpsample ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::ColorChannel16 CharacterStyle::Color::GetBlueUpsample () const
{
	_fmxcpt _x;
	ColorChannel16 _rtn = FM_Color_GetBlueUpsample ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::ColorChannel16 CharacterStyle::Color::GetAlphaUpsample () const
{
	_fmxcpt _x;
	ColorChannel16 _rtn = FM_Color_GetAlphaUpsample ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::Color &CharacterStyle::Color::operator = ( const Color &rhs )
{
	_fmxcpt _x;
	Color &_rtn = FM_Color_operatorAS ( this, rhs, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::Color::operator == ( const Color &rhs ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Color_operatorEQ ( this, rhs, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::Color::operator != ( const Color &rhs ) const
{
	_fmxcpt _x;
	bool _rtn = FM_Color_operatorNE ( this, rhs, _x );
	_x.Check ();
	return _rtn;
}
inline const CharacterStyle::Color &CharacterStyle::Color::White () throw ()
{
	return FM_Color_White ();
}
inline const CharacterStyle::Color &CharacterStyle::Color::Black () throw ()
{
	return FM_Color_Black ();
}
inline void CharacterStyle::Color::operator delete ( void *obj )
{
	_fmxcpt _x;
	FM_Color_Delete ( obj, _x );
	_x.Check ();
}
inline void CharacterStyle::EnableFont ()
{
	_fmxcpt _x;
	FM_CharacterStyle_EnableFont ( this, _x );
	_x.Check ();
}
inline void CharacterStyle::EnableFace ( Face face )
{
	_fmxcpt _x;
	FM_CharacterStyle_EnableFace ( this, face, _x );
	_x.Check ();
}
inline void CharacterStyle::EnableSize ()
{
	_fmxcpt _x;
	FM_CharacterStyle_EnableSize ( this, _x );
	_x.Check ();
}
inline void CharacterStyle::EnableColor ()
{
	_fmxcpt _x;
	FM_CharacterStyle_EnableColor ( this, _x );
	_x.Check ();
}
inline void CharacterStyle::DisableFont ()
{
	_fmxcpt _x;
	FM_CharacterStyle_DisableFont ( this, _x );
	_x.Check ();
}
inline void CharacterStyle::DisableFace ( Face face )
{
	_fmxcpt _x;
	FM_CharacterStyle_DisableFace ( this, face, _x );
	_x.Check ();
}
inline void CharacterStyle::DisableAllFaces ()
{
	_fmxcpt _x;
	FM_CharacterStyle_DisableAllFaces ( this, _x );
	_x.Check ();
}
inline void CharacterStyle::DisableSize ()
{
	_fmxcpt _x;
	FM_CharacterStyle_DisableSize ( this, _x );
	_x.Check ();
}
inline void CharacterStyle::DisableColor ()
{
	_fmxcpt _x;
	FM_CharacterStyle_DisableColor ( this, _x );
	_x.Check ();
}
inline void CharacterStyle::DisableAll ()
{
	_fmxcpt _x;
	FM_CharacterStyle_DisableAll ( this, _x );
	_x.Check ();
}
inline bool CharacterStyle::IsAllDisabled () const
{
	_fmxcpt _x;
	bool _rtn = FM_CharacterStyle_IsAllDisabled ( this, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::IsFontEnabled () const
{
	_fmxcpt _x;
	bool _rtn = FM_CharacterStyle_IsFontEnabled ( this, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::IsFaceEnabled ( Face face ) const
{
	_fmxcpt _x;
	bool _rtn = FM_CharacterStyle_IsFaceEnabled ( this, face, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::IsAnyFaceEnabled () const
{
	_fmxcpt _x;
	bool _rtn = FM_CharacterStyle_IsAnyFaceEnabled ( this, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::IsSizeEnabled () const
{
	_fmxcpt _x;
	bool _rtn = FM_CharacterStyle_IsSizeEnabled ( this, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::IsColorEnabled () const
{
	_fmxcpt _x;
	bool _rtn = FM_CharacterStyle_IsColorEnabled ( this, _x );
	_x.Check ();
	return _rtn;
}

inline void CharacterStyle::SetFontInformation ( FontID font, Face face, FontSize size )
{
	_fmxcpt _x;
	FM_CharacterStyle_SetFontInformation ( this, font, face, size, _x );
	_x.Check ();
}
inline void CharacterStyle::GetFontInformation ( FontID &font, Face &face, FontSize &size )
{
	_fmxcpt _x;
	FM_CharacterStyle_GetFontInformation ( this, font, face, size, _x );
	_x.Check ();
}
inline void CharacterStyle::SetFont ( FontID font )
{
	_fmxcpt _x;
	FM_CharacterStyle_SetFont ( this, font, _x );
	_x.Check ();
}
inline void CharacterStyle::SetFace ( Face face )
{
	_fmxcpt _x;
	FM_CharacterStyle_SetFace ( this, face, _x );
	_x.Check ();
}
inline void CharacterStyle::SetSize ( FontSize size )
{
	_fmxcpt _x;
	FM_CharacterStyle_SetSize ( this, size, _x );
	_x.Check ();
}
inline CharacterStyle::FontID CharacterStyle::GetFont () const
{
	_fmxcpt _x;
	FontID _rtn = FM_CharacterStyle_GetFont ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::Face CharacterStyle::GetFace () const
{
	_fmxcpt _x;
	Face _rtn = FM_CharacterStyle_GetFace ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle::FontSize CharacterStyle::GetSize () const
{
	_fmxcpt _x;
	FontSize _rtn = FM_CharacterStyle_GetSize ( this, _x );
	_x.Check ();
	return _rtn;
}
inline void CharacterStyle::SetColor ( const Color &color )
{
	_fmxcpt _x;
	FM_CharacterStyle_SetColor ( this, color, _x );
	_x.Check ();
}
inline const CharacterStyle::Color &CharacterStyle::GetColor () const
{
	_fmxcpt _x;
	const Color &_rtn = FM_CharacterStyle_GetColor ( this, _x );
	_x.Check ();
	return _rtn;
}
inline CharacterStyle &CharacterStyle::operator = ( const CharacterStyle &rhs )
{
	_fmxcpt _x;
	CharacterStyle &_rtn = FM_CharacterStyle_operatorAS ( this, rhs, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::operator == ( const CharacterStyle &rhs ) const
{
	_fmxcpt _x;
	bool _rtn = FM_CharacterStyle_operatorEQ ( this, rhs, _x );
	_x.Check ();
	return _rtn;
}
inline bool CharacterStyle::operator != ( const CharacterStyle &rhs ) const
{
	_fmxcpt _x;
	bool _rtn = FM_CharacterStyle_operatorNE ( this, rhs, _x );
	_x.Check ();
	return _rtn;
}
inline void CharacterStyle::operator delete ( void *obj )
{
	_fmxcpt _x;
	FM_CharacterStyle_Delete ( obj, _x );
	_x.Check ();
}

//----------------------------------------------------------------------------------------
inline BinaryData &BinaryData::operator = ( const BinaryData &source )
{
    _fmxcpt _x;
    BinaryData &_rtn = FM_BinaryData_operatorAS ( this, source, _x );
    _x.Check ();
    return _rtn;
}
inline bool BinaryData::operator == ( const BinaryData &compareData ) const
{
    _fmxcpt _x;
    bool _rtn = FM_BinaryData_operatorEQ ( this, compareData, _x );
    _x.Check ();
    return _rtn;
}
inline bool BinaryData::operator != ( const BinaryData &compareData ) const
{
    _fmxcpt _x;
    bool _rtn = FM_BinaryData_operatorNE ( this, compareData, _x );
    _x.Check ();
    return _rtn;
}
inline int32 BinaryData::GetCount () const
{
    _fmxcpt _x;
    int32 _rtn = FM_BinaryData_GetCount ( this, _x );
    _x.Check ();
    return _rtn;
}
inline int32 BinaryData::GetIndex ( const QuadChar& dataType ) const
{
    _fmxcpt _x;
    int32 _rtn = FM_BinaryData_GetIndex ( this, dataType, _x );
    _x.Check ();
    return _rtn;
}
inline uint32 BinaryData::GetTotalSize () const
{
    _fmxcpt _x;
    uint32 _rtn = FM_BinaryData_GetTotalSize ( this, _x );
    _x.Check ();
    return _rtn;
}
inline void BinaryData::GetType ( int32 index, QuadChar& dataType ) const
{
    _fmxcpt _x;
    FM_BinaryData_GetType ( this, index, dataType, _x );
    _x.Check ();
}
inline uint32 BinaryData::GetSize ( int32 index ) const
{
    _fmxcpt _x;
    uint32 _rtn = FM_BinaryData_GetSize ( this, index, _x );
    _x.Check ();
    return _rtn;
}
inline errcode BinaryData::GetData ( int32 index, uint32 offset, uint32 amount, void *buffer ) const
{
    _fmxcpt _x;
    errcode _rtn = FM_BinaryData_GetData ( this, index, offset, amount, buffer, _x );
    _x.Check ();
    return _rtn;
}
inline errcode BinaryData::Add ( const QuadChar& dataType, uint32 amount, void *buffer )
{
    _fmxcpt _x;
    errcode _rtn = FM_BinaryData_Add ( this, dataType, amount, buffer, _x );
    _x.Check ();
    return _rtn;
}
inline bool BinaryData::Remove ( const QuadChar& dataType )
{
    _fmxcpt _x;
    bool _rtn = FM_BinaryData_Remove ( this, dataType, _x );
    _x.Check ();
    return _rtn;
}
inline void BinaryData::RemoveAll ()
{
    _fmxcpt _x;
    FM_BinaryData_RemoveAll ( this, _x );
    _x.Check ();
}
inline void BinaryData::operator delete ( void *obj )
{
    _fmxcpt _x;
    FM_BinaryData_Delete ( obj, _x );
    _x.Check ();
}
inline errcode BinaryData::GetFNAMData ( Text &filepathlist ) const
{
    _fmxcpt _x;
    errcode _rtn = FM_BinaryData_GetFNAMData ( this, filepathlist, _x );
    _x.Check ();
    return _rtn;
}
inline errcode BinaryData::AddFNAMData ( const Text &filepathlist )
{
    _fmxcpt _x;
    errcode _rtn = FM_BinaryData_AddFNAMData ( this, filepathlist, _x );
    _x.Check ();
    return _rtn;
}
inline errcode BinaryData::GetSIZEData ( short &width, short &height ) const
{
    _fmxcpt _x;
    errcode _rtn = FM_BinaryData_GetSIZEData ( this, width, height, _x );
    _x.Check ();
    return _rtn;
}
inline errcode BinaryData::AddSIZEData ( short width, short height )
{
    _fmxcpt _x;
    errcode _rtn = FM_BinaryData_AddSIZEData ( this, width, height, _x );
    _x.Check ();
    return _rtn;
}


//----------------------------------------------------------------------------------------
// AUTOPTR
//----------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------
inline DataAutoPtr::DataAutoPtr ()
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_Data_Constructor1 ( _x ) );
	_x.Check ();
}
//----------------------------------------------------------------------------------------
inline QuadCharAutoPtr::QuadCharAutoPtr ()
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_QuadChar_Constructor1 ( _x ) );
	_x.Check ();
}
inline QuadCharAutoPtr::QuadCharAutoPtr ( char c0, char c1, char c2, char c3 )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_QuadChar_Constructor2 ( c0, c1, c2, c3, _x ) );
	_x.Check ();
}
inline QuadCharAutoPtr::QuadCharAutoPtr ( const QuadChar &value )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_QuadChar_Constructor3 ( value, _x ) );
	_x.Check ();
}
//----------------------------------------------------------------------------------------
inline LocaleAutoPtr::LocaleAutoPtr ( Locale::Type inputType )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_Locale_Constructor1 ( inputType, _x ) );
	_x.Check ();
}
inline LocaleAutoPtr::LocaleAutoPtr ( const Locale &copyConstruct )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_Locale_Constructor2 ( copyConstruct, _x ) );
	_x.Check ();
}
//----------------------------------------------------------------------------------------
inline DataVectAutoPtr::DataVectAutoPtr ()
{
    _fmxcpt _x;
    FMX_AUTOPTRRESET ( FM_DataVect_Constructor1 ( _x ) );
    _x.Check ();
}
//----------------------------------------------------------------------------------------
inline RowVectAutoPtr::RowVectAutoPtr ()
{
    _fmxcpt _x;
    FMX_AUTOPTRRESET ( FM_RowVect_Constructor1 ( _x ) );
    _x.Check ();
}
//----------------------------------------------------------------------------------------
inline ExprEnvAutoPtr::ExprEnvAutoPtr ()
{
    _fmxcpt _x;
    FMX_AUTOPTRRESET ( FM_ExprEnv_Constructor1 ( _x ) );
    _x.Check ();
}
//----------------------------------------------------------------------------------------
inline DateTimeAutoPtr::DateTimeAutoPtr ()
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_DateTime_Constructor1 ( _x ) );
	_x.Check ();
}
inline DateTimeAutoPtr::DateTimeAutoPtr ( const unichar* dateString, uint32 dateLength, const Locale& intl )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_DateTime_Constructor2 ( dateString, dateLength, intl, _x ) );
	_x.Check ();
}
inline DateTimeAutoPtr::DateTimeAutoPtr ( const Text &dateText, const Locale &intl )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_DateTime_Constructor3 ( dateText, intl, _x ) );
	_x.Check ();
}
//----------------------------------------------------------------------------------------
inline FixPtAutoPtr::FixPtAutoPtr ( int32 val, int precision )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_FixPt_Constructor1 ( val, precision, _x ) );
	_x.Check ();
}
inline FixPtAutoPtr::FixPtAutoPtr ( int32 val, const FixPt &precisionExample )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_FixPt_Constructor2 ( val, precisionExample, _x ) );
	_x.Check ();
}
//----------------------------------------------------------------------------------------
inline TextAutoPtr::TextAutoPtr ()
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_Text_Constructor1 ( _x ) );
	_x.Check ();
}
//----------------------------------------------------------------------------------------
inline ColorAutoPtr::ColorAutoPtr ()
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_Color_Constructor1 ( _x ) );
	_x.Check ();
}
inline ColorAutoPtr::ColorAutoPtr ( CharacterStyle::ColorChannel red, CharacterStyle::ColorChannel green, CharacterStyle::ColorChannel blue, CharacterStyle::ColorChannel alpha )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_Color_Constructor2 ( red, green, blue, alpha, _x ) );
	_x.Check ();
}
inline ColorAutoPtr::ColorAutoPtr ( const CharacterStyle::Color &color )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_Color_Constructor3 ( color, _x ) );
	_x.Check ();
}
//----------------------------------------------------------------------------------------
inline CharacterStyleAutoPtr::CharacterStyleAutoPtr ()
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_CharacterStyle_Constructor1 ( _x ) );
	_x.Check ();
}
inline CharacterStyleAutoPtr::CharacterStyleAutoPtr ( CharacterStyle::FontID font, CharacterStyle::Face face, CharacterStyle::FontSize size, const CharacterStyle::Color &color )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_CharacterStyle_Constructor2 ( font, face, size, color, _x ) );
	_x.Check ();
}
inline CharacterStyleAutoPtr::CharacterStyleAutoPtr ( const CharacterStyle &style )
{
	_fmxcpt _x;
	FMX_AUTOPTRRESET ( FM_CharacterStyle_Constructor3 ( style, _x ) );
	_x.Check ();
}
//----------------------------------------------------------------------------------------
inline BinaryDataAutoPtr::BinaryDataAutoPtr ()
{
    _fmxcpt _x;
    FMX_AUTOPTRRESET ( FM_BinaryData_Constructor1 ( _x ) );
    _x.Check ();
}
inline BinaryDataAutoPtr::BinaryDataAutoPtr ( const BinaryData &sourceData )
{
    _fmxcpt _x;
    FMX_AUTOPTRRESET ( FM_BinaryData_Constructor2 ( sourceData, _x ) );
    _x.Check ();
}

#endif
