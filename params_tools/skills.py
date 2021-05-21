# coding: utf-8
"""
@author: Mrli
@date: 2020-11-12
将skills文件夹下的所有.skl文件生成skl参数文件 -> skl.cc + skl.h
"""
import glob
import os

skillsOutPath = "./skillsOutPath"


def writeToSKL():
    skills_path = 'skills'
    skl_list = glob.glob(os.path.join(skills_path, "*.skl"))
    result = []

    headFile = "skl.h"
    ccFile = "skl.cc"
    ccf = open(os.path.join(skillsOutPath, ccFile), "a+")  # ccFile file
    hf = open(os.path.join(skillsOutPath, headFile), "a+");
    hf.write("#include <iostream>\n#include <string>\n")

    print(skl_list)

    for i, sklf in enumerate(skl_list):
        # 将.skl文件内容全部加载到result数组中
        with open(sklf, "r") as f:  # input file
            # zhushi = False
            for each in f:
                content = each.strip()
                if content:
                    result.append(content)

        def writeToCC():
            # 生成cc文件
            ccf.write("// ------------Start---------------\n")
            ccf.write("void getSkillsFromLocal" + str(i + 1) + "(std::string& skillPart){\n")

            for s in result:
                line = 'skillPart += "{}\\n";'.format(s)
                ccf.write(line)
                ccf.write('\n')
            ccf.write("}\n")

            ccf.write("// ------------End---------------\n\n")

        def writeToH():
            # 生成.h文件
            hf.write("void getSkillsFromLocal" + str(i + 1) + "(std::string& skillPart);\n")

        writeToCC()
        writeToH()
    ccf.close()
    hf.close()

    return len(skl_list)


def getMultiBody(num):
    """
    需要写在naobehavior.cc中的内容
    """
    sss = """
void NaoBehavior::readSkillsLocal{}(){
    long buffsize = 65536 * 2;

    string skillPart("");
    skillPart.reserve( buffsize );

    getSkillsFromLocal{}(skillPart);

    char buff[ buffsize ];
    
    long long  numRead=skillPart.size();
    printf("%d", numRead);
    strcpy(buff,skillPart.c_str());

    string skillDescription("");
    skillDescription.reserve( buffsize );

    for( int i = 0; i < numRead; ++i ) {
        char c = buff[i];
        if( c == '$' ) {
            // parameter - replace it

            string param("");
            i += 1;
            while( i < numRead && ( isalnum( buff[i] ) || buff[i] == '_' ) ) {
                param += buff[i];
                ++i;
            }

            map<string, string>::const_iterator it = namedParams.find( param );
            skillDescription += it->second;


            if( i < numRead )
        {
                skillDescription += buff[i];
        }

        } else {
            // not a param, just concatenate c
            skillDescription += c;
        }
    }

    // Parse

    SkillParser parser( skills, bodyModel );
    parse_info<iterator_t> info = parse( skillDescription.c_str(),
                                         parser,
                                         ( space_p | comment_p("#") )
                                       );
    }
    """
    for i in range(num):  # 根据skl文件个数来确定
        s = sss.replace(r"{}", str(i + 1))
        # print(s)
        with open(os.path.join(skillsOutPath, "output2naobehavior.cc"), "a+") as f:
            f.write(s)
            f.write("\n")


def getMultiHead(num):
    """
    需要写在naobehavior.h头文件的函数头什么
    """
    for i in range(num):  # 根据skl文件个数来确定
        s0 = """void NaoBehavior::readSkillsLocal{}();"""
        s = s0.replace(r"{}", str(i + 1))
        # print(s)
        with open(os.path.join(skillsOutPath, "output2naobehavior.h"), "a+") as f:
            f.write(s)
            f.write("\n")


def getMultiHeadCall(num):
    for i in range(num):  # 根据skl文件个数来确定
        s0 = """void readSkillsLocal{}();"""
        s = s0.replace(r"{}", str(i + 1))
        # print(s)
        with open(os.path.join(skillsOutPath, "output2naobehaviorCall.cc"), "a+") as f:
            f.write(s)
            f.write("\n")


def write2naobehavior(num):
    getMultiBody(num)
    getMultiHead(num)
    getMultiHeadCall(num)


if __name__ == "__main__":
    if not os.path.exists(skillsOutPath):
        os.mkdir(skillsOutPath)
    num = writeToSKL()
    write2naobehavior(num)
