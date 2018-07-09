#!/usr/bin/python
import os
import sys
import platform
import shutil
import difflib
from os import listdir
from os.path import isfile, join

d = difflib.Differ()

cur_path = os.getcwd()


exe = os.path.join(cur_path, "bin", "mips")
test_path = os.path.join(cur_path, "testsuit-1")
cases = [ f[:-2] for f in listdir(test_path) if f[-2:] == ".s"]


def testCase(name):
    argList = []
    argList.append(exe)
    argList.append(os.path.join(test_path, name + '.s'))
    if os.path.isfile(os.path.join(test_path, name + '.in')):
        argList.append(' < ' + os.path.join(test_path, name + '.in'))
    argList.append(' > ' + name + '.out')

    # run
    cmd = " ".join(argList)
    #print(cmd)
    os.system(cmd)

    # compare

    with open(name + '.out','r') as file1:
        content1 = file1.read().splitlines()
    with open(os.path.join(test_path, name + '.ans'),'r') as file2:
        content2 = file2.read().splitlines()
    if content1 == content2:
        return True
    else:
        print("Wrong in " + name)
        print('\n'.join(d.compare(content1,content2)))
        return False


def main():
    for f in os.listdir("."):
        if (f[-4:] == ".out"):
            os.remove(f)

    if len(sys.argv) == 3:
        start = eval(sys.argv[1])
        end   = eval(sys.argv[2]) + 1
    else:
        start = 0
        end   = len(cases)

    end = min(len(cases), end)
    for i in range(1, 60):
        print ("TEST  %-40s" % cases[i])
        sys.stdout.flush()
        if not testCase(cases[i]):
            break
        os.remove(cases[i] + '.out')
        print ("passed")
    #print(cases)

if __name__ == '__main__':
    main()
