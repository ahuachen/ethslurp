//--------------------------------------------------------------------------
#include "basetypes.h"
#include "jpg.h"
#include "jpg_internals.h"

//--------------------------------------------------------------------------
extern SFTime       exifAsSFTime (const char *timeIn);
extern int          Exif2tm      (struct tm * timeptr, const char * ExifTime);
extern double       convertVal   (const void * ValuePtr, int Format);
extern void         fatalError   (const SFString& msg, const SFString& fileName=nullString);
extern void         nonFatalError(const char * msg, int a1, int a2, const SFString& fileName=nullString);
extern int          Get16u       (const void *shortIn);
extern int          Get32s       (const void *longIn);
extern unsigned int Get32u       (const void *longIn);
extern void         Put16u       (void *shortIn, unsigned short putValue);
extern void         Put32u       (void *longIn, unsigned int putValue);
extern double       convertVal   (const void * ValuePtr, int Format);
extern SFString     copyStr      (void *valPtr, SFInt32 max);

//--------------------------------------------------------------------------
static double      FocalplaneXRes;
static double      FocalplaneUnits;
static int         ExifImageWidth;
int                MotorolaOrder = 0;

//--------------------------------------------------------------------------
CJPGFile::CJPGFile(const SFString& fileIn)
{
    memset(this, 0, sizeof(CJPGFile));
	m_Filename     = fileIn;
    m_FlashUsed    = -1;
    m_MeteringMode = -1;
    m_Whitebalance = -1;
}

//--------------------------------------------------------------------------
CJPGFile::~CJPGFile(void)
{
    for (int i=0;i<m_nSections;i++)
	{
		if (m_sections[i].m_dataPtr)
			free(m_sections[i].m_dataPtr);
		m_sections[i].m_dataPtr=NULL;
	}
}

//--------------------------------------------------------------------------
void fatalError(const SFString& msg, const SFString& fileName)
{
	CFileExportContext ctx(stderr);
	ctx << "\nError : " << msg << "in file '" << fileName << "'\n";
    exit(1);
}

//--------------------------------------------------------------------------
void nonFatalError(const char * msg, int a1, int a2, const SFString& fileName)
{
	CFileExportContext ctx(stderr);
	ctx << "\nNonfatal Error : " << msg << ":" << a1 << ":" << a2 << " in file '" << fileName << "'\n";
}

//--------------------------------------------------------------------------
int Exif2tm(struct tm * timeptr, const char * ExifTime)
{
    timeptr->tm_wday = -1;

    // Check for format: YYYY:MM:DD HH:MM:SS format.
    // Date and time normally separated by a space, but also seen a ':' there, so
    // skip the middle space with '%*c' so it can be any character.
    timeptr->tm_sec = 0;
    int a = sscanf(ExifTime, "%d%*c%d%*c%d%*c%d:%d:%d",
				   &timeptr->tm_year, &timeptr->tm_mon, &timeptr->tm_mday,
				   &timeptr->tm_hour, &timeptr->tm_min, &timeptr->tm_sec);

    if (a >= 5)
	{
        if (timeptr->tm_year <= 12 && timeptr->tm_mday > 2000 && ExifTime[2] == '.')
		{
            // LG Electronics VX-9700 seems to encode the date as 'MM.DD.YYYY HH:MM'
            // can't these people read the standard?  At least they left enough room
            // in the header to put an Exif format date in there.
            int tmp;
            tmp = timeptr->tm_year;
            timeptr->tm_year = timeptr->tm_mday;
            timeptr->tm_mday = timeptr->tm_mon;
            timeptr->tm_mon = tmp;
        }

        // Accept five or six parameters.  Some cameras do not store seconds.
        timeptr->tm_isdst = -1;
        timeptr->tm_mon -= 1;      // Adjust for unix zero-based months
        timeptr->tm_year -= 1900;  // Adjust for year starting at 1900
        return TRUE; // worked.
    }

    return FALSE; // Wasn't in Exif date format.
}

//--------------------------------------------------------------------------
SFTime exifAsSFTime(const char *timeIn)
{
	struct tm tmOut;
	Exif2tm(&tmOut, timeIn);
	SFTime ret(tmOut.tm_year+1900, tmOut.tm_mon+1, tmOut.tm_mday, tmOut.tm_hour, tmOut.tm_min, tmOut.tm_sec);
	return ret;
}

//--------------------------------------------------------------------------
int Get16u(const void *shortIn)
{
    if (MotorolaOrder)
	{
        return (((uchar *)shortIn)[0] << 8) | ((uchar *)shortIn)[1];
    } else
	{
        return (((uchar *)shortIn)[1] << 8) | ((uchar *)shortIn)[0];
    }
}

//--------------------------------------------------------------------------
int Get32s(const void *longIn)
{
    if (MotorolaOrder)
	{
        return  ((( char *)longIn)[0] << 24) | (((uchar *)longIn)[1] << 16)
		| (((uchar *)longIn)[2] << 8 ) | (((uchar *)longIn)[3] << 0 );
    } else
	{
        return  ((( char *)longIn)[3] << 24) | (((uchar *)longIn)[2] << 16)
		| (((uchar *)longIn)[1] << 8 ) | (((uchar *)longIn)[0] << 0 );
    }
}

//--------------------------------------------------------------------------
unsigned int Get32u(const void *longIn)
{
    return (unsigned int)Get32s(longIn) & 0xffffffff;
}

//--------------------------------------------------------------------------
void Put16u(void *shortIn, unsigned short putValue)
{
    if (MotorolaOrder)
	{
        ((uchar *)shortIn)[0] = (uchar)(putValue>>8);
        ((uchar *)shortIn)[1] = (uchar)putValue;
    } else
	{
        ((uchar *)shortIn)[0] = (uchar)putValue;
        ((uchar *)shortIn)[1] = (uchar)(putValue>>8);
    }
}
//--------------------------------------------------------------------------
void Put32u(void *longIn, unsigned int putValue)
{
    if (MotorolaOrder)
	{
        ((uchar *)longIn)[0] = (uchar)(putValue>>24);
        ((uchar *)longIn)[1] = (uchar)(putValue>>16);
        ((uchar *)longIn)[2] = (uchar)(putValue>>8);
        ((uchar *)longIn)[3] = (uchar)putValue;
    } else
	{
        ((uchar *)longIn)[0] = (uchar)putValue;
        ((uchar *)longIn)[1] = (uchar)(putValue>>8);
        ((uchar *)longIn)[2] = (uchar)(putValue>>16);
        ((uchar *)longIn)[3] = (uchar)(putValue>>24);
    }
}

//--------------------------------------------------------------------------
double convertVal(const void * ValuePtr, int Format)
{
    double den, num;
    switch(Format)
	{
        case FMT_SBYTE:     return      *(signed char *) ValuePtr  ;
        case FMT_BYTE:      return            *(uchar *) ValuePtr  ;
        case FMT_USHORT:    return               Get16u( ValuePtr );
        case FMT_ULONG:     return               Get32u( ValuePtr );
        case FMT_SSHORT:    return (signed short)Get16u( ValuePtr );
        case FMT_SLONG:     return               Get32s( ValuePtr );
        case FMT_SINGLE:    return    (double)*(float *) ValuePtr  ;
        case FMT_DOUBLE:    return           *(double *) ValuePtr  ;

        case FMT_URATIONAL: { den = Get32u(4+(char *)ValuePtr); num = Get32u(ValuePtr); } break;
        case FMT_SRATIONAL: { den = Get32s(4+(char *)ValuePtr); num = Get32s(ValuePtr); } break;
        default:
		{
			nonFatalError("Illegal format code %d in Exif header",Format,0);
			return 0.0;
		}
    }

	return ((den == 0.0) ? 0.0 : num/den);
}
//--------------------------------------------------------------------------
SFString copyStr(void *valPtr, SFInt32 max)
{
	char *buffer = new char[max];
	strncpy(buffer, (char *)valPtr,  max);
	SFString ret = buffer;
	delete [] buffer;
	return ret;
}

