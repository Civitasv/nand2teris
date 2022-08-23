#include "VMTranslator.h"

#include <climits>
#include <iostream>
#include <iterator>
#include <sstream>

#include "utils.h"

void Translator::Translate() {
  std::ofstream output(output_file);
  if (file_or_directory.substr(file_or_directory.find_last_of(".") + 1) ==
      "vm") {
    filename =
        std::filesystem::path(file_or_directory).replace_extension().string();
    filename = filename.substr(filename.find_last_of('/') + 1);

    TranslatePerFile(output, file_or_directory);
  } else {
    WriteInit(output);
    for (const auto &entry :
         std::filesystem::directory_iterator(file_or_directory)) {
      auto path = entry.path().string();
      if (path.substr(path.find_last_of(".") + 1) == "vm") {
        filename = std::filesystem::path(path).replace_extension().string();
        filename = filename.substr(filename.find_last_of('/') + 1);

        TranslatePerFile(output, path);
      }
    }
  }
  output.close();
}

void Translator::TranslatePerFile(std::ofstream &output, std::string vm_file) {
  std::ifstream in_file(vm_file);

  if (in_file.is_open()) {

    std::string line;
    // Scan
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

      // parse
      if (current_command_type == C_ARITHMETIC) {
        WriteArithmetic(output, command);
      } else if (current_command_type == C_PUSH ||
                 current_command_type == C_POP) {
        auto arg1 = commands[1];
        auto arg2 = std::stoi(commands[2]);
        WritePushPop(output, command, arg1, arg2);
      } else if (current_command_type == C_LABEL) {
        WriteLabel(output, commands[1]);
      } else if (current_command_type == C_GOTO) {
        WriteGoto(output, commands[1]);
      } else if (current_command_type == C_IF) {
        WriteIf(output, commands[1]);
      } else if (current_command_type == C_FUNCTION) {
        WriteFunction(output, commands[1], std::stoi(commands[2]));
      } else if (current_command_type == C_RETURN) {
        WriteReturn(output);
      } else if (current_command_type == C_CALL) {
        WriteCall(output, commands[1], std::stoi(commands[2]));
      }
    }
    // finish
    in_file.close();
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
  } else if (type == "label") {
    return C_LABEL;
  } else if (type == "goto") {
    return C_GOTO;
  } else if (type == "if-goto") {
    return C_IF;
  } else if (type == "function") {
    return C_FUNCTION;
  } else if (type == "return") {
    return C_RETURN;
  } else if (type == "call") {
    return C_CALL;
  }
  return C_EMPTY;
}

