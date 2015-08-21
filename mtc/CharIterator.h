#ifndef __MTC_CMP_ITERATOR_H
#define __MTC_CMP_ITERATOR_H

#include <assert.h>
#include <algorithm>
#include <string.h>
#include <stdint.h>

namespace MTC{ namespace DOC{

class TextDocument;
class CharIterator;
int strcmp_(const char16_t *text, unsigned long length, CharIterator itor);

class CharIterator
{
private:
	TextDocument	*	text_doc;
	uint32_t			offset;
public:
	CharIterator();
	CharIterator(TextDocument* doc, uint32_t offset);
	// default copy-constructor
	CharIterator(const CharIterator &src);
	// assignment operator
	CharIterator & operator = (const CharIterator &src);
	CharIterator & operator++();
	CharIterator & operator--();
	char16_t operator *();
	uint32_t Offset();

	bool operator == (const CharIterator &src);
	bool operator != (const CharIterator &src);
};

}}
#endif /*__MTC_CMP_ITERATOR_H*/