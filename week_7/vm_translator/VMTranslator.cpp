#include "VMTranslator.h"

#include <climits>
#include <iostream>
#include <iterator>
#include <sstream>

#include "utils.h"

void Translator::Translate() {
  std::ifstream in_file(input_file);
  std::ofstream output(output_file);

  if (in_file.is_open()) {
    std::string line;
    while (std::getline(in_file, line)) {
      // 去除注释，返回命令
      auto current_command = ResolveCurrentLine(line);
      if (current_command == "") {
        continue;
      }
      auto commands = Split(current_command, ' ');

      auto command = commands[0];
      // get command type
      auto current_command_type = ResolveCommandType(commands[0]);

      if (current_command_type == C_ARITHMETIC) {
        WriteArithmetic(output, command);
      } else if (current_command_type == C_PUSH ||
                 current_command_type == C_POP ||
                 current_command_type == C_FUNCTION ||
                 current_command_type == C_CALL) {
        auto arg1 = commands[1];
        auto arg2 = std::stoi(commands[2]);
        WritePushPop(output, command, arg1, arg2);
      }
    }
    // finish
    in_file.close();
    output.close();
  }
}

std::string Translator::ResolveCurrentLine(std::string line) {
  std::stringstream out;
  for (size_t i = 0; i < line.size(); i++) {
    if ((i != line.size() - 1) && (line[i] == '/' && line[i + 1] == '/')) {
      break;
    }
    if (line[i] != '\r')
      out << line[i];
  }
  return out.str();
}

Translator::CommandType Translator::ResolveCommandType(std::string type) {
  if (type == "add" || type == "sub" || type == "neg" || type == "eq" ||
      type == "gt" || type == "lt" || type == "and" || type == "or" ||
      type == "not") {
    return C_ARITHMETIC;
  } else if (type == "push") {
    return C_PUSH;
  } else if (type == "pop") {
    return C_POP;
  }
  return C_EMPTY;
}

