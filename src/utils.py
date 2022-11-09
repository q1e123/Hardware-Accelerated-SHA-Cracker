def get_lines_from_file(file_path, encoding='ansi') -> list:
    line_list = None
    with open(file_path, encoding=encoding) as file:
        line_list = [line.rstrip() for line in file]
    return line_list