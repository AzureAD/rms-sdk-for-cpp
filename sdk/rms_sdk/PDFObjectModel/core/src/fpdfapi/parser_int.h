/*
 * ======================================================================
 * Copyright (c) Foxit Software, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 *======================================================================
 */

#define _FPDFAPI_PARSER_INT_H_
typedef enum {
    FPDF_FEATURE_Original				= 10,
    FPDF_FEATURE_Passwords				= 11,
    FPDF_FEATURE_MD5					= 11,
    FPDF_FEATURE_RC440Bits				= 11,
    FPDF_FEATURE_AcroForm				= 12,
    FPDF_FEATURE_FDF					= 12,
    FPDF_FEATURE_Linearized				= 12,
    FPDF_FEATURE_LZWDecode				= 12,
    FPDF_FEATURE_FlateDecode			= 12,
    FPDF_FEATURE_Unicode				= 12,
    FPDF_FEATURE_DigitalSignatures		= 13,
    FPDF_FEATURE_ExplicitMask			= 13,
    FPDF_FEATURE_ColorKeyMask			= 13,
    FPDF_FEATURE_ICCColorSpace			= 13,
    FPDF_FEATURE_DeviceNColorSpace		= 13,
    FPDF_FEATURE_JavaScript				= 13,
    FPDF_FEATURE_CIDFont				= 13,
    FPDF_FEATURE_JBIG2Decode			= 14,
    FPDF_FEATURE_Transparency			= 14,
    FPDF_FEATURE_RC440128Bits			= 14,
    FPDF_FEATURE_TaggedPDF				= 14,
    FPDF_FEATURE_ObjStm					= 15,
    FPDF_FEATURE_XRefStm				= 15,
    FPDF_FEATURE_JPXDecode				= 15,
    FPDF_FEATURE_PKCS7					= 15,
    FPDF_FEATURE_URSignatures			= 15,
    FPDF_FEATURE_RSA4096Bits			= 15,
    FPDF_FEATURE_CustomEncrypt			= 15,
    FPDF_FEATURE_EmbeddedOpenType		= 16,
    FPDF_FEATURE_AESEncrypt				= 16,
    FPDF_FEATURE_XFA					= 16,
    FPDF_FEATURE_PKCS7SHA256			= 16,
    FPDF_FEATURE_DSA4096Bits			= 16,
    FPDF_FEATURE_NChannelColorSpace		= 16,
    FPDF_FEATURE_PKCS7SHA384			= 17,
    FPDF_FEATURE_PKCS7SHA512			= 17,
    FPDF_FEATURE_AES256BitsEncrypt		= 17,
    FPDF_FEATURE_PDFDocEncodedString	= 17,
    FPDF_FEATURE_XFA25					= 17,
    FPDF_FEATURE_XFA26					= 17,
    FPDF_FEATURE_XFA30					= 17,
    FPDF_FEATURE_XFA31					= 17,
    FPDF_FEATURE_XFA33					= 17,
} CPDF_PDFFeature;
class CPDF_PDFVersionChecker
{
public:
    static CPDF_PDFFeature		VersionCheck(CPDF_Parser* pParser, CPDF_PDFFeature feature);
};
