%{
#include <fstream> // 包含文件操作库
#include <stdio.h> // 标准输入输出库
#include <stdlib.h> // 包含内存分配、类型转换、数学函数等库
#include <ctype.h> // 包含字符处理函数的库
#include "NFA.h" // 包含自定义的NFA头文件

#ifndef YYSTYPE
#define YYSTYPE NFA* // 定义 YYSTYPE 类型为 NFA*，如果未定义过
#endif
int yylex(); // 声明词法分析器函数
extern int yyparse(); // 声明语法分析器函数
FILE* yyin; // 输入文件指针
void yyerror(const char* s); // 错误处理函数
%}

// 定义不同符号的单词类别
%token ALPHA DIGIT SYMBOL LINEEND
%left '|' '-'

// 语法规则开始
%%

lines : lines unionexpr LINEEND {
        PrintNFA($2); // 打印生成的 NFA
        }
      | lines LINEEND // 匹配行末
      |
      ;

unionexpr : concatexpr '|' unionexpr { $$ = unionNFA($1, $3); } // 连接正则表达式
      | concatexpr
      ;

concatexpr : basicexpr concatexpr { $$ = concatenateNFA($1, $2); } // 连接操作
     | basicexpr
     ;

basicexpr : basicexpr '*' { $$ = closureNFA($1); } // 闭包操作
     | '(' unionexpr ')' { $$ = $2; } // 括号匹配
     | onechar { $$ = $1; }
     ;


onechar : DIGIT { $$ = $1; } // 匹配单个数字
        | ALPHA { $$ = $1; } // 匹配单个字母
        | SYMBOL { $$ = $1; } // 匹配单个符号
        ;

%%

// 程序段

int yylex() {
    int t;
    while(1) {
        t = getchar(); // 读取输入流字符
        if (t == ' ' || t == '\t') {
            // 不执行任何操作
        } else if (t == '\n' || t == '\r') {
            return LINEEND; // 返回行结束符
        } else if (isalpha(t)) {
            yylval = BasicNFA((char)t); // 将字符转换为基本 NFA
            return ALPHA; // 返回字母类型
        } else if (isdigit(t)) {
            yylval = BasicNFA((char)t); // 将字符转换为基本 NFA
            return DIGIT; // 返回数字类型
        } else if (t == '\\') {
            t = getchar();
            if (t == '0') {
                yylval = BasicNFA(0); // 处理转义字符为 0
                return ALPHA;
            } else {
                yylval = BasicNFA((char)t); // 将字符转换为基本 NFA
                return ALPHA; // 返回字母类型
            }
        } else if (t == '|' || t == '(' || t == ')' || t == '*') {
            return t; // 返回相应的符号类型
        } else if (isgraph(t)) {
            yylval = BasicNFA((char)t); // 将字符转换为基本 NFA
            return SYMBOL; // 返回符号类型
        } else {
            return t; // 返回字符类型
        }
    }
}

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s); // 输出解析错误信息
    exit(1); // 退出程序，返回错误状态码
}

int main(void) {
    yyin = stdin; // 设置输入文件为标准输入
    do {
        yyparse(); // 解析输入
    } while (!feof(yyin)); // 直到文件末尾
    return 0; // 返回正常
}



