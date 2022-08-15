#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
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
  Translator(std::string input_file, std::string output_file)
      : input_file(input_file), output_file(output_file), label_number(0),
        function_name(""), function_ret_i(0) {
    filename = std::filesystem::path(input_file).replace_extension().string();
    filename = filename.substr(filename.find_last_of('/') + 1);
  }

  void Translate();

private:
  std::string input_file;
  std::string output_file;

  int label_number;
  std::string filename;
  std::string function_name; // 当前正在处理的函数名
  int function_ret_i; // 对每一个 call 命令，递增，函数改变时，需要归0

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