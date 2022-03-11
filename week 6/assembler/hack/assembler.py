def translate_line(line):
    pass


def translate_file(file):
    with open(file, "r") as hack:
        return map(translate_line, hack.readlines())
