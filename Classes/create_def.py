import random
import string
import os
import re


if __name__ == '__main__':
    print('-' * 60)
    print('          C++ blur Author csc\n')
    print('                  tips cpp py same path\n')
    print('-' * 60)
    
    file_dir = os.getcwd()
    with open('def.h','w') as f:
        file_dir = os.getcwd()
        ran_str = ''.join(random.sample(string.ascii_letters, 5))
        for root, dirs, files in os.walk(file_dir):
            for file in files:
                if os.path.splitext(file)[1] == '.h':
                    with open(file,'r') as m:
                        str_lines = m.readlines()
                        for i in str_lines:
                            matchobj = re.match(r'class (.*) :', i)
                            if matchobj:
                                f.write('#define ' + matchobj.group(1) + ' ' + ran_str + matchobj.group(1) + '\n')
    f.close()
    