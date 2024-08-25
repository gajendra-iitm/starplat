import ast

class PythonToDSLTransformer(ast.NodeVisitor):
    def __init__(self):
        self.dsl_code = []

    def visit_FunctionDef(self, node):
        self.dsl_code.append(f"function {node.name}(Graph g)\n{{\n")
        self.generic_visit(node)
        self.dsl_code.append("}\n")

    def visit_Assign(self, node):
        targets = [self.visit(t) for t in node.targets]
        value = self.visit(node.value)
        self.dsl_code.append(f"    {targets[0]} = {value};\n")

    def visit_Name(self, node):
        return node.id

    def visit_Constant(self, node):
        return str(node.value)

    def visit_Expr(self, node):
        self.dsl_code.append(self.visit(node.value) + ";\n")

    def visit_Call(self, node):
        func_name = self.visit(node.func)
        if func_name == "g.attachNodeProperty":
            keywords = ", ".join(f"{kw.arg} = {self.visit(kw.value)}" for kw in node.keywords)
            return f"{func_name}({keywords})"
        elif func_name == "filter":
            lambda_func = self.visit(node.args[0])
            iter_call = self.visit(node.args[1])
            return f"{iter_call}.filter({lambda_func})"
        else:
            args = ", ".join(self.visit(arg) for arg in node.args)
            return f"{func_name}({args})"

    def visit_Attribute(self, node):
        value = self.visit(node.value)
        attr = node.attr
        return f"{value}.{attr}"

    def visit_For(self, node):
        target = self.visit(node.target)
        iter_call = self.visit(node.iter)
        self.dsl_code.append(f"    forall({target} in {iter_call})\n    {{\n")
        self.generic_visit(node)
        self.dsl_code.append("    }\n")

    def visit_If(self, node):
        test = self.visit(node.test)
        self.dsl_code.append(f"        if({test})\n        {{\n")
        for stmt in node.body:
            self.visit(stmt)
        self.dsl_code.append("        }\n")

    def visit_Compare(self, node):
        left = self.visit(node.left)
        right = self.visit(node.comparators[0])
        op = self.visit(node.ops[0])
        return f"{left} {op} {right}"

    def visit_Eq(self, node):
        return "=="

    def visit_NotEq(self, node):
        return "!="

    def visit_Lt(self, node):
        return "<"

    def visit_UnaryOp(self, node):
        operand = self.visit(node.operand)
        if isinstance(node.op, ast.Not) and isinstance(node.operand, ast.Attribute):
            return f"{operand} == False"
        op = self.visit(node.op)
        return f"{op}{operand}"

    def visit_Not(self, node):
        return "not"

    def visit_List(self, node):
        elements = [self.visit(e) for e in node.elts]
        return f"[{', '.join(elements)}]"

    def visit_Dict(self, node):
        keys = [self.visit(k) for k in node.keys]
        values = [self.visit(v) for v in node.values]
        return f"{{{', '.join(f'{k}: {v}' for k, v in zip(keys, values))}}}"

    def visit_Return(self, node):
        value = self.visit(node.value)
        self.dsl_code.append(f"    return {value};\n")

    def visit_Lambda(self, node):
        args = ", ".join(arg.arg for arg in node.args.args)
        body = self.visit(node.body)
        return f"{args} -> {body}"

    def visit_Subscript(self, node):
        value = self.visit(node.value)
        slice = self.visit(node.slice)
        return f"{value}[{slice}]"

    def visit_Index(self, node):
        return self.visit(node.value)

    def generic_visit(self, node):
        for field, value in ast.iter_fields(node):
            if isinstance(value, list):
                for item in value:
                    if isinstance(item, ast.AST):
                        self.visit(item)
            elif isinstance(value, ast.AST):
                self.visit(value)

    def get_code(self):
        return "".join(self.dsl_code)

def translate_to_dsl(python_code):
    tree = ast.parse(python_code)
    transformer = PythonToDSLTransformer()
    transformer.visit(tree)
    return transformer.get_code()

python_code = """
def v_cover(g):
    g.attachNodeProperty(visited=False)
    for v in g.nodes().filter(lambda node: not g.visited[node]):
        for nbr in g.neighbors(v):
            if not g.visited[nbr]:
                g.visited[nbr] = True
                g.visited[v] = True
"""

dsl_code = translate_to_dsl(python_code)
print(dsl_code)