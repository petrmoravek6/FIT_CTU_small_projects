#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cassert>
using namespace std;

#define UTF8CHAR vector<uint8_t>

// Represents a node in a binary tree, that can store a UTF-8 sign
struct CNode {
    shared_ptr<CNode> m_left;
    shared_ptr<CNode> m_right;
    UTF8CHAR m_character;
};

/**
 * Loads binary data from file into its "buffer" (m_buffer).
 * Provides methods for reading specific
 * amount of bits. Every time these reading methods are called it "pops back" the data
 * and moves onto another bit in line.
 * This is done by remembering the index of current byte that is to be loaded (m_byteIndex)
 * and remembering which bit of the byte is to be loaded (m_bitCounter). It goes from the
 * highest bit to the lowest.
 */
class CBitReader {
public:
    // Constructor throws an exception if an error during reading the file occurs.
    CBitReader (const char * filename);
    bool getNextBit ();
    uint8_t getNextByte ();
    // Returnes the next 12bit unsigned number.
    // There is no data type for it, so it has to put it into 16bit unsigned int.
    uint16_t getNext12BitNumber();
    // Returnes a UTF-8 character which is stored in vector of bytes (uint8_t).
    // It can consist of from 1 to 4 bytes.
    // If the structure is incorrect or the code point does not match the specific UTF-8 rules it throws an exception.
    UTF8CHAR getNextUtf8Char();
    // Throw away all bits till the begining of next byte.
    void skipToTheEndOfByte();
    // Returnes true if there is a byte to load from buffer.
    bool moreDataToLoad() const;
    // Creates a Huffman binary tree by reading the buffer.
    // Huffman tree servers as a decoding table of Huffman code
    // If an error occures (wrong UTF-8 structure) it throws an exception.
    // Returnes a head pointing to the beggining of the tree.
    shared_ptr<CNode> createNewHuffmanTree();
    // Uses a pointer that points to the head of Huffman tree as parameter.
    // Returnes UTF-8 character by reading the buffer and slidiing down the tree.
    UTF8CHAR getCharacterFromHuffmanTree(shared_ptr<CNode> head);

private:
    vector<uint8_t> m_buffer;
    short m_bitCounter;
    size_t m_byteIndex;
    // Returnes amount of bytes of a specific UTF-8 character depending on the first byte of the char.
    static short countBytesOfUTF(uint8_t firstByte);
    // Says whether given bytes are valid UTF-8 character defined by code point convencion.
    static bool validUTF8CodePoint(const vector<uint8_t> & bytes);
};

CBitReader::CBitReader ( const char * filename ) : m_bitCounter(1), m_byteIndex(0)
{
    ifstream file(filename, ios::binary);
    if (!file)
        throw exception();
    char c;
    while (file.get(c)) {
        m_buffer.push_back((unsigned char)c);
    }
    if (!file.eof())
        throw exception();
    file.clear();
    file.close();

    if (file.fail())
        throw exception();
}

bool CBitReader::getNextBit()
{
    uint8_t & currentByte = m_buffer.at(m_byteIndex);
    bool bit;

    if ((currentByte & 0b10000000) == 0b10000000) {
        bit = true;
    }
    else {
        bit = false;
    }

    // We are reading a bit from next byte
    if (++m_bitCounter >= 9) {
        m_byteIndex++;
        m_bitCounter = 1;
    }
    else {
        currentByte <<= 1;
    }
    return bit;
}
uint8_t CBitReader::getNextByte()
{
    uint8_t res = 0b00000000;
    // Reading first 7 bites
    for (short i = 1; i <= 7; i++) {
        if (getNextBit()) {
            res |= 0b00000001;
        }
        res <<= 1;
    }
    // Reading last bit
    if (getNextBit()) {
        res |= 0b00000001;
    }
    return res;
}
// Returnes how many bytes a UTF sign has (1-4). It is telling by the first byte.
// If the first byte cannot be from UTF sign, returnes 0.
short CBitReader::countBytesOfUTF(const uint8_t firstByte) {
    if (firstByte <= 0b01111111) return 1;
    else if (firstByte >= 0b11000000 && firstByte <= 0b11011111) return 2;
    else if (firstByte >= 0b11100000 && firstByte <= 0b11101111) return 3;
    else if (firstByte >= 0b11110000 && firstByte <= 0b11110111) return 4;
    else return 0;
}

UTF8CHAR CBitReader::getNextUtf8Char()
{
    vector<uint8_t> res;
    uint8_t firstByte = getNextByte();
    short numberOfBytes = countBytesOfUTF(firstByte);
    if (numberOfBytes == 0) {
        throw invalid_argument("wrong UTF structure");
    }
    // The structure seems fine so far. We can save the first byte.
    res.push_back(firstByte);
    // Check the structure of other bytes
    for (short i = 1; i < numberOfBytes; i++) {
        uint8_t nextByte = getNextByte();
        if (nextByte >= 0b10000000 && nextByte <= 0b10111111) {
            res.push_back(nextByte);
        }
        else {
            // The byte does not match UTF-8 strcture
            throw invalid_argument("wrong UTF structure");
        }
    }
    if (!validUTF8CodePoint(res))
        throw invalid_argument("invalid UTF-8 code point");
    return res;
}

uint16_t CBitReader::getNext12BitNumber()
{
    uint16_t res = 0b0000000000000000;
    for (short i = 1; i <= 12; i++) {
        if (getNextBit()) {
            res |= 0b0000000000000001;
        }
        if (i != 12) {
            res <<= 1;
        }
    }
    return res;
}

