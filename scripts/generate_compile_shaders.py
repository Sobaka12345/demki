import os
import argparse

def dir_path(path):
    if os.path.isdir(path):
        return path
    else:
        raise argparse.ArgumentTypeError(f"readable_dir: {path} is not a valid path")

def shader_path(path):
    if os.path.isfile(path):
        return path
    else:
        raise argparse.ArgumentTypeError(f"shader_file: {path} is not a valid shader_file")


parser = argparse.ArgumentParser(
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument("-df", "--dest-folder", type=dir_path, help="Compile shaders script folder", required=True)
parser.add_argument("-sf", "--shader-file", type=shader_path, nargs="+", action="append", help="Shader source file path", required=True)

args = parser.parse_args()

script = ""

shaders_folder = os.path.join(args.dest_folder, "shaders")

try:
    os.mkdir(shaders_folder)
except OSError as exc:
    pass

for shader in args.shader_file:
    _, file = os.path.split(shader[0])
    compiled_path = os.path.join(shaders_folder, file + '.spv')
    script += "glslc " + shader[0] + " -o " + compiled_path + "\n"

with open(
    os.path.join(args.dest_folder, "compile_shaders." + "bat" if os.name == 'nt' else "sh"),
        'w', encoding='utf-8') as output:
    output.write(script)
