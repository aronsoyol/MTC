#include "CharIterator.h"
#include "Textdocument.h"
namespace MTC{ namespace DOC{
int strcmp(const char16_t *text, unsigned long length, CharIterator itor)
{
	int dif = 0;
	int i = 0;
	for (i = 0;
		i < length && (dif = text[i] - *itor) == 0;
		++itor, ++i);

	return dif;
}
CharIterator::CharIterator()
	: text_doc(0), offset(0)
{
}
CharIterator::CharIterator(TextDocument *doc, uint32_t offset)
	: text_doc(doc), offset(offset)
{

}
	// default copy-constructor
CharIterator::CharIterator(const CharIterator &src) 
	: text_doc(src.text_doc), offset(src.offset)
{
}
	// assignment operator
CharIterator& CharIterator::operator = (const CharIterator &src)
{
	text_doc	= src.text_doc;
	offset		= src.offset;
	return *this;
}
CharIterator& CharIterator::operator++()
{
	offset ++;
	//if(offset == text_doc->charCount())
	return *this;
}
CharIterator& CharIterator::operator--()
{
	offset --;
	//if(offset == text_doc->charCount())
	return *this;
}
char16_t CharIterator::operator *()
{
	if(offset < 0)
	{
		return (char16_t)0xFFFF;
	}
	else if(offset < text_doc->TextLength())
	{
		char16_t buf;
		uint32_t len	= text_doc->mSeq.render(offset, (SEQ::seqchar*)&buf, 1);
		return buf;
	}
	else
		return (char16_t)0x0;
}
uint32_t CharIterator::Offset()
{
	return (uint32_t)std::min<uint32_t>(std::max<long>(0, offset), text_doc->TextLength());
}
bool CharIterator::operator == (const CharIterator &src)
{
	return(!(*this != src));
}
bool CharIterator::operator != (const CharIterator &src)
{
	return(text_doc != src.text_doc  || src.offset != offset);
}

}}