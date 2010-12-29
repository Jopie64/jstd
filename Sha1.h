#pragma once

#include <string>

namespace JStd
{

typedef union
{
	unsigned char c[64];
	unsigned long l[16];
} SHA1_WORKSPACE_BLOCK;

class CSha1Hash
{
public:
	enum eMethod
	{
		REPORT_HEX = 0,
		REPORT_DIGIT = 1,
		REPORT_HEX_NOSPACE = 2,
		REPORT_HEX_DASHED = 3
	};

	CSha1Hash();

	std::string AsString(eMethod method = REPORT_HEX_NOSPACE);

private:
	unsigned char m_digest[20];

	friend class CSHA1;
};

class CSHA1
{
public:
	// Two different formats for ReportHash(...)


	// Constructor and Destructor
	CSHA1();
	virtual ~CSHA1();

//Added by Johan
	static CSha1Hash			GenerateHash(const char* P_szStringToHashPtr);
	static CSha1Hash			GenerateHash(const void* P_DataPtr, int P_iSize);

	//String must be hex. May have formatting.
	static CSha1Hash			FromString(const char* hashStr);

	static bool					IsValidHash(const char* hash);
private: //Door Johan
//End added by Johan

	unsigned long m_state[5];
	unsigned long m_count[2];
	unsigned char m_buffer[64];
	CSha1Hash	  m_Hash;

public:
	void Reset();

	// Update the hash value
	void Update(unsigned char *data, unsigned int len);
	bool HashFile(char *szFileName);

	// Finalize hash and report
	void Final();

	CSha1Hash GetHash() const {return m_Hash;}

private:
	// Private SHA-1 transformation
	void Transform(unsigned long state[5], unsigned char buffer[64]);

	// Member variables
	unsigned char m_workspace[64];
	SHA1_WORKSPACE_BLOCK *m_block; // SHA1 pointer to the byte array above
};

}