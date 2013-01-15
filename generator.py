#!/usr/bin/env python2

import sys, collections, pprint
import ply.lex as lex
import ply.yacc as yacc

# Lexer

tokens = ('IDENTIFIER', 'SEMICOLON')

t_SEMICOLON     = r':'
t_IDENTIFIER    = r'[a-zA-Z_]+'

t_ignore    = " \t"

def t_newline(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)

lex.lex()

# Parser

def p_rule(t):
    'rule : IDENTIFIER SEMICOLON list'
    decl_rule(t[1], t[3])

def p_list(t):
    'list : list IDENTIFIER'
    t[0] = t[1] + [t[2]]

def p_list_data(t):
    'list : IDENTIFIER'
    t[0] = [t[1]]

def p_error(t):
    print "Syntax error at '%s'" % t.value

yacc.yacc()

# Generator
rules = { }
tsyms = []
syms = []

def decl_rule(name, list):
    if name in rules:
        rules[name].append(list)
    else:
        rules[name] = [list]

def ll_error(msg):
    print "error: %s" % (msg)
    sys.exit(1)

def ll_first(rule):
    first = []
    for list in rules[rule]:
        f = list[0]
        if f in rules:
            first.extend(ll_first(f))
        else:
            first.append(f)
    counts = collections.Counter(first)
    for f in first:
        if counts[f] > 1:
            ll_error("FIRST/FIRST conflict")
    return first

def ll_follow(rule):
    follow = []
    if rule == 'START':
        follow.append('EOS')
    for r in rules:
        for list in rules[r]:
            for i in range(len(list)):
                if list[i] == rule:
                    if i < len(list) - 1:
                        if list[i + 1] in rules:
                            follow.extend(ll_first(list[i + 1]))
                        else:
                            follow.append(list[i + 1])
                    elif i == len(list) - 1:
                        follow.extend(ll_follow(r))
    return follow

class ParsingTable:
    def __init__(self):
        self.table = { }
    def __getitem__(self, pos):
        tsym, sym = pos
        if tsym not in self.table:
            self.table[tsym] = { }
        if sym not in self.table[tsym]:
            self.table[tsym][sym] = 0
        return self.table[tsym][sym]
    def __setitem__(self, pos, value):
        tsym, sym = pos
        if tsym not in self.table:
            self.table[tsym] = { }
        self.table[tsym][sym] = value
    def fill(self, rule):
        index = 0
        for list in rules[rule]:
            a = list[0]
            suffix = ''
            if index > 0:
                suffix = '_%d' % (index)
            if a in rules:
                firsts = ll_first(a)
            else:
                firsts = [a]
            for f in firsts:
                self[rule, f] = 'rule_%s%s' % (rule, suffix)
            index += 1

"""
START : EXPR
START : LPAREN START START_FOLLOW
START_FOLLOW : PLUS DATA RPAREN
DATA : EXPR
DATA : LPAREN EXPR RPAREN
START_FOLLOW : RPAREN
EXPR : A

START : EXPR
START : LPAREN START PLUS EXPR RPAREN
EXPR : A
"""

# Output
def output_header():
    lines = []
    lines.append('#include "kpl.h"\n')
    lines.append('#include "kpl_stack.h"\n')
    lines.append('\n')
    index = 256
    for tsym in tsyms:
        lines.append('#define T_SYM_%s %d\n' % (tsym, index))
        index += 1
    lines.append('\n')
    index = 256
    for sym in syms:
        if sym != 'START':
            lines.append('#define NT_SYM_%s %d\n' % (sym, index))
            index += 1
    lines.append('\n')
    return lines

def output_rule(rule, list, index):
    lines = []
    suffix = ''
    if index > 0:
        suffix = '_%d' % (index)
    lines.append('void rule_%s%s(klp_stack_t *stack, void *data)\n' % (rule, suffix))
    lines.append('{\n')
    lines.append('    klp_stack_pop(stack);\n')
    list.reverse()
    for t in list:
        if t in tsyms:
            constant = 'T_SYM_%s' % (t)
        else:
            constant = 'NT_SYM_%s' % (t)
        lines.append('    klp_stack_push(stack, %s);\n' % (constant))
    list.reverse()
    lines.append('}\n\n')
    return lines

def output_rules():
    lines = []
    for rule in rules:
        index = 0
        for index in range(len(rules[rule])):
            list = rules[rule][index]
            lines.extend(output_rule(rule, list, index))
    return lines

def output_handler_getter(table):
    lines = []
    lines.append('rule_handler_t get_rule_handler(klp_parser_t *parser, int ntsym, int tsym)\n')
    lines.append('{\n')
    index = 0
    for sym in table.table:
        for tsym in table.table[sym]:
            lines.append('    ')
            if index > 0:
                lines.append('else ')
            lines.append('if (ntsym == NT_SYM_%s' % (sym))
            lines.append(' && tsym == T_SYM_%s)\n' % (tsym))
            lines.append('        return &%s;\n' % (table.table[sym][tsym]))
            index += 1
    lines.append('    return NULL;\n')
    lines.append('}\n')
    return lines

def main(input_filename, output_filename):
    input_file = open(input_filename, 'r')
    output_file = open(output_filename, 'w+')
    for line in input_file:
        yacc.parse(line)
    if 'START' not in rules:
        ll_error("missing rule 'START'")
    for rule in rules:
        syms.append(rule)
        for list in rules[rule]:
            for t in list:
                if t not in rules and t not in tsyms:
                    tsyms.append(t)
    table = ParsingTable()
    for rule in rules:
        table.fill(rule)
    lines = output_header() + output_rules() + output_handler_getter(table)
    output_file.writelines(lines)
    input_file.close()
    output_file.close()

if __name__ == "__main__" and len(sys.argv) == 3:
    main(sys.argv[1], sys.argv[2])
