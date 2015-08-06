#include "CmpIterator.h"
#include "Textdocument.h"
namespace MTC{ namespace DOC{
int strcmp_(const char16_t *text, unsigned long length, CharCmpIterator itor)
{
	int dif = 0;
	int i = 0;
	for (i = 0;
		i < length && (dif = text[i] - *itor) == 0;
		++itor, ++i);

	return dif;
}
CharCmpIterator::CharCmpIterator()
	: text_doc(0), offset(0)
{
}
CharCmpIterator::CharCmpIterator(TextDocument *doc, uint32_t offset)
	: text_doc(doc), offset(offset)
{

}
	// default copy-constructor
CharCmpIterator::CharCmpIterator(const CharCmpIterator &src) 
	: text_doc(src.text_doc), offset(src.offset)
{
}
	// assignment operator
CharCmpIterator& CharCmpIterator::operator = (const CharCmpIterator &src)
{
	text_doc	= src.text_doc;
	offset		= src.offset;
	return *this;
}
CharCmpIterator& CharCmpIterator::operator++()
{
	offset ++;
	//if(offset == text_doc->charCount())
	return *this;
}
CharCmpIterator& CharCmpIterator::operator--()
{
	offset --;
	//if(offset == text_doc->charCount())
	return *this;
}
char16_t CharCmpIterator::operator *()
{
	if(offset < 0)
	{
		return (char16_t)0xFFFF;
	}
	else if(offset < text_doc->TextLength())
	{
		char16_t buf;
		uint32_t buflen = sizeof(char16_t) ;
		uint32_t off_b  =text_doc->charoffset_to_byteoffset(offset);
		uint32_t len = text_doc->mSeq.render(off_b, (SEQ::seqchar*)&buf, sizeof(char16_t) / sizeof(SEQ::seqchar));
		return buf;
	}
	else
		return (char16_t)0x0;
}
uint32_t CharCmpIterator::Offset()
{
	return (uint32_t)std::min<uint32_t>(std::max<long>(0, offset), text_doc->TextLength());
}
bool CharCmpIterator::operator == (const CharCmpIterator &src)
{
	return(!(*this != src));
}
bool CharCmpIterator::operator != (const CharCmpIterator &src)
{
	return(text_doc != src.text_doc  || src.offset != offset);
}

}}