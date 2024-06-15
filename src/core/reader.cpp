#include "Reader.h"
#include "def.h"
#include <fstream>

#if defined(__EMSCRIPTEN__)
#define RESOURCE_PATH std::string("res/")
#else
#define RESOURCE_PATH std::string("res/")
#endif

tkReader::tkReader(const EReaderType type, const EReaderMode mode, const EReaderFormat format) :
  m_Type(type),
  m_Mode(mode),
  m_Format(format)
{}

tkReader& tkReader::TextFileReader()
{
  static tkReader reader(EReaderType::File, EReaderMode::Read, EReaderFormat::Text);
  return reader;
}

tkReader& tkReader::CreateBinaryFileReader()
{
  static tkReader reader(EReaderType::File, EReaderMode::Read, EReaderFormat::Binary);
  return reader;
}

tkReader& tkReader::CreateTextMemoryReader()
{
  static tkReader reader(EReaderType::Memory, EReaderMode::Read, EReaderFormat::Text);
  return reader;
}

tkReader& tkReader::CreateBinaryMemoryReader()
{
  static tkReader reader(EReaderType::Memory, EReaderMode::Read, EReaderFormat::Binary);
  return reader;
}

bool tkReader::Open(const std::string& path)
{
  if (m_Type != EReaderType::File)
  {
    printf("Reader::Open: Reader type is not File\n");
    return false;
  }

  int flags = 0;
  if (m_Mode == EReaderMode::Read)
  {
    flags |= std::ios::in;
  }
  else if (m_Mode == EReaderMode::Write)
  {
    flags |= std::ios::out;
  }
  else if (m_Mode == EReaderMode::All)
  {
    flags |= std::ios::in | std::ios::out;
  }

  if (m_Format == EReaderFormat::Binary)
  {
    flags |= std::ios::binary;
  }

  m_File.open(std::string("../res/") + path, flags);
  if (!m_File.is_open())
  {
    printf("Reader::Open: Failed to open file %s\n", (RESOURCE_PATH + path).c_str());
    return false;
  }
  return true;
}

void tkReader::Close()
{
  if (m_Type != EReaderType::File)
  {
    printf("Reader::Close: Reader type is not File\n");
    return;
  }

  if (!m_File.is_open())
  {
    return;
  }

  m_File.close();
}

std::string tkReader::Read()
{
  if (m_Type != EReaderType::File)
  {
    printf("Reader::Read: Reader type is not File\n");
    return "";
  }

  std::string line;
  std::string content;
  while (std::getline(m_File, line))
  {
    content += line + "\n";
  }

  Close();
  return content;
}

std::string tkReader::Read(const std::string &path)
{
  Open(path);
  return Read();
}

const char* tkReader::ReadCStr()
{
  tkReader& reader = tkReader::TextFileReader();
  reader.SetMemory(reader.Read().c_str());
  return reader.GetMemory();
}

const char *tkReader::ReadTextFile(const std::string &path)
{
  tkReader::TextFileReader().Open(path);
  return ReadCStr();
}

tkReader::~tkReader()
{
  ClearMemory();
  Close();
}

bool tkReader::IsMemoryValid() const
{
  return m_Memory != nullptr;
}

void tkReader::ClearMemory()
{
  if(IsMemoryValid())
  {
    delete[] m_Memory;
    m_Memory = nullptr;
  }
}

void tkReader::SetMemory(const char *memory)
{
  ClearMemory();
  i32 len = strlen(memory) + 1;
  m_Memory = new char[len];
  strcpy_s(m_Memory, len, memory);
}

const char *tkReader::GetMemory()
{
  return m_Memory;
}
