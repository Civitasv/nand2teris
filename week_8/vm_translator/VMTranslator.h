#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class Translator {
private:
  enum CommandType {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    C_EMPTY
  };

public:
  Translator(std::string file_or_directory, std::string output_file)
      : file_or_directory(file_or_directory), output_file(output_file),
        label_number(0), function_name("") {}

  void Translate();

private:
  std::string file_or_directory;
  std::string output_file;

  int label_number;
  std::string filename;
  std::string function_name;
  std::map<std::string, int> function_invoke_times; // 每一个函数调用的次数

  void TranslatePerFile(std::ofstream &output, std::string vm_file);
  // 将字符串解析为 command,去除注释，则返回空字符串
  std::string ResolveCurrentLine(std::string line);
  CommandType ResolveCommandType(std::string str);

  void WriteArithmetic(std::ofstream &output, std::string command);
  void WritePushPop(std::ofstream &output, std::string command,
                    std::string segment, int index);

  void WriteInit(std::ofstream &output);
  void WriteLabel(std::ofstream &output, std::string label);
  void WriteGoto(std::ofstream &output, std::string label);
  void WriteIf(std::ofstream &output, std::string label);

  void WriteCall(std::ofstream &output, std::string function_name,
                 int num_args);
  void WritePushSegments(std::ofstream &output, int index);
  void WritePushSegments(std::ofstream &output, std::string label);

  void WriteFunction(std::ofstream &output, std::string function_name,
                     int num_locals);
  void WriteReturn(std::ofstream &output);
};