//--------------------------------------------------------------------------
#define intVal    (int)         convertVal(ValuePtr, Fmt)
#define floatVal  (float)       convertVal(ValuePtr, Fmt)
#define doubleVal (double)      convertVal(ValuePtr, Fmt)
#define unsignVal (unsigned int)convertVal(ValuePtr, Fmt)

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------
void CJPGFile::writeJPG(void)
{
    if (!m_imageRead)
        fatalError("The image itself has not been read. Cannot write file without losing the image", m_Filename);

    FILE *outfile = fopen(m_Filename,"wb");
    if (outfile == NULL)
        fatalError("Could not open file for write");

    // Initial static jpeg marker.
    fputc(0xff,outfile);
    fputc(0xd8,outfile);

    if (m_sections[0].m_dataType != M_EXIF && m_sections[0].m_dataType != M_JFIF)
	{
        // The image must start with an exif or jfif marker.  If we threw those away, create one.
        static uchar JfifHead[18] =
		{
            0xff, M_JFIF,
            0x00, 0x10, 'J' , 'F' , 'I' , 'F' , 0x00, 0x01,
            0x01, 0x01, 0x01, 0x2C, 0x01, 0x2C, 0x00, 0x00
        };

        fwrite(JfifHead, 18, 1, outfile);

        // use the values from the exif data for the jfif header, if we have found values
        if (m_ResolutionUnit != 0)
		{
            // JFIF.ResolutionUnit is {1,2}, EXIF.ResolutionUnit is {2,3}
            JfifHead[11] = (uchar)m_ResolutionUnit - 1;
        }

        if (m_xResolution > 0.0 && m_yResolution > 0.0)
		{
            JfifHead[12] = (uchar)((int)m_xResolution>>8);
            JfifHead[13] = (uchar)((int)m_xResolution);

            JfifHead[14] = (uchar)((int)m_yResolution>>8);
            JfifHead[15] = (uchar)((int)m_yResolution);
        }
    }

    // Write the sections, preceeded by exif marker and type marker. Do not write the image data because it has no marker
	for (int i=0;i<m_nSections-1;i++)
	{
        fputc(0xff,outfile);
        fputc((uchar)m_sections[i].m_dataType, outfile);
        fwrite(m_sections[i].m_dataPtr, m_sections[i].m_dataSize, 1, outfile);
    }

    // Write the image data
    fwrite(m_sections[m_nSections].m_dataPtr, m_sections[m_nSections].m_dataSize, 1, outfile);

    fclose(outfile);
}

