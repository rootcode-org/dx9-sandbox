# Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted.

import os
import sys
import shutil
import tempfile
import uuid
import subprocess
import xml.etree.ElementTree

# Compiler paths; Update these to match currently installed Visual Studio and Windows SDK versions
# Used only for the 'headers' target
MSVC_PATH = os.path.join(os.environ['ProgramFiles'], 'Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.31.31103/')
COMPILER_PATH = os.path.join(MSVC_PATH, 'bin/Hostx64/x64/cl.exe')
SDK_PATH = os.path.join(os.environ['ProgramFiles(x86)'], 'Windows Kits/10/Include/10.0.19041.0')


HELP = '''
Build application artifacts

build.py <command> <parameters...>

  clean     delete all intermediate build files
  debug     build debug executable
  release   build release executable
  msvc      generate .vcxproj project file
  headers   compile header files individually (validates dependencies)
'''


def clean_all():
    print ('Deleting intermediate files')
    script_path = os.path.abspath(os.path.dirname(sys.argv[0]))
    for item in ['.cache', '.vs', 'application.vcxproj.user']:
        item_path = os.path.join(script_path, os.pardir, item)
        if os.path.exists(item_path):
            if os.path.isdir(item_path):
                shutil.rmtree(item_path, True)
            else:
                os.remove(item_path)


def build_executable(config):
    print ('Building Windows ' + config + ' executable')
    script_path = os.path.abspath(os.path.dirname(sys.argv[0]))
    program_files = os.environ['ProgramFiles']
    ms_build_exe = os.path.join(program_files, 'Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe')
    vc_project = os.path.join(script_path, os.pardir, 'application.vcxproj')
    args = [ms_build_exe, vc_project, '/nologo', '/verbosity:minimal',
            '/p:Configuration=' + config,
            '/p:Platform=x64']
    subprocess.call(args)


def compile_file(source_file, object_file, compiler_options):
    script_path = os.path.abspath(os.path.dirname(sys.argv[0]))
    source_path = os.path.join(script_path, os.pardir, 'code')
    args = [COMPILER_PATH,
            '/nologo', '/MP', '/analyze-', '/c', '/TP', '/EHsc', '/Zc:wchar_t', '/Gm-', '/GF', '/MDd',
            '/D', 'WIN32_LEAN_AND_MEAN',
            '/D', '_WIN32_WINNT=0x0A00',
            '/I', source_path,
            '/I', os.path.join(MSVC_PATH, 'include'),
            '/I', os.path.join(os.environ['ProgramFiles(x86)'], SDK_PATH, 'um'),
            '/I', os.path.join(os.environ['ProgramFiles(x86)'], SDK_PATH, 'shared'),
            '/I', os.path.join(os.environ['ProgramFiles(x86)'], SDK_PATH, 'ucrt'),
            '/Fo' + object_file]
    args.extend(compiler_options)
    args.append(source_file)
    subprocess.call(args)


def compile_all(extension, compiler_options, output_path):
    script_path = os.path.abspath(os.path.dirname(sys.argv[0]))
    source_path = os.path.join(script_path, os.pardir, 'code')
    if not os.path.exists(output_path):
        os.makedirs(output_path)
    for dir_path, dir_list, file_list in os.walk(source_path):
        dir_path = dir_path.replace('\\', '/')
        if (dir_path.find('/external') == -1) and (dir_path.find('/generated') == -1):
            for file_name in file_list:
                path = os.path.join(dir_path, file_name)
                name = os.path.basename(path)
                base, ext = os.path.splitext(name)
                if ext == extension:
                    source_file = os.path.join(dir_path, file_name)
                    object_file = os.path.join(output_path, base + '.obj')
                    compile_file(source_file, object_file, compiler_options)


def build_headers():
    print ('Compiling header files')
    temp_path = os.path.join(tempfile.gettempdir(), 'compiler_output')
    if not os.path.exists(temp_path):
        os.makedirs(temp_path)
    compiler_options = ['/D', '_WINDOWS', '/D', '_DEBUG', '/W4']
    compile_all('.h', compiler_options, temp_path)
    shutil.rmtree(temp_path)


