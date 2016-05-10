#ifndef _JPG_FILE_H_
#define _JPG_FILE_H_

#include "sftime.h"

//--------------------------------------------------------------------------
#define MAX_COMMENT_SIZE 2000
#define MAX_DATE_COPIES 10

//--------------------------------------------------------------------------
class CJPGSection
{
public:
    uchar       *m_dataPtr;
    unsigned int m_dataSize;
    int          m_dataType;
	CJPGSection(void)
		{
			m_dataPtr  = NULL;
			m_dataSize = 0;
			m_dataType = 0;
		}
};

//--------------------------------------------------------------------------
class CJPGFile
{
public:
    SFString      m_Filename;
    SFInt32       m_FileSize;
	SFTime        m_fileTime;
	SFTime        m_exifTime;
    time_t        m_FileDateTime;
	SFString      m_DateTime;
	SFString      m_CameraMake;
    SFString      m_CameraModel;
    SFInt32       m_Height;
	SFInt32       m_Width;
    SFInt32       m_Orientation;
    SFBool        m_isBW;
    SFInt32       m_jpgProcess;
    SFInt32       m_FlashUsed;
    float         m_FocalLength;
    float         m_Exposure;
    float         m_fStop;
    float         m_Distance;
    float         m_CCDWidth;
    float         m_ExposureBias;
    float         m_DigitalZoomRatio;
    SFInt32       m_FocalLength35mmEquiv;
    SFInt32       m_Whitebalance;
    SFInt32       m_MeteringMode;
    SFInt32       m_ExposureProgram;
    SFInt32       m_ExposureMode;
    SFInt32       m_ISOequivalent;
    SFInt32       m_LightSource;
    SFInt32       m_DistanceRange;
    float         m_xResolution;
    float         m_yResolution;
    SFInt32       m_ResolutionUnit;
    char          m_Comments[MAX_COMMENT_SIZE];
    SFInt32       m_CommentWidthchars; // If nonzero, widechar comment, indicates number of chars.
    unsigned int  m_ThumbnailOffset;          // Exif offset to thumbnail
    unsigned int  m_ThumbnailSize;            // Size of thumbnail.
    unsigned int  m_LargestExifOffset;        // Last exif data referenced (to check if thumbnail is at end)
    char          m_ThumbnailAtEnd;              // Exif header ends with the thumbnail
    SFInt32       m_ThumbnailSizeOffset;
    SFInt32       m_DateTimeOffsets[MAX_DATE_COPIES];
    SFInt32       m_numDateTimeTags;
    char          m_Lattitude[31];
    char          m_Longitude[31];
    SFString      m_Altitude;
    SFInt32       m_QualityGuess;
	SFInt32       m_nAllocated;
	SFInt32       m_nSections;
	CJPGSection  *m_sections;
	SFBool        m_imageRead;

public:
	CJPGFile(const SFString& fileIn);
	~CJPGFile(void);

	SFBool   readJPG          ( SFInt32 readMode );
	SFBool   readSections     ( FILE *fp, SFInt32 readMode );

	void     processGPS       ( unsigned char * DirStart, unsigned char * OffsetBase, unsigned int ExifLength );
	void     writeJPG         ( void );
	int      processFile      ( SFBool toExif , SFBool fromExif );
	void     processExifDir   ( unsigned char * DirStart, unsigned char * OffsetBase, unsigned int ExifLength, int NestingLevel );
	void     process_EXIF     ( unsigned char *ExifSection, unsigned int length );
	void     processMakerNote ( unsigned char * ValuePtr, int ByteCount, unsigned char * OffsetBase, unsigned int ExifLength );
	void     process_DQT      ( const uchar * Data, int length );
	int      checkDates       ( void );
	void     dumpHeader       ( void );
	SFString fileTimeAsString ( void );
	void     growIfNeeded     ( void );
	SFBool   hasDiffDates     ( void );
	SFBool   hasBadDates      ( void );

private:
	CJPGFile  ( void )                 { }
	CJPGFile  ( const CJPGFile& file ) { }
	CJPGFile& operator= ( const CJPGFile& file ) { return *this; }
};


#endif
