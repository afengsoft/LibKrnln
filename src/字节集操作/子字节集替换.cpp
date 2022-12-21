﻿#include<vector>
#include<limits>
using namespace std;


static inline intptr_t  mymemchr(const unsigned char* pSrc, intptr_t nLen, unsigned char Des)
{
	unsigned int longword, dmagic, * longword_ptr;
	const unsigned char* cp;
	const unsigned char* char_ptr = pSrc;
	for (int i = 0; i < (nLen & 3); i++)
		if (*char_ptr++ == Des)
			return char_ptr - pSrc - 1;

	dmagic = Des | (Des << 8) | (Des << 16) | (Des << 24);
	for (longword_ptr = (unsigned int*)char_ptr; longword_ptr < (unsigned int*)(pSrc + nLen); longword_ptr++)
	{
		longword = *longword_ptr ^ dmagic;
		if (((longword - 0x01010101) & ~longword & 0x80808080) != 0)
		{
			cp = (unsigned char*)(longword_ptr);
			if (cp[0] == Des)
				return cp - pSrc;
			if (cp[1] == Des)
				return cp - pSrc + 1;
			if (cp[2] == Des)
				return cp - pSrc + 2;
			if (cp[3] == Des)
				return cp - pSrc + 3;
		}
	}
	return -1;
}

static inline void make_delta1(intptr_t* delta1, const unsigned char* pat, intptr_t patlen) {
	intptr_t i;
	for (i = 0; i < 256; i++) {
		delta1[i] = patlen;
	}
	for (i = 0; i < patlen - 1; i++) {
		delta1[pat[i]] = patlen - 1 - i;
	}
}
static inline int is_prefix(const unsigned char* word, intptr_t wordlen, intptr_t pos) {
	intptr_t i;
	intptr_t suffixlen = wordlen - pos;

	for (i = 0; i < suffixlen; i++) {
		if (word[i] != word[pos + i]) {
			return 0;
		}
	}
	return 1;
}

// length of the longest suffix of word ending on word[pos].
// suffix_length("dddbcabc", 8, 4) = 2
static inline intptr_t suffix_length(const unsigned char* word, intptr_t wordlen, intptr_t pos) {
	intptr_t i;
	// increment suffix length i to the first mismatch or beginning
	// of the word
	for (i = 0; (word[pos - i] == word[wordlen - 1 - i]) && (i < pos); i++);
	return i;
}
static inline void make_delta2(intptr_t* delta2, const  unsigned char* pat, intptr_t patlen) {
	intptr_t p;
	intptr_t last_prefix_index = 1;

	// first loop, prefix pattern
	for (p = patlen - 1; p >= 0; p--) {
		if (is_prefix(pat, patlen, p + 1)) {
			last_prefix_index = p + 1;
		}
		delta2[p] = (patlen - 1 - p) + last_prefix_index;
	}

	// this is overly cautious, but will not produce anything wrong
	// second loop, suffix pattern
	for (p = 0; p < patlen - 1; p++) {
		intptr_t slen = suffix_length(pat, patlen, p);
		if (pat[p - slen] != pat[patlen - 1 - slen]) {
			delta2[patlen - 1 - slen] = patlen - 1 - p + slen;
		}
	}
}
static inline intptr_t boyer_moore(const  unsigned char* string, intptr_t stringlen, const  unsigned char* pat, intptr_t patlen) {
	intptr_t i;
	intptr_t delta1[256];
	intptr_t* delta2 = (intptr_t*)malloc(patlen * sizeof(intptr_t));
	if (!delta2)
		return -1;
	make_delta1(delta1, pat, patlen);
	make_delta2(delta2, pat, patlen);
	intptr_t n_shifts = 0;

	i = patlen - 1;
	while (i < stringlen) {
		intptr_t j = patlen - 1;
		while (j >= 0 && (string[i] == pat[j])) {
			--i;
			--j;
		}

		if (j < 0) {
			free(delta2);
			return i + 1;
		}

		i += max(delta1[string[i]], delta2[j]);
	}
	free(delta2);
	return -1;
}
static inline intptr_t  myinstring(const unsigned char* src, intptr_t slen, const  unsigned char* des, intptr_t dlen)
{
	//	register unsigned char* naddr;
	//	int sl;int i;int j;
	intptr_t i;
	if (!src || !des || !slen || !dlen || dlen > slen)
		return -1;

	switch (dlen)
	{
	case 1:  // 短子串直接暴力搜索
		return mymemchr(src, slen, des[0]);
	case 2:
		for (i = 0; i < slen - 1; i++)
			if (((short*)(src + i))[0] == ((short*)des)[0])
				return i;
		return -1;
	case 3:
		for (i = 0; i < slen - 2; i++)
			if (((short*)(src + i))[0] == ((short*)des)[0])
				if (src[i + 2] == des[2])
					return i;
		return -1;
	case 4:
		for (i = 0; i < slen - 3; i++)
			if (((int*)(src + i))[0] == ((int*)des)[0])
				return i;
		return -1;
	case 5:
		for (i = 0; i < slen - 4; i++)
			if (((int*)(src + i))[0] == ((int*)des)[0])
				if (src[i + 4] == des[4])
					return i;
		return -1;
	case 6:
		for (i = 0; i < slen - 5; i++)
			if (((int*)(src + i))[0] == ((int*)des)[0])
				if (((short*)(src + i + 4))[0] == ((short*)(des + 4))[0])
					return i;
		return -1;
	default:// 长子串使用BM算法,
		return boyer_moore(src, slen, des, dlen);
	}

}







