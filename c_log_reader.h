#pragma once

class CLogReader
{
public:
	CLogReader();
	~CLogReader();

	bool Open(const char* filename);
	void Close();

	bool SetFilter(const char* filter);
	bool GetLineNext(char* buf, const int bufsize);

private:
	class Impl;

private:
	Impl* _impl;
};