void CBitReader::skipToTheEndOfByte()
{
    // We are already in the end (reader could teoretically read first bit)
    if(m_bitCounter == 1)
        return;
    for (short i = m_bitCounter; i <= 8; ++i) {
        getNextBit();
    }
}

bool CBitReader::moreDataToLoad() const
{
    return m_byteIndex < m_buffer.size();
}

shared_ptr<CNode> CBitReader::createNewHuffmanTree() {
    shared_ptr<CNode> newNode = make_shared<CNode>();
    // 1 means make a list and put UTF-8 character from buffer there.
    if (getNextBit()) {
        newNode->m_left = nullptr;
        newNode->m_right = nullptr;
        newNode->m_character = getNextUtf8Char();
    }
        // 0 means make an empty node
    else {
        newNode->m_left = createNewHuffmanTree();
        newNode->m_right = createNewHuffmanTree();
    }
    return newNode;
}

UTF8CHAR CBitReader::getCharacterFromHuffmanTree(shared_ptr<CNode> head)
{
    // Read until we procced to list (unempty node).
    while (head->m_left != nullptr) {
        if (getNextBit()) {
            head = head->m_right;
        }
        else {
            head = head->m_left;
        }
    }
    return head->m_character;
}

bool CBitReader::validUTF8CodePoint(const vector<uint8_t> & bytes) {
    // We know that the character consists of only 1 - 4 bytes, so we can store it into short.
    const short size = bytes.size();
    uint32_t codePoint = 0;
    // Getting bits of the code point from first byte
    // We don't need to check the validity of UTF-8 character that consits of only one byte
    // because we checked it in the countBytesOfUTF function.
    switch(size) {
        case 1:
            return true;
        case 2:
            codePoint = bytes[0] & 0b00011111;
            break;
        case 3:
            codePoint = bytes[0] & 0b00001111;
            break;
        case 4:
            codePoint = bytes[0] & 0b00000111;
            break;
    }
    // Getting bits of the code point from other bytes
    for(short i = 1; i < size; ++i) {
        codePoint <<= 6;
        codePoint |= bytes[i] & 0b00111111;
    }
    // Checking code point rules
    if((size == 2 && codePoint >= 0x0080 && codePoint <= 0x07FF)
    || (size == 3 && codePoint >= 0x0800 && codePoint <= 0xFFFF)
    || (size == 4 && codePoint >= 0x10000 && codePoint <= 0x10FFFF))
        return true;
    return false;
}

// Auxiliary class for binary writing UTF-8 chars into file
class CWriter {
public:
    // Returnes false if an error during opening the file occurs
    bool open(const char * filename);
    // Apends one UTF-8 character into file.
    // Returnes false if an error during writing occurs.
    bool write(const UTF8CHAR & UTF8char);
    // Returnes false if an error during closing the file occurs
    bool close();
private:
    ofstream m_file;
};

bool CWriter::open(const char * filename)
{
    if(m_file.is_open()) return false;
    m_file.open(filename, ios::binary);
    if (!m_file) return false;
    return true;
}

bool CWriter::write(const UTF8CHAR & UTF8char)
{
    if(!m_file.is_open())
        return false;
    for(uint8_t byte : UTF8char) {
        m_file.put(byte);
        if(!m_file)
            return false;
    }
    return true;
}

bool CWriter::close()
{
    if(!m_file.is_open())
        return false;
    m_file.close();
    if(!m_file)
        return false;
    return true;
}

bool decompressFile ( const char * inFileName, const char * outFileName )
{
    try {
        CBitReader br(inFileName);
        shared_ptr<CNode> headOfHuffmanTree = br.createNewHuffmanTree();
        CWriter writer;
        if (!writer.open(outFileName))
            return false;
        // 1 means that we are going to read 4096 characters
        while (br.getNextBit()) {
            for (int i = 0; i < 4096; ++i) {
                // Get character from the tree and write it into the file
                if (!writer.write(br.getCharacterFromHuffmanTree(headOfHuffmanTree)))
                    return false;
            }
        }
        // Reading last zero chunk
        uint16_t lastChunkCnt = br.getNext12BitNumber();
        for (int i = 0; i < lastChunkCnt; ++i) {
            if (!writer.write(br.getCharacterFromHuffmanTree(headOfHuffmanTree)))
                return false;
        }
        if (!writer.close())
            return false;
        // We are in the end. Skip padding and check if there is no more data.
        br.skipToTheEndOfByte();
        if(br.moreDataToLoad())
            return false;
    } catch (const exception & e) {
        // Return false if any known error occures.
        return false;
    }
    return true;
}





bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
    ifstream file1(fileName1, ios::in);
    ifstream file2(fileName2, ios::in);
    if (!file1 || !file2) {
        return false;
    }
    stringstream buffer1, buffer2;
    buffer1 << file1.rdbuf();
    buffer2 << file2.rdbuf();
    return buffer1.str() == buffer2.str();
}

int main ()
{
    assert (identicalFiles("tests/test0.orig", "tests/test0.orig"));
    assert (identicalFiles("tests/test0.orig", "tests/test0_b.orig"));
    assert (identicalFiles("tests/test0_b.orig", "tests/test0_b.orig"));
    assert (identicalFiles("tests/test0_b.orig", "tests/test0.orig"));
    assert (! identicalFiles("tests/test0.orig", "tests/test0_c.orig"));
    assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

    assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );


    assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

    assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
    assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

    return 0;
}