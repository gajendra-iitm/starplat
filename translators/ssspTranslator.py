import ast

class PythonToDSLTransformer(ast.NodeVisitor):
    def __init__(self):
        self.dsl_code = []
        self.in_function = False
        self.node_properties = []
        self.current_function = None

    def visit_FunctionDef(self, node):
        self.in_function = True
        self.current_function = node.name
        self.dsl_code.append(f"function {node.name} (Graph g, node src)\n{{\n")
        self.generic_visit(node)
        if self.node_properties:
            # Add property declarations with inferred types
            for prop in self.node_properties:
                if prop['name'] in ['distance', 'modified']:
                    prop_type = 'int' if prop['name'] == 'distance' else 'bool'
                    self.dsl_code.append(f"propNode<{prop_type}> {prop['name']};\n")
            self.node_properties = []
        self.dsl_code.append("}\n")
        self.in_function = False

    def visit_Assign(self, node):
        if isinstance(node.value, ast.Call):
            func_name = self.visit(node.value.func)
            if func_name == "g.attachNodeProperty":
                # Extract arguments for property declarations
                args = [self.visit(arg) for arg in node.value.args]
                prop_defs = ", ".join(args)
                self.dsl_code.append(f"g.attachNodeProperty({prop_defs});\n")
            else:
                # Handle other function calls
                targets = [self.visit(t) for t in node.targets]
                value = self.visit(node.value)
                self.dsl_code.append(f"{targets[0]} = {value};\n")
        else:
            targets = [self.visit(t) for t in node.targets]
            value = self.visit(node.value)
            self.dsl_code.append(f"{targets[0]} = {value};\n")

    def visit_Name(self, node):
        return node.id

    def visit_Constant(self, node):
        if isinstance(node.value, float):
            return f"{node.value:.2f}"
        elif isinstance(node.value, int):
            return str(node.value)
        elif isinstance(node.value, bool):
            return "True" if node.value else "False"
        return str(node.value)

    def visit_Expr(self, node):
        self.dsl_code.append(self.visit(node.value) + ";\n")

    def visit_Call(self, node):
        func_name = self.visit(node.func)
        args = ", ".join(self.visit(arg) for arg in node.args)
        return f"{func_name}({args})"

    def visit_Attribute(self, node):
        value = self.visit(node.value)
        attr = node.attr
        return f"{value}.{attr}"

    def visit_If(self, node):
        test = self.visit(node.test)
        self.dsl_code.append(f"if ({test})\n{{\n")
        for stmt in node.body:
            self.dsl_code.append("    " + self.visit(stmt))
        self.dsl_code.append("}\n")

    def visit_Compare(self, node):
        left = self.visit(node.left)
        comparators = [self.visit(comp) for comp in node.comparators]
        ops = [self.visit(op) for op in node.ops]
        return f"{left} {ops[0]} {comparators[0]}"

    def visit_Eq(self, node):
        return "=="

    def visit_BoolOp(self, node):
        values = [self.visit(value) for value in node.values]
        op = self.visit(node.op)
        return f" {op} ".join(values)

    def visit_And(self, node):
        return "&&"

    def visit_Or(self, node):
        return "||"

    def visit_UnaryOp(self, node):
        operand = self.visit(node.operand)
        if isinstance(node.op, ast.Not):
            return f"({operand} == False)"
        return f"{operand}"

    def visit_Lambda(self, node):
        args = [self.visit(arg) for arg in node.args.args]
        body = self.visit(node.body)
        return f"{args[0]} == {body}"

    def visit_Subscript(self, node):
        value = self.visit(node.value)
        slice = self.visit(node.slice)
        return f"{value}[{slice}]"

    def visit_Index(self, node):
        return self.visit(node.value)

    def visit_Return(self, node):
        return f"return {self.visit(node.value)};"

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

# Example usage
python_code = """
def Compute_SSSP(g, src_node):
    g.attachNodeProperty(distance=inf, modified=False, modified_next=False)
    g.modified[src_node] = True
    g.distance[src_node] = 0
"""

dsl_code = translate_to_dsl(python_code)
print(dsl_code)