KrnlnApi vector<unsigned char> 子字节集替换(vector<unsigned char> 欲被替换的字节集, vector<unsigned char> 欲被替换的子字节集, vector<unsigned char> 用作替换的子字节集, size_t 进行替换的起始位置, size_t 替换进行的次数) // 被封七号 改
{
	if (欲被替换的字节集.empty())
	{
		return {};
	}
	if (欲被替换的子字节集.empty())
	{
		return 欲被替换的字节集;
	}
	const unsigned char* pSrc = 欲被替换的字节集.data();
	intptr_t nSLen = 欲被替换的字节集.size();
	const unsigned char* pDes = 欲被替换的子字节集.data();
	intptr_t nDLen = 欲被替换的子字节集.size();
	intptr_t nCount = 替换进行的次数;
	intptr_t nStart;
	if (进行替换的起始位置 <= 1)
		nStart = 0;
	else
		nStart = 进行替换的起始位置 - 1;

	if (nStart >= nSLen || nDLen > nSLen) // 关键参数不合法，直接返回原字节集
		return 欲被替换的字节集;

	const unsigned char* pSub = NULL;
	intptr_t nSubLen = 0;
	if (!用作替换的子字节集.empty())
	{
		nSubLen = 用作替换的子字节集.size();
		pSub = 用作替换的子字节集.data();
	}

	// 开始计算
	vector<vector<unsigned char>> tbr;
	intptr_t nPos;
	const unsigned char* pFirst = pSrc;
	const unsigned char* pSearch = pSrc + nStart;
	intptr_t nSYLen = nSLen;
	const unsigned char* pLast = pSrc + nSLen;

	for (; nCount > 0; nCount--)
	{
		nPos = myinstring(pSearch, nSYLen, pDes, nDLen);
		if (nPos == -1)
			break;
		if (pSearch + nPos - pFirst > 0)
			tbr.push_back(vector<unsigned char>(pFirst, pSearch + nPos));

		if (nSubLen > 0)
			tbr.push_back(vector<unsigned char>(pSub, pSub + nSubLen));

		pSearch += nPos + nDLen;
		pFirst = pSearch;
		nSYLen -= nPos + nDLen;
	}

	if (pLast - pFirst > 0)
		tbr.push_back(vector<unsigned char>(pFirst, pLast));

	// 复制计算结果
	vector<unsigned char> pRetn;
	nCount = tbr.size();
	for (intptr_t i = 0; i < nCount; i++)
	{
		if (tbr[i].empty())
		{
			continue;
		}
		size_t poff = pRetn.size();
		pRetn.resize(poff + tbr[i].size());
		memcpy(&pRetn[poff], tbr[i].data(), tbr[i].size());
	}
	return pRetn;
}
KrnlnApi vector<unsigned char> 子字节集替换(const vector<unsigned char>& 欲被替换的字节集, const vector<unsigned char>& 欲被替换的子字节集, const vector<unsigned char>& 用作替换的子字节集, size_t 进行替换的起始位置) // 被封七号 改
{
	if (欲被替换的字节集.empty())
	{
		return {};
	}
	if (欲被替换的子字节集.empty())
	{
		return 欲被替换的字节集;
	}
	const unsigned char* pSrc = 欲被替换的字节集.data();
	intptr_t nSLen = 欲被替换的字节集.size();
	const unsigned char* pDes = 欲被替换的子字节集.data();
	intptr_t nDLen = 欲被替换的子字节集.size();
	intptr_t nCount = numeric_limits<intptr_t>::max();
	intptr_t nStart;
	if (进行替换的起始位置 <= 1)
		nStart = 0;
	else
		nStart = 进行替换的起始位置 - 1;

	if (nStart >= nSLen || nDLen > nSLen) // 关键参数不合法，直接返回原字节集
		return 欲被替换的字节集;

	const unsigned char* pSub = NULL;
	intptr_t nSubLen = 0;
	if (!用作替换的子字节集.empty())
	{
		nSubLen = 用作替换的子字节集.size();
		pSub = 用作替换的子字节集.data();
	}

	// 开始计算
	vector<vector<unsigned char>> tbr;
	intptr_t nPos;
	const unsigned char* pFirst = pSrc;
	const unsigned char* pSearch = pSrc + nStart;
	intptr_t nSYLen = nSLen;
	const unsigned char* pLast = pSrc + nSLen;

	for (; nCount > 0; nCount--)
	{
		nPos = myinstring(pSearch, nSYLen, pDes, nDLen);
		if (nPos == -1)
			break;
		if (pSearch + nPos - pFirst > 0)
			tbr.push_back(vector<unsigned char>(pFirst, pSearch + nPos));

		if (nSubLen > 0)
			tbr.push_back(vector<unsigned char>(pSub, pSub + nSubLen));

		pSearch += nPos + nDLen;
		pFirst = pSearch;
		nSYLen -= nPos + nDLen;
	}

	if (pLast - pFirst > 0)
		tbr.push_back(vector<unsigned char>(pFirst, pLast));

	// 复制计算结果
	vector<unsigned char> pRetn;
	nCount = tbr.size();
	for (intptr_t i = 0; i < nCount; i++)
	{
		if (tbr[i].empty())
		{
			continue;
		}
		size_t poff = pRetn.size();
		pRetn.resize(poff + tbr[i].size());
		memcpy(&pRetn[poff], tbr[i].data(), tbr[i].size());//子串拼接
	}
	return pRetn;
}