void Translator::WriteArithmetic(std::ofstream &output, std::string command) {
  if (command == "add") {
    // M[SP] -- ;
    output << "@SP" << '\n';
    output << "AM=M-1" << '\n';
    // y = M[M[SP]]
    output << "D=M" << '\n';
    // M[M[SP]-1] = y + M[M[SP]-1]
    output << "A=A-1" << '\n';
    output << "M=M+D" << '\n';
  } else if (command == "sub") {
    // M[SP] -- ;
    output << "@SP" << '\n';
    output << "AM=M-1" << '\n';
    // y = M[M[SP]]
    output << "D=M" << '\n';
    // M[M[SP]-1] = M[M[SP]-1]-y
    output << "A=A-1" << '\n';
    output << "M=M-D" << '\n';
  } else if (command == "and") {
    // M[SP] -- ;
    output << "@SP" << '\n';
    output << "AM=M-1" << '\n';
    // y = M[M[SP]]
    output << "D=M" << '\n';
    // M[M[SP]-1] = M[M[SP]-1] and y
    output << "A=A-1" << '\n';
    output << "M=D&M" << '\n';
  } else if (command == "or") {
    // M[SP] -- ;
    output << "@SP" << '\n';
    output << "AM=M-1" << '\n';
    // y = M[M[SP]]
    output << "D=M" << '\n';
    // M[M[SP]-1] = M[M[SP]-1] or y
    output << "A=A-1" << '\n';
    output << "M=D|M" << '\n';
  } else if (command == "not") {
    // A = M[SP]-1
    output << "@SP" << '\n';
    output << "A=M-1" << '\n';
    // not M
    output << "M=!M" << '\n';
  } else if (command == "neg") {
    // A = M[SP]-1
    output << "@SP" << '\n';
    output << "A=M-1" << '\n';
    // neg
    output << "M=-M" << '\n';
  } else if (command == "eq") {
    // M[SP] -- ;
    output << "@SP" << '\n';
    output << "AM=M-1" << '\n';
    // y = M[M[SP]]
    output << "D=M" << '\n';
    // M[M[SP]-1] = M[M[SP]-1] == y ? true : false
    output << "A=A-1" << '\n';
    output << "D=M-D" << '\n';
    output << "@True" << label_number << '\n';
    output << "D;JEQ" << '\n';
    output << "@SP" << '\n';
    output << "A=M-1" << '\n';
    output << "M=0" << '\n';
    output << "@Last" << label_number << '\n';
    output << "0;JMP" << '\n';
    output << "(True" << label_number << ')' << '\n';
    output << "@SP" << '\n';
    output << "A=M-1" << '\n';
    output << "M=-1" << '\n';
    output << "(Last" << label_number << ')' << '\n';
    label_number++;
  } else if (command == "gt") {
    // M[SP] -- ;
    output << "@SP" << '\n';
    output << "AM=M-1" << '\n';
    // y = M[M[SP]]
    output << "D=M" << '\n';
    // M[M[SP]-1] = M[M[SP]-1] == y ? true : false
    output << "A=A-1" << '\n';
    output << "D=M-D" << '\n';
    output << "@True" << label_number << '\n';
    output << "D;JGT" << '\n';
    output << "@SP" << '\n';
    output << "A=M-1" << '\n';
    output << "M=0" << '\n';
    output << "@Last" << label_number << '\n';
    output << "0;JMP" << '\n';
    output << "(True" << label_number << ')' << '\n';
    output << "@SP" << '\n';
    output << "A=M-1" << '\n';
    output << "M=-1" << '\n';
    output << "(Last" << label_number << ')' << '\n';
    label_number++;
  } else if (command == "lt") {
    // M[SP] -- ;
    output << "@SP" << '\n';
    output << "AM=M-1" << '\n';
    // y = M[M[SP]]
    output << "D=M" << '\n';
    // M[M[SP]-1] = M[M[SP]-1] == y ? true : false
    output << "A=A-1" << '\n';
    output << "D=M-D" << '\n';
    output << "@True" << label_number << '\n';
    output << "D;JLT" << '\n';
    output << "@SP" << '\n';
    output << "A=M-1" << '\n';
    output << "M=0" << '\n';
    output << "@Last" << label_number << '\n';
    output << "0;JMP" << '\n';
    output << "(True" << label_number << ')' << '\n';
    output << "@SP" << '\n';
    output << "A=M-1" << '\n';
    output << "M=-1" << '\n';
    output << "(Last" << label_number << ')' << '\n';
    label_number++;
  }
}

