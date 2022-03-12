import re
from pathlib import Path


def translate_file(file):
    def decimal_to_binary(val):
        def helper(value):
            if value > 1:
                return helper(value // 2) + str(value % 2)
            return str(value % 2)

        return helper(val)

    def init_symbol_table():
        # 1. R0..R15
        for i in range(16):
            symbol_table["R" + str(i)] = decimal_to_binary(i).zfill(15)

        # 2. others
        symbol_table["SCREEN"] = decimal_to_binary(16384)
        symbol_table["KBD"] = decimal_to_binary(24576).zfill(15)
        symbol_table["SP"] = decimal_to_binary(0).zfill(15)
        symbol_table["LCL"] = decimal_to_binary(1).zfill(15)
        symbol_table["ARG"] = decimal_to_binary(2).zfill(15)
        symbol_table["THIS"] = decimal_to_binary(3).zfill(15)
        symbol_table["THAT"] = decimal_to_binary(4).zfill(15)

    def parse_label_symbols(lines):
        label_regex = r"\(([a-zA-Z0-9_.$]+)\)"
        a_instruction_regex = r"@([a-zA-Z0-9_]+)"
        c_instruction_regex = r"([AMD]+)?=?([AMD\-0-9+]+);?([A-Z]+)?"
        comment_regex = r"//.*"
        count = 0
        for line in lines:
            line = re.sub(r"\s*", "", line)
            line = re.sub(comment_regex, "", line)

            a_match = re.match(a_instruction_regex, line)
            b_match = re.match(c_instruction_regex, line)
            label_match = re.match(label_regex, line)
            if label_match:
                symbol_table[label_match.group(1)] = decimal_to_binary(count).zfill(15)
            if a_match or b_match:
                count += 1

    def parse_variable_symbols(lines):
        a_instruction_regex = r"@([a-zA-Z0-9_.$]+)"
        c_instruction_regex = r"([AMD]+)?=?([AMD\-0-9+!&|]+);?([A-Z]+)?"
        comment_regex = r"//.*"
        result = []

        a_start_memory = 16
        for line in lines:
            line = re.sub(r"\s*", "", line)
            line = re.sub(comment_regex, "", line)

            a_match = re.match(a_instruction_regex, line)
            b_match = re.match(c_instruction_regex, line)
            if a_match:
                symbol = a_match.group(1)
                if symbol.isdecimal():
                    result.append("0" + decimal_to_binary(int(symbol)).zfill(15))
                elif symbol in symbol_table:
                    result.append("0" + symbol_table[symbol])
                else:
                    symbol_table[symbol] = decimal_to_binary(a_start_memory).zfill(15)
                    a_start_memory += 1
                    result.append("0" + symbol_table[symbol])
            elif b_match:
                dest = b_match.group(1)
                comp = b_match.group(2)
                jmp = b_match.group(3)
                a = 0
                c1, c2, c3, c4, c5, c6 = 0, 0, 0, 0, 0, 0
                d1, d2, d3 = 0, 0, 0
                j1, j2, j3 = 0, 0, 0
                if dest == "M":
                    d3 = 1
                elif dest == "D":
                    d2 = 1
                elif dest == "MD":
                    d2, d3 = 1, 1
                elif dest == "A":
                    d1 = 1
                elif dest == "AM":
                    d1, d3 = 1, 1
                elif dest == "AD":
                    d1, d2 = 1, 1
                elif dest == "AMD":
                    d1, d2, d3 = 1, 1, 1

                if comp == "0":
                    c1, c3, c5 = 1, 1, 1
                elif comp == "1":
                    c1, c2, c3, c4, c5, c6 = 1, 1, 1, 1, 1, 1
                elif comp == "-1":
                    c1, c2, c3, c5 = 1, 1, 1, 1
                elif comp == "D":
                    c3, c4 = 1, 1
                elif comp == "A":
                    c1, c2 = 1, 1
                elif comp == "!D":
                    c3, c4, c6 = 1, 1, 1
                elif comp == "!A":
                    c1, c2, c6 = 1, 1, 1
                elif comp == "-D":
                    c3, c4, c5, c6 = 1, 1, 1, 1
                elif comp == "-A":
                    c1, c2, c5, c6 = 1, 1, 1, 1
                elif comp == "D+1":
                    c2, c3, c4, c5, c6 = 1, 1, 1, 1, 1
                elif comp == "A+1":
                    c1, c2, c4, c5, c6 = 1, 1, 1, 1, 1
                elif comp == "D-1":
                    c3, c4, c5 = 1, 1, 1
                elif comp == "A-1":
                    c1, c2, c5 = 1, 1, 1
                elif comp == "D+A":
                    c5 = 1
                elif comp == "D-A":
                    c2, c5, c6 = 1, 1, 1
                elif comp == "A-D":
                    c4, c5, c6 = 1, 1, 1
                elif comp == "D|A":
                    c2, c4, c6 = 1, 1, 1
                elif comp == "M":
                    a, c1, c2 = 1, 1, 1
                elif comp == "!M":
                    a, c1, c2, c6 = 1, 1, 1, 1
                elif comp == "-M":
                    a, c1, c2, c5, c6 = 1, 1, 1, 1, 1
                elif comp == "M+1":
                    a, c1, c2, c4, c5, c6 = 1, 1, 1, 1, 1, 1
                elif comp == "M-1":
                    a, c1, c2, c5 = 1, 1, 1, 1
                elif comp == "D+M":
                    a, c5 = 1, 1
                elif comp == "D-M":
                    a, c2, c5, c6 = 1, 1, 1, 1
                elif comp == "M-D":
                    a, c4, c5, c6 = 1, 1, 1, 1
                elif comp == "D&M":
                    a = 1
                elif comp == "D|M":
                    a, c2, c4, c6 = 1, 1, 1, 1

                if jmp == "JGT":
                    j3 = 1
                elif jmp == "JEQ":
                    j2 = 1
                elif jmp == "JGE":
                    j2, j3 = 1, 1
                elif jmp == "JLT":
                    j1 = 1
                elif jmp == "JNE":
                    j1, j3 = 1, 1
                elif jmp == "JLE":
                    j1, j2 = 1, 1
                elif jmp == "JMP":
                    j1, j2, j3 = 1, 1, 1

                result.append("111{}{}{}{}{}{}{}{}{}{}{}{}{}".format(a, c1, c2, c3, c4, c5, c6, d1, d2, d3, j1, j2, j3))
        return result

    symbol_table = {}
    # init symbol table: add all predefined symbols
    init_symbol_table()

    with open(file, "r") as hack:
        lines = hack.readlines()
        # first pass: add the label symbols
        parse_label_symbols(lines)
        # second pass: add the variable symbols, and translate all lines
        result = parse_variable_symbols(lines)
        p = Path(file)
        with open(p.with_suffix(".hack"), "w") as machine:
            for line in result:
                machine.write(line+"\n")


translate_file("/home/civitasv/文档/workflow/study/nand2teris/projects/06/add/Add.asm")
translate_file("/home/civitasv/文档/workflow/study/nand2teris/projects/06/max/Max.asm")
translate_file("/home/civitasv/文档/workflow/study/nand2teris/projects/06/max/MaxL.asm")
translate_file("/home/civitasv/文档/workflow/study/nand2teris/projects/06/pong/Pong.asm")
translate_file("/home/civitasv/文档/workflow/study/nand2teris/projects/06/pong/PongL.asm")
translate_file("/home/civitasv/文档/workflow/study/nand2teris/projects/06/rect/Rect.asm")
translate_file("/home/civitasv/文档/workflow/study/nand2teris/projects/06/rect/RectL.asm")