//--------------------------------------------------------------------------
int CJPGFile::processFile(SFBool toExif, SFBool fromExif)
{
	struct stat st;
	if (stat(m_Filename, &st) >= 0)
	{
		m_FileDateTime = st.st_mtime;
		m_FileSize     = (long)st.st_size;

	} else
	{
		fatalError("No such file", m_Filename);
    }

    if ((toExif || fromExif) && access(m_Filename, F_OK|W_OK))
	{
		printf("Skipping readonly file '%s'\n", (const char*)m_Filename);
		return 0;
	}

    SFInt32 readMode = (toExif ? RD_ALL : RD_METADATA);
    if (!readJPG(readMode))
		return 0;

    int wasModified = FALSE;
    if (toExif)
	{
		if (m_numDateTimeTags)
		{
            struct tm tm;
            time_t UnixTime;
            char TempBuf[50];

			if (toExif)
				m_DateTime = fileTimeAsString();

			// A time offset to adjust by was specified.
			if (!Exif2tm(&tm, (const char*)m_DateTime))
			{
				printf("Error: Time '%s': cannot convert to Unix time\n",(const char*)m_DateTime);
				dumpHeader();
				return 0;
			}

			// Convert to unix 32 bit time value, add offset, and convert back.
			UnixTime = mktime(&tm);
			if ((int)UnixTime == -1)
			{
				printf("Error: Time '%s': cannot convert to Unix time\n", (const char*)m_DateTime);
				dumpHeader();
				return 0;
			}

            tm = *localtime(&UnixTime);

            // Print to temp buffer first to avoid putting null termination in destination.
            // snprintf() would do the trick, hbut not available everywhere (like FreeBSD 4.4)
            sprintf(TempBuf, "%04d:%02d:%02d %02d:%02d:%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

            CJPGSection *theSection = NULL;
			for (int i=0;i<m_nSections;i++)
				if (m_sections[i].m_dataType == M_EXIF)
					theSection = &m_sections[i];

            for (int i=0;i<m_numDateTimeTags;i++)
			{
                uchar *datePtr = theSection->m_dataPtr + m_DateTimeOffsets[i] + 8;
                memcpy(datePtr, TempBuf, 19);
            }

            m_DateTime = TempBuf;

            wasModified = TRUE;

        } else
		{
            printf("File '%s' contains no Exif timestamp to change\n", (const char*)m_Filename);
        }
    }

    if (wasModified)
	{
        char backupName[PATH_MAX+5];
		printf("wasModified: %s\n", (const char*)m_Filename);

        strcpy(backupName, m_Filename);
        strcat(backupName, ".t");

        // Remove any .old file name that may pre-exist
        unlink(backupName);

        // Rename the old file.
        rename(m_Filename, backupName);

        // Write the new file.
        writeJPG();

        // Copy the access rights from original file
        struct stat buf;
        if (stat(backupName, &buf) == 0)
		{
            // set Unix access rights and time to new file
            chmod(m_Filename, buf.st_mode);

            struct utimbuf mtime;
            mtime.actime = buf.st_mtime;
            mtime.modtime = buf.st_mtime;
            utime(m_Filename, &mtime);
        }

        // Now that we are done, remove original file.
        unlink(backupName);
    }

	//-fromExif
	if (fromExif)
	{
		// Set the file date to the date from the exif header.
		if (m_numDateTimeTags)
		{
			struct tm exifTime;
			if (!Exif2tm(&exifTime, (const char*)m_DateTime))
			{
				printf("Error: Time '%s': cannot convert to Unix time\n",(const char*)m_DateTime);
				dumpHeader();
				return 1;
			}

			time_t asLinux = mktime(&exifTime);
			if ((int)asLinux == -1)
			{
				printf("Error: Time '%s': cannot convert to Unix time\n",(const char*)m_DateTime);
				dumpHeader();
				return 1;
			}

			struct utimbuf mtime;
			mtime.actime  = asLinux;
			mtime.modtime = asLinux;

			if (hasBadDates())
			{
				if (0) //utime(fileName, &mtime))
				{
					printf("Error: Could not change time of file '%s'\n",(const char*)m_Filename);

				} else
				{
					printf("wasModified: %s\n", (const char*)m_Filename);
				}
			}

		} else
		{
			printf("File '%s' contains no Exif timestamp\n", (const char*)m_Filename);
		}

	}

	if (!toExif)
		dumpHeader();

    return 1;
}

const int BytesPerFormat[] = {0,1,1,2,4,8,1,1,2,4,8,4,8};

void CJPGFile::processGPS(unsigned char * DirStart, unsigned char * OffsetBase, unsigned int ExifLength)
{
    int NumDirEntries = Get16u(DirStart);
	m_Lattitude[0] = 0;
	m_Longitude[0] = 0;
    m_Altitude = EMPTY;

    for (int entry=0;entry<NumDirEntries;entry++)
	{
        unsigned int Tag, Format, Components;
        unsigned char * ValuePtr;
        int ComponentSize;
        unsigned int ByteCount;
        unsigned char *theEntry = DIR_ENTRY_ADDR(DirStart, entry);

        if (theEntry+12 > OffsetBase+ExifLength)
		{
            nonFatalError("GPS info directory goes past end of exif",0,0);
            return;
        }

        Tag        = Get16u(theEntry);
        Format     = Get16u(theEntry+2);
        Components = Get32u(theEntry+4);

        if ((Format-1) >= NUM_FORMATS)
		{
            // (-1) catches illegal zero case as unsigned underflows to positive large.
            nonFatalError("Illegal number format %d for Exif gps tag %04x", Format, Tag);
            continue;
        }

        ComponentSize = BytesPerFormat[Format];
        ByteCount     = Components * ComponentSize;

        if (ByteCount > 4)
		{
            unsigned int OffsetVal;
            OffsetVal = Get32u(theEntry+8);
            // If its bigger than 4 bytes, the dir entry contains an offset.
            if (OffsetVal+ByteCount > ExifLength)
			{
                // Bogus pointer offset and / or bytecount value
                nonFatalError("Illegal value pointer for Exif gps tag %04x", Tag,0);
                continue;
            }
            ValuePtr = OffsetBase+OffsetVal;
        } else
		{
            // 4 bytes or less and value is in the dir entry itself
            ValuePtr = theEntry+8;
        }

        switch(Tag)
		{
            case TAG_GPS_LAT_REF:
                m_Lattitude[0] = ValuePtr[0];
                break;

            case TAG_GPS_LONG_REF:
                m_Longitude[0] = ValuePtr[0];
                break;

            case TAG_GPS_LAT:
            case TAG_GPS_LONG:
                if (Format != FMT_URATIONAL)
                    nonFatalError("Inappropriate format (%d) for Exif GPS coordinates!", Format, 0);

				char FmtString[21];
				double Values[3];
                strncpy(FmtString, "%0.0fd %0.0fm %0.0fs", 20);
                for (int a=0;a<3;a++)
				{
                    int den, digits;

                    den = Get32s(ValuePtr+4+a*ComponentSize);
                    digits = 0;
                    while (den > 1 && digits <= 6)
					{
                        den = den / 10;
                        digits += 1;
                    }
                    if (digits > 6) digits = 6;
                    FmtString[1+a*7] = (char)('2'+digits+(digits ? 1 : 0));
                    FmtString[3+a*7] = (char)('0'+digits);
                    Values[a] = convertVal(ValuePtr+a*ComponentSize, Format);
                }

				char TempString[50];
                sprintf(TempString, FmtString, Values[0], Values[1], Values[2]);

                if (Tag == TAG_GPS_LAT)
				{
                    strncpy(m_Lattitude+2, TempString, 29);
                } else
				{
                    strncpy(m_Longitude+2, TempString, 29);
                }
                break;

            case TAG_GPS_ALT_REF:
                m_Altitude.SetAt(0, (char)(ValuePtr[0] ? '-' : ' '));
                break;

            case TAG_GPS_ALT:
				char val[20];
                sprintf(val, "%.2fm", convertVal(ValuePtr, Format));
				m_Altitude = m_Altitude.Left(1) + SFString(val);
                break;
        }
    }
}

//--------------------------------------------------------------------------
void CJPGFile::processExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned int ExifLength, int NestingLevel)
{
    unsigned int ThumbnailOffset = 0;
    unsigned int ThumbnailSize = 0;

    if (NestingLevel > 4)
	{
        nonFatalError("Maximum Exif directory nesting exceeded (corrupt Exif header)", 0,0);
        return;
    }

    int NumDirEntries = Get16u(DirStart);

	unsigned char * DirEnd;
	DirEnd = DIR_ENTRY_ADDR(DirStart, NumDirEntries);
	if (DirEnd+4 > (OffsetBase+ExifLength))
	{
		if (!(DirEnd+2 == OffsetBase+ExifLength || DirEnd == OffsetBase+ExifLength))
		{
			nonFatalError("Illegally sized Exif subdirectory (%d entries)",NumDirEntries,0);
			return;
		}
	}

    for (int entry=0;entry<NumDirEntries;entry++)
	{
        unsigned char *theEntry = DIR_ENTRY_ADDR(DirStart, entry);

        int Tag        = Get16u(theEntry);
        int Fmt        = Get16u(theEntry+2);
        int Components = Get32u(theEntry+4);

        if ((Fmt-1) >= NUM_FORMATS)
		{
            // (-1) catches illegal zero case as unsigned underflows to positive large.
            nonFatalError("Illegal number format %d for tag %04x in Exif", Fmt, Tag);
            continue; // skip this entry
        }

        if ((unsigned int)Components > 0x10000)
		{
            nonFatalError("Too many components %d for tag %04x in Exif", Components, Tag);
            continue; // skip this entry
        }

		unsigned char *ValuePtr = NULL;
        int ByteCount = Components * BytesPerFormat[Fmt];
        if (ByteCount > 4)
		{
            unsigned int OffsetVal = Get32u(theEntry+8);
            if (OffsetVal + ByteCount > ExifLength)
			{
                nonFatalError("Illegal value pointer for tag %04x in Exif", Tag,0);
                continue; // skip this entry
            }

            ValuePtr = OffsetBase + OffsetVal;
            if (OffsetVal > m_LargestExifOffset)
                m_LargestExifOffset = OffsetVal;

        } else
		{
            // 4 bytes or less and value is in the dir entry itself
            ValuePtr = theEntry + 8;
        }

        if (Tag == TAG_MAKER_NOTE)
		{
			if (m_CameraMake == "Canon")
			{
				// Canon cameras store some useful data in the maker mark, extract it
				int MotorolaOrderSave = MotorolaOrder;
				MotorolaOrder = 0; // Temporarily switch to little endian.
				processMakerNote(ValuePtr, ByteCount, OffsetBase, ExifLength);
				MotorolaOrder = MotorolaOrderSave;
			}

            continue;
        }

        switch(Tag)
		{
            case TAG_MAKE:
				m_CameraMake = copyStr(ValuePtr, MIN(31, ByteCount));
                break;

            case TAG_MODEL:
                m_CameraModel = copyStr(ValuePtr, MIN(39, ByteCount));
                break;

            case TAG_DATETIME_ORIGINAL:
				m_DateTime = (const char *)ValuePtr;
                // fall through

            case TAG_DATETIME_DIGITIZED:
            case TAG_DATETIME:
				// If we don't already have a DATETIME_ORIGINAL, use whatever time fields we may have.
                if (m_DateTime.IsEmpty())
					m_DateTime = (const char *)ValuePtr;

                if (m_numDateTimeTags >= MAX_DATE_COPIES)
				{
                    nonFatalError("More than %d date fields in Exif.  This is nuts", MAX_DATE_COPIES, 0);
                    break;
                }

                m_DateTimeOffsets[m_numDateTimeTags++] = (char *)ValuePtr - (char *)OffsetBase;
                break;

            case TAG_USERCOMMENT:
			{
				// Already have a comment (probably windows comment), skip this one.
				if (m_Comments[0])
					break;

				// Comment is often padded with trailing spaces.  Remove these first.
				for (int a=ByteCount;;)
				{
					a--;
					if ((ValuePtr)[a] == ' ')
					{
						(ValuePtr)[a] = '\0';
					} else
					{
						break;
					}
					if (a == 0)
						break;
				}

				// Copy the comment
				int msiz = (int)(ExifLength - (ValuePtr-OffsetBase));
				if (msiz > ByteCount) msiz = ByteCount;
				if (msiz > MAX_COMMENT_SIZE-1) msiz = MAX_COMMENT_SIZE-1;
				if (msiz > 5 && memcmp(ValuePtr, "ASCII",5) == 0)
				{
					for (int a=5;a<10 && a < msiz;a++)
					{
						int c = (ValuePtr)[a];
						if (c != '\0' && c != ' ')
						{
							strncpy(m_Comments, (char *)ValuePtr+a, msiz-a);
							break;
						}
					}
				} else
				{
					strncpy(m_Comments, (char *)ValuePtr, msiz);
				}
			}
                break;

            case TAG_FNUMBER:
                // Simplest way of expressing aperture, so I trust it the most (overwrite previously computd value if there is one).
                m_fStop = floatVal;
                break;

            case TAG_APERTURE:
            case TAG_MAXAPERTURE:
                // More relevant info always comes earlier, so only use this field if we don't have appropriate aperture information yet.
                if (m_fStop == 0)
                    m_fStop = (float)exp(doubleVal*log(2)*0.5);
                break;

            case TAG_FOCALLENGTH:
                // Nice digital cameras actually save the focal length as a function of how far they are zoomed in.
                m_FocalLength = floatVal;
                break;

            case TAG_SUBJECT_DISTANCE:
                // Inidcates the distacne the autofocus camera is focused to. Tends to be less accurate as distance increases.
                m_Distance = floatVal;
                break;

            case TAG_EXPOSURETIME:
                // Simplest way of expressing exposure time, so I trust it most (overwrite previously computd value if there is one).
                m_Exposure = floatVal;
                break;

            case TAG_SHUTTERSPEED:
                // More complicated way of expressing exposure time, so only use this value if we don't already have it from somewhere else.
                if (m_Exposure == 0)
                    m_Exposure = (float)(1/exp(doubleVal*log(2)));
                break;

            case TAG_FLASH:
                m_FlashUsed = intVal;
                break;

            case TAG_ORIENTATION:
				m_Orientation = intVal;
                if (m_Orientation < 0 || m_Orientation > 8)
				{
                    nonFatalError("Undefined rotation value %ld in Exif", (int)m_Orientation, 0);
                    m_Orientation = 0;
                }
                break;

            case TAG_PIXEL_Y_DIMENSION:
            case TAG_PIXEL_X_DIMENSION:
				// Use largest of height and width to deal with images that have been rotated to portrait format.
			{
				int a = intVal;
				if (ExifImageWidth < a)
					ExifImageWidth = a;
			}
                break;

            case TAG_FOCAL_PLANE_XRES:
                FocalplaneXRes = doubleVal;
                break;

            case TAG_FOCAL_PLANE_UNITS:
                switch(intVal)
			{
					// According to the information I was using, 2 means meters. But looking at the Cannon powershot's files, inches is the only sensible value.
				case 1:
				case 2: FocalplaneUnits = 25.4; break; // inches
				case 3: FocalplaneUnits = 10;   break; // centimeter
				case 4: FocalplaneUnits = 1;    break; // millimeter
				case 5: FocalplaneUnits = .001; break; // micrometer
			}
                break;

            case TAG_EXPOSURE_BIAS:
                m_ExposureBias = floatVal;
                break;

            case TAG_WHITEBALANCE:
                m_Whitebalance = intVal;
                break;

            case TAG_LIGHT_SOURCE:
                m_LightSource = intVal;
                break;

            case TAG_METERING_MODE:
                m_MeteringMode = intVal;
                break;

            case TAG_EXPOSURE_PROGRAM:
                m_ExposureProgram = intVal;
                break;

            case TAG_EXPOSURE_INDEX:
				// Exposure index and ISO equivalent are often used interchangeably, so we will do the same in jhead.
                if (m_ISOequivalent == 0)
                    m_ISOequivalent = intVal;
                break;

            case TAG_EXPOSURE_MODE:
                m_ExposureMode = intVal;
                break;

            case TAG_ISO_EQUIVALENT:
                m_ISOequivalent = intVal;
                break;

            case TAG_DIGITALZOOMRATIO:
                m_DigitalZoomRatio = floatVal;
                break;

            case TAG_THUMBNAIL_OFFSET:
                ThumbnailOffset = unsignVal;
                break;

            case TAG_THUMBNAIL_LENGTH:
                ThumbnailSize = unsignVal;
                m_ThumbnailSizeOffset = ValuePtr-OffsetBase;
                break;

            case TAG_EXIF_OFFSET:
            case TAG_INTEROP_OFFSET:
			{
				unsigned char * SubdirStart;
				SubdirStart = OffsetBase + Get32u(ValuePtr);
				if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength)
					nonFatalError("Illegal Exif or interop ofset directory link",0,0);
				else
					processExifDir(SubdirStart, OffsetBase, ExifLength, NestingLevel+1);
				continue;
			}
                break;

            case TAG_GPSINFO:
			{
				unsigned char * SubdirStart = OffsetBase + Get32u(ValuePtr);
				if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength)
					nonFatalError("Illegal GPS directory link in Exif",0,0);
				else
					processGPS(SubdirStart, OffsetBase, ExifLength);
				continue;
			}
                break;

            case TAG_FOCALLENGTH_35MM:
                // The focal length equivalent 35 mm if its present, use it to compute equivalent focal length instead of computing it from sensor geometry and actual focal length.
                m_FocalLength35mmEquiv = unsignVal;
                break;

            case TAG_DISTANCE_RANGE:
                // Three possible standard values: 1 = macro, 2 = close, 3 = distant
                m_DistanceRange = intVal;
                break;

            case TAG_X_RESOLUTION:
				// Only use the values from the top level directory. If yResolution has not been set, use the value of xResolution
                if (NestingLevel==0)
				{
                    m_xResolution = floatVal;
                    if (m_yResolution == 0.0)
						m_yResolution = m_xResolution;
                }
                break;

            case TAG_Y_RESOLUTION:
				// // Only use the values from the top level directory. If xResolution has not been set, use the value of yResolution
                if (NestingLevel==0)
				{
                    m_yResolution = floatVal;
                    if (m_xResolution == 0.0)
						m_xResolution = m_yResolution;
                }
                break;

            case TAG_RESOLUTION_UNIT:
				// Only use the values from the top level directory
                if (NestingLevel==0)
                    m_ResolutionUnit = intVal;
                break;
        }
    }


    {
        // In addition to linking to subdirectories via exif tags,
        // there's also a potential link to another directory at the end of each
        // directory.  this has got to be the result of a committee!
        unsigned char * SubdirStart;
        unsigned int Offset;

        if (DIR_ENTRY_ADDR(DirStart, NumDirEntries) + 4 <= OffsetBase+ExifLength){
            Offset = Get32u(DirStart+2+12*NumDirEntries);
            if (Offset){
                SubdirStart = OffsetBase + Offset;
                if (SubdirStart > OffsetBase+ExifLength || SubdirStart < OffsetBase){
                    if (SubdirStart > OffsetBase && SubdirStart < OffsetBase+ExifLength+20)
					{
                    } else
					{
                        nonFatalError("Illegal subdirectory link in Exif header",0,0);
                    }
                }else{
                    if (SubdirStart <= OffsetBase+ExifLength)
					{
                        processExifDir(SubdirStart, OffsetBase, ExifLength, NestingLevel+1);
                    }
                }
                if (Offset > m_LargestExifOffset){
                    m_LargestExifOffset = Offset;
                }
            }
        }else{
            // The exif header ends before the last next directory pointer.
        }
    }

    if (ThumbnailOffset)
	{
        m_ThumbnailAtEnd = FALSE;

        if (ThumbnailOffset <= ExifLength){
            if (ThumbnailSize > ExifLength-ThumbnailOffset){
                // If thumbnail extends past exif header, only save the part that
                // actually exists.  Canon's EOS viewer utility will do this - the
                // thumbnail extracts ok with this hack.
                ThumbnailSize = ExifLength-ThumbnailOffset;

            }
            // The thumbnail pointer appears to be valid.  Store it.
//            ThumbnailOffset = ThumbnailOffset;
//            ThumbnailSize = ThumbnailSize;
        }
    }
}

