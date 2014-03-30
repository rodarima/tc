g = [
{'h':'S', 't':['A','B']},
{'h':'S', 't':['B','C']},
{'h':'A', 't':['B','A']},
{'h':'A', 't':['a']},
{'h':'B', 't':['C','C']},
{'h':'B', 't':['b']},
{'h':'C', 't':['A','B']},
{'h':'C', 't':['a']}
]

def table(n):
    table = []
    for i in range(n):
        row = []
        for j in range(n):
            row.append(set())
        table.append(row)
    return table

def unit(g, terminal):
    heads = set()
    for rule in g:
        if [terminal] == rule['t']:
            heads.add(rule['h'])
    return heads

def prod(g, var1, var2):
    heads = set()
    for rule in g:
        if len(rule['t']) == 2:
            if (rule['t'][0] in var1) and (rule['t'][1] in var2):
                heads.add(rule['h'])
    return heads

def cyk(g, string):
    n = len(string)
    t = table(n)
    for x in range(n):
        t[0][x] = unit(g, string[x])
    
    for y in range(1,n):
        for x in range(n-y):
            for k in range(y):
                left=t[k][x]
                right = t[y-1-k][x+k+1]
                #print(left)
                #print(right)
                t[y][x]=t[y][x].union(prod(g, left, right))
                #print(t[y][x])
                #print('-------')

    return t

def cyk_in(g, string):
    t = cyk(g, string)
    if(g[0]['h'] in t[len(string)-1][0]):
        print("YES")
    else:
        print("NO")

cyk_in(g, ['b','a','a','b','a'])
