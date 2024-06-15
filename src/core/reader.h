#ifndef TK_READER_H
#define TK_READER_H

#include <string>
#include <fstream>
#include <vector>

enum class EReaderType
{
    File,
    Memory
};

enum class EReaderMode
{
    Read,
    Write,
    All,
};

enum class EReaderFormat
{
    Binary,
    Text
};

class tkReader
{
private:
    EReaderType m_Type;
    EReaderMode m_Mode;
    EReaderFormat m_Format;

    char* m_Memory = nullptr;

    std::fstream m_File;
public:
    static const char* ReadTextFile(const std::string& path);
private:
    tkReader(EReaderType type = EReaderType::File, EReaderMode mode = EReaderMode::Read, EReaderFormat format = EReaderFormat::Text);
    ~tkReader();
    bool Open(const std::string& path);
    std::string Read(const std::string& path);
    std::string Read();
    [[maybe_unused]] static tkReader& TextFileReader();
    [[maybe_unused]] static tkReader& CreateBinaryFileReader();
    [[maybe_unused]] static tkReader& CreateTextMemoryReader();
    [[maybe_unused]] static tkReader& CreateBinaryMemoryReader();
    static const char* ReadCStr();
    [[nodiscard]] bool IsMemoryValid() const;
    void ClearMemory();
    const char* GetMemory();
    void SetMemory(const char* memory);
    void Close();
};

#endif//TK_READER_H
