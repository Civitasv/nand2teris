#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

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
      : input_file(input_file), output_file(output_file), label_number(0) {
    std::cout << input_file;
    filename = std::filesystem::path(input_file).replace_extension().string();
    filename = filename.substr(filename.find_last_of('/') + 1);
  }

  void Translate();

private:
  std::string input_file;
  std::string output_file;

  int label_number;
  std::string filename;

  // 将字符串解析为 command,去除注释，则返回空字符串
  std::string ResolveCurrentLine(std::string line);
  CommandType ResolveCommandType(std::string str);

  void WriteArithmetic(std::ofstream &output, std::string command);
  void WritePushPop(std::ofstream &output, std::string command,
                    std::string segment, int index);
};