def generate_msvc_project():
    print ('Generating project file')

    # Register the namespace used by the VS2013 project file
    namespace = 'http://schemas.microsoft.com/developer/msbuild/2003'
    xml.etree.ElementTree.register_namespace('', namespace)

    # Load the project file
    script_path = os.path.abspath(os.path.dirname(sys.argv[0]))
    project_file = os.path.join(script_path, os.pardir, 'application.vcxproj')
    project_tree = xml.etree.ElementTree.parse(project_file)

    # Delete the existing ItemGroups that contain source files
    root = project_tree.getroot()
    delete_elements = []
    for element in root:
        if element.tag.find('ItemGroup') != -1:
            for item in element:
                if item.tag.find('ClCompile') != -1 or item.tag.find('ClInclude') != -1:
                    delete_elements.append(element)
                    break
    for element in delete_elements:
        root.remove(element)

    # Define extensions we will include in the project file
    compile_with_pch = ['.cpp']
    compile_no_pch = ['.c']
    compile_protobuf = ['.cc']
    include_only = ['.h', '.hpp']
    all_compile_extensions = compile_with_pch + compile_no_pch + compile_protobuf
    all_extensions = all_compile_extensions + include_only

    # Create a new ItemGroup and add all source files to it
    item_group = xml.etree.ElementTree.SubElement(root, 'ItemGroup')
    item_group.text = '\n    '
    item_group.tail = '\n  '
    source_path = os.path.join(script_path, os.pardir, 'code')
    for dir_path, dir_list, file_list in os.walk(source_path):
        for file_name in file_list:
            extension = os.path.splitext(file_name)[1]
            source_file_path = 'code' + dir_path[len(source_path):] + '\\' + file_name
            if extension in all_extensions:
                item_type = 'ClCompile' if extension in all_compile_extensions else 'ClInclude'
                source_element = xml.etree.ElementTree.SubElement(item_group, item_type, attrib={'Include': source_file_path})
                source_element.tail = '\n    '
                if file_name == 'precompiled.cpp':
                    # The precompiled.cpp file needs to set its precompile mode to 'create'
                    source_element.text = '\n      '
                    pch_element = xml.etree.ElementTree.SubElement(source_element, 'PrecompiledHeader')
                    pch_element.text = 'Create'
                    pch_element.tail = '\n    '
                elif extension in compile_no_pch:
                    # Disable pre-compiled header for specified files
                    pch_element = xml.etree.ElementTree.SubElement(source_element, 'PrecompiledHeader')
                    pch_element.text = 'NotUsing'
                    pch_element.tail = '\n    '
                elif extension in compile_protobuf:
                    # Protobuf source files need PCH disabled and some compiler warnings disabled
                    pch_element = xml.etree.ElementTree.SubElement(source_element, 'PrecompiledHeader')
                    pch_element.text = 'NotUsing'
                    pch_element.tail = '\n    '
                    pch_element = xml.etree.ElementTree.SubElement(source_element, 'AdditionalOptions', attrib={'Condition': "'$(Configuration)|$(Platform)'=='Release|x64'"})
                    pch_element.text = '/wd4125 /wd4127 /wd4244 /wd4267 %(AdditionalOptions)'
                    pch_element.tail = '\n    '
                    pch_element = xml.etree.ElementTree.SubElement(source_element, 'AdditionalOptions', attrib={'Condition': "'$(Configuration)|$(Platform)'=='Debug|x64'"})
                    pch_element.text = '/wd4125 /wd4127 /wd4244 /wd4267 %(AdditionalOptions)'
                    pch_element.tail = '\n    '

    # Write the project file
    # We want this to be identical to the formatting that MSVC generates, so some fixups are required
    project_xml = b'<?xml version="1.0" encoding="utf-8"?>\n'
    project_xml += xml.etree.ElementTree.tostring(root)
    project_xml = project_xml.replace(b'    </ItemGroup>', b'  </ItemGroup>')
    project_xml = project_xml.replace(b'  </Project>', b'</Project>')
    project_xml = project_xml.decode()
    with open(project_file, 'w') as f:
        f.write(project_xml)

    # Create a new XML tree for the filters
    root = xml.etree.ElementTree.Element('Project', attrib={'ToolsVersion': '4.0', 'xmlns': namespace})
    root.text = '\n  '

    # Create a new ItemGroup and add each source folder (except the top-level source folder) as a filter
    item_group = xml.etree.ElementTree.SubElement(root, 'ItemGroup')
    item_group.text = '\n    '
    item_group.tail = '\n  '
    for dir_path, dir_list, file_list in os.walk(source_path):
        for d in dir_list:
            dir_name = dir_path[len(source_path)+1:]
            dir_name = dir_name + '\\' + d if len(dir_name) else d
            filter_element = xml.etree.ElementTree.SubElement(item_group, 'Filter', attrib={'Include': dir_name})
            filter_element.text = '\n      '
            filter_element.tail = '\n    '
            unique_id_element = xml.etree.ElementTree.SubElement(filter_element, 'UniqueIdentifier')
            identifier = uuid.uuid3(uuid.NAMESPACE_URL, dir_name)
            unique_id_element.text = '{' + str(identifier) + '}'
            unique_id_element.tail = '\n    '

    # Create a new ItemGroup and add all source files; each source file will reference the filter it belongs to
    item_group = xml.etree.ElementTree.SubElement(root, 'ItemGroup')
    item_group.text = '\n    '
    item_group.tail = '\n  '
    for dir_path, dir_list, file_list in os.walk(source_path):
        for file_name in file_list:
            extension = os.path.splitext(file_name)[1]
            source_file_path = 'code' + dir_path[len(source_path):] + '\\' + file_name
            dir_name = dir_path[len(source_path)+1:]
            if extension in all_extensions:
                item_type = 'ClCompile' if extension in all_compile_extensions else 'ClInclude'
                source_element = xml.etree.ElementTree.SubElement(item_group, item_type, attrib={'Include': source_file_path})
                source_element.tail = '\n    '
                if dir_name:
                    source_element.text = '\n      '
                    filter_element = xml.etree.ElementTree.SubElement(source_element, 'Filter')
                    filter_element.text = dir_name
                    filter_element.tail = '\n    '

    # Write the filters file
    # We want this to be identical to the formatting that MSVC generates, so some fixups are required
    filters_file = os.path.join(script_path, os.pardir, 'application.vcxproj.filters')
    filters_xml = b'<?xml version="1.0" encoding="utf-8"?>\n'
    filters_xml += xml.etree.ElementTree.tostring(root)
    filters_xml = filters_xml.replace(b'    </ItemGroup>', b'  </ItemGroup>')
    filters_xml = filters_xml.replace(b'  </Project>', b'</Project>')
    filters_xml = filters_xml.decode()
    with open(filters_file, 'w') as f:
        f.write(filters_xml)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        sys.exit(HELP)

    for target in sys.argv[1:]:
        if target == 'clean':
            clean_all()
        elif target == 'debug':
            build_executable('debug')
        elif target == 'release':
            build_executable('release')
        elif target == 'msvc':
            generate_msvc_project()
        elif target == 'headers':
            build_headers()
        else:
            sys.exit(target + ' is an unknown target')