void Translator::WriteArithmetic(std::ofstream &output, std::string command) {
  output << "// " << command << '\n';
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

  output << "// " << command << ' ' << segment << ' ' << index << '\n';
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

void Translator::WriteInit(std::ofstream &output) {
  output << "// Init" << '\n';
  output << "@256" << '\n';
  output << "D=A" << '\n';
  output << "@SP" << '\n';
  output << "M=D" << '\n';
  WriteCall(output, "Sys.init", 0);
}

void Translator::WriteLabel(std::ofstream &output, std::string label) {
  output << "// label: " << label << '\n';
  output << '(' << function_name << '$' << label << ')' << '\n';
}

void Translator::WriteGoto(std::ofstream &output, std::string label) {
  output << "// goto: " << label << '\n';
  output << '@' << function_name << '$' << label << '\n';
  output << "0;JMP" << '\n';
}

void Translator::WriteIf(std::ofstream &output, std::string label) {
  output << "// if-goto: " << label << '\n';
  output << "@SP" << '\n';
  output << "AM=M-1" << '\n';
  output << "D=M" << '\n';
  output << '@' << function_name << '$' << label << '\n';
  output << "D;JNE" << '\n';
}

void Translator::WriteCall(std::ofstream &output, std::string function_name,
                           int num_args) {
  output << "// call: " << function_name << '\n';
  output << "// Saves return address: " << '\n';
  output << "@" << function_name << "$ret."
         << function_invoke_times[function_name] << '\n';
  output << "D=A" << '\n';
  output << "@SP" << '\n';
  output << "A=M" << '\n';
  output << "M=D" << '\n';
  output << "@SP" << '\n';
  output << "M=M+1" << '\n';

  WritePushSegments(output, "LCL");  // LCL
  WritePushSegments(output, "ARG");  // ARG
  WritePushSegments(output, "THIS"); // THIS
  WritePushSegments(output, "THAT"); // THAT

  // Set Arg position
  output << "@SP" << '\n';
  output << "D=M" << '\n';

  output << "@5" << '\n';
  output << "D=D-A" << '\n';

  output << "@" << num_args << '\n';
  output << "D=D-A" << '\n';

  output << "@ARG" << '\n';
  output << "M=D" << '\n';

  // Set LCL position
  output << "// SET LCL POSITION: " << '\n';
  output << "@SP" << '\n';
  output << "D=M" << '\n';

  output << "@LCL" << '\n';
  output << "M=D" << '\n';

  // goto function
  output << '@' << function_name << '\n';
  output << "0;JMP" << '\n';

  // make a label about return address
  output << '(' << function_name << "$ret."
         << function_invoke_times[function_name] << ')' << '\n';

  std::cout << '(' << function_name << "$ret."
            << function_invoke_times[function_name] << ')' << '\n';
  function_invoke_times[function_name]++;
  std::cout << "第" << function_invoke_times[function_name] << "次调用！"
            << function_name << '\n';
}

void Translator::WritePushSegments(std::ofstream &output, int index) {
  output << "@" << index << '\n';
  output << "D=M" << '\n';
  output << "@SP" << '\n';
  output << "A=M" << '\n';
  output << "M=D" << '\n';
  output << "@SP" << '\n';
  output << "M=M+1" << '\n';
}

void Translator::WritePushSegments(std::ofstream &output, std::string label) {
  output << "@" << label << '\n';
  output << "D=M" << '\n';
  output << "@SP" << '\n';
  output << "A=M" << '\n';
  output << "M=D" << '\n';
  output << "@SP" << '\n';
  output << "M=M+1" << '\n';
}

void Translator::WriteFunction(std::ofstream &output, std::string function_name,
                               int num_locals) {
  std::cout << "FUNCTION NAME: " << function_name << '\n';
  output << "// function: " << function_name << '\n';

  output << '(' << function_name << ')' << '\n';
  for (int i = 0; i < num_locals; i++) {
    output << "@0" << '\n';
    output << "D=A" << '\n';
    output << "@SP" << '\n';
    output << "A=M" << '\n';
    output << "M=D" << '\n';
    output << "@SP" << '\n';
    output << "M=M+1" << '\n';
  }
  this->function_name = function_name;
  function_invoke_times.insert(std::pair<std::string, int>(function_name, 0));
}

void Translator::WriteReturn(std::ofstream &output) {
  output << "// return: " << function_name << '\n';

  output << "// endFrame=LCL: " << '\n';
  // endFrame = LCL
  output << "@LCL" << '\n';
  output << "D=M" << '\n';
  output << "@R13" << '\n';
  output << "M=D" << '\n';

  output << "// retAddr=*(endFrame-5): " << '\n';
  // retAddr = *(endFrame-5)
  output << "@5" << '\n';
  output << "A=D-A" << '\n';
  output << "D=M" << '\n';
  output << "@R14" << '\n';
  output << "M=D" << '\n';

  output << "// *ARG=pop(): " << '\n';
  // *ARG = pop()
  output << "@SP" << '\n';
  output << "AM=M-1" << '\n';
  output << "D=M" << '\n';
  output << "@ARG" << '\n';
  output << "A=M" << '\n';
  output << "M=D" << '\n';

  output << "// SP+ARG+1: " << '\n';
  // SP = ARG+1
  output << "@ARG" << '\n';
  output << "D=M" << '\n';
  output << "@1" << '\n';
  output << "D=D+A" << '\n';
  output << "@SP" << '\n';
  output << "M=D" << '\n';

  output << "// THAT = *(endFrame - 1): " << '\n';
  // THAT = *(endFrame - 1)
  output << "@R13" << '\n';
  output << "D=M" << '\n';
  output << "@1" << '\n';
  output << "A=D-A" << '\n';
  output << "D=M" << '\n';
  output << "@THAT" << '\n';
  output << "M=D" << '\n';

  // THIS = *(endFrame - 2)
  output << "@R13" << '\n';
  output << "D=M" << '\n';
  output << "@2" << '\n';
  output << "A=D-A" << '\n';
  output << "D=M" << '\n';
  output << "@THIS" << '\n';
  output << "M=D" << '\n';

  // ARG = *(endFrame - 3)
  output << "@R13" << '\n';
  output << "D=M" << '\n';
  output << "@3" << '\n';
  output << "A=D-A" << '\n';
  output << "D=M" << '\n';
  output << "@ARG" << '\n';
  output << "M=D" << '\n';

  // LCL = *(endFrame - 4)
  output << "@R13" << '\n';
  output << "D=M" << '\n';
  output << "@4" << '\n';
  output << "A=D-A" << '\n';
  output << "D=M" << '\n';
  output << "@LCL" << '\n';
  output << "M=D" << '\n';

  // goto retAddr
  output << "@R14" << '\n';
  output << "A=M" << '\n';
  output << "0;JMP" << '\n';
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    return 0;
  }
  if (argc == 2) {
    std::string file = argv[1];

    if (file.substr(file.find_last_of(".") + 1) == "vm") {
      Translator tranlator(file,
                           file.substr(0, file.find_last_of(".")) + ".asm");
      tranlator.Translate();
    } else {
      auto filename = std::filesystem::path(file).filename().string();
      Translator tranlator(file, file + "/" + filename + ".asm");
      tranlator.Translate();
    }
  }
}