void Translator::WritePushPop(std::ofstream &output, std::string command,
                              std::string segment, int index) {
  if (command == "push") {
    if (segment == "constant") {
      output << "@" << index << '\n';
      output << "D=A" << '\n';
      output << "@SP" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
      output << "@SP" << '\n';
      output << "M=M+1" << '\n';
    } else if (segment == "argument") {
      output << "@ARG" << '\n';
      output << "D=M" << '\n';
      output << "@" << index << '\n';
      output << "A=D+A" << '\n';
      output << "D=M" << '\n';
      output << "@SP" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
      output << "@SP" << '\n';
      output << "M=M+1" << '\n';
    } else if (segment == "local") {
      output << "@LCL" << '\n';
      output << "D=M" << '\n';
      output << "@" << index << '\n';
      output << "A=D+A" << '\n';
      output << "D=M" << '\n';
      output << "@SP" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
      output << "@SP" << '\n';
      output << "M=M+1" << '\n';
    } else if (segment == "this") {
      output << "@THIS" << '\n';
      output << "D=M" << '\n';
      output << "@" << index << '\n';
      output << "A=D+A" << '\n';
      output << "D=M" << '\n';
      output << "@SP" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
      output << "@SP" << '\n';
      output << "M=M+1" << '\n';
    } else if (segment == "that") {
      output << "@THAT" << '\n';
      output << "D=M" << '\n';
      output << "@" << index << '\n';
      output << "A=D+A" << '\n';
      output << "D=M" << '\n';
      output << "@SP" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
      output << "@SP" << '\n';
      output << "M=M+1" << '\n';
    } else if (segment == "static") {
      output << "@" << filename << '.' << index << '\n';
      output << "D=M" << '\n';
      output << "@SP" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
      output << "@SP" << '\n';
      output << "M=M+1" << '\n';
    } else if (segment == "temp") {
      output << "@" << (5 + index) << '\n';
      output << "D=M" << '\n';
      output << "@SP" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
      output << "@SP" << '\n';
      output << "M=M+1" << '\n';
    } else if (segment == "pointer") {
      output << "@" << (3 + index) << '\n';
      output << "D=M" << '\n';
      output << "@SP" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
      output << "@SP" << '\n';
      output << "M=M+1" << '\n';
    }
  } else if (command == "pop") {
    if (segment == "local") {
      output << "@LCL" << '\n';
      output << "D=M" << '\n';
      output << "@" << index << '\n';
      output << "D=D+A" << '\n';
      output << "@R13" << '\n';
      output << "M=D" << '\n';

      output << "@SP" << '\n';
      output << "AM=M-1" << '\n';
      output << "D=M" << '\n';
      output << "@R13" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
    } else if (segment == "argument") {
      output << "@ARG" << '\n';
      output << "D=M" << '\n';
      output << "@" << index << '\n';
      output << "D=D+A" << '\n';
      output << "@R13" << '\n';
      output << "M=D" << '\n';

      output << "@SP" << '\n';
      output << "AM=M-1" << '\n';
      output << "D=M" << '\n';
      output << "@R13" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
    } else if (segment == "this") {
      output << "@THIS" << '\n';
      output << "D=M" << '\n';
      output << "@" << index << '\n';
      output << "D=D+A" << '\n';
      output << "@R13" << '\n';
      output << "M=D" << '\n';

      output << "@SP" << '\n';
      output << "AM=M-1" << '\n';
      output << "D=M" << '\n';
      output << "@R13" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
    } else if (segment == "that") {
      output << "@THAT" << '\n';
      output << "D=M" << '\n';
      output << "@" << index << '\n';
      output << "D=D+A" << '\n';
      output << "@R13" << '\n';
      output << "M=D" << '\n';

      output << "@SP" << '\n';
      output << "AM=M-1" << '\n';
      output << "D=M" << '\n';
      output << "@R13" << '\n';
      output << "A=M" << '\n';
      output << "M=D" << '\n';
    } else if (segment == "pointer") {
      output << "@SP" << '\n';
      output << "AM=M-1" << '\n';
      output << "D=M" << '\n';

      output << "@" << (3 + index) << '\n';
      output << "M=D" << '\n';
    } else if (segment == "temp") {
      output << "@SP" << '\n';
      output << "AM=M-1" << '\n';
      output << "D=M" << '\n';

      output << "@" << (5 + index) << '\n';
      output << "M=D" << '\n';
    } else if (segment == "static") {
      output << "@SP" << '\n';
      output << "AM=M-1" << '\n';
      output << "D=M" << '\n';

      output << "@" << filename << '.' << index << '\n';
      output << "M=D" << '\n';
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    return 0;
  }
  if (argc == 2) {
    std::string file = argv[1];
    if (file.substr(file.find_last_of(".") + 1) == "vm") {
      auto filename = std::filesystem::path(file).replace_extension();

      Translator tranlator(file, std::string(filename) + ".asm");
      tranlator.Translate();
    } else {
      for (const auto &entry : std::filesystem::directory_iterator(file)) {
        auto path = entry.path().string();
        if (path.substr(path.find_last_of(".") + 1) == "vm") {
          auto filename = std::filesystem::path(path).replace_extension();

          Translator tranlator(path, std::string(filename) + ".asm");
          tranlator.Translate();
        }
      }
    }
  }
}