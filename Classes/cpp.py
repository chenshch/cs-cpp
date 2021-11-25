import random
import string
import os
import shutil

out_dir = 'blur'

keyword_list = ['cout ', '+=', '-=', ' int ', 'goto', 'asm', ' do ', 'if ', '[', ']',
                'return', 'typedef', 'auto ', 'double', 'inline', '{', '}',
                'short', 'typeid', 'bool', 'int ', '(', ')',
                'signed', 'typename', ' break', 'else', '&gt;=', '&lt;=',
                'sizeof', 'union', ' case ', 'enum', 'mutable', ';',
                ' static ', 'unsigned', 'catch', 'explicit', 'try',
                'namespace', 'using', ' char ', 'main', ' const ',
                'export', ' new ', 'struct', 'class', 'switch',
                'false', 'private', 'long', '::', 'void', 'endl',
                'float', 'protected', 'this', 'continue', '++', '--',
                ' for ', 'public', 'throw ', 'while', 'default', 'friend',
                'true', '&lt;&lt;', 'cin ', ' printf', '==', '&gt;&gt;', '!=', ]
                
              
'''keyword_list = ['cout', '+=', '-=','||','+=','-=','::','auto ',' int ' 
                'return', 'typedef', 'double', 'inline',
                'signed', 'typename', 'break',
                'sizeof', 'union', 'case', 'enum', 'mutable',
                'unsigned', 'catch', 'explicit', 'try',
                'namespace', 'using','main', 'const',
                'export', ' new ', 'struct', 'class', 'switch', 'private', 'long', 'void', 'endl',
                'float', 'protected', 'this', 'continue','format',
                'public', 'while', 'default', 'friend']'''
                
def random_char():
    r = chr(random.randint(97, 122))
    char, char_r, list_chr = [], [], []
    for i in range(len(keyword_list)):
        char.append(r + str(i))
        char_r.append(keyword_list[i])
    random.shuffle(char)
    random.shuffle(char_r)
    for i in range(len(char)):
        list_chr.append([char[i], char_r[i]])
    return list_chr


def generate_define(list_chr):
    define = []
    for i in range(len(list_chr)):
        define.append('#define ' + list_chr[i][0] + ' ' + list_chr[i][1])
    return define


def replace(list_char, str, confusion):
    for i in list_char:
        if i[1] in str:
            str = str.replace(i[1], confusion + i[0] + confusion)
    return str


def open_file(cpp,list_char):
    confusion = ' /*' + ''.join(random.sample(string.hexdigits, 6)) + '*/ '
    rname = cpp.replace('.cpp','') + ''.join(random.sample(string.ascii_letters, 4)) + '.cpp'
    filenmae = out_dir + '/' + rname
    with open(cpp, 'r') as f:
        with open(filenmae, 'w') as m:
            define = generate_define(list_char)
            
            str_lines = f.readlines()
            for i in str_lines:
                if '#include' in i:
                    m.write(i)
                
            for i in define:
                m.write(i + '\n')
                
            for i in str_lines:
                if not '#include' in i:
                    i = i.strip()
                    i_replace = replace(list_char, i, confusion)
                    m.write(i_replace + '\n')
            print('blur finish' + cpp + '\n')


if __name__ == '__main__':
    print('-' * 60)
    print('          C++ blur Author csc\n')
    print('                  tips cpp py same path\n')
    print('-' * 60)
    
    file_dir = os.getcwd()
    if os.path.exists(file_dir+'/'+out_dir):
        shutil.rmtree(file_dir+'/'+out_dir)
    os.mkdir(file_dir+'/'+out_dir)
    for root, dirs, files in os.walk(file_dir):  
        for file in files:
            if os.path.splitext(file)[1] == '.cpp':  
                open_file(file,random_char())
            elif os.path.splitext(file)[1] == '.h':
                shutil.copy(file,out_dir)