//--------------------------------------------------------------------------
void CJPGFile::process_EXIF(unsigned char *ExifSection, unsigned int length)
{
    unsigned int FirstOffset;

    FocalplaneXRes  = 0;
    FocalplaneUnits = 0;
    ExifImageWidth  = 0;

	static uchar ExifHeader[] = "Exif\0\0";
	if (memcmp(ExifSection+2, ExifHeader, 6))
	{
		nonFatalError("Incorrect Exif header",0,0);
		return;
	}

    if (memcmp(ExifSection+8,"II",2) == 0)
	{
        MotorolaOrder = 0;

    } else
	{
        if (memcmp(ExifSection+8,"MM",2) == 0)
		{
            MotorolaOrder = 1;

        } else
		{
            nonFatalError("Invalid Exif alignment marker.",0,0);
            return;
        }
    }

    // Check the next value for correctness.
    if (Get16u(ExifSection+10) != 0x2a)
	{
        nonFatalError("Invalid Exif start (1)",0,0);
        return;
    }

    FirstOffset = Get32u(ExifSection+12);
    if (FirstOffset < 8 || FirstOffset > 16)
	{
        if (FirstOffset < 16 || FirstOffset > length-16)
		{
            nonFatalError("invalid offset for first Exif IFD value",0,0);
            return;
        }

        // Usually set to 8, but other values valid too.
        nonFatalError("Suspicious offset of first Exif IFD value",0,0);
    }

    // First directory starts 16 bytes in.  All offset are relative to 8 bytes in.
    processExifDir(ExifSection+8+FirstOffset, ExifSection+8, length-8, 0);

    m_ThumbnailAtEnd = m_ThumbnailOffset >= m_LargestExifOffset ? TRUE : FALSE;

    // Compute the CCD width, in millimeters.
    if (FocalplaneXRes != 0 && ExifImageWidth != 0)
	{
        // Some cameras its impossible to calc this correctly they don't adjust the focal plane resolution when using less than maximum res, so the CCDWidth value comes out too small.
        m_CCDWidth = (float)(ExifImageWidth * FocalplaneUnits / FocalplaneXRes);
		// Compute 35 mm equivalent focal length based on sensor geometry if we haven't already got it explicitly from a tag.
        if (m_FocalLength!=0.0 && m_FocalLength35mmEquiv == 0)
            m_FocalLength35mmEquiv = (int)(m_FocalLength/m_CCDWidth*36 + 0.5);
    }
}

