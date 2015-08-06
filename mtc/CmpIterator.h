#ifndef __MTC_CMP_ITERATOR_H
#define __MTC_CMP_ITERATOR_H

#include <assert.h>
#include <algorithm>
#include <string.h>
#include <stdint.h>

namespace MTC{ namespace DOC{

class TextDocument;
class CharCmpIterator;
int strcmp_(const char16_t *text, unsigned long length, CharCmpIterator itor);

class CharCmpIterator
{
private:
	TextDocument	*	text_doc;
	uint32_t			offset;
public:
	CharCmpIterator();
	CharCmpIterator(TextDocument* doc, uint32_t offset);
	// default copy-constructor
	CharCmpIterator(const CharCmpIterator &src);
	// assignment operator
	CharCmpIterator & operator = (const CharCmpIterator &src);
	CharCmpIterator & operator++();
	CharCmpIterator & operator--();
	char16_t operator *();
	uint32_t Offset();

	bool operator == (const CharCmpIterator &src);
	bool operator != (const CharCmpIterator &src);
};

}}
#endif /*__MTC_CMP_ITERATOR_H*/