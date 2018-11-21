class Solver:
    def __init__(self):
        self.string = ''

    def can_solve(self):
        for char in self.string:
            if char in ['+', '-', '*', '/', '^']:
                return True
        return False

    def find_elements(self):
        for i in range(len(self.string)):
            if self.string[i] in ['+', '-', '*', '/', '^']:
                index = i
        return (int(self.string[0:index]), int(self.string[index + 1:]), self.string[index])

    def solve(self):
        if self.can_solve():
            a, b, op = self.find_elements()

            if op == '+':
                return a + b
            elif op == '-':
                return a - b
            elif op == '*':
                return a * b
            elif op == '/':
                return a / b
            elif op == '^':
                return a ** b
        else:
            return 'Error. No es una operación'