//--------------------------------------------------------------------------------------------------------------------
void CJPGFile::processMakerNote(unsigned char * valPtrIn, int byteCountIn, unsigned char * OffsetBase, unsigned int ExifLength)
{
	int NumDirEntries = Get16u(valPtrIn);

	unsigned char * DirEnd = DIR_ENTRY_ADDR(valPtrIn, NumDirEntries);
	if (DirEnd > (OffsetBase + ExifLength))
	{
		nonFatalError("Illegally sized Exif makernote subdir (%d entries)", NumDirEntries, 0);
		return;
	}

	for (int de=0; de<NumDirEntries; de++)
	{
		unsigned char *theEntry = DIR_ENTRY_ADDR(valPtrIn, de);
		int Tag        = Get16u(theEntry);
		int Format     = Get16u(theEntry+2);
		int Components = Get32u(theEntry+4);

		if ((Format-1) >= NUM_FORMATS)
		{
			// (-1) catches illegal zero case as unsigned underflows to positive large.
			nonFatalError("Illegal Exif number format %d for maker tag %04x", Format, Tag);
			continue;
		}

		if ((unsigned int)Components > 0x10000)
		{
			nonFatalError("Too many components (%d) for Exif maker tag %04x", Components, Tag);
			continue;
		}

		unsigned char * ValuePtr;
		int ByteCount = Components * BytesPerFormat[Format];
		if (ByteCount > 4)
		{
			unsigned int OffsetVal;
			OffsetVal = Get32u(theEntry+8);
			// If its bigger than 4 bytes, the dir entry contains an offset.
			if (OffsetVal+ByteCount > ExifLength)
			{
				// Bogus pointer offset and / or bytecount value
				nonFatalError("Illegal value pointer for Exif maker tag %04x", Tag,0);
				continue;
			}
			ValuePtr = OffsetBase+OffsetVal;

		} else
		{
			// 4 bytes or less and value is in the dir entry itself
			ValuePtr = theEntry+8;
		}

		if (Tag == 1 && Components > 16)
		{
			int IsoCode = Get16u(ValuePtr + 16*sizeof(unsigned short));
			if (IsoCode >= 16 && IsoCode <= 24)
				m_ISOequivalent = 50 << (IsoCode-16);
		}

		if (Tag == 4 && Format == FMT_USHORT)
		{
			if (Components > 7)
			{
				int WhiteBalance = Get16u(ValuePtr + 7*sizeof(unsigned short));
				switch(WhiteBalance)
				{
						// 0=Auto, 6=Custom
					case 1: m_LightSource = 1; break; // Sunny
					case 2: m_LightSource = 1; break; // Cloudy
					case 3: m_LightSource = 3; break; // Thungsten
					case 4: m_LightSource = 2; break; // Fourescent
					case 5: m_LightSource = 4; break; // Flash
				}
			}

			if (Components > 19 && m_Distance <= 0)
			{
				// Indicates the distance the autofocus camera is focused to.
				// Tends to be less accurate as distance increases.
				int temp_dist = Get16u(ValuePtr + 19*sizeof(unsigned short));
				if (temp_dist == 65535)
				{
					m_Distance = -1; // infinity

				} else
				{
					m_Distance = (float)temp_dist/100;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------
void CJPGFile::process_DQT (const uchar * Data, int length)
{
	//--------------------------------------------------------------------------------------------------------------------
	static int std_luminance_quant_tbl[64] =
	{
		16,  11,  12,  14,  12,  10,  16,  14,
		13,  14,  18,  17,  16,  19,  24,  40,
		26,  24,  22,  22,  24,  49,  35,  37,
		29,  40,  58,  51,  61,  60,  57,  51,
		56,  55,  64,  72,  92,  78,  64,  68,
		87,  69,  55,  56,  80, 109,  81,  87,
		95,  98, 103, 104, 103,  62,  77, 113,
		121, 112, 100, 120,  92, 101, 103,  99
	};

	//--------------------------------------------------------------------------------------------------------------------
	static int std_chrominance_quant_tbl[64] =
	{
		17,  18,  18,  24,  21,  24,  47,  26,
		26,  47,  99,  66,  56,  66,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99,
		99,  99,  99,  99,  99,  99,  99,  99
	};

	//--------------------------------------------------------------------------------------------------------------------
	static int *deftabs[2] =
	{
		std_luminance_quant_tbl,
		std_chrominance_quant_tbl
	};

    unsigned int table[64];
    int *reftable = NULL;
    double cumsf = 0.0, cumsf2 = 0.0;
    int allones = 1;

    int a=2; // first two bytes is length
    while (a<length)
    {
        int c = Data[a++];
        int tableindex = c & 0x0f;
        if (tableindex < 2)
            reftable = deftabs[tableindex];

        for (int coefindex = 0; coefindex < 64; coefindex++)
		{
            unsigned int val;
            if (c>>4)
			{
                //register
				unsigned int temp=(unsigned int) (Data[a++]);
                temp *= 256;
                val=(unsigned int) Data[a++] + temp;

            } else {
                val=(unsigned int) Data[a++];

            }

            table[coefindex] = val;
            if (reftable)
			{
                double x;
                // scaling factor in percent
                x = 100.0 * (double)val / (double)reftable[coefindex];
                cumsf += x;
                cumsf2 += x * x;
                // separate check for all-ones table (Q 100)
                if (val != 1)
					allones = 0;
            }
        }

        if (reftable)
		{
			// terse output includes quality
            double qual, var;
            cumsf /= 64.0;    // mean scale factor
            cumsf2 /= 64.0;
            var = cumsf2 - (cumsf * cumsf); // variance
            if (allones)
			{
				// special case for all-ones table
                qual = 100.0;

            } else if (cumsf <= 100.0)
			{
                qual = (200.0 - cumsf) / 2.0;

            } else
			{
                qual = 5000.0 / cumsf;
            }

			if (tableindex == 0)
				m_QualityGuess = (int)(qual+0.5);
        }
    }
}

//--------------------------------------------------------------------------
SFBool CJPGFile::hasDiffDates(void)
{
	m_fileTime = SFos::fileLastModifyDate(m_Filename);
	m_exifTime = exifAsSFTime(m_DateTime);

	if (m_fileTime.GetYear()==1969)
		return FALSE;

	return (m_exifTime != m_fileTime);
}

//--------------------------------------------------------------------------
SFBool CJPGFile::hasBadDates()
{
#if 0
	if (!hasDiffDates())
		return FALSE;

	SFTimeSpan upper( 150000, 0, 0, 0);
	SFTimeSpan lower(-150000, 0, 0, 0);
	SFTimeSpan ts;
	ts = (m_fileTime - m_exifTime);
	return ((lower.getSeconds_double() < ts.getSeconds_double()) && (ts.getSeconds_double() <= upper.getSeconds_double()));
#else
	return hasDiffDates();
#endif
}

//--------------------------------------------------------------------------
int CJPGFile::checkDates(void)
{
    if (!readJPG(RD_METADATA))
		return 0;

	if (m_numDateTimeTags && hasBadDates())
	{
		SFString path = getCWD() + m_Filename;
		path.Replace("/./", "/");
		path.ReplaceAll(" ", "\\ ");
		path.ReplaceAll("'", "\\'");
		path.ReplaceAll("(", "\\(");
		path.ReplaceAll(")", "\\)");
		//		fprintf(stderr, "/Users/jrush/zzzBuilds/jhead -fromExif %s\n", (const char*)path);
		//		fprintf(stderr, "open %s\n", (const char*)path);
	}
    return 1;
}

//--------------------------------------------------------------------------
SFString CJPGFile::fileTimeAsString(void)
{
    struct tm ts;
    ts = *localtime(&m_FileDateTime);
    char TimeStr[20];
	strftime(TimeStr, 20, "%Y:%m:%d %H:%M:%S", &ts);
	return TimeStr;
}

//--------------------------------------------------------------------------
void CJPGFile::growIfNeeded(void)
{
    ASSERT(m_nSections <= m_nAllocated);
    if (m_nSections >= m_nAllocated)
	{
		if (!m_sections)
		{
			m_nAllocated = 10; // start with five
			m_sections   = (CJPGSection*)malloc(sizeof(CJPGSection)*m_nAllocated);

		} else
		{
			m_nAllocated = m_nAllocated + (m_nAllocated / 2); // add half again as many
			m_sections   = (CJPGSection*)realloc(m_sections, sizeof(CJPGSection) * m_nAllocated);
		}
		ASSERT(m_sections);
    }
}

//--------------------------------------------------------------------------
SFBool CJPGFile::readJPG(SFInt32 readMode)
{
	if (!SFos::fileExists(m_Filename))
	{
		fprintf(stderr, "Cannot find file: %s\n", (const char*)m_Filename);
		return FALSE;
	}

    FILE *fp = fopen(m_Filename, binaryReadOnly);
	if (!fp)
	{
		fprintf(stderr, "Could not open file: %s\n", (const char*)m_Filename);
		return FALSE;
	}

	SFBool ret = readSections(fp, readMode);
	if (!ret)
		fprintf(stderr, "File %s does not appear to be a JPG file\n", (const char*)m_Filename);

	fclose(fp);
	return ret;
}

//--------------------------------------------------------------------------
SFBool CJPGFile::readSections(FILE *fp, SFInt32 readMode)
{
    int cc = fgetc(fp);
    if (cc != 0xff || fgetc(fp) != M_SOI)
        return FALSE;

	SFBool readingMetaData = ( readMode & RD_METADATA );
	SFBool readingImage    = ( readMode & RD_IMAGE    );

    int hasComment = FALSE;
    for ( ; ; )
	{
        growIfNeeded();

        int marker = 0;
        int prev = 0;
		int x;
        for (x=0;;x++)
		{
            marker = fgetc(fp);
            if (marker != 0xff && prev == 0xff)
				break;
            if (marker == EOF)
                fatalError("Unexpected end of file", m_Filename);
            prev = marker;
        }

        if (x > 10)
            nonFatalError("Extraneous %d padding bytes before section %02X",x-1,marker);

        m_sections[m_nSections].m_dataType = marker;

        // Read the length of the section.
        int lh = fgetc(fp);
        int ll = fgetc(fp);
        if (lh == EOF || ll == EOF)
            fatalError("Unexpected end of file", m_Filename);

        int itemlen = (lh << 8) | ll;
        if (itemlen < 2)
            fatalError("invalid marker", m_Filename);

        m_sections[m_nSections].m_dataSize = itemlen;

        uchar *Data = (uchar *)malloc(itemlen);
        if (Data == NULL)
            fatalError("Could not allocate memory", m_Filename);

        m_sections[m_nSections].m_dataPtr = Data;

        // Store first two pre-read bytes.
        Data[0] = (uchar)lh;
        Data[1] = (uchar)ll;

        int got = (int)fread(Data+2, 1, itemlen-2, fp); // Read the whole section.
        if (got != itemlen-2)
            fatalError("Premature end of file?", m_Filename);

        m_nSections += 1;
        switch(marker)
		{
            case M_SOS:
				// stop before hitting compressed data. If reading entire image is requested, read the rest of the data.
                if (readingImage)
				{
                    int cp, ep, size;
                    // Determine how much file is left.
                    cp = (int)ftell(fp);
                    fseek(fp, 0, SEEK_END);
                    ep = (int)ftell(fp);
                    fseek(fp, cp, SEEK_SET);

                    size = ep-cp;
                    Data = (uchar *)malloc(size);
                    if (Data == NULL){
                        fatalError("could not allocate data for entire image", m_Filename);
                    }

                    got = (int)fread(Data, 1, size, fp);
                    if (got != size){
                        fatalError("could not read the rest of the image", m_Filename);
                    }

                    growIfNeeded();
                    m_sections[m_nSections].m_dataPtr = Data;
                    m_sections[m_nSections].m_dataSize = size;
                    m_sections[m_nSections].m_dataType = PSEUDO_IMAGE_MARKER;
                    m_nSections++;
                    m_imageRead = TRUE;
                }
                return TRUE;

            case M_DQT:
		        // Use for jpeg quality guessing
                process_DQT(Data, itemlen);
                break;

            case M_EOI:
				// in case it's a tables-only JPEG stream
                fprintf(stderr,"No image in jpeg!\n");
                return FALSE;

            case M_COM:
			{
				if (hasComment || !readingMetaData)
				{
					// Discard this section.
					free(m_sections[--m_nSections].m_dataPtr);

				} else
				{
					char *dataIn = (char*)Data;
					SFString str;
					for (int i=2;i<itemlen;i++)
						if (strncmp(dataIn, "\r\n", 2)) // skip \r\n
							str += (char)((dataIn[i] >= 32 || dataIn[i] == '\n' || dataIn[i] == '\t') ? dataIn[i] : '?');
					strcpy(m_Comments, (const char*)str.Left(MAX_COMMENT_SIZE));
					m_CommentWidthchars = 0;
					hasComment = TRUE;
				}
			}
                break;

            case M_JFIF:
                free(m_sections[--m_nSections].m_dataPtr);
                break;

            case M_EXIF:
                // There may be different sections using the same marker.
                if (readingMetaData)
				{
                    if (memcmp(Data+2, "Exif", 4) == 0)
					{
                        process_EXIF(Data, itemlen);
                        break;

                    } else if (memcmp(Data+2, "http:", 5) == 0)
					{
                        m_sections[m_nSections-1].m_dataType = M_XMP; // Change tag for internal purposes.
                        break;
                    }
                }

                // Otherwise, discard this section.
                free(m_sections[--m_nSections].m_dataPtr);
                break;

            case M_IPTC:
                if (!readingMetaData)
                    free(m_sections[--m_nSections].m_dataPtr);
                break;

            case M_SOF0:
            case M_SOF1:
            case M_SOF2:
            case M_SOF3:
            case M_SOF5:
            case M_SOF6:
            case M_SOF7:
            case M_SOF9:
            case M_SOF10:
            case M_SOF11:
            case M_SOF13:
            case M_SOF14:
            case M_SOF15:
				m_Height  =  ((Data+3)[0] << 8) | (Data+3)[1];
				m_Width   =  ((Data+5)[0] << 8) | (Data+5)[1];
				m_isBW    = !( Data   [7] == 3);
				m_jpgProcess = marker;
                break;

            default:
                // Skip any other sections.
                break;
        }
    }
	
    return TRUE;
}

//--------------------------------------------------------------------------
void CJPGFile::dumpHeader()
{
	CStringExportContext ctx;

	ctx << "File name    : " << m_Filename << "\n";
	ctx << "File size    : " << m_FileSize << " bytes\n";
	ctx << "File date    : " << fileTimeAsString() << "\n";
    if (!m_DateTime.IsEmpty())
	{
		if (hasBadDates())
		{
			SFTimeSpan ts = (m_fileTime - m_exifTime);
			ctx << "File date2   : " << m_fileTime.Format("%Y:%m:%d %H:%M:%S") << "\n";
			ctx << "Date/Time    : " << m_DateTime << "<----| " << (ts).Format("%D:%H:%M:%S") << "\n";
			ctx << "Date/Time2   : " << m_exifTime.Format("%Y:%m:%d %H:%M:%S") << "\n";
//			ctx << "Seconds      : " << (long)lower.getSeconds_double() << " | " << (long)ts.getSeconds_double() << " | " << (long)upper.getSeconds_double() << "\n";

		} else
		{
			ctx << "Date/Time    : " << m_exifTime.Format("%Y:%m:%d %H:%M:%S") << "\n";
		}
	} else
	{
		ctx << "Date/Time    : exif create date is missing\n";
		SFString path = getCWD() + m_Filename;
		path.Replace("/./", "/");
		path.ReplaceAll(" ", "\\ ");
		path.ReplaceAll("'", "\\'");
		path.ReplaceAll("(", "\\(");
		path.ReplaceAll(")", "\\)");
		SFString file = path.Mid(path.ReverseFind('/')+1,1000);
		//		fprintf(stderr, "%s+open %s\n", (const char*)file, (const char*)path);
		//		fprintf(stderr, "%s+/Users/jrush/zzzBuilds/jhead -toExif %s\n", (const char*)file, (const char*)path);
	}

    if (!m_CameraMake.IsEmpty())
	{
		ctx << "Camera make  : " << m_CameraMake << "\n";
		ctx << "Camera model : " << m_CameraModel << "\n";
    }

	ctx << "Resolution   : " << m_Width << " x " << m_Height << "\n";

    if (TRUE) //m_Orientation > 1)
	{
		// 1 - "The 0th row is at the visual top of the image,    and the 0th column is the visual left-hand side."
		// 2 - "The 0th row is at the visual top of the image,    and the 0th column is the visual right-hand side."
		// 3 - "The 0th row is at the visual bottom of the image, and the 0th column is the visual right-hand side."
		// 4 - "The 0th row is at the visual bottom of the image, and the 0th column is the visual left-hand side."
		// 5 - "The 0th row is the visual left-hand side of of the image,  and the 0th column is the visual top."
		// 6 - "The 0th row is the visual right-hand side of of the image, and the 0th column is the visual top."
		// 7 - "The 0th row is the visual right-hand side of of the image, and the 0th column is the visual bottom."
		// 8 - "The 0th row is the visual left-hand side of of the image,  and the 0th column is the visual bottom."

		static const char * OrientTab[9] = {
			"Undefined",
			"Normal",           // 1
			"flip horizontal",  // left right reversed mirror
			"rotate 180",       // 3
			"flip vertical",    // upside down mirror
			"transpose",        // Flipped about top-left <--> bottom-right axis.
			"rotate 90",        // rotate 90 cw to right it.
			"transverse",       // flipped about top-right <--> bottom-left axis
			"rotate 270",       // rotate 270 to right it.
		};
		ctx << "Orientation  : " << OrientTab[m_Orientation] << "\n";
	}

    if (TRUE) //m_isBW)
		ctx << "Color/bw     : " << (m_isBW ? "Black and white" : "Color") << "\n";

    if (m_FlashUsed >= 0)
	{
        if (m_FlashUsed & 1)
		{
            ctx << "Flash used   : Yes";
            switch (m_FlashUsed)
			{
	            case 0x5:  ctx << " (Strobe light not detected)"; break;
	            case 0x7:  ctx << " (Strobe light detected)"; break;
	            case 0x9:  ctx << " (manual)"; break;
	            case 0xd:  ctx << " (manual, return light not detected)"; break;
	            case 0xf:  ctx << " (manual, return light  detected)"; break;
	            case 0x19: ctx << " (auto)"; break;
	            case 0x1d: ctx << " (auto, return light not detected)"; break;
	            case 0x1f: ctx << " (auto, return light detected)"; break;
	            case 0x41: ctx << " (red eye reduction mode)"; break;
	            case 0x45: ctx << " (red eye reduction mode return light not detected)"; break;
	            case 0x47: ctx << " (red eye reduction mode return light  detected)"; break;
	            case 0x49: ctx << " (manual, red eye reduction mode)"; break;
	            case 0x4d: ctx << " (manual, red eye reduction mode, return light not detected)"; break;
	            case 0x4f: ctx << " (red eye reduction mode, return light detected)"; break;
	            case 0x59: ctx << " (auto, red eye reduction mode)"; break;
	            case 0x5d: ctx << " (auto, red eye reduction mode, return light not detected)"; break;
	            case 0x5f: ctx << " (auto, red eye reduction mode, return light detected)"; break;
            }

        } else
		{
            ctx << "Flash used   : No";
            switch (m_FlashUsed)
			{
	            case 0x18: ctx << " (auto)"; break;
			}
        }
        ctx << "\n";
    }

    if (TRUE) //m_FocalLength)
	{
        ctx << "Focal length : " << Fmt("%4.1f") << (double)m_FocalLength << "mm";
        if (TRUE) //FocalLength35mmEquiv)
            ctx << "  (35mm equivalent: " << Fmt("%2d") << m_FocalLength35mmEquiv << "mm)";
        ctx << "\n";
    }

    if (TRUE) //m_DigitalZoomRatio > 1)
        ctx << "Digital Zoom : " << Fmt("%1.3f") << (double)m_DigitalZoomRatio << "x\n";

    if (TRUE) //m_CCDWidth)
        ctx << "CCD width    : " << Fmt("%4.2f") << (double)m_CCDWidth << "mm\n";

    if (TRUE) //m_Exposure)
	{
        if (m_Exposure < 0.010)
            ctx << "Exposure time: " << Fmt("%6.4f") << (double)m_Exposure << " s ";
        else
            ctx << "Exposure time: " << Fmt("%5.3f") << (double)m_Exposure << " s ";

        if (m_Exposure <= 0.5 && m_Exposure != 0.0)
            ctx << " (1/" << Fmt("%d") << (int)(0.5 + 1/m_Exposure) << ")";
        ctx << "\n";
    }

    if (TRUE) //m_fStop)
        ctx << "Aperture     : f/" << Fmt("%3.1f") << (double)m_fStop << "\n";

	if (TRUE) //m_Distance < 0)
		ctx << "Focus dist.  : Infinite\n";
	else //if (m_Distance > 0)
		ctx << "Focus dist.  : " << Fmt("%4.2f") << (double)m_Distance << "m\n";

    if (TRUE) //ISOequivalent)
        ctx << "ISO equiv.   : " << Fmt("%2d") << (int)m_ISOequivalent << "\n";

    if (TRUE) //m_ExposureBias)
        ctx << "Exposure bias: " << Fmt("%4.2f") << (double)m_ExposureBias << "\n";

    switch(m_Whitebalance)
	{
        case 1: ctx << "Whitebalance : Manual\n"; break;
        case 0: ctx << "Whitebalance : Auto\n";   break;
    }

    switch(m_LightSource)
	{
        case 1:  ctx << "Light Source : Daylight\n";     break;
        case 2:  ctx << "Light Source : Fluorescent\n";  break;
        case 3:  ctx << "Light Source : Incandescent\n"; break;
        case 4:  ctx << "Light Source : Flash\n";        break;
        case 9:  ctx << "Light Source : Fine weather\n"; break;
        case 11: ctx << "Light Source : Shade\n";        break;
        default: break;
    }

	switch(m_MeteringMode)
	{
		case -1:                                             break;
		case  1:   ctx << "Metering Mode: average\n";       break;
		case  2:   ctx << "Metering Mode: center weight\n"; break;
		case  3:   ctx << "Metering Mode: spot\n";          break;
		case  4:   ctx << "Metering Mode: multi spot\n";    break;
		case  5:   ctx << "Metering Mode: pattern\n";       break;
		case  6:   ctx << "Metering Mode: partial\n";       break;
		case  255: ctx << "Metering Mode: other\n";         break;
		default:   ctx << "Metering Mode: unknown (" << Fmt("%d") << m_MeteringMode << ")\n"; break;
	}

	switch(m_ExposureProgram)
	{
		case 0:                                                                                break;
		case 1:  ctx << "Exposure     : Manual\n";                                            break;
		case 2:  ctx << "Exposure     : program (auto)\n";                                    break;
		case 3:  ctx << "Exposure     : aperture priority (semi-auto)\n";                     break;
		case 4:  ctx << "Exposure     : shutter priority (semi-auto)\n";                      break;
		case 5:  ctx << "Exposure     : Creative Program (based towards depth of field)\n";   break;
		case 6:  ctx << "Exposure     : Action program (based towards fast shutter speed)\n"; break;
		case 7:  ctx << "Exposure     : Portrait Mode\n";                                     break;
		case 8:  ctx << "Exposure     : LandscapeMode \n";                                    break;
		default: break;
	}

    switch(m_ExposureMode)
	{
        case 0:                                            break; // automatic
        case 1: ctx << "Exposure Mode: Manual\n";          break;
        case 2: ctx << "Exposure Mode: Auto bracketing\n"; break;
    }

	switch(m_DistanceRange)
	{
		case 0:                                    break;
		case 1: ctx << "Focus range  : macro\n";   break;
		case 2: ctx << "Focus range  : close\n";   break;
		case 3: ctx << "Focus range  : distant\n"; break;
	}

    if (TRUE) //m_jpgProcess != M_SOF0)
	{
		//--------------------------------------------------------------------------------------------------------------------
		// Table of Jpeg encoding process names
		static const CTagTable processTable[] =
		{
			{ M_SOF0,   "Baseline"                                    },
			{ M_SOF1,   "Extended sequential"                         },
			{ M_SOF2,   "Progressive"                                 },
			{ M_SOF3,   "Lossless"                                    },
			{ M_SOF5,   "Differential sequential"                     },
			{ M_SOF6,   "Differential progressive"                    },
			{ M_SOF7,   "Differential lossless"                       },
			{ M_SOF9,   "Extended sequential, arithmetic coding"      },
			{ M_SOF10,  "Progressive, arithmetic coding"              },
			{ M_SOF11,  "Lossless, arithmetic coding"                 },
			{ M_SOF13,  "Differential sequential, arithmetic coding"  },
			{ M_SOF14,  "Differential progressive, arithmetic coding" },
			{ M_SOF15,  "Differential lossless, arithmetic coding"    },
		};
#define PROCESS_TABLE_SIZE  (sizeof(processTable) / sizeof(CTagTable))

        for (int i=0;;i++)
		{
            if (i >= (int)PROCESS_TABLE_SIZE)
			{
                ctx << "Jpeg process : Unknown\n";
                break;
            }

            if (processTable[i].Tag == m_jpgProcess)
			{
                ctx << "Jpeg process : " << processTable[i].Desc << "\n";
                break;
            }
        }
    }

    if (m_Lattitude[0] || m_Longitude[0])
	{
        ctx << "GPS Latitude : " << m_Lattitude << "\n";
        ctx << "GPS Longitude: " << m_Longitude << "\n";
        if (!m_Altitude.IsEmpty())
			ctx << "GPS Altitude : " << m_Altitude << "\n";
    }

    if (TRUE) //QualityGuess)
        ctx << "JPEG Quality : " << Fmt("%d") << m_QualityGuess << "\n";

    // Print the comment. Print 'Comment:' for each new line of comment.
    if (m_Comments[0])
	{
        ctx << "Comment      : ";
        if (!m_CommentWidthchars)
		{
            for (int a=0;a<MAX_COMMENT_SIZE;a++)
			{
                char c = m_Comments[a];
                if (c == '\0') break;
                if (c == '\n')
				{
                    // Do not start a new line if the string ends with a carriage return.
                    if (m_Comments[a+1] != '\0')
					{
                        ctx << "\nComment      : ";
                    } else
					{
                        ctx << "\n";
                    }

                } else
				{
                    ctx << c;
                }
            }
            ctx << "\n";

        } else
		{
			char ss[1000];
			sprintf(ss, "%.*ls", (int)m_CommentWidthchars, (wchar_t *)m_Comments);
			ctx << ss << "\n";
        }
    }
	
	printf("%s\n", (const char*)ctx.str);
}
